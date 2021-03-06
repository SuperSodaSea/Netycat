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

#ifndef CATS_NETYCAT_NETWORK_IMPL_SOCKET_HPP
#define CATS_NETYCAT_NETWORK_IMPL_SOCKET_HPP


#include "../IP/IPAddress.hpp"
#include "../../IOExecutor.hpp"

#include "Cats/Corecat/Util/Byte.hpp"

#if defined(CORECAT_OS_WINDOWS)
#   include "../Win32/WSA.hpp"
#endif


namespace Cats {
namespace Netycat {
inline namespace Network {
namespace Impl {

class Socket {
    
private:
    
    using Byte = Corecat::Byte;
    using ExceptionPtr = Corecat::ExceptionPtr;
    
public:
    
    using NativeHandleType = SOCKET;
    
    using AcceptCallback = std::function<void(const ExceptionPtr&)>;
    using ConnectCallback = std::function<void(const ExceptionPtr&)>;
    
    using ReadCallback = std::function<void(const ExceptionPtr&, std::size_t)>;
    using WriteCallback = std::function<void(const ExceptionPtr&, std::size_t)>;
    
    static constexpr std::size_t DEFAULT_BACKLOG = 128;
    
private:
    
    IOExecutor* executor = nullptr;
    NativeHandleType handle = 0;
#if defined(NETYCAT_IOEXECUTOR_IOCP)
    int family = 0;
    int type = 0;
    int protocol = 0;
#endif
    
public:
    
    Socket();
    Socket(IOExecutor& executor_);
    Socket(NativeHandleType handle_);
    Socket(IOExecutor& executor_, NativeHandleType handle_);
    Socket(const Socket& src) = delete;
    ~Socket();
    
    Socket& operator =(const Socket& src) = delete;
    
    void close() noexcept;
    
    void socket(int family_, int type_, int protocol_, ExceptionPtr& e) noexcept;
    
    void bind(const void* address, socklen_t size, ExceptionPtr& e) noexcept;
    
    void connect(const void* address, socklen_t size, ExceptionPtr& e) noexcept;
    void connect(const void* address, socklen_t size, ConnectCallback cb) noexcept;
    
    void listen(ExceptionPtr& e) noexcept;
    void listen(std::size_t backlog, ExceptionPtr& e) noexcept;
    
    void accept(Socket& s, ExceptionPtr& e) noexcept;
    void accept(Socket& s, AcceptCallback cb) noexcept;
    
    std::size_t read(void* buffer, std::size_t count, ExceptionPtr& e) noexcept;
    void read(void* buffer, std::size_t count, ReadCallback cb) noexcept;
    
    std::size_t readAll(void* buffer, std::size_t count, ExceptionPtr& e) noexcept;
    void readAll(void* buffer, std::size_t count, ReadCallback cb) noexcept;
    
    std::size_t readFrom(void* buffer, std::size_t count, void* address, socklen_t& size, ExceptionPtr& e) noexcept;
    void readFrom(void* buffer, std::size_t count, void* address, socklen_t& size, ReadCallback cb) noexcept;
    
    std::size_t write(const void* buffer, std::size_t count, ExceptionPtr& e) noexcept;
    void write(const void* buffer, std::size_t count, WriteCallback cb) noexcept;
    
    std::size_t writeAll(const void* buffer, std::size_t count, ExceptionPtr& e) noexcept;
    void writeAll(const void* buffer, std::size_t count, WriteCallback cb) noexcept;
    
    std::size_t writeTo(const void* buffer, std::size_t count, const void* address, socklen_t size, ExceptionPtr& e) noexcept;
    void writeTo(const void* buffer, std::size_t count, const void* address, socklen_t size, WriteCallback cb) noexcept;
    
    void getRemoteEndpoint(void* address, socklen_t& size, ExceptionPtr& e) noexcept;
    
    NativeHandleType getHandle() noexcept;
    void setHandle(NativeHandleType handle_) noexcept;
    
private:
    
    void readAllImpl(Byte* buffer, std::size_t n, ReadCallback cb, std::size_t count) noexcept;
    void writeAllImpl(const Byte* buffer, std::size_t n, WriteCallback cb, std::size_t count) noexcept;
    
#if defined(NETYCAT_IOEXECUTOR_IOCP)
    void getSocketInfo(ExceptionPtr& e) noexcept;
#endif
    
};

}
}
}
}


#endif
