#include "ShotResource.hpp"
namespace th08 {
namespace {
u32 word(const u8* p){u32 result;std::memcpy(&result,p,4);return result;}
constexpr ShotCreate creates[]={ShotCreate::Periodic,ShotCreate::EnemyAimed,ShotCreate::OutsideBomb,ShotCreate::OutsideBomb,ShotCreate::Laser,ShotCreate::TargetAimed,ShotCreate::OptionAimed,ShotCreate::RandomSpread,ShotCreate::OrbitAimed};
constexpr ShotUpdate updates[]={ShotUpdate::None,ShotUpdate::Homing,ShotUpdate::None,ShotUpdate::Accelerating,ShotUpdate::PlayerLaser,ShotUpdate::OptionLaser};
}
bool ShotResource::load(const u8* bytes,u32 size){
    release();if(!bytes||size<56||size>16*1024*1024)return false;
    ShotProfile header;std::memcpy(&header,bytes,56);
    if(!header.stream_count||u32(header.stream_count)*8>size-56)return false;
    std::vector<ShotStream> result;result.reserve(header.stream_count);
    for(u32 i=0;i<header.stream_count;++i){
        u32 cursor=word(bytes+56+i*8);ShotStream stream;stream.power_limit=signed_bits(word(bytes+60+i*8));
        for(;;){
            if(cursor>size||size-cursor<2)return false;i16 interval;std::memcpy(&interval,bytes+cursor,2);if(interval<0)break;
            if(size-cursor<56)return false;const auto* raw=bytes+cursor;
            const u32 create=word(raw+40),update=word(raw+44),draw=word(raw+48),hit=word(raw+52);
            if(!interval||create>=9||update>=6||draw>=2||hit>=3)return false;
            ShotDefinition shot;std::memcpy(&shot,raw,40);shot.create=creates[create];shot.update=updates[update];shot.draw=ShotDraw(draw);shot.hit=ShotHit(hit);
            stream.shots.push_back(shot);cursor+=56;
        }
        result.push_back(std::move(stream));
    }
    profile=header;streams=std::move(result);return true;
}
i32 ShotResource::select(i32 power,u8 character,bool bomb_active,i32 bomb_type,bool bomb_ready)const noexcept{
    if(bomb_active&&((character==2&&(bomb_type&1))||character==9)&&bomb_ready){const u32 index=6+((u32(bomb_type)&2)!=0);return index<streams.size()?i32(index):-1;}
    for(u32 i=0;i<streams.size();++i)if(power<streams[i].power_limit)return i;
    return -1;
}
}
