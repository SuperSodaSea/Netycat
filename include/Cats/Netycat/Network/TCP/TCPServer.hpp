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
    
    using Byte = Corecat::Byte;
    
public:
    
    using NativeHandleType = SOCKET;
    using EndpointType = TCPEndpoint;
    
    using AcceptCallback = std::function<void(const Corecat::ExceptionWrapper&)>;
    
private:
    
    IOExecutor* executor = nullptr;
    NativeHandleType socket = 0;
    IPAddress::Type type;
    
public:
    
    TCPServer();
    TCPServer(IOExecutor& executor_);
    TCPServer(NativeHandleType socket_);
    TCPServer(IOExecutor& executor_, NativeHandleType socket_);
    TCPServer(const TCPServer& src) = delete;
    ~TCPServer();
    
    TCPServer& operator =(const TCPServer& src) = delete;
    
    void listen(const EndpointType& endpoint, std::size_t backlog = 128);
    void listen(std::uint16_t port, std::size_t backlog = 128);
    void listen(const IPAddress& address, std::uint16_t port, std::size_t backlog = 128);
    void accept(TCPSocket& s);
    void accept(TCPSocket& s, AcceptCallback cb);
    Corecat::Promise<> acceptAsync(TCPSocket& s);
    void close();
    
};

}
}
}
}


#endif
