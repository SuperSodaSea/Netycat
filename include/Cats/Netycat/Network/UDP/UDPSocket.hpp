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

#ifndef CATS_NETYCAT_NETWORK_UDP_UDPSOCKET_HPP
#define CATS_NETYCAT_NETWORK_UDP_UDPSOCKET_HPP


#include "UDPEndpoint.hpp"
#include "../Impl/Socket.hpp"
#include "../Win32/WSA.hpp"
#include "../../IOExecutor.hpp"

#include "Cats/Corecat/Util/Byte.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace UDP {

class UDPSocket {
    
private:
    
    using Byte = Corecat::Byte;
    
public:
    
    using NativeHandleType = Impl::Socket::NativeHandleType;
    using EndpointType = UDPEndpoint;
    
    using ReadCallback = std::function<void(const Corecat::ExceptionWrapper&, std::size_t)>;
    using WriteCallback = std::function<void(const Corecat::ExceptionWrapper&, std::size_t)>;
    
private:
    
    Impl::Socket socket;
    
public:
    
    UDPSocket();
    UDPSocket(IOExecutor& executor_);
    UDPSocket(NativeHandleType socket_);
    UDPSocket(IOExecutor& executor_, NativeHandleType socket_);
    UDPSocket(const UDPSocket& src) = delete;
    ~UDPSocket();
    
    UDPSocket& operator =(const UDPSocket& src) = delete;
    
    void close();
    
    void bind(std::uint16_t port = 0);
    void bind(const IPAddress& address, std::uint16_t port);
    void bind(const EndpointType& endpoint);
    
    std::size_t readFrom(void* buffer, std::size_t count, IPAddress& address, std::uint16_t& port);
    std::size_t readFrom(void* buffer, std::size_t count, EndpointType& endpoint);
    void readFrom(void* buffer, std::size_t count, IPAddress& address, std::uint16_t& port, ReadCallback cb);
    void readFrom(void* buffer, std::size_t count, EndpointType& endpoint, ReadCallback cb);
    Corecat::Promise<std::size_t> readFromAsync(void* buffer, std::size_t count, IPAddress& address, std::uint16_t& port);
    Corecat::Promise<std::size_t> readFromAsync(void* buffer, std::size_t count, EndpointType& endpoint);
    
    std::size_t writeTo(const void* buffer, std::size_t count, const IPAddress& address, std::uint16_t port);
    std::size_t writeTo(const void* buffer, std::size_t count, const EndpointType& endpoint);
    void writeTo(const void* buffer, std::size_t count, const IPAddress& address, std::uint16_t port, WriteCallback cb);
    void writeTo(const void* buffer, std::size_t count, const EndpointType& endpoint, WriteCallback cb);
    Corecat::Promise<std::size_t> writeToAsync(const void* buffer, std::size_t count, const IPAddress& address, std::uint16_t port);
    Corecat::Promise<std::size_t> writeToAsync(const void* buffer, std::size_t count, const EndpointType& endpoint);
    
    NativeHandleType getHandle();
    void setHandle(NativeHandleType handle);
    
};

}
}
}
}


#endif
