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

void TCPServer::close() { socket.close(); }

void TCPServer::listen(std::uint16_t port, std::size_t backlog) { listen(IPv4Address::getAny(), port, backlog); }
void TCPServer::listen(const IPAddress& address, std::uint16_t port, std::size_t backlog) {
    
    switch(address.getType()) {
    case IPAddress::Type::IPv4: socket.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); break;
    case IPAddress::Type::IPv6: socket.socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP); break;
    default: throw Corecat::InvalidArgumentException("Invalid address type");
    }
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(saddr);
    Impl::toSockaddr(&saddr, saddrSize, address, port);
    socket.bind(&saddr, saddrSize);
    socket.listen(backlog);
    
}
void TCPServer::listen(const EndpointType& endpoint, std::size_t backlog) { listen(endpoint.getAddress(), endpoint.getPort(), backlog); }

void TCPServer::accept(TCPSocket& s) { socket.accept(s.socket); }
void TCPServer::accept(TCPSocket& s, AcceptCallback cb) { socket.accept(s.socket, std::move(cb)); }
Corecat::Promise<> TCPServer::acceptAsync(TCPSocket& s) {
    
    Corecat::Promise<> promise;
    accept(s, [=](auto& e) {
        e ? promise.reject(e) : promise.resolve();
    });
    return promise;
    
}

TCPServer::NativeHandleType TCPServer::getHandle() { return socket.getHandle(); }
void TCPServer::setHandle(NativeHandleType handle) { socket.setHandle(handle); }

}
}
}
}
