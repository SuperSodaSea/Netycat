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

#ifndef CATS_NETYCAT_NETWORK_IP_IPADDRESS_HPP
#define CATS_NETYCAT_NETWORK_IP_IPADDRESS_HPP


#include <cstdint>
#include <cstring>

#include "Cats/Corecat/Text/Formatter.hpp"
#include "Cats/Corecat/Util/Exception.hpp"
#include "Cats/Corecat/Util/Operator.hpp"


namespace Cats {
namespace Netycat {
inline namespace Network {
inline namespace IP {

class IPv4Address : public Corecat::EqualityOperator<IPv4Address>, public Corecat::RelationalOperator<IPv4Address> {
    
private:
    
    using String8 = Corecat::String8;
    
private:
    
    std::uint8_t data[4];
    
public:
    
    IPv4Address() = default;
    IPv4Address(const std::uint8_t* data_) noexcept : data{data_[0], data_[1], data_[2], data_[3]} {}
    IPv4Address(std::uint8_t d0, std::uint8_t d1, std::uint8_t d2, std::uint8_t d3) noexcept : data{d0, d1, d2, d3} {}
    IPv4Address(std::uint32_t data_) noexcept :
        data{static_cast<std::uint8_t>(data_ >> 24), static_cast<std::uint8_t>(data_ >> 16),
            static_cast<std::uint8_t>(data_ >> 8), static_cast<std::uint8_t>(data_)} {}
    IPv4Address(const IPv4Address& src) = default;
    
    IPv4Address& operator =(const IPv4Address& src) = default;
    
    friend bool operator ==(const IPv4Address& a, const IPv4Address& b) noexcept { return !std::memcmp(a.data, b.data, 4); }
    friend bool operator <(const IPv4Address& a, const IPv4Address& b) noexcept { return std::memcmp(a.data, b.data, 4) < 0; }
    
    operator std::uint32_t() const { return data[0] << 24 | (data[1] << 16) | (data[2] << 8) | data[3]; }
    
    const std::uint8_t* getData() const noexcept { return data; }
    std::uint8_t* getData() noexcept { return data; }
    void setData(const std::uint8_t* data_) noexcept { std::memcpy(data, data_, 4); }
    void setData(std::uint8_t d0, std::uint8_t d1, std::uint8_t d2, std::uint8_t d3) noexcept { data[0] = d0, data[1] = d1, data[2] = d2, data[3] = d3; }
    
    String8 toString() const {
        
        using namespace Corecat;
        return "{}.{}.{}.{}"_format(data[0], data[1], data[2], data[3]);
        
    }
    
public:
    
    static IPv4Address getAny() noexcept { return {0, 0, 0, 0}; }
    static IPv4Address getLoopback() noexcept { return {127, 0, 0, 1}; }
    
};

class IPv6Address : public Corecat::EqualityOperator<IPv6Address>, public Corecat::RelationalOperator<IPv6Address> {
    
private:
    
    using String8 = Corecat::String8;
    
private:
    
    std::uint8_t data[16];
    std::uint32_t scope;
    
public:
    
    IPv6Address() = default;
    IPv6Address(const std::uint8_t* data_, std::uint32_t scope_ = 0) noexcept :
        data{data_[0], data_[1], data_[2], data_[3], data_[4], data_[5], data_[6], data_[7],
            data_[8], data_[9], data_[10], data_[11], data_[12], data_[13], data_[14], data_[15]}, scope(scope_) {}
    IPv6Address(std::uint8_t d0, std::uint8_t d1, std::uint8_t d2, std::uint8_t d3,
        std::uint8_t d4, std::uint8_t d5, std::uint8_t d6, std::uint8_t d7,
        std::uint8_t d8, std::uint8_t d9, std::uint8_t d10, std::uint8_t d11,
        std::uint8_t d12, std::uint8_t d13, std::uint8_t d14, std::uint8_t d15, std::uint32_t scope_ = 0) noexcept :
        data{d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15}, scope(scope_) {}
    IPv6Address(const IPv6Address& src) = default;
    
    IPv6Address& operator =(const IPv6Address& src) = default;
    
    friend bool operator ==(const IPv6Address& a, const IPv6Address& b) noexcept { return !std::memcmp(a.data, b.data, 16) && a.scope == b.scope; }
    friend bool operator <(const IPv6Address& a, const IPv6Address& b) noexcept {
        
        int x = std::memcmp(a.data, b.data, 16);
        return x ? x < 0 : a.scope < b.scope;
        
    }
    
    const std::uint8_t* getData() const noexcept { return data; }
    std::uint8_t* getData() noexcept { return data; }
    void setData(const std::uint8_t* data_) noexcept { std::memcpy(data, data_, 16); }
    void setData(std::uint8_t d0, std::uint8_t d1, std::uint8_t d2, std::uint8_t d3,
        std::uint8_t d4, std::uint8_t d5, std::uint8_t d6, std::uint8_t d7,
        std::uint8_t d8, std::uint8_t d9, std::uint8_t d10, std::uint8_t d11,
        std::uint8_t d12, std::uint8_t d13, std::uint8_t d14, std::uint8_t d15) noexcept {
        
        data[0] = d0, data[1] = d1, data[2] = d2, data[3] = d3, data[4] = d4, data[5] = d5, data[6] = d6, data[7] = d7;
        data[8] = d8, data[9] = d9, data[10] = d10, data[11] = d11, data[12] = d12, data[13] = d13, data[14] = d14, data[15] = d15;
        
    }
    
    const std::uint32_t& getScope() const noexcept { return scope; }
    std::uint32_t& getScope() noexcept { return scope; }
    void setScope(std::uint32_t scope_) noexcept { scope = scope_; }
    
    String8 toString() const {
        
        using namespace Corecat;
        return "{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}%{}"_format(
            (std::uint16_t(data[0]) << 8) | data[1], (std::uint16_t(data[2]) << 8) | data[3],
            (std::uint16_t(data[4]) << 8) | data[5], (std::uint16_t(data[6]) << 8) | data[7],
            (std::uint16_t(data[8]) << 8) | data[9], (std::uint16_t(data[10]) << 8) | data[11],
            (std::uint16_t(data[12]) << 8) | data[13], (std::uint16_t(data[14]) << 8) | data[15],
            scope);
        
    }
    
public:
    
    static IPv6Address getAny() noexcept { return {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; }
    static IPv6Address getLoopback() noexcept { return {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}; }
    
};

class IPAddress : public Corecat::EqualityOperator<IPAddress>, public Corecat::RelationalOperator<IPAddress> {
    
private:
    
    using String8 = Corecat::String8;
    
public:
    
    enum class Type {IPv4, IPv6};
    
private:
    
    Type type;
    union {
        
        IPv4Address v4;
        IPv6Address v6;
        
    };
    
public:
    
    IPAddress() = default;
    IPAddress(const IPv4Address& src) noexcept : type(Type::IPv4), v4(src) {}
    IPAddress(const IPv6Address& src) noexcept : type(Type::IPv6), v6(src) {}
    IPAddress(const IPAddress& src) = default;
    
    IPAddress& operator =(const IPv4Address& src) noexcept { type = Type::IPv4, v4 = src; return *this; }
    IPAddress& operator =(const IPv6Address& src) noexcept { type = Type::IPv6, v6 = src; return *this; }
    IPAddress& operator =(const IPAddress& src) = default;
    
    friend bool operator ==(const IPAddress& a, const IPAddress& b) noexcept {
        
        if(a.type != b.type) return false;
        switch(a.type) {
        case Type::IPv4: return a.v4 == b.v4;
        case Type::IPv6: return a.v6 == b.v6;
        default: return false;
        }
        
    }
    friend bool operator <(const IPAddress& a, const IPAddress& b) noexcept {
        
        if(a.type != b.type) return a.type < b.type;
        switch(a.type) {
        case Type::IPv4: return a.v4 < b.v4;
        case Type::IPv6: return a.v6 < b.v6;
        default: return false;
        }
        
    }
    
    Type getType() const noexcept { return type; }
    
    bool isIPv4() const noexcept { return type == Type::IPv4; }
    bool isIPv6() const noexcept { return type == Type::IPv6; }
    
    const IPv4Address& getIPv4() const { if(type != Type::IPv4) throw Corecat::InvalidArgumentException("Address is not IPv4"); return v4; }
    IPv4Address& getIPv4() { if(type != Type::IPv4) throw Corecat::InvalidArgumentException("Address is not IPv4"); return v4; }
    const IPv6Address& getIPv6() const { if(type != Type::IPv6) throw Corecat::InvalidArgumentException("Address is not IPv6"); return v6; }
    IPv6Address& getIPv6() { if(type != Type::IPv6) throw Corecat::InvalidArgumentException("Address is not IPv6"); return v6; }
    
    String8 toString() const {
        
        switch(type) {
        case Type::IPv4: return v4.toString();
        case Type::IPv6: return v6.toString();
        default: return "<Unknown>";
        }
        
    }
    
};

}
}
}
}


#endif
