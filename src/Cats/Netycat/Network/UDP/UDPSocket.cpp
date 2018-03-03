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

#include "Cats/Netycat/Network/UDP/UDPSocket.hpp"

#include "Cats/Corecat/Util/Endian.hpp"

#include "Cats/Netycat/Network/Impl/Address.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace UDP {

UDPSocket::UDPSocket() {}
UDPSocket::UDPSocket(IOExecutor& executor) : socket(executor) {}
UDPSocket::UDPSocket(NativeHandleType handle) : socket(handle) {}
UDPSocket::UDPSocket(IOExecutor& executor, NativeHandleType handle) : socket(executor, handle) {}
UDPSocket::~UDPSocket() {}

void UDPSocket::close() noexcept { socket.close(); }

void UDPSocket::bind(std::uint16_t port) {
    
    ExceptionPtr e;
    bind(IPv4Address::getAny(), port, e);
    if(e) e.rethrow();
    
}
void UDPSocket::bind(const IPAddress& address, std::uint16_t port) {
    
    ExceptionPtr e;
    bind(address, port, e);
    if(e) e.rethrow();
    
}
void UDPSocket::bind(const EndpointType& endpoint) {
    
    ExceptionPtr e;
    bind(endpoint.getAddress(), endpoint.getPort(), e);
    if(e) e.rethrow();
    
}
void UDPSocket::bind(ExceptionPtr& e) noexcept { bind(IPv4Address::getAny(), 0, e); }
void UDPSocket::bind(std::uint16_t port, ExceptionPtr& e) noexcept { bind(IPv4Address::getAny(), port, e); }
void UDPSocket::bind(const IPAddress& address, std::uint16_t port, ExceptionPtr& e) noexcept {
    
    switch(address.getType()) {
    case IPAddress::Type::IPv4: socket.socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, e); if(e) return; break;
    case IPAddress::Type::IPv6: socket.socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP, e); if(e) return; break;
    default: e = Corecat::InvalidArgumentException("Invalid address type"); return;
    }
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(saddr);
    Impl::toSockaddr(&saddr, saddrSize, address, port, e);
    if(e) { socket.close(); return; }
    socket.bind(&saddr, saddrSize, e);
    if(e) { socket.close(); return; }
    
}
void UDPSocket::bind(const EndpointType& endpoint, ExceptionPtr& e) noexcept { bind(endpoint.getAddress(), endpoint.getPort(), e); }

