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

#include "Cats/Netycat/FileSystem/MappedFile.hpp"


namespace Cats {
namespace Netycat {
inline namespace FileSystem {

MappedFile::MappedFile(File& file, std::uint64_t offset, std::size_t size_, Mode mode) : size(size_) {
    
    DWORD protect;
    DWORD access;
    switch(mode) {
    case Mode::READ: protect = PAGE_READONLY, access = FILE_MAP_READ, writable = false; break;
    case Mode::READ_WRITE: protect = PAGE_READWRITE, access = FILE_MAP_WRITE, writable = true; break;
    case Mode::READ_WRITE_COPY: protect = PAGE_WRITECOPY, access = FILE_MAP_COPY, writable = true; break;
    default: throw std::invalid_argument("Invalid mode");
    }
    if(!(handle = CreateFileMappingW(file.getHandle(), nullptr, protect, 0, 0, nullptr))) {
        
        std::cout << GetLastError() << std::endl;
        throw std::runtime_error("CreateMappedFile failed");
        
    }
    if(!(data = static_cast<Byte*>(MapViewOfFile(handle, access, offset >> 32, static_cast<DWORD>(offset), size)))) {
        
        std::cout << GetLastError() << std::endl;
        throw std::runtime_error("MapViewOfFile failed");
        
    }
    
}
MappedFile::~MappedFile() {
    
    if(data) UnmapViewOfFile(data);
    
}
void MappedFile::read(Byte* buffer, std::size_t count, std::uint64_t offset) {
    
    if(offset + count > getSize())
        throw std::invalid_argument("End of data");
    std::copy(data + offset, data + offset + count, buffer);
    
}
void MappedFile::write(const Byte* buffer, std::size_t count, std::uint64_t offset) {
    
    if(offset + count > getSize())
        throw std::invalid_argument("End of data");
    std::copy(buffer, buffer + count, data + offset);
    
}
void MappedFile::flush() {
    
    if(!FlushViewOfFile(data, size)) {
        
        std::cout << GetLastError() << std::endl;
        throw std::runtime_error("FlushViewOfFile failed");
        
    }
    
}

}
}
}
