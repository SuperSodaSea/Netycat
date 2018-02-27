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
#include <thread>

#include "Cats/Corecat/Concurrent.hpp"
#include "Cats/Netycat/Network.hpp"


using namespace Cats::Corecat;
using namespace Cats::Netycat;


class Server : public Coroutine<Server> {
    
private:
    
    IOExecutor& executor;
    UDPSocket socket{executor};
    UDPEndpoint endpoint;
    char buffer[256];
    std::size_t size;
    
public:
    
    Server(IOExecutor& executor_) : executor(executor_) {}
    
    void operator ()() {
        
        try {
            
            CORECAT_COROUTINE {
                
                socket.bind(12345);
                CORECAT_AWAIT_FOR(socket.readFromAsync(buffer, sizeof(buffer), endpoint), size);
                (std::cout << "Server read & write: ").write(buffer, size) << std::endl;
                CORECAT_AWAIT(socket.writeToAsync(buffer, size, endpoint));
                
            }
            
        } catch(std::exception& e) { std::cerr << e.what() << std::endl; }
        
    }
    
};

class Client : public Coroutine<Client> {
    
private:
    
    IOExecutor& executor;
    UDPSocket socket{executor};
    UDPEndpoint endpoint{IPv4Address::getLoopback(), 12345};
    char buffer[256] = "Hello, Netycat!";
    std::size_t size = std::strlen(buffer);
    
public:
    
    Client(IOExecutor& executor_) : executor(executor_) {}
    
    void operator ()() {
        
        try {
            
            CORECAT_COROUTINE {
                
                socket.bind();
                std::cout << "Client write: " << buffer << std::endl;
                CORECAT_AWAIT(socket.writeToAsync(buffer, size, endpoint));
                CORECAT_AWAIT_FOR(socket.readFromAsync(buffer, sizeof(buffer), endpoint), size);
                (std::cout << "Client read: ").write(buffer, size) << std::endl;
                
            }
            
        } catch(std::exception& e) { std::cerr << e.what() << std::endl; }
        
    }
    
};


int main() {
    
    try {
        
        IOExecutor executor;
        (*std::make_shared<Server>(executor))();
        (*std::make_shared<Client>(executor))();
        executor.run();
        
    } catch(std::exception& e) { std::cerr << e.what() << std::endl; return 1; }
    
    return 0;
    
}
