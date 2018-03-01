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

#include "Cats/Netycat/Network/Impl/Address.hpp"


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
    
    switch(address.getType()) {
    case IPAddress::Type::IPv4: socket.socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); break;
    case IPAddress::Type::IPv6: socket.socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP); break;
    default: throw Corecat::InvalidArgumentException("Invalid address type");
    }
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(saddr);
    Impl::toSockaddr(&saddr, saddrSize, address, port);
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
    Impl::fromSockaddr(&saddr, saddrSize, address, port);
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
        EndpointType endpoint;
        Impl::fromSockaddr(&addressBuffer->saddr, addressBuffer->saddrSize, endpoint.getAddress(), endpoint.getPort());
        delete addressBuffer;
        cb({}, count, endpoint);
        
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
    socklen_t saddrSize = sizeof(sockaddr_storage);
    Impl::toSockaddr(&saddr, saddrSize, address, port);
    return socket.writeTo(buffer, count, &saddr, saddrSize);
    
}
std::size_t UDPSocket::writeTo(const void* buffer, std::size_t count, const EndpointType& endpoint) {
    
    return writeTo(buffer, count, endpoint.getAddress(), endpoint.getPort());
    
}
void UDPSocket::writeTo(const void* buffer, std::size_t count, const IPAddress& address, std::uint16_t port, WriteCallback cb) {
    
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(sockaddr_storage);
    Impl::toSockaddr(&saddr, saddrSize, address, port);
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
