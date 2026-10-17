#include "trophy.hpp"
#include <algorithm>
#include <sstream>
#include <string>
#include <stdexcept>
namespace th20::source::trophy {
namespace {char decoded[256];}
void encode_string(std::uint8_t* output,const char* input){
    std::uint8_t key=0x77,step=7,value;
    do{value=static_cast<std::uint8_t>(*input++);*output++=value^key;key+=step;step+=0x10;}while(value);
}
const char* decode_string(const std::uint8_t* input){
    std::uint8_t key=0x77,step=7,value;unsigned i=0;
    do{value=*input++^key;decoded[i++]=static_cast<char>(value);key+=step;step+=0x10;}while(value);
    return decoded;
}
void parse_messages(std::span<Message,128> out,std::string_view input){
    // The original constructs std::string from the NUL-terminated file buffer,
    // erases CR and form-feed, then reads LF-separated lines.
    std::string contents(input.substr(0,input.find('\0')));
    contents.erase(std::remove(contents.begin(),contents.end(),'\r'),contents.end());
    contents.erase(std::remove(contents.begin(),contents.end(),'\f'),contents.end());
    std::istringstream stream(contents);std::string line;
    auto read=[&](){if(!std::getline(stream,line))throw std::runtime_error("Truncated trophy.txt record");};
    auto encode=[&](std::uint8_t* destination){if(line.size()>=256)throw std::length_error("trophy.txt line exceeds original256 byte storage");encode_string(destination,line.c_str());};
    while(std::getline(stream,line)){
        if(line.empty()||line[0]=='#')continue;if(line[0]=='\\')break;if(line[0]!='@')continue;
        const auto id=std::stoi(line.substr(1),nullptr,10);if(id<0||id>=128)throw std::out_of_range("trophy.txt index outside128 records");
        auto& message=out[id];message.id=-1;message.id=id;read();encode(message.title);
        for(auto& variant:message.description)for(auto& row:variant){do{read();}while(line.empty());encode(row);}
    }
}
}
