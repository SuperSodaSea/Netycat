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

#include "Cats/Netycat/Network/Impl/Socket.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
namespace Impl {

Socket::Socket() {
    
    WSA::init();
    
}
Socket::Socket(IOExecutor& executor_) : executor(&executor_) {
    
    WSA::init();
    
}
Socket::Socket(NativeHandleType handle_) { setHandle(handle_); }
Socket::Socket(IOExecutor& executor_, NativeHandleType handle_) : executor(&executor_) { setHandle(handle_); }
Socket::~Socket() {
    
    if(handle) close();
    
}

void Socket::close() {
    
    ::closesocket(handle);
    handle = 0;
#if defined(NETYCAT_IOEXECUTOR_IOCP)
    family = 0;
    type = 0;
    protocol = 0;
#endif
    
}

void Socket::socket(int family_, int type_, int protocol_) {
    
    if((handle = ::socket(family_, type_, protocol_)) == INVALID_SOCKET)
        throw Corecat::IOException("::socket failed");
    if(executor) executor->attachHandle(HANDLE(handle));
    family = family_, type = type_, protocol = protocol_;
    
}

void Socket::bind(const void* address, std::size_t size) {
    
    if(::bind(handle, reinterpret_cast<const sockaddr*>(address), int(size)))
        throw Corecat::IOException("::bind failed");
    
}

void Socket::connect(const void* address, std::size_t size) {
    
    if(::connect(handle, reinterpret_cast<const sockaddr*>(address), int(size)))
        throw Corecat::IOException("::connect failed");
    
}
void Socket::connect(const void* address, std::size_t size, ConnectCallback cb) {
#if defined(NETYCAT_IOEXECUTOR_IOCP)
    if(!family && !type && !protocol) getSocketInfo();
    sockaddr_storage saddr = {};
    saddr.ss_family = family;
    if(::bind(handle, reinterpret_cast<sockaddr*>(&saddr), sizeof(saddr))) {
        
        cb(Corecat::IOException("::bind failed"));
        return;
        
    }
    auto overlapped = executor->createOverlapped([=](auto& e, auto) { cb(e); });
    if(!WSA::ConnectEx(handle, reinterpret_cast<const sockaddr*>(address), int(size), nullptr, 0, nullptr, overlapped)
        && ::WSAGetLastError() != ERROR_IO_PENDING) {
        
        executor->destroyOverlapped(overlapped);
        cb(Corecat::IOException("::ConnectEx failed"));
        
    }
#endif
}

void Socket::listen(std::size_t backlog) {
    
    if(::listen(handle, int(backlog)))
        throw Corecat::IOException("::listen failed");
    
}

void Socket::accept(Socket& s) {
    
    sockaddr_storage saddr;
    int saddrSize = sizeof(saddr);
    SOCKET sock = ::accept(handle, reinterpret_cast<sockaddr*>(&saddr), &saddrSize);
    if(sock == INVALID_SOCKET) throw Corecat::IOException("::accept failed");
    s.setHandle(sock);
    
}
void Socket::accept(Socket& s, AcceptCallback cb) {
#if defined(NETYCAT_IOEXECUTOR_IOCP)
    if(!family && !type && !protocol) getSocketInfo();
    SOCKET h = ::socket(family, type, protocol);
    if(h == INVALID_SOCKET) {
        
        cb(Corecat::IOException("::socket failed"));
        return;
        
    }
    auto buffer = new Corecat::Byte[(sizeof(sockaddr_storage) + 16) * 2];
    auto overlapped = executor->createOverlapped([=, &s](auto& e, auto) {
        
        delete[] buffer;
        if(e) ::closesocket(h);
        else s.setHandle(h);
        cb(e);
        
    });
    if(!WSA::AcceptEx(handle, h, buffer, 0, sizeof(sockaddr_storage) + 16, sizeof(sockaddr_storage) + 16, nullptr, overlapped)
        && ::WSAGetLastError() != ERROR_IO_PENDING) {
        
        delete[] buffer;
        executor->destroyOverlapped(overlapped);
        ::closesocket(h);
        cb(Corecat::IOException("::AccpetEx failed"));
        
    }
#endif
}

std::size_t Socket::read(void* buffer, std::size_t count) {
    
    // TODO: The "count" argument is int on Windows, but size_t on Linux
    std::ptrdiff_t ret = ::recv(handle, static_cast<char*>(buffer), int(count), 0);
    if(ret < 0) throw Corecat::IOException("::recv failed");
    return ret;
    
}
void Socket::read(void* buffer, std::size_t count, ReadCallback cb) {
#if defined(NETYCAT_IOEXECUTOR_IOCP)
    auto overlapped = executor->createOverlapped([=](auto& e, auto count) { cb(e, count); });
    WSABUF buf = {u_long(count), static_cast<char*>(buffer)};
    DWORD flags = 0;
    if(::WSARecv(handle, &buf, 1, nullptr, &flags, overlapped, nullptr)
        && ::WSAGetLastError() != ERROR_IO_PENDING) {
        
        executor->destroyOverlapped(overlapped);
        cb(Corecat::IOException("::WSARecv failed"), 0);
        
    }
#endif
}

std::size_t Socket::readAll(void* buffer, std::size_t count) {
    
    auto p = static_cast<Byte*>(buffer);
    for(auto n = count; n; ) {
        
        std::size_t ret = read(p, n);
        p += ret, n -= ret;
        
    }
    return count;
    
}
void Socket::readAllImpl(Byte* buffer, std::size_t n, ReadCallback cb, std::size_t count) {
    
    auto self = this;
    read(buffer, n, [=](auto& e, auto c) {
        
        if(e || n == c) cb(e, count);
        else self->readAllImpl(buffer + c, n - c, cb, count);
        
    });
    
}
void Socket::readAll(void* buffer, std::size_t count, ReadCallback cb) {
    
    readAllImpl(static_cast<Byte*>(buffer), count, cb, count);
    
}

std::size_t Socket::write(const void* buffer, std::size_t count) {
    
    std::ptrdiff_t ret = ::send(handle, static_cast<const char*>(buffer), int(count), 0);
    if(ret < 0) throw Corecat::IOException("::send failed");
    return ret;
    
}
void Socket::write(const void* buffer, std::size_t count, WriteCallback cb) {
#if defined(NETYCAT_IOEXECUTOR_IOCP)
    auto overlapped = executor->createOverlapped([=](auto& e, auto count) { cb(e, count); });
    WSABUF buf = {u_long(count), static_cast<char*>(const_cast<void*>(buffer))};
    if(::WSASend(handle, &buf, 1, nullptr, 0, overlapped, nullptr)
        && ::WSAGetLastError() != ERROR_IO_PENDING) {
        
        executor->destroyOverlapped(overlapped);
        cb(Corecat::IOException("::WSASend failed"), 0);
        
    }
#endif
}

std::size_t Socket::writeAll(const void* buffer, std::size_t count) {
    
    auto p = static_cast<const Byte*>(buffer);
    for(auto n = count; n; ) {
        
        std::size_t ret = write(p, n);
        p += ret, n -= ret;
        
    }
    return count;
    
}
void Socket::writeAllImpl(const Byte* buffer, std::size_t n, WriteCallback cb, std::size_t count) {
    
    auto self = this;
    write(buffer, n, [=](auto& e, auto c) {
        
        if(e || n == c) cb(e, count);
        else self->writeAllImpl(buffer + c, n - c, cb, count);
        
    });
    
}
void Socket::writeAll(const void* buffer, std::size_t count, WriteCallback cb) {
    
    writeAllImpl(static_cast<const Byte*>(buffer), count, cb, count);
    
}

void Socket::getRemoteEndpoint(void* address, std::size_t* size) {
    
    int len = int(*size);
    if(::getpeername(handle, reinterpret_cast<sockaddr*>(address), &len)) throw Corecat::IOException("::getpeername failed");
    *size = len;
    
}

Socket::NativeHandleType Socket::getHandle() { return handle; }
void Socket::setHandle(NativeHandleType handle_) {
    
    if(executor) executor->attachHandle(HANDLE(handle_));
    handle = handle_;
    
}

#if defined(NETYCAT_IOEXECUTOR_IOCP)
void Socket::getSocketInfo() {
    
    WSAPROTOCOL_INFOW info;
    int size = sizeof(info);
    if(::getsockopt(handle, SOL_SOCKET, SO_PROTOCOL_INFOW, reinterpret_cast<char*>(&info), &size))
        throw Corecat::IOException("::getsockopt failed");
    family = info.iAddressFamily;
    type = info.iSocketType;
    protocol = info.iProtocol;
    
}
#endif

}
}
}
}
