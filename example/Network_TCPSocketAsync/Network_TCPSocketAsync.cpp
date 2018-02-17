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

#include "Cats/Corecat/Util.hpp"
#include "Cats/Netycat/Network.hpp"


using namespace Cats::Corecat;
using namespace Cats::Netycat;


void runServer(IOExecutor& executor) {
    
    TCPEndpoint endpoint(IPv4Address::getAny(), 12345);
    auto server = std::make_shared<TCPServer>(executor);
    server->listen(endpoint);
    struct Session {
        
        TCPSocket socket;
        std::uint8_t size;
        char buffer[256];
        Session(IOExecutor& executor) : socket(executor) {}
        
    };
    auto session = std::make_shared<Session>(executor);
    server->acceptAsync(session->socket)
        .then([=] {
            
            const char data[] = "Hello, Netycat!";
            session->size = sizeof(data) - 1;
            std::memcpy(session->buffer, data, session->size);
            (std::cout << "Server write: ").write(data, session->size) << std::endl;
            return session->socket.writeAllAsync(&session->size, 1);
            
        }).then([=] {
            
            return session->socket.writeAllAsync(session->buffer, session->size);
            
        }).fail([server, session](auto e) {
            
            e.with([](const std::exception& e) { std::cerr << e.what() << std::endl; });
            
        });
    
}

void runClient(IOExecutor& executor) {
    
    TCPEndpoint endpoint(IPv4Address::getLoopback(), 12345);
    struct Session {
        
        TCPSocket socket;
        std::uint8_t size;
        char buffer[256];
        Session(IOExecutor& executor) : socket(executor) {}
        
    };
    auto session = std::make_shared<Session>(executor);
    session->socket.connectAsync(endpoint)
        .then([=] {
            
            return session->socket.readAllAsync(&session->size, 1);
            
        }).then([=] {
            
            return session->socket.readAllAsync(session->buffer, session->size);
            
        }).then([=] {
            
            (std::cout << "Client read: ").write(session->buffer, session->size) << std::endl;
            
        }).fail([session](auto e) {
            
            e.with([](const std::exception& e) { std::cerr << e.what() << std::endl; });
            
        });
    
}


int main() {
    
    try {
        
        IOExecutor executor;
        runServer(executor);
        runClient(executor);
        executor.run();
        
    } catch(std::exception& e) { std::cerr << e.what() << std::endl; return 1; }
    
    return 0;
    
}
