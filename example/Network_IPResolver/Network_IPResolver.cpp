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

#include <iostream>

#include "Cats/Corecat/Util.hpp"
#include "Cats/Netycat/Network/IP.hpp"


using namespace Cats::Corecat;
using namespace Cats::Netycat;


int main(int argc, char** argv) {
    
    try {
        
        if(argc < 2) throw InvalidArgumentException("Host name needed");
        
        IPResolver resolver;
        for(int i = 1; i < argc; ++i) {
            
            try {
                
                auto addressList = resolver.resolve(argv[i]);
                for(auto&& x : addressList) std::cout << argv[i] << ": " << x.toString() << std::endl;
                
            } catch(std::exception& e) { std::cerr << argv[i] << ": " << e.what() << std::endl; }
            
        }
        
    } catch(std::exception& e) { std::cerr << e.what() << std::endl; return 1; }
    
    return 0;
    
}
