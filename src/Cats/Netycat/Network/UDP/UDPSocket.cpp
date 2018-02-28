/*
 *
 * MIT License
 *
 * Copyright (c) 2016-2018 The Cats Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "Cats/Netycat/Network/UDP/UDPSocket.hpp"

#include "Cats/Corecat/Util/Endian.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace UDP {

UDPSocket::UDPSocket() {}
UDPSocket::UDPSocket(IOExecutor& executor) : socket(executor) {}
UDPSocket::UDPSocket(NativeHandleType handle) : socket(handle) {}
UDPSocket::UDPSocket(IOExecutor& executor, NativeHandleType handle) : socket(executor, handle) {}
UDPSocket::~UDPSocket() {}

void UDPSocket::close() { socket.close(); }

void UDPSocket::bind(std::uint16_t port) { bind(IPv4Address::getAny(), port); }
void UDPSocket::bind(const IPAddress& address, std::uint16_t port) {
    
    sockaddr_storage saddr;
    socklen_t saddrSize;
    switch(address.getType()) {
    case IPAddress::Type::IPv4: {
        
        socket.socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        std::uint32_t addr = address.getIPv4();
        sockaddr_in* saddr4 = reinterpret_cast<sockaddr_in*>(&saddr);
        *saddr4 = {};
        saddr4->sin_family = AF_INET;
        saddr4->sin_port = Corecat::convertNativeToBig(port);
        saddr4->sin_addr.s_addr = Corecat::convertNativeToBig(addr);
        saddrSize = sizeof(sockaddr_in);
        break;
        
    }
    case IPAddress::Type::IPv6: {
        
        socket.socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
        const std::uint8_t* addr = address.getIPv6().getData();
        std::uint32_t scope = address.getIPv6().getScope();
        sockaddr_in6* saddr6 = reinterpret_cast<sockaddr_in6*>(&saddr);
        *saddr6 = {};
        saddr6->sin6_family = AF_INET6;
        saddr6->sin6_port = Corecat::convertNativeToBig(port);
        saddr6->sin6_scope_id = Corecat::convertNativeToBig(scope);
        std::memcpy(saddr6->sin6_addr.s6_addr, addr, 16);
        saddrSize = sizeof(sockaddr_in6);
        break;
        
    }
    default: throw Corecat::InvalidArgumentException("Invalid endpoint type");
    }
    socket.bind(&saddr, saddrSize);
    
}
void UDPSocket::bind(const EndpointType& endpoint) { bind(endpoint.getAddress(), endpoint.getPort()); }

std::pair<std::size_t, UDPSocket::EndpointType> UDPSocket::readFrom(void* buffer, std::size_t count) {
    
    EndpointType endpoint;
    std::size_t ret = readFrom(buffer, count, endpoint.getAddress(), endpoint.getPort());
    return {ret, endpoint};
    
}
std::size_t UDPSocket::readFrom(void* buffer, std::size_t count, IPAddress& address, std::uint16_t& port) {
    
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(saddr);
    std::size_t ret = socket.readFrom(buffer, count, &saddr, saddrSize);
    switch(saddr.ss_family) {
    case AF_INET: {
        
        sockaddr_in* saddr4 = reinterpret_cast<sockaddr_in*>(&saddr);
        address = Corecat::convertBigToNative(saddr4->sin_addr.s_addr);
        port = Corecat::convertBigToNative(saddr4->sin_port);
        break;
        
    }
    case AF_INET6: {
        
        sockaddr_in6* saddr6 = reinterpret_cast<sockaddr_in6*>(&saddr);
        address = {saddr6->sin6_addr.s6_addr, ::ntohl(saddr6->sin6_scope_id)};
        port = Corecat::convertBigToNative(saddr6->sin6_port);
        break;
        
    }
    default: throw Corecat::InvalidArgumentException("Invalid endpoint");
    }
    return ret;
    
}
std::size_t UDPSocket::readFrom(void* buffer, std::size_t count, EndpointType& endpoint) {
    
    return readFrom(buffer, count, endpoint.getAddress(), endpoint.getPort());
    
}
void UDPSocket::readFrom(void* buffer, std::size_t count, ReadFromCallback cb) {
    
    struct AddressBuffer {
        
        sockaddr_storage saddr;
        socklen_t saddrSize = sizeof(saddr);
        
    };
    auto addressBuffer = new AddressBuffer;
    socket.readFrom(buffer, count, &addressBuffer->saddr, addressBuffer->saddrSize, [=, cb = std::move(cb)](auto& e, auto count) {
        
        if(e) {
            
            delete addressBuffer;
            cb(e, count, {});
            return;
            
        }
        switch(addressBuffer->saddr.ss_family) {
        case AF_INET: {
            
            sockaddr_in* saddr4 = reinterpret_cast<sockaddr_in*>(&addressBuffer->saddr);
            cb({}, count, {IPv4Address(Corecat::convertBigToNative(saddr4->sin_addr.s_addr)), Corecat::convertBigToNative(saddr4->sin_port)});
            break;
            
        }
        case AF_INET6: {
            
            sockaddr_in6* saddr6 = reinterpret_cast<sockaddr_in6*>(&addressBuffer->saddr);
            cb({}, count, {IPv6Address(saddr6->sin6_addr.s6_addr, ::ntohl(saddr6->sin6_scope_id)), Corecat::convertBigToNative(saddr6->sin6_port)});
            break;
            
        }
        default: delete addressBuffer, cb(Corecat::InvalidArgumentException("Invalid endpoint"), 0, {}); return;
        }
        delete addressBuffer;
        
    });
    
}
void UDPSocket::readFrom(void* buffer, std::size_t count, IPAddress& address, std::uint16_t& port, ReadCallback cb) {
    
    readFrom(buffer, count, [&, cb = std::move(cb)](auto& e, auto count, auto& endpoint) {
        
        if(e) cb(e, count);
        else {
            
            address = endpoint.getAddress();
            port = endpoint.getPort();
            cb({}, count);
            
        }
        
    });
    
}
void UDPSocket::readFrom(void* buffer, std::size_t count, EndpointType& endpoint, ReadCallback cb) {
    
    return readFrom(buffer, count, endpoint.getAddress(), endpoint.getPort(), std::move(cb));
    
}
Corecat::Promise<std::pair<std::size_t, UDPSocket::EndpointType>> UDPSocket::readFromAsync(void* buffer, std::size_t count) {
    
    Corecat::Promise<std::pair<std::size_t, EndpointType>> promise;
    readFrom(buffer, count, [=](auto& e, auto count, auto& endpoint) {
        e ? promise.reject(e) : promise.resolve(std::pair<std::size_t, UDPSocket::EndpointType>(count, endpoint));
    });
    return promise;
    
}
Corecat::Promise<std::size_t> UDPSocket::readFromAsync(void* buffer, std::size_t count, IPAddress& address, std::uint16_t& port) {
    
    Corecat::Promise<std::size_t> promise;
    readFrom(buffer, count, address, port, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}
Corecat::Promise<std::size_t> UDPSocket::readFromAsync(void* buffer, std::size_t count, EndpointType& endpoint) {
    
    return readFromAsync(buffer, count, endpoint.getAddress(), endpoint.getPort());
    
}

std::size_t UDPSocket::writeTo(const void* buffer, std::size_t count, const IPAddress& address, std::uint16_t port) {
    
    sockaddr_storage saddr;
    socklen_t saddrSize;
    switch(address.getType()) {
    case IPAddress::Type::IPv4: {
        
        std::uint32_t addr = address.getIPv4();
        sockaddr_in* saddr4 = reinterpret_cast<sockaddr_in*>(&saddr);
        *saddr4 = {};
        saddr4->sin_family = AF_INET;
        saddr4->sin_port = Corecat::convertNativeToBig(port);
        saddr4->sin_addr.s_addr = Corecat::convertNativeToBig(addr);
        saddrSize = sizeof(sockaddr_in);
        break;
        
    }
    case IPAddress::Type::IPv6: {
        
        const std::uint8_t* addr = address.getIPv6().getData();
        std::uint32_t scope = address.getIPv6().getScope();
        sockaddr_in6* saddr6 = reinterpret_cast<sockaddr_in6*>(&saddr);
        *saddr6 = {};
        saddr6->sin6_family = AF_INET6;
        saddr6->sin6_port = Corecat::convertNativeToBig(port);
        saddr6->sin6_scope_id = Corecat::convertNativeToBig(scope);
        std::memcpy(saddr6->sin6_addr.s6_addr, addr, 16);
        saddrSize = sizeof(sockaddr_in6);
        break;
        
    }
    default: throw Corecat::InvalidArgumentException("Invalid endpoint type");
    }
    return socket.writeTo(buffer, count, &saddr, saddrSize);
    
}
std::size_t UDPSocket::writeTo(const void* buffer, std::size_t count, const EndpointType& endpoint) {
    
    return writeTo(buffer, count, endpoint.getAddress(), endpoint.getPort());
    
}
void UDPSocket::writeTo(const void* buffer, std::size_t count, const IPAddress& address, std::uint16_t port, WriteCallback cb) {
    
    sockaddr_storage saddr;
    socklen_t saddrSize;
    switch(address.getType()) {
    case IPAddress::Type::IPv4: {
        
        std::uint32_t addr = address.getIPv4();
        sockaddr_in* saddr4 = reinterpret_cast<sockaddr_in*>(&saddr);
        *saddr4 = {};
        saddr4->sin_family = AF_INET;
        saddr4->sin_port = Corecat::convertNativeToBig(port);
        saddr4->sin_addr.s_addr = Corecat::convertNativeToBig(addr);
        saddrSize = sizeof(sockaddr_in);
        break;
        
    }
    case IPAddress::Type::IPv6: {
        
        const std::uint8_t* addr = address.getIPv6().getData();
        std::uint32_t scope = address.getIPv6().getScope();
        sockaddr_in6* saddr6 = reinterpret_cast<sockaddr_in6*>(&saddr);
        *saddr6 = {};
        saddr6->sin6_family = AF_INET6;
        saddr6->sin6_port = Corecat::convertNativeToBig(port);
        saddr6->sin6_scope_id = Corecat::convertNativeToBig(scope);
        std::memcpy(saddr6->sin6_addr.s6_addr, addr, 16);
        saddrSize = sizeof(sockaddr_in6);
        break;
        
    }
    default: cb(Corecat::InvalidArgumentException("Invalid endpoint type"), 0); return;
    }
    socket.writeTo(buffer, count, &saddr, saddrSize, std::move(cb));
    
}
void UDPSocket::writeTo(const void* buffer, std::size_t count, const EndpointType& endpoint, WriteCallback cb) {
    
    return writeTo(buffer, count, endpoint.getAddress(), endpoint.getPort(), std::move(cb));
    
}
Corecat::Promise<std::size_t> UDPSocket::writeToAsync(const void* buffer, std::size_t count, const IPAddress& address, std::uint16_t port) {
    
    Corecat::Promise<std::size_t> promise;
    writeTo(buffer, count, address, port, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}
Corecat::Promise<std::size_t> UDPSocket::writeToAsync(const void* buffer, std::size_t count, const EndpointType& endpoint) {
    
    return writeToAsync(buffer, count, endpoint.getAddress(), endpoint.getPort());
    
}

UDPSocket::NativeHandleType UDPSocket::getHandle() { return socket.getHandle(); }
void UDPSocket::setHandle(NativeHandleType handle) { socket.setHandle(handle); }

}
}
}
}
