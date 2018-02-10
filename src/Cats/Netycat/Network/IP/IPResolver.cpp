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

#include "Cats/Netycat/Network/IP/IPResolver.hpp"

#include <cstddef>

#include "Cats/Corecat/Util/Endian.hpp"
#include "Cats/Netycat/Network/Win32/WSA.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace IP {

SystemIPResolver::SystemIPResolver() {
    
    initWSA();
    
}

std::vector<IPAddress> SystemIPResolver::resolve(const String8& name) {
    
    addrinfo hints;
    addrinfo* result;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    int errcode;
    if((errcode = getaddrinfo(name.getData(), nullptr, &hints, &result)))
        throw Corecat::IOException("getaddrinfo failed");
    std::size_t count = 0;
    for(addrinfo* cur = result; cur != nullptr; cur = cur->ai_next) ++count;
    std::vector<IPAddress> addressList;
    addressList.reserve(count);
    for(addrinfo* cur = result; cur != nullptr; cur = cur->ai_next) {
        
        switch(cur->ai_family) {
        case AF_INET: {
            
            sockaddr_in* saddr4 = reinterpret_cast<sockaddr_in*>(cur->ai_addr);
            addressList.emplace_back(IPv4Address(Corecat::convertBigToNative(saddr4->sin_addr.s_addr)));
            break;
            
        }
        case AF_INET6: {
            
            sockaddr_in6* saddr6 = reinterpret_cast<sockaddr_in6*>(cur->ai_addr);
            addressList.emplace_back(IPv6Address(saddr6->sin6_addr.s6_addr, saddr6->sin6_scope_id));
            break;
            
        }
        default: continue;
        }
        
    }
    freeaddrinfo(result);
    if(addressList.empty()) throw Corecat::IOException("Unable to resolve hostname");
    return addressList;
    
}

}
}
}
}
