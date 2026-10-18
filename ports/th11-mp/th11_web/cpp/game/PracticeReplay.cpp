// USER/PRAC serialization follows TH08/TH10 and THPracParam for TH11.
#include "PracticeConfig.hpp"
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
namespace th11 {
namespace {
u32 load32(const u8* p){u32 n;std::memcpy(&n,p,4);return n;}
void store32(u8* p,u32 n){std::memcpy(p,&n,4);}
double json_number(const std::string& json,const char* key,double fallback){
    const std::string needle=std::string("\"")+key+"\"";
    size_t position=json.find(needle);
    if(position==std::string::npos||(position=json.find(':',position+needle.size()))==std::string::npos)return fallback;
    const char* begin=json.c_str()+position+1;char* end=nullptr;
    const double value=std::strtod(begin,&end);
    return end==begin?fallback:value;
}
bool json_bool(const std::string& json,const char* key,bool fallback){
    const std::string needle=std::string("\"")+key+"\"";
    size_t position=json.find(needle);
    if(position==std::string::npos||(position=json.find(':',position+needle.size()))==std::string::npos)return fallback;
    position=json.find_first_not_of(" \t\r\n",position+1);
    if(position==std::string::npos)return fallback;
    if(json.compare(position,4,"true")==0)return true;
    if(json.compare(position,5,"false")==0)return false;
    return fallback;
}
}

std::string practice_replay_json(const PracticeConfig& p){
 if(!p.valid())return {};char buffer[1024];int n=std::snprintf(buffer,sizeof(buffer),"{\"version\":\"2.3.0.3\",\"game\":\"th11\",\"mode\":%d,\"stage\":%d",p.mode,p.stage);
 auto append=[&](const char* fmt,auto... args){const int w=std::snprintf(buffer+n,sizeof(buffer)-n,fmt,args...);if(w<0||w>=int(sizeof(buffer))-n)return false;n+=w;return true;};
 if(p.section&&!append(",\"section\":%d",p.section))return {};
 if(p.phase&&!append(",\"phase\":%d",p.phase))return {};
 if(p.dlg&&!append("%s",",\"dlg\":true"))return {};
 if(!append(",\"life\":%d,\"life_fragment\":%d,\"power\":%d,\"graze\":%d,\"signal\":%d,\"value\":%d,\"score\":%lld,\"marisa_b_formation\":%d}",p.life,p.life_fragment,p.power,p.graze,p.signal,p.value,static_cast<long long>(p.score),p.marisa_b_formation))return {};
 return std::string(buffer,n);
}
bool practice_replay_parse(const char* json,u32 size,PracticeConfig& out){
 out.reset();if(!json||!size||size>4096)return false;const std::string text(json,size);
 if(text.find("\"version\":")==std::string::npos||text.find("\"game\":\"th11\"")==std::string::npos)return false;
 PracticeConfig p;p.reset();bool valid=true;
 auto number=[&](const char* key,i32& field){const double v=json_number(text,key,0);if(!std::isfinite(v)||std::trunc(v)!=v||v<0||v>2147483647.){valid=false;return;}field=i32(v);};
 number("mode",p.mode);number("stage",p.stage);number("section",p.section);number("phase",p.phase);number("life",p.life);number("life_fragment",p.life_fragment);number("power",p.power);number("graze",p.graze);number("signal",p.signal);number("value",p.value);number("marisa_b_formation",p.marisa_b_formation);
 const double score=json_number(text,"score",0);if(!std::isfinite(score)||std::trunc(score)!=score||score<0||score>9999999990.)return false;p.score=i64(score);p.dlg=json_bool(text,"dlg",false);
 if(!valid||!p.valid())return false;out=p;return true;
}
std::vector<u8> practice_replay_block(const PracticeConfig& p){
    const auto json=practice_replay_json(p);
    if(json.empty())return {};
    // Upstream ReplaySaveParam (the non-T6RP/T7RP branch): 'USER', total size
    // aligned to 4, 'PRAC', then the NUL-padded JSON payload.
    u32 paramSize=u32(json.size())+12;
    for(paramSize++;paramSize&3u;paramSize++);
    std::vector<u8> block(paramSize,0);
    std::memcpy(block.data(),"USER",4);
    store32(block.data()+4,paramSize);
    std::memcpy(block.data()+8,"PRAC",4);
    std::memcpy(block.data()+12,json.data(),json.size());
    return block;
}
bool practice_replay_read(const u8* data,u32 size,PracticeConfig& out){
    out=PracticeConfig{};
    // TH11 ReplayHeader::signature and version (Replay.hpp / ReplayFile.cpp).
    if(!data||size<0x24||load32(data)!=0x72313174u)return false;
    const u32 version=data[4]|(u32(data[5])<<8);
    if((version&0xfff)!=4)return false;
    // The encoded header's user_offset (0x0c) is where the plain USER block
    // area starts; blocks are walked exactly like upstream ReplayLoadParam.
    const u32 packed=load32(data+0x1c);if(packed>size-0x24)return false;
    u32 position=0x24+packed;
    if(position<0x24||position>size)return false;
    while(position+12<=size){
        if(std::memcmp(data+position,"USER",4)!=0)break;
        const u32 length=load32(data+position+4),number=load32(data+position+8);
        if(length<12||length>size-position)break;
        if(number==(u32(u8('P'))|(u32(u8('R'))<<8)|(u32(u8('A'))<<16)|(u32(u8('C'))<<24))){
            const char* json=reinterpret_cast<const char*>(data+position+12);
            u32 n=0;
            while(n<length-12&&json[n])++n;
            return n&&practice_replay_parse(json,n,out);
        }
        position+=length;
    }
    return false;
}

}
