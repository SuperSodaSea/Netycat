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

#include "Cats/Netycat/Network/Win32/WSA.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace TCP {

TCPServer::TCPServer() {
    
    initWSA();
    
}
TCPServer::~TCPServer() {
    
    if(socket) close();
    
}

void TCPServer::listen(TCPEndpoint endpoint, std::size_t backlog) {
    
    SOCKET sock;
    sockaddr_storage saddr;
    switch(endpoint.getAddress().getType()) {
    case IPAddress::Type::IPv4: {
        
        sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sock == INVALID_SOCKET) throw Corecat::SystemException("::socket failed");
        std::uint32_t addr = endpoint.getAddress().getIPv4();
        std::uint16_t port = endpoint.getPort();
        sockaddr_in* saddr4 = reinterpret_cast<sockaddr_in*>(&saddr);
        saddr4->sin_family = AF_INET;
        saddr4->sin_port = ::htons(port);
        saddr4->sin_addr.s_addr = ::htonl(addr);
        break;
        
    }
    case IPAddress::Type::IPv6: {
        
        sock = ::socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
        if(sock == INVALID_SOCKET) throw Corecat::SystemException("::socket failed");
        const std::uint8_t* addr = endpoint.getAddress().getIPv6().getData();
        std::uint32_t scope = endpoint.getAddress().getIPv6().getScope();
        std::uint16_t port = endpoint.getPort();
        sockaddr_in6* saddr6 = reinterpret_cast<sockaddr_in6*>(&saddr);
        saddr6->sin6_family = AF_INET6;
        saddr6->sin6_port = ::htons(port);
        saddr6->sin6_flowinfo = 0;
        saddr6->sin6_scope_id = ::htonl(scope);
        std::memcpy(saddr6->sin6_addr.s6_addr, addr, 16);
        break;
        
    }
    default: throw Corecat::InvalidArgumentException("Invalid endpoint");
    }
    if(::bind(sock, reinterpret_cast<sockaddr*>(&saddr), sizeof(saddr))) {
        
        ::closesocket(sock);
        throw Corecat::SystemException("::bind failed");
        
    }
    if(::listen(sock, int(backlog))) {
        
        ::closesocket(sock);
        throw Corecat::SystemException("::listen failed");
        
    }
    socket = sock;
    
}
void TCPServer::accept(TCPSocket& s) {
    
    sockaddr_storage saddr;
    int saddrSize = sizeof(saddr);
    SOCKET sock = ::accept(socket, reinterpret_cast<sockaddr*>(&saddr), &saddrSize);
    if(sock == INVALID_SOCKET) throw Corecat::SystemException("::accept failed");
    s = {sock};
    
}
void TCPServer::close() {
    
    ::closesocket(socket);
    socket = 0;
    
}

}
}
}
}
