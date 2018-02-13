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

#include "Cats/Netycat/Filesystem/File.hpp"

#include "Cats/Corecat/Util/Exception.hpp"


namespace Cats {
namespace Netycat {
inline namespace Filesystem {

File::File(const FilePath& path, Mode mode) {
    
    DWORD access;
    switch(mode & Mode::READ_WRITE) {
    case Mode::READ: access = GENERIC_READ, readable = true; break;
    case Mode::WRITE: access = GENERIC_WRITE, writable = true; break;
    case Mode::READ_WRITE: access = GENERIC_READ | GENERIC_WRITE, readable = true, writable = true; break;
    default: throw Corecat::InvalidArgumentException("Invalid mode");
    }
    
    DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    
    DWORD disposition;
    switch(mode & Mode::CREATE_TRUNCATE_EXCLUDE) {
    case Mode::NONE:
    case Mode::EXCLUDE: disposition = OPEN_EXISTING; break;
    case Mode::CREATE: disposition = OPEN_ALWAYS; break;
    case Mode::TRUNCATE:
    case Mode::TRUNCATE_EXCLUDE: disposition = TRUNCATE_EXISTING; break;
    case Mode::CREATE_TRUNCATE: disposition = CREATE_ALWAYS; break;
    case Mode::CREATE_EXCLUDE:
    case Mode::CREATE_TRUNCATE_EXCLUDE: disposition = CREATE_NEW; break;
    default: throw Corecat::InvalidArgumentException("Invalid mode");
    }
    
    DWORD attribute = FILE_ATTRIBUTE_NORMAL;
    
    if(!(handle = ::CreateFileW(path.getData(), access, share, nullptr, disposition, attribute, nullptr))) {
        
        throw Corecat::IOException("::CreateFileW failed");
        
    }
    
}
void File::read(Byte* buffer, std::size_t count, std::uint64_t offset) {
    
    assert(handle);
    
    if(offset + count > getSize())
        throw Corecat::IOException("End of file");
    
    OVERLAPPED overlapped = {};
    overlapped.Offset = DWORD(offset);
    overlapped.OffsetHigh = offset >> 32;
    DWORD byteCount;
    if(!::ReadFile(handle, buffer, DWORD(count), &byteCount, &overlapped)) {
        
        throw Corecat::IOException("::ReadFile failed");
        
    }
    
}
void File::write(const Byte* buffer, std::size_t count, std::uint64_t offset) {
    
    assert(handle);
    
    OVERLAPPED overlapped = {};
    overlapped.Offset = DWORD(offset);
    overlapped.OffsetHigh = offset >> 32;
    DWORD byteCount;
    if(!::WriteFile(handle, buffer, DWORD(count), &byteCount, &overlapped)) {
        
        throw Corecat::IOException("::WriteFile failed");
        
    }
    
}
void File::flush() {
    
    if(!::FlushFileBuffers(handle)) {
        
        throw Corecat::IOException("::FlushFileBuffers failed");
        
    }
    
}
std::uint64_t File::getSize() {
    
    assert(handle);
    
    LARGE_INTEGER s;
    if(!::GetFileSizeEx(handle, &s)) {
        
        throw Corecat::IOException("::GetFileSizeEx failed");
        
    }
    return s.QuadPart;
    
}
void File::setSize(std::uint64_t size) {
    
    assert(handle);
    
    LARGE_INTEGER s;
    s.QuadPart = size;
    if(!::SetFilePointerEx(handle, s, nullptr, FILE_BEGIN))
        throw Corecat::IOException("::SetFilePointerEx failed");
    if(!::SetEndOfFile(handle))
        throw Corecat::IOException("::SetEndOfFile failed");
    
}

}
}
}
