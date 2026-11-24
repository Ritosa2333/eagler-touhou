#pragma once
#include <charconv>
#include <concepts>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace th128::runtime {
class DrawJsonWriter {
public:
    DrawJsonWriter(){body_.reserve(65536);}
    DrawJsonWriter& operator<<(char value){body_.push_back(value);return *this;}
    DrawJsonWriter& operator<<(const char* value){body_.append(value);return *this;}
    DrawJsonWriter& operator<<(std::string_view value){body_.append(value);return *this;}
    DrawJsonWriter& operator<<(const std::string& value){body_.append(value);return *this;}
    DrawJsonWriter& operator<<(bool value){body_.push_back(value?'1':'0');return *this;}
    template<std::integral Number> DrawJsonWriter& operator<<(Number value){
        char buffer[32];
        const auto result=std::to_chars(buffer,buffer+sizeof(buffer),value);
        if(result.ec!=std::errc{})throw std::runtime_error("Draw JSON integer conversion failed");
        body_.append(buffer,result.ptr);return *this;
    }
    template<std::floating_point Number> DrawJsonWriter& operator<<(Number value){
        char buffer[64];
        const auto result=std::to_chars(buffer,buffer+sizeof(buffer),double(value),std::chars_format::general,9);
        if(result.ec!=std::errc{})throw std::runtime_error("Draw JSON scalar conversion failed");
        body_.append(buffer,result.ptr);return *this;
    }
    const std::string& str()const noexcept{return body_;}
private:
    std::string body_;
};
}
