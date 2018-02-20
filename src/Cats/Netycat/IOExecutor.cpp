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

void IOExecutor::execute(std::function<void()> f) {
#if defined(NETYCAT_IOEXECUTOR_IOCP)
    auto overlapped = createOverlapped([f = std::move(f)](auto&, auto) { f(); });
    if(!::PostQueuedCompletionStatus(completionPort, 0, 0, overlapped)) {
        
        destroyOverlapped(overlapped);
        throw Corecat::IOException("::PostQueuedCompletionStatus failed");
        
    }
#endif
}

void IOExecutor::wait(double time, WaitCallback cb) {
    
    timerQueue.emplace(Corecat::HighResolutionClock::now() + std::chrono::duration<double>(time), std::move(cb));
    
}
Corecat::Promise<> IOExecutor::waitAsync(double time) {
    
    Corecat::Promise<> promise;
    wait(time, [=]() { promise.resolve(); });
    return promise;
    
}

void IOExecutor::run() {
#if defined(NETYCAT_IOEXECUTOR_IOCP)
    // TODO: Add mutithread support
    while(overlappedCount || timerQueue.size()) {
        
        DWORD byteCount;
        ULONG_PTR completionKey;
        OVERLAPPED* o = nullptr;
        
        auto now = Corecat::HighResolutionClock::now();
        while(timerQueue.size() && timerQueue.top().timePoint <= now) {
            
            timerQueue.top().cb();
            timerQueue.pop();
            
        }
        
        DWORD time = timerQueue.size() ? DWORD((timerQueue.top().timePoint - now).count() * 1000) : INFINITE;
        if(overlappedCount) {
            
            // TODO: Use ::GetQueuedCompletionStatusEx
            bool success = ::GetQueuedCompletionStatus(completionPort, &byteCount, &completionKey, &o, time);
            if(o) {
                
                Corecat::ExceptionWrapper e;
                if(!success) e = Corecat::IOException("::GetQueuedCompletionStatus failed");
                Overlapped* overlapped = static_cast<Overlapped*>(o);
                overlapped->cb(e, byteCount);
                destroyOverlapped(overlapped);
                
            }
            
        } else if(time != INFINITE) {
            
            ::Sleep(time);
            
        }
        
    }
#endif
}

#if defined(NETYCAT_IOEXECUTOR_IOCP)
void IOExecutor::attachHandle(HANDLE handle) {
    
    if(::CreateIoCompletionPort(handle, completionPort, 0, 0) != completionPort)
        throw Corecat::IOException("::CreateIoCompletionPort failed");
    
}
IOExecutor::Overlapped* IOExecutor::createOverlapped(OverlappedCallback cb) {
    
    Overlapped* overlapped = new Overlapped(std::move(cb));
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
