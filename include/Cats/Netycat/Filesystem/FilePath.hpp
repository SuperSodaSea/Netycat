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
#include "Cats/Corecat/Util/Exception.hpp"
#include "Cats/Corecat/Util/Iterator.hpp"
#include "Cats/Corecat/Util/Operator.hpp"

#if defined(CORECAT_OS_WINDOWS)
#   include "Cats/Corecat/Win32/Windows.hpp"
#else
#   error Unknown OS
#endif


namespace Cats {
namespace Netycat {
inline namespace Filesystem {

class FilePath : public Corecat::EqualityOperator<FilePath>, public Corecat::RelationalOperator<FilePath> {
    
private:
    
    template <typename C>
    using String = Corecat::String<C>;
    
public:
    
#if defined(CORECAT_OS_WINDOWS)
    using CharsetType = Corecat::WideCharset<>;
#elif defined(CORECAT_OS_LINUX) || defined(CORECAT_OS_MACOS)
    using CharsetType = Corecat::UTF8Charset<>;
#endif
    
    using CharType = CharsetType::CharType;
    using StringType = String<CharsetType>;
    using StringViewType = StringType::StringViewType;
    
    class Iterator;
    using ReverseIterator = Corecat::ReverseIterator<Iterator>;
    
public:
    
#if defined(CORECAT_OS_WINDOWS)
    static constexpr wchar_t SEPARATOR = L'\\';
#elif defined(CORECAT_OS_LINUX) || defined(CORECAT_OS_MACOS)
    static constexpr char SEPARATOR = '/';
#endif

    constexpr static bool isSeparator(CharType c) {
#if defined(CORECAT_OS_WINDOWS)
        return c == L'/' || c == L'\\';
#elif defined(CORECAT_OS_LINUX) || defined(CORECAT_OS_MACOS)
        return c == '/';
#endif
    }
    
private:
    
    StringType data;
    std::size_t rootLength = 0;
    std::size_t rootDirectoryLength = 0;
    std::size_t filenameLength = 0;
    
private:
    
    void init() noexcept {
        
        auto b = data.begin(), e = data.end();
        
        // Root length
#if defined(CORECAT_OS_WINDOWS)
        if(data.getLength() >= 2 && ((data[0] >= L'A' && data[0] <= L'Z') || (data[0] >= L'a' && data[0] <= L'z')) && data[1] == L':')
            rootLength = 2;
#endif
        
        // Root directory length
        {
            auto p = b + rootLength;
            while(p != e && isSeparator(*p)) ++p;
            rootDirectoryLength = p - b;
        }
        
        // Filename length
        if(rootLength != data.getLength()) {
            
            auto p = b + rootDirectoryLength, q = e;
            while(!isSeparator(q[-1]) && p != --q);
            filenameLength = e - q;
            
        }
        
    }
    
public:
    
    FilePath() = default;
    FilePath(StringType&& data_) : data(std::move(data_)) { init(); }
    template <typename T>
    FilePath(const T& t) : FilePath(StringType(t)) {}
    template <typename I>
    FilePath(I b, I e) : FilePath({b, e}) {}
    FilePath(const FilePath& src) = default;
    
    FilePath& operator =(const FilePath& src) = default;
    
    FilePath& operator /=(const FilePath& b) {
        
        if(b.isAbsolute() || (b.hasRoot() && getRootString() != b.getRootString())) *this = b;
        else {
            
            if(b.hasRootDirectory()) data.setLength(rootLength), data += b.getString();
            else if(hasFilename() || (!hasRootDirectory() && isAbsolute())) data += SEPARATOR + b.getString();
            else data += b.getString();
            init();
            
        }
        return *this;
        
    }
    
    friend FilePath operator /(const FilePath& a, const FilePath& b) { return FilePath(a) /= b; }
    friend FilePath operator /(FilePath&& a, const FilePath& b) { return a /= b; }
    
    friend bool operator ==(const FilePath& a, const FilePath& b) noexcept { return a.data == b.data; }
    friend bool operator <(const FilePath& a, const FilePath& b) noexcept { return a.data < b.data; }
    
    operator StringType() const { return data; }
    operator StringViewType() const { return data; }
    
    const StringType& getString() const noexcept { return data; }
    const CharType* getData() const noexcept { return data.getData(); }
    
    bool isEmpty() const noexcept { return data.isEmpty(); }
    
    bool hasRoot() const noexcept { return rootLength; }
    bool hasRootDirectory() const noexcept { return rootDirectoryLength; }
    bool hasFilename() const noexcept { return filenameLength; }
    
    bool isAbsolute() const noexcept {
#if defined(CORECAT_OS_WINDOWS)
        return hasRoot() && hasRootDirectory();
#elif defined(CORECAT_OS_LINUX) || defined(CORECAT_OS_MACOS)
        return hasRootDirectory();
#endif
    }
    
    StringViewType getRootString() const noexcept { return data.substr(0, rootLength); }
    FilePath getRoot() const { return getRootString(); }
    StringViewType getRootDirectoryString() const noexcept { return data.substr(0, rootDirectoryLength); }
    FilePath getRootDirectory() const { return getRootDirectoryString(); }
    StringViewType getRelativePathString() const noexcept { return data.substr(rootDirectoryLength); }
    FilePath getRelativePath() const { return getRelativePathString(); }
    StringViewType getFilenameString() const noexcept { return data.slice(data.getLength() - filenameLength); }
    FilePath getFilename() const noexcept { return getFilenameString(); }
    
    void clear() noexcept { data.clear(); }
    
    void swap(FilePath& path) noexcept { std::swap(data, path.data); }
    
public:
    
    static FilePath getCurrent() {
        
        DWORD size = ::GetCurrentDirectoryW(0, nullptr);
        if(!size) throw Corecat::SystemException("::GetCurrentDirectoryW failed");
        Corecat::WString data;
        data.setLength(size);
        if(!::GetCurrentDirectoryW(size, data.getData()))
            throw Corecat::SystemException("::GetCurrentDirectoryW failed");
        return data;
        
    }
    
};

}
}
}


#endif
