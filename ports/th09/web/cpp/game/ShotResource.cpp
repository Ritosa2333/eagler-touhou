#include "ShotResource.hpp"
#include "Binary.hpp"
#include <cmath>
namespace th09 {
bool ShotResource::load(const u8* data,u32 size){
    sets.clear();if(!data||size<0x42c)return false;const u32 count=read16(data+2);if(count>64||size<0x42c+count*8)return false;
    auto number=[&](u32 offset){const u32 bits=read32(data+offset);float f;std::memcpy(&f,&bits,4);return f;};
    hit_size=number(4);graze_size=number(8);capture_size=number(12);item_size=number(16);movement.normal=number(20);movement.focused=number(24);
    const double diagonal=std::sin(double(.7853981852531433f));movement.diagonal=float(diagonal*double(movement.normal));movement.focused_diagonal=float(diagonal*double(movement.focused));
    charge_speed=number(36);charge_duration=number(40);
    for(u32 i=0;i<3;++i){const char* p=reinterpret_cast<const char*>(data+44+i*64);u32 length=0;while(length<64&&p[length])++length;spell_names[i].assign(p,length);}
    sets.resize(count);
    for(u32 group=0;group<count;++group){u32 offset=read32(data+0x42c+group*8);bool terminated=false;
        for(u32 n=0;n<4096&&offset+2<=size;++n){if(i16(read16(data+offset))<0){terminated=true;break;}if(offset>size||size-offset<56)return false;
            ShotDefinition d;d.frame=i16(read16(data+offset));d.reserved=i16(read16(data+offset+2));d.offset={number(offset+4),number(offset+8)};d.hitbox={number(offset+12),number(offset+16)};d.angle=number(offset+20);d.speed=number(offset+24);
            d.damage=i16(read16(data+offset+28));d.reserved1e=i16(read16(data+offset+30));d.option=i16(read16(data+offset+32));d.type=i16(read16(data+offset+34));d.animation=i16(read16(data+offset+36));d.sound=i16(read16(data+offset+38));
            const u32 create=read32(data+offset+40),update=read32(data+offset+44),draw=read32(data+offset+48),hit=read32(data+offset+52);
            if(create>7||update>7||update==5||draw>2||(hit!=0&&hit!=2)||d.option<0||d.option>4)return false;
            d.creation=ShotCreation(create);d.update=ShotUpdate(update);d.drawing=ShotDrawing(draw);d.hit=ShotHit(hit);sets[group].push_back(d);offset+=56;
        }if(!terminated)return false;
    }return true;
}
}
