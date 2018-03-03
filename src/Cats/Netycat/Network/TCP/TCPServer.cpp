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

#include "Cats/Netycat/Network/TCP/TCPServer.hpp"

#include "Cats/Corecat/Util/Endian.hpp"

#include "Cats/Netycat/Network/Impl/Address.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace TCP {

TCPServer::TCPServer() {}
TCPServer::TCPServer(IOExecutor& executor) : socket(executor) {}
TCPServer::TCPServer(NativeHandleType handle) : socket(handle) {}
TCPServer::TCPServer(IOExecutor& executor, NativeHandleType handle) : socket(executor, handle) {}
TCPServer::~TCPServer() {}

void TCPServer::close() noexcept { socket.close(); }

void TCPServer::listen(std::uint16_t port, std::size_t backlog) {
    
    ExceptionPtr e;
    listen(IPv4Address::getAny(), port, backlog, e);
    if(e) e.rethrow();
    
}
void TCPServer::listen(const IPAddress& address, std::uint16_t port, std::size_t backlog) {
    
    ExceptionPtr e;
    listen(address, port, backlog, e);
    if(e) e.rethrow();
    
}
void TCPServer::listen(const EndpointType& endpoint, std::size_t backlog) {
    
    ExceptionPtr e;
    listen(endpoint.getAddress(), endpoint.getPort(), backlog, e);
    if(e) e.rethrow();
    
}
void TCPServer::listen(std::uint16_t port, ExceptionPtr& e) noexcept { listen(IPv4Address::getAny(), port, DEFAULT_BACKLOG, e); }
void TCPServer::listen(std::uint16_t port, std::size_t backlog, ExceptionPtr& e) noexcept { listen(IPv4Address::getAny(), port, backlog, e); }
void TCPServer::listen(const IPAddress& address, std::uint16_t port, ExceptionPtr& e) noexcept { listen(address, port, DEFAULT_BACKLOG, e); }
void TCPServer::listen(const IPAddress& address, std::uint16_t port, std::size_t backlog, ExceptionPtr& e) noexcept {
    
    switch(address.getType()) {
    case IPAddress::Type::IPv4: socket.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, e); if(e) return; break;
    case IPAddress::Type::IPv6: socket.socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP, e); if(e) return; break;
    default: e = Corecat::InvalidArgumentException("Invalid address type"); return;
    }
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(saddr);
    Impl::toSockaddr(&saddr, saddrSize, address, port, e);
    if(e) { close(); return; }
    socket.bind(&saddr, saddrSize, e);
    if(e) { close(); return; }
    socket.listen(backlog, e);
    if(e) { close(); return; }
    
}
void TCPServer::listen(const EndpointType& endpoint, ExceptionPtr& e) noexcept { listen(endpoint.getAddress(), endpoint.getPort(), DEFAULT_BACKLOG, e); }
void TCPServer::listen(const EndpointType& endpoint, std::size_t backlog, ExceptionPtr& e) noexcept { listen(endpoint.getAddress(), endpoint.getPort(), backlog, e); }

void TCPServer::accept(TCPSocket& s) {
    
    ExceptionPtr e;
    accept(s, e);
    if(e) e.rethrow();
    
}
void TCPServer::accept(TCPSocket& s, ExceptionPtr& e) noexcept { socket.accept(s.socket, e); }
void TCPServer::accept(TCPSocket& s, AcceptCallback cb) noexcept { socket.accept(s.socket, std::move(cb)); }
Corecat::Promise<> TCPServer::acceptAsync(TCPSocket& s) noexcept {
    
    Promise<> promise;
    accept(s, [=](auto& e) {
        e ? promise.reject(e) : promise.resolve();
    });
    return promise;
    
}

TCPServer::NativeHandleType TCPServer::getHandle() noexcept { return socket.getHandle(); }
void TCPServer::setHandle(NativeHandleType handle) noexcept { socket.setHandle(handle); }

}
}
}
}
