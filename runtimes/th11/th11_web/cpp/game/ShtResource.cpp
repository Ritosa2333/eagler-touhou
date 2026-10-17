#include "ShtResource.hpp"
#include <cmath>
namespace th11 {
namespace {template<class T>T read(const u8* p){T v;std::memcpy(&v,p,sizeof v);return v;}}
bool ShtResource::open(const u8* bytes,u32 size){
    if(!bytes||size<sizeof(ShtHeader))return false;
    ShtResource next;std::memcpy(&next.header,bytes,sizeof(ShtHeader));
    const auto& h=next.header;
    if(h.version!=3||!h.group_count||h.group_count>(size-sizeof(ShtHeader))/8||h.max_power_level<0||h.power_step<=0)return false;
    const u32 table_end=sizeof(ShtHeader)+h.group_count*8;
    next.groups.reserve(h.group_count);
    for(u32 i=0;i<h.group_count;++i){
        const auto* entry=bytes+sizeof(ShtHeader)+i*8;u32 offset=read<u32>(entry);ShotGroup group;group.metadata=read<u32>(entry+4);
        if(offset<table_end||offset>=size)return false;
        for(;;){
            if(offset>=size)return false;
            if(read<i8>(bytes+offset)<0)break;
            if(size-offset<sizeof(ShotSpec))return false;
            const auto s=read<ShotSpec>(bytes+offset);
            if(s.interval<=0||s.option<0||s.option>8||u32(s.spawn)>3||u32(s.update)>3||s.hit_callback||s.extra_callback||!std::isfinite(s.angle)||!std::isfinite(s.speed))return false;
            group.shots.push_back(s);offset+=sizeof(ShotSpec);
        }
        next.groups.push_back(std::move(group));
    }
    *this=std::move(next);return true;
}
i32 ShtResource::group_index(i32 power,i32 step,i32 character,i32 subtype,bool focused,i32 mode)const noexcept{
    if(step<=0)return -1;
    const i64 level=power/step,levels=i64(header.max_power_level)+1;
    const i64 index=level+(i64(character)*3+subtype==4?levels*mode:focused?levels:0);
    return index>=0&&index<i64(groups.size())?i32(index):-1;
}
}
