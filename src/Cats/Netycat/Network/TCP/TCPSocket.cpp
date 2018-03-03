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

void TCPSocket::close() noexcept { socket.close(); }

void TCPSocket::connect(const IPAddress& address, std::uint16_t port) {
    
    ExceptionPtr e;
    connect(address, port, e);
    if(e) e.rethrow();
    
}
void TCPSocket::connect(const EndpointType& endpoint) {
    
    ExceptionPtr e;
    connect(endpoint.getAddress(), endpoint.getPort(), e);
    if(e) e.rethrow();
    
}
void TCPSocket::connect(const IPAddress& address, std::uint16_t port, ExceptionPtr& e) noexcept {
    
    switch(address.getType()) {
    // TODO: AF_INET vs PF_INET
    case IPAddress::Type::IPv4: socket.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, e); if(e) return; break;
    case IPAddress::Type::IPv6: socket.socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP, e); if(e) return; break;
    default: e = Corecat::InvalidArgumentException("Invalid address type"); return;
    }
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(saddr);
    Impl::toSockaddr(&saddr, saddrSize, address, port, e);
    if(e) { socket.close(); return; }
    socket.connect(&saddr, saddrSize, e);
    if(e) { socket.close(); return; }
    
}
void TCPSocket::connect(const EndpointType& endpoint, ExceptionPtr& e) noexcept { connect(endpoint.getAddress(), endpoint.getPort(), e); }
void TCPSocket::connect(const IPAddress& address, std::uint16_t port, ConnectCallback cb) noexcept {
    
    ExceptionPtr e;
    switch(address.getType()) {
    case IPAddress::Type::IPv4: {
        
        socket.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, e);
        if(e) { cb(e); return; }
        break;
        
    }
    case IPAddress::Type::IPv6: {
        
        socket.socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP, e);
        if(e) { cb(e); return; }
        break;
        
    }
    default: cb(Corecat::InvalidArgumentException("Invalid address type")); return;
    }
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(saddr);
    Impl::toSockaddr(&saddr, saddrSize, address, port, e);
    if(e) { socket.close(); cb(e); return; }
    socket.connect(&saddr, saddrSize, std::move(cb));
    
}
void TCPSocket::connect(const EndpointType& endpoint, ConnectCallback cb) noexcept {
    
    connect(endpoint.getAddress(), endpoint.getPort(), std::move(cb));
    
}
Corecat::Promise<> TCPSocket::connectAsync(const IPAddress& address, std::uint16_t port) noexcept {
    
    Promise<> promise;
    connect(address, port, [=](auto& e) {
        e ? promise.reject(e) : promise.resolve();
    });
    return promise;
    
}
Corecat::Promise<> TCPSocket::connectAsync(const EndpointType& endpoint) noexcept {
    
    return connectAsync(endpoint.getAddress(), endpoint.getPort());
    
}

std::size_t TCPSocket::read(void* buffer, std::size_t count) {
    
    ExceptionPtr e;
    auto ret = read(buffer, count, e);
    if(e) e.rethrow();
    return ret;
    
}
std::size_t TCPSocket::read(void* buffer, std::size_t count, ExceptionPtr& e) noexcept { return socket.read(buffer, count, e); }
void TCPSocket::read(void* buffer, std::size_t count, ReadCallback cb) noexcept { socket.read(buffer, count, std::move(cb)); }
Corecat::Promise<std::size_t> TCPSocket::readAsync(void* buffer, std::size_t count) noexcept {
    
    Promise<std::size_t> promise;
    read(buffer, count, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}

std::size_t TCPSocket::readAll(void* buffer, std::size_t count) {
    
    ExceptionPtr e;
    auto ret = readAll(buffer, count, e);
    if(e) e.rethrow();
    return ret;
    
}
std::size_t TCPSocket::readAll(void* buffer, std::size_t count, ExceptionPtr& e) noexcept { return socket.readAll(buffer, count, e); }
void TCPSocket::readAll(void* buffer, std::size_t count, ReadCallback cb) noexcept { socket.readAll(buffer, count, std::move(cb)); }
Corecat::Promise<std::size_t> TCPSocket::readAllAsync(void* buffer, std::size_t count) noexcept {
    
    Promise<std::size_t> promise;
    readAll(buffer, count, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}

std::size_t TCPSocket::write(const void* buffer, std::size_t count) {
    
    ExceptionPtr e;
    auto ret = write(buffer, count, e);
    if(e) e.rethrow();
    return ret;
    
}
std::size_t TCPSocket::write(const void* buffer, std::size_t count, ExceptionPtr& e) noexcept { return socket.write(buffer, count, e); }
void TCPSocket::write(const void* buffer, std::size_t count, WriteCallback cb) noexcept { socket.write(buffer, count, std::move(cb)); }
Corecat::Promise<std::size_t> TCPSocket::writeAsync(const void* buffer, std::size_t count) noexcept {
    
    Promise<std::size_t> promise;
    write(buffer, count, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}

std::size_t TCPSocket::writeAll(const void* buffer, std::size_t count) {
    
    ExceptionPtr e;
    auto ret = writeAll(buffer, count, e);
    if(e) e.rethrow();
    return ret;
    
}
std::size_t TCPSocket::writeAll(const void* buffer, std::size_t count, ExceptionPtr& e) noexcept { return socket.writeAll(buffer, count, e); }
void TCPSocket::writeAll(const void* buffer, std::size_t count, WriteCallback cb) noexcept { socket.writeAll(buffer, count, std::move(cb)); }
Corecat::Promise<std::size_t> TCPSocket::writeAllAsync(const void* buffer, std::size_t count) noexcept {
    
    Promise<std::size_t> promise;
    writeAll(buffer, count, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}

TCPSocket::EndpointType TCPSocket::getRemoteEndpoint() {
    
    ExceptionPtr e;
    auto ret = getRemoteEndpoint(e);
    if(e) e.rethrow();
    return ret;
    
}
TCPSocket::EndpointType TCPSocket::getRemoteEndpoint(ExceptionPtr& e) noexcept {
    
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(saddr);
    socket.getRemoteEndpoint(&saddr, saddrSize, e);
    if(e) return {};
    IPAddress address;
    std::uint16_t port = 0;
    Impl::fromSockaddr(&saddr, saddrSize, address, port, e);
    if(e) return {};
    return {address, port};
    
}

TCPSocket::NativeHandleType TCPSocket::getHandle() noexcept { return socket.getHandle(); }
void TCPSocket::setHandle(NativeHandleType handle) noexcept { socket.setHandle(handle); }

}
}
}
}
