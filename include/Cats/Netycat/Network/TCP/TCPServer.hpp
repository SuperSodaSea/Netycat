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

#ifndef CATS_NETYCAT_NETWORK_TCP_TCPSERVER_HPP
#define CATS_NETYCAT_NETWORK_TCP_TCPSERVER_HPP


#include "TCPSocket.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace TCP {

class TCPServer {
    
private:
    
    using ExceptionPtr = Corecat::ExceptionPtr;
    template <typename T = void>
    using Promise = Corecat::Promise<T>;
    
public:
    
    using NativeHandleType = Impl::Socket::NativeHandleType;
    using EndpointType = TCPEndpoint;
    
    using AcceptCallback = std::function<void(const ExceptionPtr&)>;
    
    static constexpr std::size_t DEFAULT_BACKLOG = Impl::Socket::DEFAULT_BACKLOG;
    
private:
    
    Impl::Socket socket;
    
public:
    
    TCPServer();
    TCPServer(IOExecutor& executor);
    TCPServer(NativeHandleType handle);
    TCPServer(IOExecutor& executor, NativeHandleType handle);
    TCPServer(const TCPServer& src) = delete;
    ~TCPServer();
    
    TCPServer& operator =(const TCPServer& src) = delete;
    
    void close() noexcept;
    
    void listen(std::uint16_t port, std::size_t backlog = DEFAULT_BACKLOG);
    void listen(const IPAddress& address, std::uint16_t port, std::size_t backlog = DEFAULT_BACKLOG);
    void listen(const EndpointType& endpoint, std::size_t backlog = DEFAULT_BACKLOG);
    void listen(std::uint16_t port, ExceptionPtr& e) noexcept;
    void listen(std::uint16_t port, std::size_t backlog, ExceptionPtr& e) noexcept;
    void listen(const IPAddress& address, std::uint16_t port, ExceptionPtr& e) noexcept;
    void listen(const IPAddress& address, std::uint16_t port, std::size_t backlog, ExceptionPtr& e) noexcept;
    void listen(const EndpointType& endpoint, ExceptionPtr& e) noexcept;
    void listen(const EndpointType& endpoint, std::size_t backlog, ExceptionPtr& e) noexcept;
    
    void accept(TCPSocket& s);
    void accept(TCPSocket& s, ExceptionPtr& e) noexcept;
    void accept(TCPSocket& s, AcceptCallback cb) noexcept;
    Promise<> acceptAsync(TCPSocket& s) noexcept;
    
    NativeHandleType getHandle() noexcept;
    void setHandle(NativeHandleType handle) noexcept;
    
};

}
}
}
}


#endif
