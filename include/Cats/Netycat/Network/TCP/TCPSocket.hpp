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
#include "../Win32/WSA.hpp"
#include "../../IOExecutor.hpp"

#include "Cats/Corecat/Util/Byte.hpp"


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
    ~TCPSocket();
    
    TCPSocket& operator =(const TCPSocket& src) = delete;
    
    void connect(const EndpointType& endpoint);
    void connect(const IPAddress& address, std::uint16_t port);
    void connect(const EndpointType& endpoint, ConnectCallback cb);
    void connect(const IPAddress& address, std::uint16_t port, ConnectCallback cb);
    Corecat::Promise<> connectAsync(const EndpointType& endpoint);
    Corecat::Promise<> connectAsync(const IPAddress& address, std::uint16_t port);
    void close();
    
    std::size_t read(void* buffer, std::size_t count);
    void read(void* buffer, std::size_t count, ReadCallback cb);
    Corecat::Promise<std::size_t> readAsync(void* buffer, std::size_t count);
    std::size_t readAll(void* buffer, std::size_t count);
    void readAll(void* buffer, std::size_t count, ReadCallback cb);
    Corecat::Promise<std::size_t> readAllAsync(void* buffer, std::size_t count);
    
    std::size_t write(const void* buffer, std::size_t count);
    void write(const void* buffer, std::size_t count, WriteCallback cb);
    Corecat::Promise<std::size_t> writeAsync(const void* buffer, std::size_t count);
    std::size_t writeAll(const void* buffer, std::size_t count);
    void writeAll(const void* buffer, std::size_t count, WriteCallback cb);
    Corecat::Promise<std::size_t> writeAllAsync(const void* buffer, std::size_t count);
    
    EndpointType getRemoteEndpoint();
    
    NativeHandleType getHandle();
    void setHandle(NativeHandleType socket_);
    
private:
    
    void readAllImpl(Byte* buffer, std::size_t n, ReadCallback cb, std::size_t count);
    void writeAllImpl(const Byte* buffer, std::size_t n, WriteCallback cb, std::size_t count);
    
};

}
}
}
}


#endif
