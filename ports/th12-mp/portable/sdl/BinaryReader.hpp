#pragma once
#include "Platform.hpp"
#include <cstring>
#include <stdexcept>
namespace th12host {
// Bounded, little-endian platform metadata reader. Payloads are supplied by
// the content package, never embedded in the distributed runtime.
class BinaryReader {
    const std::vector<u8>& data;size_t at=0;
public:
    explicit BinaryReader(const std::vector<u8>& bytes):data(bytes){}
    void magic(const char* text,size_t length){if(at>data.size()||length>data.size()-at||std::memcmp(data.data()+at,text,length))throw std::runtime_error("Invalid platform resource header");at+=length;}
    u32 word(){if(at>data.size()||4>data.size()-at)throw std::runtime_error("Truncated platform resource");const auto* p=data.data()+at;at+=4;return u32(p[0])|u32(p[1])<<8|u32(p[2])<<16|u32(p[3])<<24;}
    i32 integer(){return i32(word());}
    float real(){u32 v=word();float f;std::memcpy(&f,&v,4);return f;}
    std::string text(){const auto n=word();if(n>1024*1024||at>data.size()||n>data.size()-at)throw std::runtime_error("Invalid platform string length");std::string value(reinterpret_cast<const char*>(data.data()+at),n);at+=n;return value;}
    bool ended()const{return at==data.size();}
};
std::u32string decode_utf8(const std::string& text);
std::u32string translate_text(const std::u32string& text);
void load_translations();
void translation_context(const std::string& resource);
}
