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

#include "Cats/Netycat/Network/Impl/Address.hpp"


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
    
    switch(address.getType()) {
    // TODO: AF_INET vs PF_INET
    case IPAddress::Type::IPv4: socket.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); break;
    case IPAddress::Type::IPv6: socket.socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP); break;
    default: throw Corecat::InvalidArgumentException("Invalid address type");
    }
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(saddr);
    Impl::toSockaddr(&saddr, saddrSize, address, port);
    socket.connect(&saddr, saddrSize);
    
}
void TCPSocket::connect(const EndpointType& endpoint) { connect(endpoint.getAddress(), endpoint.getPort()); }
void TCPSocket::connect(const IPAddress& address, std::uint16_t port, ConnectCallback cb) {
    
    switch(address.getType()) {
    case IPAddress::Type::IPv4: socket.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); break;
    case IPAddress::Type::IPv6: socket.socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP); break;
    default: cb(Corecat::InvalidArgumentException("Invalid address type")); return;
    }
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(saddr);
    Impl::toSockaddr(&saddr, saddrSize, address, port);
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
    socklen_t saddrSize = sizeof(saddr);
    socket.getRemoteEndpoint(&saddr, saddrSize);
    IPAddress address;
    std::uint16_t port;
    Impl::fromSockaddr(&saddr, saddrSize, address, port);
    return {address, port};
    
}

TCPSocket::NativeHandleType TCPSocket::getHandle() { return socket.getHandle(); }
void TCPSocket::setHandle(NativeHandleType handle) { socket.setHandle(handle); }

}
}
}
}
