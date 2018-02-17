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

#include "Cats/Netycat/Network/Win32/WSA.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace Win32 {

void WSA::getExtensionFunction(SOCKET socket, GUID guid, void** p) {
    
    DWORD byteCount;
    if(WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), p, sizeof(*p), &byteCount, nullptr, nullptr))
        *p = nullptr;
    
}
LPFN_ACCEPTEX WSA::AcceptEx = nullptr;
LPFN_CONNECTEX WSA::ConnectEx = nullptr;

WSA::WSA() {
    
    ::WSAStartup(MAKEWORD(2, 2), &wsaData);
    
    SOCKET socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socket) {
        
        getExtensionFunction(socket, WSAID_ACCEPTEX, reinterpret_cast<void**>(&AcceptEx));
        getExtensionFunction(socket, WSAID_CONNECTEX, reinterpret_cast<void**>(&ConnectEx));
        ::closesocket(socket);
        
    }
    
}
WSA::~WSA() { ::WSACleanup(); }

void WSA::init() { static WSA wsa; }
const WSADATA& WSA::getData() { return wsaData; }

}
}
}
}
