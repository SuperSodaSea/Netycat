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
#include "../Impl/Socket.hpp"
#include "../../IOExecutor.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace TCP {

class TCPSocket {
    
private:
    
    friend class TCPServer;
    
    using ExceptionPtr = Corecat::ExceptionPtr;
    template <typename T = void>
    using Promise = Corecat::Promise<T>;
    
public:
    
    using NativeHandleType = Impl::Socket::NativeHandleType;
    using EndpointType = TCPEndpoint;
    
    using ConnectCallback = std::function<void(const ExceptionPtr&)>;
    
    using ReadCallback = std::function<void(const ExceptionPtr&, std::size_t)>;
    using WriteCallback = std::function<void(const ExceptionPtr&, std::size_t)>;
    
private:
    
    Impl::Socket socket;
    
public:
    
    TCPSocket();
    TCPSocket(IOExecutor& executor_);
    TCPSocket(NativeHandleType socket_);
    TCPSocket(IOExecutor& executor_, NativeHandleType socket_);
    TCPSocket(const TCPSocket& src) = delete;
    ~TCPSocket();
    
    TCPSocket& operator =(const TCPSocket& src) = delete;
    
    void close() noexcept;
    
    void connect(const IPAddress& address, std::uint16_t port);
    void connect(const EndpointType& endpoint);
    void connect(const IPAddress& address, std::uint16_t port, ExceptionPtr& e) noexcept;
    void connect(const EndpointType& endpoint, ExceptionPtr& e) noexcept;
    void connect(const IPAddress& address, std::uint16_t port, ConnectCallback cb) noexcept;
    void connect(const EndpointType& endpoint, ConnectCallback cb) noexcept;
    Promise<> connectAsync(const IPAddress& address, std::uint16_t port) noexcept;
    Promise<> connectAsync(const EndpointType& endpoint) noexcept;
    
    std::size_t read(void* buffer, std::size_t count);
    std::size_t read(void* buffer, std::size_t count, ExceptionPtr& e) noexcept;
    void read(void* buffer, std::size_t count, ReadCallback cb) noexcept;
    Promise<std::size_t> readAsync(void* buffer, std::size_t count) noexcept;
    
    std::size_t readAll(void* buffer, std::size_t count);
    std::size_t readAll(void* buffer, std::size_t count, ExceptionPtr& e) noexcept;
    void readAll(void* buffer, std::size_t count, ReadCallback cb) noexcept;
    Promise<std::size_t> readAllAsync(void* buffer, std::size_t count) noexcept;
    
    std::size_t write(const void* buffer, std::size_t count);
    std::size_t write(const void* buffer, std::size_t count, ExceptionPtr& e) noexcept;
    void write(const void* buffer, std::size_t count, WriteCallback cb) noexcept;
    Promise<std::size_t> writeAsync(const void* buffer, std::size_t count) noexcept;
    
    std::size_t writeAll(const void* buffer, std::size_t count);
    std::size_t writeAll(const void* buffer, std::size_t count, ExceptionPtr& e) noexcept;
    void writeAll(const void* buffer, std::size_t count, WriteCallback cb) noexcept;
    Promise<std::size_t> writeAllAsync(const void* buffer, std::size_t count) noexcept;
    
    EndpointType getRemoteEndpoint();
    EndpointType getRemoteEndpoint(ExceptionPtr& e) noexcept;
    
    NativeHandleType getHandle() noexcept;
    void setHandle(NativeHandleType handle) noexcept;
    
};

}
}
}
}


#endif
