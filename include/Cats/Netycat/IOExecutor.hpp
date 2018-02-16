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

#ifndef CATS_NETYCAT_IOEXECUTOR_HPP
#define CATS_NETYCAT_IOEXECUTOR_HPP


#include <cstddef>

#include <atomic>

#include "Cats/Corecat/System/OS.hpp"

#if defined(CORECAT_OS_WINDOWS)
#   include "Cats/Corecat/Win32/Handle.hpp"
#   define NETYCAT_IOEXECUTOR_IOCP
#else
#   error Unknown OS
#endif


namespace Cats {
namespace Netycat {

class IOExecutor {
    
#if defined(NETYCAT_IOEXECUTOR_IOCP)
public:
    
    using OverlappedCallback = void(*)(bool, std::size_t, void*);
    struct Overlapped : public OVERLAPPED {
        
        OverlappedCallback cb;
        void* data;
        
        Overlapped(OverlappedCallback cb_, void* data_) : OVERLAPPED(), cb(cb_), data(data_) {}
        Overlapped(const Overlapped& src) = delete;
        
        Overlapped& operator =(const Overlapped& src) = delete;
        
    };
    
private:
    
    Corecat::Handle completionPort;
    std::atomic<size_t> overlappedCount;
#endif
    
public:
    
    IOExecutor();
    IOExecutor(const IOExecutor& src) = delete;
    ~IOExecutor();
    
    IOExecutor& operator =(const IOExecutor& src) = delete;
    
    void run();
    
#if defined(NETYCAT_IOEXECUTOR_IOCP)
    void attachHandle(HANDLE handle);
    Overlapped* createOverlapped(OverlappedCallback cb, void* data);
    void destroyOverlapped(Overlapped* overlapped);
#endif
    
};

}
}


#endif
