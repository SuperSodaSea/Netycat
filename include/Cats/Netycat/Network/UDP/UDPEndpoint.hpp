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

#ifndef CATS_NETYCAT_NETWORK_UDP_UDPENDPOINT_HPP
#define CATS_NETYCAT_NETWORK_UDP_UDPENDPOINT_HPP


#include "../IP/IPAddress.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace UDP {

class UDPEndpoint {
    
private:
    
    IPAddress address;
    std::uint16_t port;
    
public:
    
    UDPEndpoint() = default;
    UDPEndpoint(const IPAddress& address_, std::uint16_t port_) noexcept : address(address_), port(port_) {}
    UDPEndpoint(const UDPEndpoint& src) = default;
    
    UDPEndpoint& operator =(const UDPEndpoint& src) = default;
    
    const IPAddress& getAddress() const noexcept { return address; }
    IPAddress& getAddress() noexcept { return address; }
    void setAddress(const IPAddress& address_) noexcept { address = address_; }
    
    const std::uint16_t& getPort() const noexcept { return port; }
    std::uint16_t& getPort() noexcept { return port; }
    void setPort(std::uint16_t port_) noexcept { port = port_; }
    
};

}
}
}
}


#endif
