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

#ifndef CATS_NETYCAT_NETWORK_TCP_TCPSOCKET_HPP
#define CATS_NETYCAT_NETWORK_TCP_TCPSOCKET_HPP


#include "TCPEndpoint.hpp"

#include "Cats/Corecat/Concurrent/Promise.hpp"
#include "Cats/Corecat/Util/Byte.hpp"
#include "Cats/Netycat/IOExecutor.hpp"

#include <winsock2.h>


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace TCP {

class TCPSocket {
    
private:
    
    using Byte = Corecat::Byte;
    
public:
    
    using NativeHandleType = SOCKET;
    using EndpointType = TCPEndpoint;
    
    using ConnectCallback = std::function<void(const Corecat::ExceptionWrapper&)>;
    using ReadCallback = std::function<void(const Corecat::ExceptionWrapper&, std::size_t)>;
    using WriteCallback = std::function<void(const Corecat::ExceptionWrapper&, std::size_t)>;
    
private:
    
    IOExecutor* executor = nullptr;
    NativeHandleType socket = 0;
    
public:
    
    TCPSocket();
    TCPSocket(IOExecutor& executor_);
    TCPSocket(NativeHandleType socket_);
    TCPSocket(IOExecutor& executor_, NativeHandleType socket_);
    TCPSocket(const TCPSocket& src) = delete;
    TCPSocket(TCPSocket&& src) : executor(src.executor), socket(src.socket) { src.socket = 0; }
    ~TCPSocket();
    
    TCPSocket& operator =(const TCPSocket& src) = delete;
    TCPSocket& operator =(TCPSocket&& src) { if(socket) close(); executor = src.executor, socket = src.socket, src.socket = 0; return *this; }
    
    void connect(const EndpointType& endpoint);
    void connect(const EndpointType& endpoint, ConnectCallback cb);
    Corecat::Promise<> connectAsync(const EndpointType& endpoint);
    void close();
    
    std::size_t readSome(void* buffer, std::size_t count);
    void readSome(void* buffer, std::size_t count, ReadCallback cb);
    Corecat::Promise<std::size_t> readSomeAsync(void* buffer, std::size_t count);
    std::size_t readAll(void* buffer, std::size_t count);
    void readAll(void* buffer, std::size_t count, ReadCallback cb);
    Corecat::Promise<std::size_t> readAllAsync(void* buffer, std::size_t count);
    
    std::size_t writeSome(const void* buffer, std::size_t count);
    void writeSome(const void* buffer, std::size_t count, WriteCallback cb);
    Corecat::Promise<std::size_t> writeSomeAsync(const void* buffer, std::size_t count);
    std::size_t writeAll(const void* buffer, std::size_t count);
    void writeAll(const void* buffer, std::size_t count, WriteCallback cb);
    Corecat::Promise<std::size_t> writeAllAsync(const void* buffer, std::size_t count);
    
    EndpointType getRemoteEndpoint();
    
private:
    
    void readAllImpl(Byte* buffer, std::size_t n, ReadCallback cb, std::size_t count);
    void writeAllImpl(const Byte* buffer, std::size_t n, WriteCallback cb, std::size_t count);
    
};

}
}
}
}


#endif
