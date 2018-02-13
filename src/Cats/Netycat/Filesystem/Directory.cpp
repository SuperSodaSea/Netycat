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

void Directory::Iterator::FindDeleter::operator ()(HANDLE handle) const noexcept { ::FindClose(handle); }

Directory::Iterator::Iterator(const FilePath& path_) : path(path_) {
    
    auto str = path.getString();
    str += str.isEmpty() || (!str.endsWith(L'\\') && !str.endsWith(L'/') && !str.endsWith(L':')) ? L"\\*" : L"*";
    WIN32_FIND_DATAW data;
    auto handle_ = ::FindFirstFileW(str.getData(), &data);
    if(handle_ == INVALID_HANDLE_VALUE) {
        
        if(GetLastError() == ERROR_FILE_NOT_FOUND) { handle = nullptr; return; }
        throw Corecat::SystemException("::FindFirstFileW failed");
        
    }
    handle = {handle_, FindDeleter()};
    name = reinterpret_cast<char16_t*>(data.cFileName);
    if(name == L"." || name == L"..") ++*this;
    else if(handle) name = path / name;
    
}

Directory::Iterator& Directory::Iterator::operator ++() {
    
    while(true) {
        
        WIN32_FIND_DATAW data;
        if(!::FindNextFileW(handle.get(), &data)) {
            
            if(GetLastError() == ERROR_NO_MORE_FILES) { handle = nullptr; break; }
            else throw Corecat::SystemException("::FindNextFileW failed");
            
        }
        name = reinterpret_cast<char16_t*>(data.cFileName);
        if(name != L"." && name != L"..") { name = path / name; break; }
        
    }
    return *this;
    
}


RecursiveDirectory::Iterator::Iterator(const FilePath& path) {
    
    Directory dir(path);
    auto b = dir.begin(), e = dir.end();
    if(b != e) {
        
        stack.emplace_back(dir, b);
        name = *b;
        
    }
    
}

RecursiveDirectory::Iterator& RecursiveDirectory::Iterator::operator ++() {
    
    if(FileInfo::isDirectory(name)) {
        
        Directory dir(name);
        auto b = dir.begin(), e = dir.end();
        if(b != e) {
            
            stack.emplace_back(dir, b);
            name = *b;
            return *this;
            
        }
        
    }
    ++stack.back().second;
    while(stack.back().second == stack.back().first.end()) {
        
        stack.pop_back();
        if(stack.empty()) return *this;
        ++stack.back().second;
        
    }
    name = *stack.back().second;
    return *this;
    
}

}
}
}
