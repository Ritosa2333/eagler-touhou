#include "ShtResource.hpp"
#include <cmath>
namespace th15 {
namespace {template<class T>T read(const u8* p){T value;std::memcpy(&value,p,sizeof value);return value;}}
bool ShtResource::open(const u8* bytes,u32 size){
    if(!bytes||size<0x108)return false;ShtResource next;std::memcpy(&next.header,bytes,0xe0);
    if(next.header.version!=4||next.header.group_count!=10||next.header.max_power_level<0||next.header.power_step<=0)return false;
    for(u32 i=0;i<10;i++){
        const u32 relative=read<u32>(bytes+0xe0+i*4);if(relative>size-0x108)return false;u32 cursor=relative+0x108;std::vector<ShotSpec> shots;
        for(;;){if(cursor>=size)return false;if(read<i8>(bytes+cursor)<0)break;if(size-cursor<0x58)return false;const ShotSpec shot=read<ShotSpec>(bytes+cursor);
            if(shot.interval<=0||shot.spawn_kind>5||shot.update_kind>4||shot.hit_kind>1||shot.collision_kind>6||!std::isfinite(shot.angle)||!std::isfinite(shot.speed))return false;
            shots.push_back(shot);cursor+=0x58;
        }
        next.groups.push_back(std::move(shots));
    }
    *this=std::move(next);return true;
}
}
