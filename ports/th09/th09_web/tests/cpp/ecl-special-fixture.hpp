#pragma once
#include "../../cpp/game/EclSpecial.hpp"
#include <vector>
namespace special_test {
using namespace th09;
struct Event {u32 type=0;i32 kind=0,side=0;Vec3 position;u32 has_extra=0;Vec3 extra;};
struct Fixture:EclSpecialActions {
    EclVm vm;EclWorldState world;EclPlayfieldState fields[2];Bullet pool[2][536];std::vector<Event> events;
    Fixture(){vm.values.world=&world;vm.values.field=&fields[0];vm.values.opponent=&fields[1];fields[1].side=1;vm.bind_context();}
    void queue_attack(i32 kind,i32 side,const Vec3& position,const Vec3* extra)override{
        Event e;e.type=1;e.kind=kind;e.side=side;e.position=position;e.has_extra=extra!=nullptr;if(extra)e.extra=*extra;events.push_back(e);
    }
    void play_sound(i32 sound,i32 pan)override{Event e;e.type=2;e.kind=sound;e.side=pan;events.push_back(e);}
    Bullet* bullets(i32 side,u32& count)override{count=536;return side>=0&&side<2?pool[side]:nullptr;}
    void set_sprite(Bullet& bullet,i32 sprite)override{
        Event e;e.type=3;e.kind=sprite;
        e.side=i32((reinterpret_cast<u8*>(&bullet)-reinterpret_cast<u8*>(pool))/sizeof(Bullet));events.push_back(e);
        static const float widths[]={16,32,8};bullet.animation_height=widths[u32(sprite)%3];
    }
};
struct Field {u32 original,offset,size;};
#define BF(original,field) {original,offsetof(Bullet,field),sizeof(Bullet::field)}
inline const Field fields[]={BF(0xd4c,position),BF(0xd58,velocity),BF(0xd70,speed),BF(0xd74,acceleration),BF(0xd78,speed_delta),BF(0xd7c,direction),BF(0xd80,angular_velocity),BF(0xd84,turn_delta),BF(0x30,animation_height),BF(0xd48,base_sprite),BF(0xdbe,state),BF(0x10c0,sprite),BF(0x10c2,color),BF(0xdb4,active_extras),BF(0xdb8,available_extras),BF(0xdd4,extra_index),BF(0xdd0,transform_sound),BF(0xdd8,extras)};
#undef BF
}
