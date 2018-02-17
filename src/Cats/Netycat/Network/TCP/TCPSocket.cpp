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
#include "Cats/Netycat/Network/Win32/WSA.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace TCP {

TCPSocket::TCPSocket() {
    
    WSA::init();
    
}
TCPSocket::TCPSocket(IOExecutor& executor_) : executor(&executor_) {
    
    WSA::init();
    
}
TCPSocket::TCPSocket(NativeHandleType socket_) : socket(socket_) {}
TCPSocket::TCPSocket(IOExecutor& executor_, NativeHandleType socket_) : executor(&executor_), socket(socket_) {
    
    executor->attachHandle(HANDLE(socket));
    
}
TCPSocket::~TCPSocket() {
    
    if(socket) close();
    
}

void TCPSocket::connect(const EndpointType& endpoint) {
    
    SOCKET sock;
    sockaddr_storage saddr = {};
    switch(endpoint.getAddress().getType()) {
    case IPAddress::Type::IPv4: {
        
        sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sock == INVALID_SOCKET) throw Corecat::IOException("::socket failed");
        std::uint32_t addr = endpoint.getAddress().getIPv4();
        std::uint16_t port = endpoint.getPort();
        sockaddr_in* saddr4 = reinterpret_cast<sockaddr_in*>(&saddr);
        saddr4->sin_family = AF_INET;
        saddr4->sin_port = Corecat::convertNativeToBig(port);
        saddr4->sin_addr.s_addr = Corecat::convertNativeToBig(addr);
        break;
        
    }
    case IPAddress::Type::IPv6: {
        
        sock = ::socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
        if(sock == INVALID_SOCKET) throw Corecat::IOException("::socket failed");
        const std::uint8_t* addr = endpoint.getAddress().getIPv6().getData();
        std::uint32_t scope = endpoint.getAddress().getIPv6().getScope();
        std::uint16_t port = endpoint.getPort();
        sockaddr_in6* saddr6 = reinterpret_cast<sockaddr_in6*>(&saddr);
        saddr6->sin6_family = AF_INET6;
        saddr6->sin6_port = Corecat::convertNativeToBig(port);
        saddr6->sin6_scope_id = Corecat::convertNativeToBig(scope);
        std::memcpy(saddr6->sin6_addr.s6_addr, addr, 16);
        break;
        
    }
    default: throw Corecat::InvalidArgumentException("Invalid endpoint type");
    }
    if(::connect(sock, reinterpret_cast<sockaddr*>(&saddr), sizeof(saddr))) {
        
        ::closesocket(sock);
        throw Corecat::IOException("::connect failed");
        
    }
    socket = sock;
    
}
void TCPSocket::connect(const EndpointType& endpoint, ConnectCallback cb) {
    
    SOCKET sock;
    sockaddr_storage saddr = {};
    switch(endpoint.getAddress().getType()) {
    case IPAddress::Type::IPv4: {
        
        sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sock == INVALID_SOCKET) { cb(Corecat::IOException("::socket failed")); return; }
        sockaddr_in local = {};
        local.sin_family = AF_INET;
        if(::bind(sock, reinterpret_cast<struct sockaddr*>(&local), sizeof(local))) {
            
            ::closesocket(sock);
            cb(Corecat::IOException("::bind failed"));
            return;
            
        }
        std::uint32_t addr = endpoint.getAddress().getIPv4();
        std::uint16_t port = endpoint.getPort();
        sockaddr_in* saddr4 = reinterpret_cast<sockaddr_in*>(&saddr);
        saddr4->sin_family = AF_INET;
        saddr4->sin_port = Corecat::convertNativeToBig(port);
        saddr4->sin_addr.s_addr = Corecat::convertNativeToBig(addr);
        break;
        
    }
    case IPAddress::Type::IPv6: {
        
        sock = ::socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
        if(sock == INVALID_SOCKET) { cb(Corecat::IOException("::socket failed")); return; }
        sockaddr_in6 local = {};
        local.sin6_family = AF_INET6;
        if(::bind(sock, reinterpret_cast<struct sockaddr*>(&local), sizeof(local))) {
            
            ::closesocket(sock);
            cb(Corecat::IOException("::bind failed"));
            return;
            
        }
        const std::uint8_t* addr = endpoint.getAddress().getIPv6().getData();
        std::uint32_t scope = endpoint.getAddress().getIPv6().getScope();
        std::uint16_t port = endpoint.getPort();
        sockaddr_in6* saddr6 = reinterpret_cast<sockaddr_in6*>(&saddr);
        saddr6->sin6_family = AF_INET6;
        saddr6->sin6_port = Corecat::convertNativeToBig(port);
        saddr6->sin6_scope_id = Corecat::convertNativeToBig(scope);
        std::memcpy(saddr6->sin6_addr.s6_addr, addr, 16);
        break;
        
    }
    default: cb(Corecat::InvalidArgumentException("Invalid endpoint type")); return;
    }
    executor->attachHandle(HANDLE(sock));
    auto overlapped = executor->createOverlapped([=](auto& e, auto) {
        
        if(e) ::closesocket(sock);
        else socket = sock;
        cb(e);
        
    });
    if(!WSA::ConnectEx(sock, reinterpret_cast<struct sockaddr*>(&saddr), sizeof(saddr), nullptr, 0, nullptr, overlapped)
        && ::GetLastError() != ERROR_IO_PENDING) {
        
        executor->destroyOverlapped(overlapped);
        ::closesocket(sock);
        cb(Corecat::IOException("::ConnectEx failed"));
        
    }
    
}
Corecat::Promise<> TCPSocket::connectAsync(const EndpointType& endpoint) {
    
    Corecat::Promise<> promise;
    connect(endpoint, [=](auto& e) {
        e ? promise.reject(e) : promise.resolve();
    });
    return promise;
    
}
void TCPSocket::close() {
    
    ::closesocket(socket);
    socket = 0;
    
}

