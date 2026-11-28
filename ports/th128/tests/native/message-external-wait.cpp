#include "game/MessageHost.hpp"
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace {
void word(std::vector<std::uint8_t>& bytes,std::uint32_t value){
    bytes.push_back(std::uint8_t(value));bytes.push_back(std::uint8_t(value>>8));
    bytes.push_back(std::uint8_t(value>>16));bytes.push_back(std::uint8_t(value>>24));
}
std::vector<std::uint8_t> messageBytes(){
    std::vector<std::uint8_t> bytes;
    word(bytes,1);
    word(bytes,12);
    word(bytes,0);
    word(bytes,0x001e0000u);
    word(bytes,0x00150001u);
    word(bytes,0x00160001u);
    word(bytes,0x00000003u);
    return bytes;
}
bool check(bool value,const char* message){
    if(!value)std::cerr<<message<<'\n';
    return value;
}
struct Trace {
    std::vector<int> opcodes;
    std::vector<int> times;
};
bool configure(th128::message::Host& host,Trace& trace,std::string& error){
    if(!host.load(messageBytes(),error))return false;
    return host.setEventHost([&trace](const th128::message::Event& event,std::string&){
        if(event.opcode>=0){trace.opcodes.push_back(event.opcode);trace.times.push_back(event.time);}return true;
    },error);
}
bool runDefault(){
    th128::message::Host host;Trace trace;std::string error;
    if(!check(configure(host,trace,error),error.c_str()))return false;
    if(!check(host.start(0,error),error.c_str()))return false;
    for(unsigned tick=0;tick<16&&host.state().active;++tick)
        if(!check(host.tick({},error),error.c_str()))return false;
    return check(!host.state().active,"default message did not finish")
        &&check(trace.opcodes==std::vector<int>({30,21,22,0}),"default events changed")
        &&check(host.state().eclReleased,"default message did not release ECL");
}
bool runHeld(){
    th128::message::Host host;Trace trace;std::string error;
    if(!check(configure(host,trace,error),error.c_str()))return false;
    bool held=true;unsigned beginThirty=0,polls=0;
    if(!check(host.setExternalWait({
        [&](const th128::message::Event& event,std::string&){if(event.opcode==30)++beginThirty;return event.opcode==30;},
        [&](std::string&)->std::optional<bool>{++polls;return held;}
    },error),error.c_str()))return false;
    if(!check(host.start(0,error),error.c_str()))return false;
    if(!check(host.tick({},error),error.c_str()))return false;
    if(!check(trace.opcodes==std::vector<int>({30}),"external wait did not stop after MSG30"))return false;
    if(!check(host.state().active&&!host.state().eclReleased&&host.state().clock==0,"held MSG clock/gate changed"))return false;
    for(unsigned tick=0;tick<5;++tick)
        if(!check(host.tick({},error),error.c_str()))return false;
    if(!check(trace.opcodes.size()==1&&host.state().clock==0,"held ticks repeated messages or advanced clock"))return false;
    held=false;
    if(!check(host.tick({},error),error.c_str()))return false;
    if(!check(trace.opcodes.size()==1&&host.state().clock==1,"release tick did not preserve original time"))return false;
    for(unsigned tick=0;tick<4&&host.state().active;++tick)
        if(!check(host.tick({},error),error.c_str()))return false;
    return check(trace.opcodes==std::vector<int>({30,21,22,0}),"released message order changed")
        &&check(trace.times==std::vector<int>({0,1,1,3}),"released message times changed")
        &&check(beginThirty==1,"MSG30 external wait began more than once")
        &&check(polls>=6,"external wait poll was not used")
        &&check(host.state().eclReleased,"released message did not release ECL");
}
bool runPollFailure(){
    th128::message::Host host;Trace trace;std::string error;
    if(!check(configure(host,trace,error),error.c_str()))return false;
    unsigned polls=0;
    if(!check(host.setExternalWait({
        [](const th128::message::Event& event,std::string&){return event.opcode==30;},
        [&](std::string& reason)->std::optional<bool>{++polls;reason="route choice owner failed";return std::nullopt;}
    },error),error.c_str()))return false;
    if(!check(host.start(0,error),error.c_str()))return false;
    if(!check(host.tick({},error),error.c_str()))return false;
    if(!check(!host.tick({},error),"external wait failure was hidden"))return false;
    const auto count=trace.opcodes.size();
    if(!check(!host.tick({},error),"external wait failure was not sticky"))return false;
    return check(trace.opcodes.size()==count&&polls==1,"failed external wait retried its owner");
}
bool runBeginFailure(){
    th128::message::Host host;Trace trace;std::string error;
    if(!check(configure(host,trace,error),error.c_str()))return false;
    unsigned begins=0;
    if(!check(host.setExternalWait({
        [&](const th128::message::Event& event,std::string& reason){
            if(event.opcode!=30)return false;
            ++begins;reason="route choice owner rejected MSG30";return false;
        },
        [](std::string&)->std::optional<bool>{return false;}
    },error),error.c_str()))return false;
    if(!check(host.start(0,error),error.c_str()))return false;
    if(!check(!host.tick({},error),"external wait begin failure was hidden"))return false;
    const auto count=trace.opcodes.size();
    if(!check(!host.tick({},error),"external wait begin failure was not sticky"))return false;
    return check(begins==1&&trace.opcodes.size()==count,"failed external wait begin retried its owner");
}
}
int main(){
    if(!runDefault())return 1;
    if(!runHeld())return 1;
    if(!runPollFailure())return 1;
    if(!runBeginFailure())return 1;
    std::cout<<"message external wait default compatibility, one-shot MSG30 and release timing passed\n";
    return 0;
}
