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

#ifndef CATS_NETYCAT_FILESYSTEM_FILE_HPP
#define CATS_NETYCAT_FILESYSTEM_FILE_HPP


#include "Cats/Corecat/Data/DataView/DataView.hpp"
#include "Cats/Corecat/Text/String.hpp"
#include "Cats/Corecat/Util/Byte.hpp"
#include "Cats/Corecat/Win32/Handle.hpp"


namespace Cats {
namespace Netycat {
inline namespace FileSystem {

class File : public Corecat::DataView<Corecat::Byte> {
    
private:
    
    using String8 = Corecat::String8;
    using Byte = Corecat::Byte;
    using Handle = Corecat::Handle;
    
public:
    
    enum class Mode : std::uint32_t {
        
        NONE = 0x00000000,
        
        READ = 0x00000001,
        WRITE = 0x00000002,
        READ_WRITE = READ | WRITE,
        
        CREATE = 0x00000004,
        TRUNCATE = 0x0000008,
        EXCLUDE = 0x00000010,
        CREATE_TRUNCATE = CREATE | TRUNCATE,
        CREATE_EXCLUDE = CREATE | EXCLUDE,
        TRUNCATE_EXCLUDE = TRUNCATE | EXCLUDE,
        CREATE_TRUNCATE_EXCLUDE = CREATE | TRUNCATE | EXCLUDE,
        
    };
    friend constexpr Mode operator &(Mode a, Mode b) { return static_cast<Mode>(static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b)); }
    friend constexpr Mode operator |(Mode a, Mode b) { return static_cast<Mode>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b)); }
    
private:
    
    Handle handle;
    bool readable = false;
    bool writable = false;
    
public:
    
    File(String8 path, Mode mode);
    File(const File& src) = delete;
    File(File&& src) = default;
    ~File() override = default;
    
    File& operator =(const File& src) = delete;
    File& operator =(File&& src) = default;
    
    bool isReadable() override { return readable; }
    bool isWritable() override { return writable; }
    bool isResizable() override { return writable; }
    void read(Byte* buffer, std::size_t count, std::uint64_t offset) override;
    void write(const Byte* buffer, std::size_t count, std::uint64_t offset) override;
    void flush() override;
    std::uint64_t getSize() override;
    void setSize(std::uint64_t size) override;
    
    const Handle& getHandle() const { return handle; }
    
};

}
}
}


#endif