std::pair<std::size_t, UDPSocket::EndpointType> UDPSocket::readFrom(void* buffer, std::size_t count) {
    
    ExceptionPtr e;
    auto ret = readFrom(buffer, count, e);
    if(e) e.rethrow();
    return ret;
    
}
std::size_t UDPSocket::readFrom(void* buffer, std::size_t count, IPAddress& address, std::uint16_t& port) {
    
    ExceptionPtr e;
    auto ret = readFrom(buffer, count, address, port, e);
    if(e) e.rethrow();
    return ret;
    
}
std::size_t UDPSocket::readFrom(void* buffer, std::size_t count, EndpointType& endpoint) {
    
    ExceptionPtr e;
    auto ret = readFrom(buffer, count, endpoint.getAddress(), endpoint.getPort(), e);
    if(e) e.rethrow();
    return ret;
    
}
std::pair<std::size_t, UDPSocket::EndpointType> UDPSocket::readFrom(void* buffer, std::size_t count, ExceptionPtr& e) noexcept {
    
    EndpointType endpoint;
    std::size_t ret = readFrom(buffer, count, endpoint.getAddress(), endpoint.getPort(), e);
    if(e) return {};
    return {ret, endpoint};
    
}
std::size_t UDPSocket::readFrom(void* buffer, std::size_t count, IPAddress& address, std::uint16_t& port, ExceptionPtr& e) noexcept {
    
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(saddr);
    std::size_t ret = socket.readFrom(buffer, count, &saddr, saddrSize, e);
    if(e) return 0;
    Impl::fromSockaddr(&saddr, saddrSize, address, port, e);
    if(e) return 0;
    return ret;
    
}
std::size_t UDPSocket::readFrom(void* buffer, std::size_t count, EndpointType& endpoint, ExceptionPtr& e) noexcept {
    
    return readFrom(buffer, count, endpoint.getAddress(), endpoint.getPort(), e);
    
}
void UDPSocket::readFrom(void* buffer, std::size_t count, ReadFromCallback cb) noexcept {
    
    struct AddressBuffer {
        
        sockaddr_storage saddr;
        socklen_t saddrSize = sizeof(saddr);
        
    };
    auto addressBuffer = new AddressBuffer;
    socket.readFrom(buffer, count, &addressBuffer->saddr, addressBuffer->saddrSize, [=, cb = std::move(cb)](auto& e, auto count) {
        
        if(e) {
            
            delete addressBuffer;
            cb(e, count, {});
            return;
            
        }
        EndpointType endpoint;
        ExceptionPtr e1;
        Impl::fromSockaddr(&addressBuffer->saddr, addressBuffer->saddrSize, endpoint.getAddress(), endpoint.getPort(), e1);
        delete addressBuffer;
        if(e1) { cb(e1, 0, {}); return; }
        cb({}, count, endpoint);
        
    });
    
}
void UDPSocket::readFrom(void* buffer, std::size_t count, IPAddress& address, std::uint16_t& port, ReadCallback cb) noexcept {
    
    readFrom(buffer, count, [&, cb = std::move(cb)](auto& e, auto count, auto& endpoint) {
        
        if(e) cb(e, count);
        else {
            
            address = endpoint.getAddress();
            port = endpoint.getPort();
            cb({}, count);
            
        }
        
    });
    
}
void UDPSocket::readFrom(void* buffer, std::size_t count, EndpointType& endpoint, ReadCallback cb) noexcept {
    
    return readFrom(buffer, count, endpoint.getAddress(), endpoint.getPort(), std::move(cb));
    
}
Corecat::Promise<std::pair<std::size_t, UDPSocket::EndpointType>> UDPSocket::readFromAsync(void* buffer, std::size_t count) noexcept {
    
    Promise<std::pair<std::size_t, EndpointType>> promise;
    readFrom(buffer, count, [=](auto& e, auto count, auto& endpoint) {
        e ? promise.reject(e) : promise.resolve(std::pair<std::size_t, UDPSocket::EndpointType>(count, endpoint));
    });
    return promise;
    
}
Corecat::Promise<std::size_t> UDPSocket::readFromAsync(void* buffer, std::size_t count, IPAddress& address, std::uint16_t& port) noexcept {
    
    Promise<std::size_t> promise;
    readFrom(buffer, count, address, port, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}
Corecat::Promise<std::size_t> UDPSocket::readFromAsync(void* buffer, std::size_t count, EndpointType& endpoint) noexcept {
    
    return readFromAsync(buffer, count, endpoint.getAddress(), endpoint.getPort());
    
}

std::size_t UDPSocket::writeTo(const void* buffer, std::size_t count, const IPAddress& address, std::uint16_t port) {
    
    ExceptionPtr e;
    auto ret = writeTo(buffer, count, address, port, e);
    if(e) e.rethrow();
    return ret;
    
}
std::size_t UDPSocket::writeTo(const void* buffer, std::size_t count, const EndpointType& endpoint) {
    
    ExceptionPtr e;
    auto ret = writeTo(buffer, count, endpoint.getAddress(), endpoint.getPort(), e);
    if(e) e.rethrow();
    return ret;
    
}
std::size_t UDPSocket::writeTo(const void* buffer, std::size_t count, const IPAddress& address, std::uint16_t port, ExceptionPtr& e) noexcept {
    
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(sockaddr_storage);
    Impl::toSockaddr(&saddr, saddrSize, address, port, e);
    if(e) return 0;
    return socket.writeTo(buffer, count, &saddr, saddrSize, e);
    
}
std::size_t UDPSocket::writeTo(const void* buffer, std::size_t count, const EndpointType& endpoint, ExceptionPtr& e) noexcept {
    
    return writeTo(buffer, count, endpoint.getAddress(), endpoint.getPort(), e);
    
}
void UDPSocket::writeTo(const void* buffer, std::size_t count, const IPAddress& address, std::uint16_t port, WriteCallback cb) noexcept {
    
    ExceptionPtr e;
    sockaddr_storage saddr;
    socklen_t saddrSize = sizeof(sockaddr_storage);
    Impl::toSockaddr(&saddr, saddrSize, address, port, e);
    if(e) { cb(e, 0); return; }
    socket.writeTo(buffer, count, &saddr, saddrSize, std::move(cb));
    
}
void UDPSocket::writeTo(const void* buffer, std::size_t count, const EndpointType& endpoint, WriteCallback cb) noexcept {
    
    return writeTo(buffer, count, endpoint.getAddress(), endpoint.getPort(), std::move(cb));
    
}
Corecat::Promise<std::size_t> UDPSocket::writeToAsync(const void* buffer, std::size_t count, const IPAddress& address, std::uint16_t port) noexcept {
    
    Promise<std::size_t> promise;
    writeTo(buffer, count, address, port, [=](auto& e, auto count) {
        e ? promise.reject(e) : promise.resolve(count);
    });
    return promise;
    
}
Corecat::Promise<std::size_t> UDPSocket::writeToAsync(const void* buffer, std::size_t count, const EndpointType& endpoint) noexcept {
    
    return writeToAsync(buffer, count, endpoint.getAddress(), endpoint.getPort());
    
}

UDPSocket::NativeHandleType UDPSocket::getHandle() noexcept { return socket.getHandle(); }
void UDPSocket::setHandle(NativeHandleType handle) noexcept { socket.setHandle(handle); }

}
}
}
}
