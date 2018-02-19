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
#include "Cats/Corecat/Util.hpp"
#include "Cats/Netycat/Network.hpp"


using namespace Cats::Corecat;
using namespace Cats::Netycat;


class Server : public Coroutine<Server> {
    
private:
    
    IOExecutor& executor;
    TCPServer server{executor};
    TCPSocket socket{executor};
    const char* data = "Hello, Netycat!";
    std::uint8_t size = std::uint8_t(std::strlen(data));
    
public:
    
    Server(IOExecutor& executor_) : executor(executor_) {}
    
    void operator ()() {
        
        try {
            
            CORECAT_COROUTINE {
                
                server.listen({IPv4Address::getAny(), 12345});
                CORECAT_AWAIT(server.acceptAsync(socket));
                (std::cout << "Server write: ").write(data, size) << std::endl;
                CORECAT_AWAIT(socket.writeAllAsync(&size, 1));
                CORECAT_AWAIT(socket.writeAllAsync(data, size));
                
            }
            
        } catch(std::exception& e) { std::cerr << e.what() << std::endl; }
        
    }
    
};

class Client : public Coroutine<Client> {
    
private:
    
    IOExecutor& executor;
    TCPSocket socket{executor};
    char data[256];
    std::uint8_t size;
    
public:
    
    Client(IOExecutor& executor_) : executor(executor_) {}
    
    void operator ()() {
        
        try {
            
            CORECAT_COROUTINE {
                
                CORECAT_AWAIT(socket.connectAsync({IPv4Address::getLoopback(), 12345}));
                CORECAT_AWAIT(socket.readAllAsync(&size, 1));
                CORECAT_AWAIT(socket.readAllAsync(data, size));
                (std::cout << "Client read: ").write(data, size) << std::endl;
                
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
