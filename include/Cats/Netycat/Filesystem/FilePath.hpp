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

#ifndef CATS_NETYCAT_FILESYSTEM_FILEPATH_HPP
#define CATS_NETYCAT_FILESYSTEM_FILEPATH_HPP


#include "Cats/Corecat/System/OS.hpp"
#include "Cats/Corecat/Text/String.hpp"
#include "Cats/Corecat/Util/Operator.hpp"


namespace Cats {
namespace Netycat {
inline namespace Filesystem {

class FilePath : public Corecat::EqualityOperator<FilePath>, public Corecat::RelationalOperator<FilePath> {
    
private:
    
    template <typename C>
    using String = Corecat::String<C>;
    using UTF8Charset = Corecat::UTF8Charset<>;
    using UTF16Charset = Corecat::UTF16Charset<>;
    
public:
    
#if defined(CATS_CORECAT_SYSTEM_OS_WINDOWS)
    using CharsetType = UTF16Charset;
#elif defined(CATS_CORECAT_SYSTEM_OS_LINUX) || defined(CATS_CORECAT_SYSTEM_OS_MACOS)
    using CharsetType = UTF8Charset;
#else
#   error Unknown OS
#endif
    
    using StringType = String<CharsetType>;
    
private:
    
    StringType data;
    
public:
    
    FilePath() = default;
    FilePath(StringType&& data_) : data(data_) {}
    template <typename T>
    FilePath(const T& t) : data(t) {}
    FilePath(const FilePath& src) = default;
    
    FilePath& operator =(const FilePath& src) = default;
    
    friend bool operator ==(const FilePath& a, const FilePath& b) noexcept { return a.data == b.data; }
    friend bool operator <(const FilePath& a, const FilePath& b) noexcept { return a.data < b.data; }
    
    operator StringType() const { return data; }
    
    const StringType& getData() const noexcept { return data; }
    
    bool isEmpty() const noexcept { return data.isEmpty(); }
    
    void clear() noexcept { data.clear(); }
    
    void swap(FilePath& path) noexcept { std::swap(data, path.data); }
    
};

}
}
}


#endif
