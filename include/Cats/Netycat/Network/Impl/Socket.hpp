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
    
public:
    
    using NativeHandleType = SOCKET;
    
    using AcceptCallback = std::function<void(const Corecat::ExceptionWrapper&)>;
    using ConnectCallback = std::function<void(const Corecat::ExceptionWrapper&)>;
    
    using ReadCallback = std::function<void(const Corecat::ExceptionWrapper&, std::size_t)>;
    using WriteCallback = std::function<void(const Corecat::ExceptionWrapper&, std::size_t)>;
    
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
    
    void close();
    
    void socket(int family_, int type_, int protocol_);
    
    void bind(const void* address, std::size_t size);
    
    void connect(const void* address, std::size_t size);
    void connect(const void* address, std::size_t size, ConnectCallback cb);
    
    void listen(std::size_t backlog = DEFAULT_BACKLOG);
    
    void accept(Socket& s);
    void accept(Socket& s, AcceptCallback cb);
    
    std::size_t read(void* buffer, std::size_t count);
    void read(void* buffer, std::size_t count, ReadCallback cb);
    
    std::size_t readAll(void* buffer, std::size_t count);
    void readAll(void* buffer, std::size_t count, ReadCallback cb);
    
    std::size_t write(const void* buffer, std::size_t count);
    void write(const void* buffer, std::size_t count, WriteCallback cb);
    
    std::size_t writeAll(const void* buffer, std::size_t count);
    void writeAll(const void* buffer, std::size_t count, WriteCallback cb);
    
    void getRemoteEndpoint(void* address, std::size_t* size);
    
    NativeHandleType getHandle();
    void setHandle(NativeHandleType handle_);
    
private:
    
    void readAllImpl(Byte* buffer, std::size_t n, ReadCallback cb, std::size_t count);
    void writeAllImpl(const Byte* buffer, std::size_t n, WriteCallback cb, std::size_t count);
    
#if defined(NETYCAT_IOEXECUTOR_IOCP)
    void getSocketInfo();
#endif
    
};

}
}
}
}


#endif
