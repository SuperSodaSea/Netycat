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

#include "Cats/Netycat/Network/TCP/TCPSocket.hpp"

#include "Cats/Corecat/Util/Endian.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace TCP {

TCPSocket::TCPSocket() {}
TCPSocket::TCPSocket(IOExecutor& executor) : socket(executor) {}
TCPSocket::TCPSocket(NativeHandleType handle) : socket(handle) {}
TCPSocket::TCPSocket(IOExecutor& executor, NativeHandleType handle) : socket(executor, handle) {}
TCPSocket::~TCPSocket() {}

void TCPSocket::close() { socket.close(); }

void TCPSocket::connect(const IPAddress& address, std::uint16_t port) {
    
    sockaddr_storage saddr;
    std::size_t saddrSize;
    switch(address.getType()) {
    case IPAddress::Type::IPv4: {
        
        // TODO: AF_INET vs PF_INET
        socket.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
        
        socket.socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
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
    socket.connect(&saddr, saddrSize);
    
}
void TCPSocket::connect(const EndpointType& endpoint) { connect(endpoint.getAddress(), endpoint.getPort()); }
void TCPSocket::connect(const IPAddress& address, std::uint16_t port, ConnectCallback cb) {
    
    sockaddr_storage saddr;
    std::size_t saddrSize;
    switch(address.getType()) {
    case IPAddress::Type::IPv4: {
        
        socket.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
        
        socket.socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
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
    default: cb(Corecat::InvalidArgumentException("Invalid endpoint type")); return;
    }
    socket.connect(&saddr, saddrSize, std::move(cb));
    
}
void TCPSocket::connect(const EndpointType& endpoint, ConnectCallback cb) {
    
    connect(endpoint.getAddress(), endpoint.getPort(), std::move(cb));
    
}
Corecat::Promise<> TCPSocket::connectAsync(const IPAddress& address, std::uint16_t port) {
    
    Corecat::Promise<> promise;
    connect(address, port, [=](auto& e) {
        e ? promise.reject(e) : promise.resolve();
    });
    return promise;
    
}
Corecat::Promise<> TCPSocket::connectAsync(const EndpointType& endpoint) {
    
    return connectAsync(endpoint.getAddress(), endpoint.getPort());
    
}

std::size_t TCPSocket::read(void* buffer, std::size_t count) { return socket.read(buffer, count); }
void TCPSocket::read(void* buffer, std::size_t count, ReadCallback cb) { socket.read(buffer, count, std::move(cb)); }
Corecat::Promise<std::size_t> TCPSocket::readAsync(void* buffer, std::size_t count) {
    
    Corecat::Promise<std::size_t> promise;
    read(buffer, count, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}

std::size_t TCPSocket::readAll(void* buffer, std::size_t count) { return socket.readAll(buffer, count); }
void TCPSocket::readAll(void* buffer, std::size_t count, ReadCallback cb) { socket.readAll(buffer, count, std::move(cb)); }
Corecat::Promise<std::size_t> TCPSocket::readAllAsync(void* buffer, std::size_t count) {
    
    Corecat::Promise<std::size_t> promise;
    readAll(buffer, count, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}

std::size_t TCPSocket::write(const void* buffer, std::size_t count) { return socket.write(buffer, count); }
void TCPSocket::write(const void* buffer, std::size_t count, WriteCallback cb) { socket.write(buffer, count, std::move(cb)); }
Corecat::Promise<std::size_t> TCPSocket::writeAsync(const void* buffer, std::size_t count) {
    
    Corecat::Promise<std::size_t> promise;
    write(buffer, count, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}

std::size_t TCPSocket::writeAll(const void* buffer, std::size_t count) { return socket.writeAll(buffer, count); }
void TCPSocket::writeAll(const void* buffer, std::size_t count, WriteCallback cb) { socket.writeAll(buffer, count, std::move(cb)); }
Corecat::Promise<std::size_t> TCPSocket::writeAllAsync(const void* buffer, std::size_t count) {
    
    Corecat::Promise<std::size_t> promise;
    writeAll(buffer, count, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}

TCPSocket::EndpointType TCPSocket::getRemoteEndpoint() {
    
    sockaddr_storage saddr;
    std::size_t saddrSize = sizeof(saddr);
    socket.getRemoteEndpoint(&saddr, &saddrSize);
    switch(saddr.ss_family) {
    case AF_INET: {
        
        sockaddr_in* saddr4 = reinterpret_cast<sockaddr_in*>(&saddr);
        IPv4Address address = Corecat::convertBigToNative(saddr4->sin_addr.s_addr);
        std::uint16_t port = Corecat::convertBigToNative(saddr4->sin_port);
        return {address, port};
        
    }
    case AF_INET6: {
        
        sockaddr_in6* saddr6 = reinterpret_cast<sockaddr_in6*>(&saddr);
        IPv6Address address = {saddr6->sin6_addr.s6_addr, ::ntohl(saddr6->sin6_scope_id)};
        std::uint16_t port = Corecat::convertBigToNative(saddr6->sin6_port);
        return {address, port};
        
    }
    default: throw Corecat::InvalidArgumentException("Invalid endpoint");
    }
    
}

TCPSocket::NativeHandleType TCPSocket::getHandle() { return socket.getHandle(); }
void TCPSocket::setHandle(NativeHandleType handle) { socket.setHandle(handle); }

}
}
}
}
