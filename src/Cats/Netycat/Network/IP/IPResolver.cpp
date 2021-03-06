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

#include <thread>

#include "Cats/Corecat/Util/Endian.hpp"
#include "Cats/Netycat/Network/Win32/WSA.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace IP {

IPResolver::IPResolver() {
    
    WSA::init();
    
}
IPResolver::IPResolver(IOExecutor& executor_) : executor(&executor_) {
    
    WSA::init();
    
}

std::vector<IPAddress> IPResolver::resolve(const String8& name) {
    
    Corecat::ExceptionPtr e;
    auto addressList = resolve(name, e);
    if(e) e.rethrow();
    return addressList;
    
}
std::vector<IPAddress> IPResolver::resolve(const String8& name, Corecat::ExceptionPtr& e) {
    
    // TODO: Use GetAddrInfoW on Windows
#if defined(CORECAT_OS_WINDOWS)
    using AddrInfoType = addrinfoW;
#else
    using AddrInfoType = addrinfo;
#endif
    AddrInfoType hints;
    AddrInfoType* result;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    int errcode;
#if defined(CORECAT_OS_WINDOWS)
    if((errcode = ::GetAddrInfoW(Corecat::WString(name).getData(), nullptr, &hints, &result))) {
        
        e = Corecat::IOException("::GetAddrInfoW failed");
#else
    if((errcode = ::getaddrinfo(name.getData(), nullptr, &hints, &result))) {
        
        e = Corecat::IOException("::getaddrinfo failed");
#endif
        return {};
        
    }
    std::size_t count = 0;
    for(auto cur = result; cur != nullptr; cur = cur->ai_next) ++count;
    std::vector<IPAddress> addressList;
    addressList.reserve(count);
    for(auto cur = result; cur != nullptr; cur = cur->ai_next) {
        
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
#if defined(CORECAT_OS_WINDOWS)
    ::FreeAddrInfoW(result);
#else
    ::freeaddrinfo(result);
#endif
    if(addressList.empty()) {
        
        e = Corecat::IOException("No address available for name");
        return {};
        
    }
    return addressList;
    
}
void IPResolver::resolve(const String8& name, ResolveCallback cb) {
    
    executor->beginWork();
    executor->getThreadPool().execute([=, cb = std::move(cb)] {
        
        Corecat::ExceptionPtr e;
        auto addressList = resolve(name, e);
        executor->execute([cb = std::move(cb), e = std::move(e), addressList = std::move(addressList)] { cb(e, std::move(addressList)); });
        executor->endWork();
        
    });
    
}
Corecat::Promise<std::vector<IPAddress>> IPResolver::resolveAsync(const String8& name) {
    
    Promise<std::vector<IPAddress>> promise;
    resolve(name, [=](auto& e, auto addressList) {
        e ? promise.reject(e) : promise.resolve(std::move(addressList));
    });
    return promise;
    
}

}
}
}
}
