#include "enemy_opcode_laser.hpp"
#include "../laser_system/type0.hpp"
#include "../laser_system/type1.hpp"
#include "../laser_system/type2.hpp"
#include "../laser_system/type3.hpp"
#include "../ecl_vm/math.hpp"
namespace th20::source::gameplay {
EnemyOpcodeResult execute_enemy_laser_opcode(EnemyOpcodeReader& r,EnemyLaserOpcodeServices& env){
    const auto op=r.opcode();if(op<700||op>714)return std::nullopt;
    const auto queued=[&]()->EnemyQueuedRecord&{return queued_shot(r.state,r.integer(0),env.shots(),true);};
    const auto metadata=[](EnemyQueuedRecord& q)->bullet::ShotMetadata&{return *static_cast<bullet::ShotMetadata*>(q.owner.get());};
    const auto target=[&](){return env.find(r.context(),r.integer(0));};
    switch(op){
    case 700:{auto& q=queued();auto v=shot_values(q);v.parameters.position.x=r.real(1);v.parameters.position.y=r.real(2);v.parameters.position.z=r.real(3);metadata(q).fields_14[0]=r.real(4);store_shot_values(q,v);break;}
    case 701:{auto& q=queued();auto& m=metadata(q);for(int i=0;i<5;++i)m.fields_24[i]=unsigned(r.integer(1+i));break;}
    case 702:{auto& q=queued();const auto v=shot_values(q);const auto& m=metadata(q);laser::Type0Parameters p;p.commands=m.commands;p.position=shot_origin(r.state,v);p.type=v.parameters.fields_00[0];p.color=v.parameters.fields_00[1];p.angle=ecl::math::wrap_angle(v.parameters.angle);p.speed=v.parameters.speed;p.field_14=v.parameters.position.x;p.length=v.parameters.position.y;p.length_limit=v.parameters.position.z;p.width=m.fields_14[0];p.flags=m.fields_24[4]|1u;p.field_48=m.field_3c;p.field_4c=m.field_40;p.radial_offset=m.field_00;p.view_index=int(r.state.fields_2c8[8]);env.create(r.context(),0,&p);break;}
    case 703:{auto& q=queued();const auto v=shot_values(q);const auto& m=metadata(q);laser::Type1Parameters p;p.commands=m.commands;p.position=shot_origin(r.state,v);p.type=v.parameters.fields_00[0];p.color=v.parameters.fields_00[1];p.angle=ecl::math::wrap_angle(v.parameters.angle);p.growth_speed=v.parameters.speed;p.length=v.parameters.position.x;p.length_limit=v.parameters.position.y;p.width=m.fields_14[0];p.delay=int(m.fields_24[0]);p.grow=int(m.fields_24[1]);p.sustain=int(m.fields_24[2]);p.shrink=int(m.fields_24[3]);p.flags=m.fields_24[4]|2u;p.sound=m.field_3c;p.field_44=m.field_40;p.radial_offset=m.field_00;p.handle=unsigned(r.integer(1));p.view_index=int(r.state.fields_2c8[8]);env.create(r.context(),1,&p);break;}
    case 704:{auto* l=target();const sprite::Vec3 p{r.real(1),r.real(2),0};if(l)l->set_position(p);break;}
    case 705:{auto* l=target();const sprite::Vec3 p{r.real(1),r.real(2),0};if(l)std::memcpy(reinterpret_cast<std::uint8_t*>(l)+0x704,&p,12);break;} //49c800, exact storage even for a different concrete type
    case 706:if(auto* l=target())l->field_7c=r.real(1);break;
    case 707:if(auto* l=target())l->speed=r.real(1);break;
    case 708:if(auto* l=target())l->angle=r.real(1);break;
    case 709:if(auto* l=target()){const auto value=r.real(1);std::memcpy(reinterpret_cast<std::uint8_t*>(l)+0x714,&value,4);}break;
    case 710:{while(auto* l=target()){if(l->handle==0)throw std::logic_error("Original erase-by-id requires a nonzero id to terminate");l->erase(0,0);l->handle=0;}break;}
    case 711:{auto& q=queued();const auto v=shot_values(q);const auto& m=metadata(q);laser::Type2Parameters p;p.commands=m.commands;p.position=shot_origin(r.state,v);p.type=v.parameters.fields_00[0];p.color=v.parameters.fields_00[1];p.angle=ecl::math::wrap_angle(v.parameters.angle);p.speed=v.parameters.speed;p.width=m.fields_14[0];p.count=m.fields_24[0];p.flags|=1u;p.sound=m.field_3c;p.motion_sound=m.field_40;p.radial_offset=m.field_00;p.view_index=int(r.state.fields_2c8[8]);env.create(r.context(),2,&p);break;}
    case 712:{const sprite::Vec3 size{r.real(0),r.real(1),0};const auto angle=env.animation_angle(r.state.animations.at(0).handle);env.cancel_rectangle(r.context(),r.get<sprite::Vec3>(0x110),size,angle);break;}
    case 713:{auto& q=queued();const auto v=shot_values(q);const auto& m=metadata(q);laser::Type3Parameters p;p.commands=m.commands;p.position=shot_origin(r.state,v);p.field_2c=v.parameters.fields_00[1];p.angle=ecl::math::wrap_angle(v.parameters.angle);p.field_34=m.fields_24[0];p.field_24=m.fields_14[0];p.field_20=v.parameters.position.z;std::memcpy(&p.field_30,&m.field_00,4);p.handle=unsigned(r.integer(1));p.view_index=int(r.state.fields_2c8[8]);env.create(r.context(),3,&p);break;}
    case 714:if(auto* l=target())l->set_parameter_flag(unsigned(r.integer(1)));break;
    }
    return 0;
}
}