std::size_t TCPSocket::readSome(void* buffer, std::size_t count) {
    
    // TODO: The "count" argument is int on Windows, but size_t on Linux
    std::ptrdiff_t ret = ::recv(socket, static_cast<char*>(buffer), int(count), 0);
    if(ret < 0) throw Corecat::IOException("::recv failed");
    return ret;
    
}
void TCPSocket::readSome(void* buffer, std::size_t count, ReadCallback cb) {
    
    auto overlapped = executor->createOverlapped([=](auto& e, auto count) {
        
        cb(e, count);
        
    });
    WSABUF buf = {u_long(count), static_cast<char*>(buffer)};
    DWORD flags = 0;
    if(::WSARecv(socket, &buf, 1, nullptr, &flags, overlapped, nullptr)
        && ::GetLastError() != ERROR_IO_PENDING) {
        
        executor->destroyOverlapped(overlapped);
        cb(Corecat::IOException("::WSARecv failed"), 0);
        
    }
    
}
Corecat::Promise<std::size_t> TCPSocket::readSomeAsync(void* buffer, std::size_t count) {
    
    Corecat::Promise<std::size_t> promise;
    readSome(buffer, count, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}
std::size_t TCPSocket::readAll(void* buffer, std::size_t count) {
    
    auto p = static_cast<Byte*>(buffer);
    for(auto n = count; count; ) {
        
        std::size_t ret = readSome(p, n);
        p += ret, n -= ret;
        
    }
    return count;
    
}
void TCPSocket::readAllImpl(Byte* buffer, std::size_t n, ReadCallback cb, std::size_t count) {
    
    auto self = this;
    readSome(buffer, n, [=](auto& e, auto c) {
        
        if(e || n == c) cb(e, count);
        else self->readAllImpl(buffer + c, n - c, cb, count);
        
    });
    
}
void TCPSocket::readAll(void* buffer, std::size_t count, ReadCallback cb) {
    
    readAllImpl(static_cast<Byte*>(buffer), count, cb, count);
    
}
Corecat::Promise<std::size_t> TCPSocket::readAllAsync(void* buffer, std::size_t count) {
    
    Corecat::Promise<std::size_t> promise;
    readAll(buffer, count, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}

std::size_t TCPSocket::writeSome(const void* buffer, std::size_t count) {
    
    std::ptrdiff_t ret = ::send(socket, static_cast<const char*>(buffer), int(count), 0);
    if(ret < 0) throw Corecat::IOException("::send failed");
    return ret;
    
}
void TCPSocket::writeSome(const void* buffer, std::size_t count, WriteCallback cb) {
    
    auto overlapped = executor->createOverlapped([=](auto& e, auto count) {
        
        cb(e, count);
        
    });
    WSABUF buf = {u_long(count), static_cast<char*>(const_cast<void*>(buffer))};
    if(::WSASend(socket, &buf, 1, nullptr, 0, overlapped, nullptr)
        && ::GetLastError() != ERROR_IO_PENDING) {
        
        executor->destroyOverlapped(overlapped);
        cb(Corecat::IOException("::WSASend failed"), 0);
        
    }
    
}
Corecat::Promise<std::size_t> TCPSocket::writeSomeAsync(const void* buffer, std::size_t count) {
    
    Corecat::Promise<std::size_t> promise;
    writeSome(buffer, count, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}
std::size_t TCPSocket::writeAll(const void* buffer, std::size_t count) {
    
    auto p = static_cast<const Byte*>(buffer);
    for(auto n = count; count; ) {
        
        std::size_t ret = writeSome(p, n);
        p += ret, n -= ret;
        
    }
    return count;
    
}
void TCPSocket::writeAllImpl(const Byte* buffer, std::size_t n, WriteCallback cb, std::size_t count) {
    
    auto self = this;
    writeSome(buffer, n, [=](auto& e, auto c) {
        
        if(e || n == c) cb(e, count);
        else self->writeAllImpl(buffer + c, n - c, cb, count);
        
    });
    
}
void TCPSocket::writeAll(const void* buffer, std::size_t count, WriteCallback cb) {
    
    writeAllImpl(static_cast<const Byte*>(buffer), count, cb, count);
    
}
Corecat::Promise<std::size_t> TCPSocket::writeAllAsync(const void* buffer, std::size_t count) {
    
    Corecat::Promise<std::size_t> promise;
    writeAll(buffer, count, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}

TCPSocket::EndpointType TCPSocket::getRemoteEndpoint() {
    
    sockaddr_storage saddr;
    int saddrSize = sizeof(saddr);
    if(::getpeername(socket, reinterpret_cast<sockaddr*>(&saddr), &saddrSize)) throw Corecat::IOException("::getpeername failed");
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

}
}
}
}
