#include "command.hpp"
#include "../laser_system/type0.hpp"
#include "../laser_system/type1.hpp"
#include "../player_entity/player.hpp"
namespace th20::source::bullet::unrecovered {
void spawn_extended_laser(Bullet& b,const Command& op){ //47dcf0 case27
    auto& owner=*static_cast<laser::Controller*>(b.context->objects_04[4]);
    if(op.words[4]==0){
        laser::Type0Parameters p;p.commands=static_cast<const ShotMetadata*>(b.metadata.get())->commands;p.position=b.position;p.type=op.words[5];p.color=op.words[6];const auto erase_parent=op.words[7];
        const auto& extra=*(&op+1);p.angle=resolve_angle(b.view_index,b.angle,b.position,op.f(0),extra.f(3));p.speed=op.f(1)<=-999990?b.field_20:op.f(1);p.field_14=op.f(2);p.length=op.f(3);++b.field_38;p.flags|=1;
        p.field_48=extra.i(4);p.field_4c=extra.i(5);p.length_limit=extra.f(0);p.width=extra.f(1);p.radial_offset=extra.f(2);p.command_index=extra.words[6];
        laser::spawn_type0(owner,p);++b.field_38;if(erase_parent)cancel(b,0);
    }else if(op.words[4]==1){
        laser::Type1Parameters p;p.commands=static_cast<const ShotMetadata*>(b.metadata.get())->commands;p.flags=(op.words[7]&0xffu)|2u;p.position=b.position;p.type=op.words[5];p.color=op.words[6];const auto erase_parent=(op.words[7]>>16)&1u;p.command_index=(op.words[7]>>8)&0xffu;
        p.angle=op.f(0)<=-999990?b.angle:((op.f(0)>=999990&&op.f(0)<1999990)?player_entity::angle_to_player(b.context->objects_04[0],b.position):op.f(0));
        p.growth_speed=op.f(1)<=-999990?b.field_20:op.f(1);p.length=op.f(2);p.length_limit=op.f(3);++b.field_38;p.sound=18;p.field_44=-1;
        const auto& extra=*(&op+1);p.delay=extra.i(4);p.grow=extra.i(5);p.sustain=extra.i(6);p.shrink=extra.i(7);p.width=extra.f(0);p.radial_offset=extra.f(1);
        laser::spawn_type1(owner,p);++b.field_38;if(erase_parent)cancel(b,0);
    }
    //Other type values leave the original command cursor unchanged.
}
}
