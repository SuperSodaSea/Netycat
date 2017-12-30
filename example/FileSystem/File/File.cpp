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

#include <exception>
#include <iostream>

#include "Cats/Corecat/Data/Stream.hpp"
#include "Cats/Netycat/FileSystem.hpp"

using namespace Cats::Corecat::Data::Stream;
using namespace Cats::Corecat::Util;
using namespace Cats::Netycat::FileSystem;

int main(int argc, char** argv) {
    
    if(argc < 3) {
        
        std::cerr << "error: file name needed" << std::endl;
        return 1;
        
    }
    try {
        
        File file1(argv[1], File::Mode::READ);
        auto is = createDataViewInputStream(file1);
        File file2(argv[2], File::Mode::WRITE | File::Mode::CREATE);
        auto os = createDataViewOutputStream(file2);
        Byte data[256];
        while(std::size_t size = is.readSome(data, sizeof(data))) {
            
            std::cout << size << std::endl;
            std::cout.write(reinterpret_cast<char*>(data), size) << std::endl;
            os.writeAll(data, size);
            
        }
        
    } catch(std::exception& e) { std::cout << e.what() << std::endl; }
    
    return 0;
    
}
