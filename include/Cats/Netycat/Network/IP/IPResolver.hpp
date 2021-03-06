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

#ifndef CATS_NETYCAT_NETWORK_IP_IPRESOLVER_HPP
#define CATS_NETYCAT_NETWORK_IP_IPRESOLVER_HPP


#include <vector>

#include "IPAddress.hpp"
#include "../../IOExecutor.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace IP {

class IPResolver {
    
private:
    
    using String8 = Corecat::String8;
    using ExceptionPtr = Corecat::ExceptionPtr;
    template <typename T = void>
    using Promise = Corecat::Promise<T>;
    
public:
    
    using ResolveCallback = std::function<void(const ExceptionPtr&, std::vector<IPAddress>)>;
    
private:
    
    IOExecutor* executor = nullptr;
    
public:
    
    IPResolver();
    IPResolver(IOExecutor& executor_);
    IPResolver(const IPResolver& src) = delete;
    
    IPResolver& operator =(const IPResolver& src) = delete;
    
    std::vector<IPAddress> resolve(const String8& name);
    std::vector<IPAddress> resolve(const String8& name, Corecat::ExceptionPtr& e);
    void resolve(const String8& name, ResolveCallback cb);
    Promise<std::vector<IPAddress>> resolveAsync(const String8& name);
    
};

}
}
}
}


#endif
