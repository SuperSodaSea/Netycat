/*
 *
 * MIT License
 *
 * Copyright (c) 2016-2017 The Cats Project
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

#ifndef CATS_NETYCAT_FILESYSTEM_MAPPEDFILE_HPP
#define CATS_NETYCAT_FILESYSTEM_MAPPEDFILE_HPP


#include "Cats/Corecat/Data/DataView/DataView.hpp"
#include "Cats/Corecat/Util/Byte.hpp"
#include "Cats/Corecat/Win32/Handle.hpp"

#include "File.hpp"

#include <stdexcept>


namespace Cats {
namespace Netycat {
inline namespace FileSystem {

class MappedFile : public Corecat::DataView<Corecat::Byte> {
    
private:
    
    using Byte = Corecat::Byte;
    using Handle = Corecat::Handle;
    
public:
    
    enum class Mode : std::uint32_t {
        
        READ,
        READ_WRITE,
        READ_WRITE_COPY,
        
    };
    
private:
    
    Handle handle;
    std::size_t size;
    Byte* data = nullptr;
    bool writable;
    
public:
    
    MappedFile(File& file, std::uint64_t offset, std::size_t size_, Mode mode);
    MappedFile(const MappedFile& src) = delete;
    MappedFile(MappedFile&& src) : handle(std::move(src.handle)), size(src.size), data(src.data) { src.data = nullptr; }
    ~MappedFile() override;
    
    MappedFile& operator =(const MappedFile& src) = delete;
    MappedFile& operator =(MappedFile&& src) { handle = std::move(src.handle), size = src.size, data = src.data, src.data = nullptr; return *this; }
    
    const Byte* getData() const noexcept { return data; }
    Byte* getData() noexcept { return data; }
    
    bool isReadable() override { return true; }
    bool isWritable() override { return writable; }
    bool isResizable() override { return false; }
    void read(Byte* buffer, std::size_t count, std::uint64_t offset) override;
    void write(const Byte* buffer, std::size_t count, std::uint64_t offset) override;
    void flush();
    std::uint64_t getSize() override { return size; }
    void setSize(std::uint64_t /*size*/) override { throw std::runtime_error("DataView is not resizable"); }
    
};

}
}
}


#endif
