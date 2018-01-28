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

Directory::Iterator::Iterator(const FilePath& path) {
    
    Corecat::WString str = path.getData();
    str += str.isEmpty() || (str[str.getLength() - 1] != u'\\' && str[str.getLength() - 1] != u'/' && str[str.getLength() - 1] != u':') ? L"\\*" : L"*";
    WIN32_FIND_DATAW data;
    handle = FindFirstFileW(str.getData(), &data);
    if(handle == INVALID_HANDLE_VALUE) {
        
        if(GetLastError() == ERROR_FILE_NOT_FOUND) { handle = nullptr; return; }
        throw Corecat::SystemException("FindFirstFileW failed");
        
    }
    name = reinterpret_cast<char16_t*>(data.cFileName);
    if(name == L"." || name == L"..") ++*this;
    
}
Directory::Iterator::~Iterator() {
    
    if(handle) FindClose(handle);
    
}

Directory::Iterator& Directory::Iterator::operator ++() {
    
    while(true) {
        
        WIN32_FIND_DATAW data;
        if(!FindNextFileW(handle, &data)) {
            
            if(GetLastError() == ERROR_NO_MORE_FILES) { FindClose(handle), handle = nullptr; break; }
            else throw Corecat::SystemException("FindNextFileW failed");
            
        }
        name = reinterpret_cast<char16_t*>(data.cFileName);
        if(name != L"." && name != L"..") break;
        
    }
    return *this;
    
}

}
}
}
