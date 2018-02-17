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


void runServer() {
    
    TCPServer server;
    server.listen({IPv4Address::getAny(), 12345});
    TCPSocket socket;
    server.accept(socket);
    const char data[] = "Hello, Netycat!";
    std::uint8_t size = sizeof(data) - 1;
    (std::cout << "Server write: ").write(data, size) << std::endl;
    socket.writeAll(&size, 1);
    socket.writeAll(data, size);
    
}

void runClient() {
    
    TCPSocket socket;
    socket.connect({IPv4Address::getLoopback(), 12345});
    std::uint8_t size;
    char buffer[256];
    socket.readAll(&size, 1);
    socket.readAll(buffer, size);
    (std::cout << "Client read: ").write(buffer, size) << std::endl;
    
}


int main() {
    
    try {
        
        std::thread serverThread(runServer);
        std::thread clientThread(runClient);
        serverThread.join();
        clientThread.join();
        
    } catch(std::exception& e) { std::cerr << e.what() << std::endl; return 1; }
    
    return 0;
    
}
