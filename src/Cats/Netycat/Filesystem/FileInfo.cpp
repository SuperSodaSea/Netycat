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

#include "Cats/Netycat/Filesystem/Directory.hpp"

#include "Cats/Corecat/Util/Exception.hpp"


namespace Cats {
namespace Netycat {
inline namespace Filesystem {

FileType FileInfo::getType(const FilePath& path) {
    
    DWORD attribute = ::GetFileAttributesW(path.getData());
    if(attribute == INVALID_FILE_ATTRIBUTES) {
        
        if(GetLastError() == ERROR_FILE_NOT_FOUND) return FileType::NOT_FOUND;
        else throw Corecat::SystemException("::GetFileAttributesW failed");
        
    }
    if(attribute & FILE_ATTRIBUTE_DIRECTORY) return FileType::DIRECTORY;
    else return FileType::FILE;
    
}

std::uint64_t FileInfo::getSize(const FilePath& path) {
    
    WIN32_FILE_ATTRIBUTE_DATA data;
    if(!::GetFileAttributesExW(path.getData(), GetFileExInfoStandard, &data))
        throw Corecat::SystemException("::GetFileAttributesExW failed");
    return (std::uint64_t(data.nFileSizeHigh) << 32) | data.nFileSizeLow;
    
}

SpaceInfo FileInfo::getSpace(const FilePath& path) {
    
    ULARGE_INTEGER capacity, free, available;
    if(!::GetDiskFreeSpaceExW(path.getData(), &available, &capacity, &free))
        throw Corecat::SystemException("::GetDiskFreeSpaceEx failed");
    SpaceInfo space;
    space.capacity = capacity.QuadPart;
    space.free = free.QuadPart;
    space.available = available.QuadPart;
    return space;
    
}

}
}
}
