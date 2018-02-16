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

#include "Cats/Netycat/IOExecutor.hpp"

#include "Cats/Corecat/Util/Exception.hpp"


namespace Cats {
namespace Netycat {

IOExecutor::IOExecutor() {
#if defined(NETYCAT_IOEXECUTOR_IOCP)
    if(!(completionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0)))
        throw Corecat::IOException("::CreateIoCompletionPort failed");
#endif
}
IOExecutor::~IOExecutor() {}

void IOExecutor::run() {
#if defined(NETYCAT_IOEXECUTOR_IOCP)
    while(true) {
        
        DWORD byteCount;
        ULONG_PTR completionKey;
        OVERLAPPED* o = nullptr;
        bool success = GetQueuedCompletionStatus(completionPort, &byteCount, &completionKey, &o, INFINITE);
        if(o) {
            
            Overlapped* overlapped = static_cast<Overlapped*>(o);
            overlapped->cb(success, byteCount, 0);
            destroyOverlapped(overlapped);
            
        }
        
    }
#endif
}

#if defined(NETYCAT_IOEXECUTOR_IOCP)
void IOExecutor::attachHandle(HANDLE handle) {
    
    if(::CreateIoCompletionPort(handle, completionPort, 0, 0) != completionPort)
        throw Corecat::IOException("::CreateIoCompletionPort failed");
    
}
IOExecutor::Overlapped* IOExecutor::createOverlapped(OverlappedCallback cb, void* data) {
    
    Overlapped* overlapped = new Overlapped(cb, data);
    ++overlappedCount;
    return overlapped;
    
}
void IOExecutor::destroyOverlapped(Overlapped* overlapped) {
    
    delete overlapped;
    --overlappedCount;
    
}
#endif

}
}
