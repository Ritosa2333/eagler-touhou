#include "MessageHost.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <utility>

namespace th128::message {
namespace {
std::uint32_t word(std::span<const std::uint8_t> bytes,std::size_t offset){return std::uint32_t(bytes[offset])|std::uint32_t(bytes[offset+1])<<8|std::uint32_t(bytes[offset+2])<<16|std::uint32_t(bytes[offset+3])<<24;}
struct Guard {bool& value;explicit Guard(bool& state):value(state){value=true;}~Guard(){value=false;}};
bool integer(int opcode){return opcode==1||opcode==2||opcode==9||opcode==10||opcode==11||opcode==13||opcode==14||opcode==23||opcode==25||opcode==29;}
}
bool Host::fail(std::string reason,std::string& error){if(failure_.empty())failure_=std::move(reason);error=failure_;return false;}
bool Host::load(std::span<const std::uint8_t> bytes,std::string& error){
    error.clear();
    if(executing_){return fail("message load reentered",error);}
    if(bytes.size()<12||bytes.size()>1048576){error="invalid original stage MSG span";return false;}
    const auto count=word(bytes,0);if(count<1||count>256||4+std::size_t(count)*8>bytes.size()){error="invalid original MSG entry table";return false;}
    std::vector<std::size_t> offsets;
    for(std::size_t index=0;index<count;++index){const auto offset=word(bytes,4+index*8);if(offset<4+count*8||offset>=bytes.size()){error="invalid original MSG entry offset";return false;}offsets.push_back(offset);}
    auto sorted=offsets;std::sort(sorted.begin(),sorted.end());if(std::adjacent_find(sorted.begin(),sorted.end())!=sorted.end()){error="aliased MSG entries";return false;}
    sorted.push_back(bytes.size());std::vector<std::vector<Command>> candidate;std::size_t total=0;
    bool routeChoicePresent=false;
    for(const auto start:offsets){
        const auto limit=*std::upper_bound(sorted.begin(),sorted.end(),start);std::vector<Command> commands;bool ended=false;
        for(std::size_t offset=start;offset<limit;){
            if(limit-offset<4){error="truncated MSG command header";return false;}
            const auto opcode=bytes[offset+2],length=bytes[offset+3];const auto time=std::uint16_t(bytes[offset])|std::uint16_t(bytes[offset+1])<<8;
            if(length>limit-offset-4||++total>16384||opcode>30){error="invalid original MSG command bound";return false;}
            const auto payload=bytes.subspan(offset+4,length);
            if((integer(opcode)&&length!=4)||(opcode==27&&length!=4)||(opcode==28&&length!=8)
                ||(!integer(opcode)&&opcode!=17&&opcode!=27&&opcode!=28&&length!=0)){error="invalid original MSG command shape";return false;}
            if(opcode==17){
                std::uint8_t key=0x77,step=7;bool terminated=false;
                for(const auto value:payload){terminated|=(value^key)==0;key=std::uint8_t(key+step);step=std::uint8_t(step+16);}
                if(length==0||!terminated){error="unterminated original MSG text";return false;}
            }
            if(opcode==27||opcode==28)for(std::size_t index=0;index<length;index+=4){const auto value=std::bit_cast<float>(word(payload,index));if(!std::isfinite(value)||std::abs(value)>65536){error="invalid original MSG float";return false;}}
            routeChoicePresent|=opcode==30;
            commands.push_back({std::uint16_t(time),opcode,{payload.begin(),payload.end()}});offset+=4+length;
            if(opcode==0){ended=true;if(std::any_of(bytes.begin()+std::ptrdiff_t(offset),bytes.begin()+std::ptrdiff_t(limit),[](auto value){return value!=0;})){error="nonzero trailing MSG bytes";return false;}break;}
        }
        if(!ended){error="original MSG entry has no terminator";return false;}
        candidate.push_back(std::move(commands));
    }
    entries_=std::move(candidate);routeChoicePresent_=routeChoicePresent;
    state_={};cursor_=0;externalHeld_=false;failure_.clear();return true;
}
bool Host::setEventHost(EventHost host,std::string& error){
    error.clear();if(executing_)return fail("message event replacement reentered",error);
    if(state_.active||event_||!host){error="message event owner must be configured once before execution";return false;}event_=std::move(host);return true;
}
bool Host::setExternalWait(ExternalWait wait,std::string& error){
    error.clear();if(executing_)return fail("message external wait replacement reentered",error);
    if(!failure_.empty())return fail(failure_,error);
    if(state_.active||externalWait_.begin||externalWait_.poll||!wait.begin||!wait.poll){error="message external wait owner must be configured once before execution";return false;}
    externalWait_=std::move(wait);return true;
}
bool Host::deliver(const Event& event,std::string& error){
    if(!event_)return fail("required original message event owner is unavailable",error);
    try{if(!event_(event,error)||!error.empty()||!failure_.empty())return fail(error.empty()?"original message event rejected":error,error);}
    catch(...){return fail("original message event owner threw",error);}return true;
}
bool Host::start(int entry,std::string& error){
    error.clear();if(executing_)return fail("message start reentered",error);
    if(!failure_.empty()||entry<0||std::size_t(entry)>=entries_.size()||state_.active)return fail(failure_.empty()?"invalid or overlapping original message entry":failure_,error);
    Guard guard(executing_);Event event;event.opcode=-1;event.entry=entry;
    if(!deliver(event,error))return false;
    state_={};state_.entry=entry;state_.active=true;state_.eclReleased=false;cursor_=0;externalHeld_=false;return true;
}
bool Host::tick(Input input,std::string& error){
    error.clear();if(executing_)return fail("message tick reentered",error);
    if(!failure_.empty())return fail(failure_,error);if(!state_.active)return true;
    Guard guard(executing_);state_.eclReleased=false;
    if(externalHeld_){
        std::optional<bool> held;
        try{held=externalWait_.poll(error);}catch(...){return fail("original external message wait owner threw",error);}
        if(!held||!error.empty())return fail(error.empty()?"original external message wait owner failed":error,error);
        if(*held)return true;
        externalHeld_=false;
    }
    state_.heldShoot=input.holding&1u?std::min(state_.heldShoot+1,100000):0;
    state_.heldRapid=input.holding&512u?std::min(state_.heldRapid+1,100000):0;
    if(input.pressed&513u)state_.pressSeen=true;
    const auto& commands=entries_[std::size_t(state_.entry)];
    if(state_.skipEnabled&&state_.pressSeen&&(state_.heldShoot>=20||state_.heldRapid>=20)&&cursor_<commands.size())state_.clock=commands[cursor_].time;
    for(unsigned budget=0;budget<4096;++budget){
        if(cursor_>=commands.size())return fail("original MSG fell outside entry",error);
        const auto& command=commands[cursor_];if(command.time>state_.clock)break;
        if(command.opcode==11){
            if(!state_.waiting){state_.waitRemaining=std::bit_cast<std::int32_t>(word(command.bytes,0));if(state_.waitRemaining<0||state_.waitRemaining>65535)return fail("unsupported original MSG wait duration",error);state_.waiting=true;}
            --state_.waitRemaining;
            const bool held=state_.skipEnabled&&state_.pressSeen&&(state_.heldShoot>=20||state_.heldRapid>=20);
            if(!(input.pressed&0x80201u)&&state_.waitRemaining>0&&!held)return true;
            Event event;event.opcode=11;event.entry=state_.entry;event.time=state_.clock;event.integers[0]=state_.waitRemaining;
            if(!deliver(event,error))return false;state_.waiting=false;state_.waitRemaining=0;state_.textLines.clear();++cursor_;continue;
        }
        Event event;event.opcode=command.opcode;event.entry=state_.entry;event.time=state_.clock;event.bytes=command.bytes;
        if(integer(command.opcode))event.integers[0]=std::bit_cast<std::int32_t>(word(command.bytes,0));
        if(command.opcode==27||command.opcode==28)for(std::size_t index=0;index<command.bytes.size()/4;++index)event.floats[index]=std::bit_cast<float>(word(command.bytes,index*4));
        if(command.opcode==17){std::uint8_t key=0x77,step=7;event.bytes.clear();for(const auto value:command.bytes){const auto decoded=std::uint8_t(value^key);key=std::uint8_t(key+step);step=std::uint8_t(step+16);if(decoded==0)break;event.bytes.push_back(decoded);}}
        if(!deliver(event,error))return false;
        if(externalWait_.begin){
            bool hold=false;
            try{hold=externalWait_.begin(event,error);}catch(...){return fail("original external message wait begin threw",error);}
            if(!error.empty())return fail(error,error);
            if(hold){externalHeld_=true;++cursor_;state_.eclReleased=false;return true;}
        }
        if(command.opcode==0){state_.active=false;state_.eclReleased=true;return true;}
        if(command.opcode==10)state_.skipEnabled=(event.integers[0]&1)!=0;
        if(command.opcode==12)state_.eclReleased=true;
        if(command.opcode==7||command.opcode==8){state_.speaker=command.opcode==7?0:1;state_.textLines.clear();}
        if(command.opcode==17){if(state_.textLines.size()>=8)return fail("original dialogue line budget exceeded",error);state_.textLines.push_back(event.bytes);}
        if(command.opcode==28){state_.x=event.floats[0];state_.y=event.floats[1];}
        ++cursor_;
    }
    if(state_.clock>=1000000)return fail("original message clock budget exceeded",error);
    ++state_.clock;return true;
}
}
