#pragma once
#include "PracticeTh14.hpp"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string_view>
#include <vector>

namespace th11 {
// The Purple PRAC block is a padded, flat JSON object (THPracParam::GetJson).
inline constexpr u32 purple_practice_user=0x43415250;
inline bool parse_purple_practice(std::string_view json,int entry_stage,int rank,int spell,PracticeTh14& out){
    if(json.size()>4096)return false;
    size_t at=0;
    const auto space=[&]{while(at<json.size()&&(json[at]==' '||json[at]=='\t'||json[at]=='\n'||json[at]=='\r'))++at;};
    const auto eat=[&](char value){space();if(at==json.size()||json[at]!=value)return false;++at;return true;};
    const auto quoted=[&]()->std::string_view{
        if(!eat('"'))return {};
        const size_t start=at;
        while(at<json.size()&&json[at]!='"'&&json[at]!='\\'&&u8(json[at])>=32)++at;
        if(at==json.size()||json[at]!='"')return {};
        const auto result=json.substr(start,at-start);++at;return result;
    };
    PracticeTh14 p{};int mode=0;bool game=false;
    if(!eat('{'))return false;
    for(;;){
        const auto key=quoted();
        if(key.empty()||!eat(':'))return false;
        space();
        if(at<json.size()&&json[at]=='"'){
            const auto value=quoted();
            if(value.empty())return false;
            if(key=="game")game=value=="th14";
        }else{
            const size_t start=at;
            while(at<json.size()&&json[at]!=','&&json[at]!='}'&&json[at]!=' '&&json[at]!='\n'&&json[at]!='\r'&&json[at]!='\t')++at;
            const auto value=json.substr(start,at-start);
            if(value.empty())return false;
            i64 n=0;const auto last=value.data()+value.size();
            const auto integer=std::from_chars(value.data(),last,n);
            if(key=="st6bs8_phase"){
                char tmp[32];
                if(value.size()>=sizeof tmp)return false;
                std::copy(value.begin(),value.end(),tmp);tmp[value.size()]=0;
                char* end=nullptr;const float f=std::strtof(tmp,&end);
                if(end!=tmp+value.size()||!std::isfinite(f)||std::abs(f)>1.f)return false;
                p.angle=i32(std::lround(f*100));
            }else if(key=="dlg"||key=="keepSpellDrops"){
                if(value!="true"&&value!="false")return false;
                if(value=="true")p.flags|=key=="dlg"?1:2;
            }else{
                if(integer.ec!=std::errc{}||integer.ptr!=last||n<0||n>9999999990LL)return false;
                if(key=="score")p.score=i32(n/10);
                else if(n>2147483647)return false;
                else if(key=="mode")mode=i32(n);
                else if(key=="stage")p.stage=i32(n);
                else if(key=="section")p.section=i32(n);
                else if(key=="phase")p.phase=i32(n);
                else if(key=="life")p.life=i32(n);
                else if(key=="life_fragment")p.life_fragment=i32(n);
                else if(key=="bomb")p.bomb=i32(n);
                else if(key=="bomb_fragment")p.bomb_fragment=i32(n);
                else if(key=="cycle")p.cycle=i32(n);
                else if(key=="power")p.power=i32(n);
                else if(key=="value")p.value=i32(n);
                else if(key=="graze")p.graze=i32(n);
                else if(key=="faketype"){if(n>2)return false;p.flags=(p.flags&3)|(i32(n)<<2);}
            }
        }
        if(eat('}'))break;
        if(!eat(','))return false;
    }
    space();if(at!=json.size()||!game||mode<1||mode>2)return false;
    if(mode==2){if(spell<0)return false;p.stage=entry_stage;}
    else{
        // GuiCombo exposes a zero-based stage index to Purple's JSON.
        if(p.stage<0||p.stage>6)return false;
        ++p.stage;
    }
    if(!p.valid(rank,spell))return false;
    out=p;return true;
}
inline void append_purple_practice_block(std::vector<u8>& out,const PracticeTh14& p,bool spell){
    if(!p)return;
    char buf[512];
    const int bytes=spell?std::snprintf(buf,sizeof buf,
        "{\"version\":\"2.2.2.7\",\"game\":\"th14\",\"mode\":2,\"phase\":%d}",p.phase):
        std::snprintf(buf,sizeof buf,"{\"version\":\"2.2.2.7\",\"game\":\"th14\",\"mode\":1,\"stage\":%d,"
            "\"section\":%d,\"phase\":%d,\"dlg\":%s,\"keepSpellDrops\":%s,\"score\":%lld,\"life\":%d,"
            "\"life_fragment\":%d,\"bomb\":%d,\"bomb_fragment\":%d,\"cycle\":%d,\"power\":%d,"
            "\"value\":%d,\"graze\":%d,\"faketype\":%d,\"st6bs8_phase\":%s%d.%02d}",
            p.stage-1,p.section,p.phase,p.flags&1?"true":"false",p.flags&2?"true":"false",static_cast<long long>(p.score)*10,
            p.life,p.life_fragment,p.bomb,p.bomb_fragment,p.cycle,p.power,p.value,p.graze,(p.flags>>2)&3,
            p.angle<0?"-":"",std::abs(p.angle)/100,std::abs(p.angle)%100);
    if(bytes<=0||size_t(bytes)>=sizeof buf)return;
    const u32 length=(u32(bytes)+16)&~3u;
    const auto put=[&](u32 n){for(int s=0;s<32;s+=8)out.push_back(u8(n>>s));};
    put(0x52455355);put(length);put(purple_practice_user);
    out.insert(out.end(),buf,buf+bytes);out.resize(out.size()+length-12-bytes,0);
}
}
