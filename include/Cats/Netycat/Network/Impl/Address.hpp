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

#ifndef CATS_NETYCAT_NETWORK_IMPL_ADDRESS_HPP
#define CATS_NETYCAT_NETWORK_IMPL_ADDRESS_HPP


#include "../IP/IPAddress.hpp"

#if defined(CORECAT_OS_WINDOWS)
#   include "../Win32/WSA.hpp"
#endif


namespace Cats {
namespace Netycat {
inline namespace Network {
namespace Impl {

inline void fromSockaddr(const void* saddr, socklen_t saddrSize, IPAddress& address, std::uint16_t& port) {
    
    if(saddrSize < socklen_t(sizeof(sockaddr))) throw Corecat::InvalidArgumentException("Invalid sockaddr size");
    switch(reinterpret_cast<const sockaddr*>(saddr)->sa_family) {
    case AF_INET: {
        
        if(saddrSize < socklen_t(sizeof(sockaddr_in))) throw Corecat::InvalidArgumentException("Invalid sockaddr size");
        auto saddr4 = reinterpret_cast<const sockaddr_in*>(saddr);
        address = IPv4Address(Corecat::convertBigToNative(saddr4->sin_addr.s_addr));
        port = Corecat::convertBigToNative(saddr4->sin_port);
        break;
        
    }
    case AF_INET6: {
        
        if(saddrSize < socklen_t(sizeof(sockaddr_in6))) throw Corecat::InvalidArgumentException("Invalid sockaddr size");
        auto saddr6 = reinterpret_cast<const sockaddr_in6*>(saddr);
        address = IPv6Address(saddr6->sin6_addr.s6_addr, Corecat::convertBigToNative(saddr6->sin6_scope_id));
        port = Corecat::convertBigToNative(saddr6->sin6_port);
        break;
        
    }
    default: throw Corecat::InvalidArgumentException("Invalid address type");
    }
    
}
inline void toSockaddr(void* saddr, socklen_t& saddrSize, const IPAddress& address, std::uint16_t port) {
    
    switch(address.getType()) {
    case IPAddress::Type::IPv4: {
        
        if(saddrSize < socklen_t(sizeof(sockaddr_in))) throw Corecat::InvalidArgumentException("Invalid sockaddr size");
        std::uint32_t addr = address.getIPv4();
        sockaddr_in* saddr4 = reinterpret_cast<sockaddr_in*>(saddr);
        *saddr4 = {};
        saddr4->sin_family = AF_INET;
        saddr4->sin_port = Corecat::convertNativeToBig(port);
        saddr4->sin_addr.s_addr = Corecat::convertNativeToBig(addr);
        saddrSize = sizeof(sockaddr_in);
        break;
        
    }
    case IPAddress::Type::IPv6: {
        
        if(saddrSize < socklen_t(sizeof(sockaddr_in6))) throw Corecat::InvalidArgumentException("Invalid sockaddr size");
        const std::uint8_t* addr = address.getIPv6().getData();
        std::uint32_t scope = address.getIPv6().getScope();
        sockaddr_in6* saddr6 = reinterpret_cast<sockaddr_in6*>(saddr);
        *saddr6 = {};
        saddr6->sin6_family = AF_INET6;
        saddr6->sin6_port = Corecat::convertNativeToBig(port);
        saddr6->sin6_scope_id = Corecat::convertNativeToBig(scope);
        std::memcpy(saddr6->sin6_addr.s6_addr, addr, 16);
        saddrSize = sizeof(sockaddr_in6);
        break;
        
    }
    default: throw Corecat::InvalidArgumentException("Invalid address type");
    }
    
}

}
}
}
}


#endif
