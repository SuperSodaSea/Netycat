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

#ifndef CATS_NETYCAT_FILESYSTEM_DIRECTORY_HPP
#define CATS_NETYCAT_FILESYSTEM_DIRECTORY_HPP


#include "Cats/Corecat/Win32/Windows.hpp"
#include "Cats/Netycat/Filesystem/FilePath.hpp"


namespace Cats {
namespace Netycat {
inline namespace Filesystem {

class Directory {
    
public:
    
    class Iterator : public Corecat::EqualityOperator<Iterator> {
        
    private:
        
        friend Directory;
        
    public:
        
        using value_type = FilePath;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::input_iterator_tag;
        
    private:
        
        HANDLE handle = nullptr;
        FilePath name;
        
    private:
        
        Iterator() = default;
        Iterator(const FilePath& path);
        
    public:
        
        ~Iterator();
        
        const FilePath& operator *() const noexcept { return name; }
        const FilePath* operator ->() const noexcept { return &name; }
        Iterator& operator ++();
        friend bool operator ==(const Iterator& a, const Iterator& b) noexcept { return a.handle == b.handle; }
        
    };
    
private:
    
    FilePath path;
    
public:
    
    Directory(FilePath path_) : path(std::move(path_)) {}
    
    Iterator begin() const { return path; }
    Iterator end() const noexcept { return {}; }
    
};

}
}
}


#endif
