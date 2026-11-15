#pragma once
#include "Types.hpp"
#include <algorithm>
#include <array>
#include <tuple>
#include <iterator>
#include <cstring>
#include <vector>

namespace th11 {
// Record format and correction rules from thprac_purple THMarisaLaser (MIT).
struct PurpleLaserRecordTh14 {
    u32 stage,frame,ordinal,value,fix;
    auto key()const noexcept{return std::tuple{stage,frame,ordinal};}
    bool operator<(const PurpleLaserRecordTh14& other)const noexcept{return key()<other.key();}
};
inline constexpr u32 purple_laser_user=0x52434552; // RCER, in native little-endian files.
inline constexpr size_t purple_laser_limit=200000;
inline u32 purple_laser_word(const u8* p){return u32(p[0])|u32(p[1])<<8|u32(p[2])<<16|u32(p[3])<<24;}
inline void append_purple_laser_block(std::vector<u8>& output,const std::vector<PurpleLaserRecordTh14>& records){
    if(records.empty()||records.size()>purple_laser_limit||output.size()<36)return;
    const auto put=[&](u32 n){for(int s=0;s<32;s+=8)output.push_back(u8(n>>s));};
    // Recovery replaces the old RCER block, leaving every other USER block intact.
    for(size_t pos=36u+size_t(purple_laser_word(output.data()+0x1c));pos<=output.size()&&output.size()-pos>=12;){
        const auto* p=output.data()+pos;
        const auto len=purple_laser_word(p+4);
        if(purple_laser_word(p)!=0x52455355||len<12||len>output.size()-pos)break;
        if(purple_laser_word(p+8)==purple_laser_user)output.erase(output.begin()+pos,output.begin()+pos+len);
        else pos+=len;
    }
    put(0x52455355);put(u32(12+records.size()*20));put(purple_laser_user);
    for(const auto& r:records){put(r.stage);put(r.frame);put(r.ordinal);put(r.value);put(r.fix);}
}
struct PurpleLaserTh14 {
    int mode=0;
    std::vector<PurpleLaserRecordTh14> recorded,playback,recovered;
    std::vector<u8> source;
    u32 stage=0,frame=0,ordinal=0;
    bool reading=false,marisa=false,force_zero=false;
    void tick(u32 st,u32 f,bool replay,bool marisa_shot,bool fix)noexcept{
        if(st!=stage||f!=frame){stage=st;frame=f;ordinal=0;}
        reading=replay;marisa=marisa_shot;force_zero=fix;
    }
    void merge_playback(){
        std::vector<PurpleLaserRecordTh14> merged;
        merged.reserve(std::min(purple_laser_limit,recovered.size()+playback.size()));
        std::set_union(recovered.begin(),recovered.end(),playback.begin(),playback.end(),std::back_inserter(merged));
        if(merged.size()>purple_laser_limit)merged.resize(purple_laser_limit);
        recovered=std::move(merged);
    }
    static u32 corrected(const PurpleLaserRecordTh14& record,u32 original)noexcept{
        const u32 fix=record.fix&0xffff, exponent=record.fix>>16;
        if(!fix)return record.value;
        if(fix==1)return 0;
        if(fix==2||fix==3){
            float result=fix==2?1.f:-1.f;
            for(u32 n=0;n<std::min(38u,exponent);++n)result*=10.f;
            u32 bits;std::memcpy(&bits,&result,4);return bits;
        }
        constexpr u32 fixed[]={0x00400000,0x80400000,0x7f800001,0xff800001,0x7f800000,0xff800000};
        return fix>=4&&fix<10?fixed[fix-4]:original;
    }
    u32 access(u32 value){
        if(force_zero)value=0;
        if(!marisa||!mode||!stage)return value;
        PurpleLaserRecordTh14 current{stage,frame,ordinal++,value,0};
        if(mode==1&&!reading){
            if(recorded.size()<purple_laser_limit)recorded.push_back(current);
        }else if(reading&&(mode==2||mode==3)){
            auto& records=mode==2?playback:recovered;
            const auto where=std::lower_bound(records.begin(),records.end(),current);
            if(where!=records.end()&&where->key()==current.key())return corrected(*where,value);
            if(mode==3&&records.size()<purple_laser_limit)records.insert(where,current);
        }
        return value;
    }
};
}
