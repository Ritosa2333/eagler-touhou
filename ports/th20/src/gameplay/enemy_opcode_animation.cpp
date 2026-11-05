#include "enemy_opcode_animation.hpp"
#include "enemy_variables.hpp"
#include <bit>
namespace th20::source::gameplay {
namespace {
int add(int a,int b){return recovered::signed_bits(unsigned(a)+unsigned(b));}
EnemyAnimationLink& resize_link(EnemyState& state,int index){const auto count=unsigned(index)+1u;if(state.animations.size()<count)state.animations.resize(count);return state.animations.at(static_cast<unsigned>(index));}
template<class T>void interpolate(sprite::Interpolation<T>& value,int duration,int mode,T start,T end){value.duration=duration;value.mode=mode;value.start=start;value.end=end;value.current=start;recovered::timer_set(value.timer,0);}
void rotation(sprite::Animation& animation,float angle){animation.base.vector_38.z=angle;animation.base.flags[1]|=2;}
void dimensions(EnemyState& state,sprite::Animation& a,EnemyAnimationOpcodeServices& env){state.vector_170.x=env.height(a);state.vector_170.y=env.width(a);}
void hide_if_needed(EnemyState& state,unsigned handle,EnemyAnimationOpcodeServices& env){if(state.fields_2c8[0]&32u)env.hide(handle);}
}
void spawn_enemy_from_opcode(EnemyOpcodeReader& r,EnemyAnimationOpcodeServices& env){ //496fb0
    auto& owner=r.controller();if(std::bit_cast<int>(owner.field_124)>=std::bit_cast<int>(owner.data.field_88))return;
    const auto bytes=r.raw_word(0);const int shifted=add(std::bit_cast<int>(bytes),4);const int words=shifted/4;
    SpawnParameters p;construct_spawn_parameters(p);
    p.position.x=r.real_value(1,std::bit_cast<float>(r.raw_word(unsigned(words)*4)));
    p.position.y=r.real_value(2,std::bit_cast<float>(r.raw_word(unsigned(words)*4+4)));
    switch(r.opcode()){case 300:case 309:case 321:case 311:case 304:p.position.x=r.get<float>(0x110)+p.position.x;p.position.y=r.get<float>(0x114)+p.position.y;break;}
    switch(r.opcode()){case 311:case 304:case 312:case 305:p.flags_18=1;break;}
    if(r.state.fields_2c8[1]&8u){p.position.x*=-1.f;p.flags_18^=1;}
    p.health=r.integer_value(3,r.raw_word(unsigned(words)*4+8));p.field_0c=r.integer_value(4,r.raw_word(unsigned(words)*4+12));p.field_10=r.integer_value(5,r.raw_word(unsigned(words)*4+16));
    std::memcpy(p.variables,r.state.fields_78,48);p.field_50=r.entity.state.identifier;
    env.create_enemy(owner,r.text(4,bytes),p,(r.state.fields_2c8[2]&4u)?&r.entity:nullptr);
}
void change_enemy_animation(EnemyOpcodeReader& r,EnemyAnimationOpcodeServices& env){ //496b90
    const int index=r.integer(0);auto& link=resize_link(r.state,index);auto* a=env.animation(link.handle);if(!a)return;auto& b=a->base;
    switch(r.opcode()){
    case 319:rotation(*a,r.real(1));break;
    case 325:{const auto blue=std::uint8_t(r.integer(3)),green=std::uint8_t(r.integer(2)),red=std::uint8_t(r.integer(1));b.field_490=(b.field_490&0xff000000u)|(unsigned(red)<<16)|(unsigned(green)<<8)|blue;break;}
    case 326:{const int blue=std::uint8_t(r.integer(3)),green=std::uint8_t(r.integer(4)),red=std::uint8_t(r.integer(5));const int mode=r.integer(2),frames=r.integer(1);const sprite::Vec3i start{int(b.field_490&255),int((b.field_490>>8)&255),int((b.field_490>>16)&255)};interpolate(b.interpolation_e0,frames,mode,start,{red,green,blue});break;}
    case 327:b.field_490=(b.field_490&0x00ffffffu)|(unsigned(std::uint8_t(r.integer(1)))<<24);break;
    case 328:{const int alpha=std::uint8_t(r.integer(3)),mode=r.integer(2),frames=r.integer(1);interpolate(b.interpolation_134,frames,mode,int(b.field_490>>24),alpha);break;}
    case 329:{const float y=r.real(2),x=r.real(1);b.vector_50={x,y};b.flags[1]|=4;break;}
    case 330:{const float y=r.real(4),x=r.real(3);const int mode=r.integer(2),frames=r.integer(1);interpolate(b.interpolation_1e0,frames,mode,b.vector_50,sprite::Vec2{x,y});break;}
    case 331:b.field_494=(b.field_494&0x00ffffffu)|(unsigned(std::uint8_t(r.integer(1)))<<24);break;
    case 332:{const int alpha=std::uint8_t(r.integer(3)),mode=r.integer(2),frames=r.integer(1);interpolate(b.interpolation_2f4,frames,mode,int(b.field_494>>24),alpha);if(!(b.flags[2]&0x1c00u))b.flags[2]|=0x400u;break;}
    case 333:{const float x=r.real(3),y=r.real(4);const auto start=a->vector_5bc;const int mode=r.integer(2),frames=r.integer(1);interpolate(b.interpolation_8c,frames,mode,start,sprite::Vec3{x,y,0});break;}
    case 335:{const float y=r.real(2),x=r.real(1);b.vector_58={x,y};b.flags[1]|=4;break;}
    case 336:env.layer(*a,r.integer(1));break;
    case 337:{auto* flags=reinterpret_cast<std::uint8_t*>(&b.flags[0]);flags[1]=std::uint8_t(r.integer(1));break;}
    }
}
EnemyOpcodeResult execute_enemy_animation_opcode(EnemyOpcodeReader& r,EnemyAnimationOpcodeServices& env){
    auto& s=r.state;auto current_file=[&]() ->sprite::AnimationFile& {return env.file(r.controller(),s.fields_1c[0]);};
    auto attach=[&](EnemyAnimationLink& link,int script){link.handle=env.spawn(current_file(),script,nullptr,0,add(std::bit_cast<int>(s.fields_1c[6]),7),2);};
    switch(r.opcode()){
    case 300:case 301:case 304:case 305:case 321:spawn_enemy_from_opcode(r,env);break;
    case 309:case 310:case 311:case 312:if(!env.selected(r.controller(),0))spawn_enemy_from_opcode(r,env);break;
    case 302:s.fields_1c[0]=unsigned(r.integer(0));break;
    case 303:{const int index=r.integer(0),script=r.integer(1);auto& link=resize_link(s,index);env.delete_animation(link.handle);if(script>=0){attach(link,r.integer(1));if(!index){s.fields_1c[2]=unsigned(r.integer(1));s.fields_1c[1]=s.fields_1c[0];}auto* a=env.animation(link.handle);if(!index)dimensions(s,*a,env);hide_if_needed(s,link.handle,env);}break;}
    case 306:{const int index=r.integer(0),script=r.integer(1);auto& link=resize_link(s,index);env.delete_animation(link.handle);attach(link,script);auto* a=env.animation(link.handle);if(!index)dimensions(s,*a,env);hide_if_needed(s,link.handle,env);if(!index){s.fields_2c8[1]|=16;s.fields_1c[3]=script;s.fields_1c[4]=0;s.fields_1c[2]=script;s.fields_1c[1]=s.fields_1c[0];}break;}
    case 307:case 308:case 314:case 315:case 338:case 339:{
        sprite::Vec3 position=r.get<sprite::Vec3>(0x110);float angle=0;
        if(r.opcode()==338){const float y=r.real(3),x=r.real(2);position={x+position.x,y+position.y,0.f+position.z};}
        const int file_index=r.integer(0);auto& file=env.file(r.controller(),unsigned(file_index));
        if(r.opcode()==338)angle=r.real(4);
        const int script=r.integer(1);const bool positioned=r.opcode()==307||r.opcode()==314||r.opcode()==315||r.opcode()==338;
        auto handle=env.spawn(file,script,positioned?&position:nullptr,angle,-1,r.opcode()==314?0u:2u);
        if(r.opcode()==315)if(auto* a=env.animation(handle)){a->vector_5bc=r.get<sprite::Vec3>(0x110);rotation(*a,r.real(2));}
        env.remember(r.context(),handle);
        if(r.opcode()==339){auto* a=env.animation(handle);for(int i=0;i<r.integer(2);++i)env.execute(*a);}
        break;
    }
    case 313:case 316:{const int index=r.integer(0);const int variant=r.opcode()==316?r.integer(1):0;auto& link=resize_link(s,index);env.delete_animation(link.handle);const int base=std::bit_cast<int>(s.fields_1c[3]);attach(link,r.opcode()==313?add(base,5):variant<0?base:add(add(base,5),variant));auto* a=env.animation(link.handle);if(!index)dimensions(s,*a,env);hide_if_needed(s,link.handle,env);break;}
    case 317:{const int index=r.integer(0);auto& link=s.animations.at(unsigned(index));const int event=std::int16_t(r.integer(1));env.interrupt(link.handle,event);break;}
    case 318:{auto& link=s.animations.at(0);env.delete_animation(link.handle);attach(link,std::bit_cast<int>(s.fields_1c[3]));s.fields_1c[4]=0;s.fields_1c[2]=s.fields_1c[3];s.fields_1c[1]=s.fields_1c[0];s.fields_2c8[1]&=~16u;break;}
    case 319:case 325:case 326:case 327:case 328:case 329:case 330:case 331:case 332:case 333:case 335:case 336:case 337:change_enemy_animation(r,env);break;
    case 320:{const int index=r.integer(0);s.animations.at(unsigned(index)).offset[0]=r.real(1);s.animations.at(unsigned(index)).offset[1]=r.real(2);s.animations.at(unsigned(index)).offset[2]=0;break;}
    case 322:{const int parent=r.integer(1),index=r.integer(0);s.animations.at(unsigned(index)).parent=parent;break;}
    case 323:s.fields_250[2]=unsigned(r.integer(0));s.fields_250[1]=unsigned(r.integer(1));break;
    case 324:{const int identifier=r.integer(2);const auto* target=env.find(r.controller(),unsigned(identifier));const auto position=target?enemy_position(target):r.get<sprite::Vec3>(0x110);r.float_destination(0)=std::bit_cast<unsigned>(position.x);r.float_destination(1)=std::bit_cast<unsigned>(position.y);break;}
    case 334:{const auto position=r.get<sprite::Vec3>(0x110);const int kind=r.integer(0);env.effect(r.context(),kind,position);break;}
    case 340:{const int identifier=r.integer(0);if(auto* target=env.find(r.controller(),unsigned(identifier)))target->state.fields_2c8[1]|=0x200u;break;}
    case 341:s.fields_2c8[2]|=4;break;
    case 342:s.fields_2c8[2]&=~4u;break;
    case 343:{const auto value=unsigned(r.integer(0));s.fields_2c8[2]=(s.fields_2c8[2]&~8u)|((value&1u)<<3);break;}
    case 344:{const int identifier=r.integer(1);const bool present=env.find(r.controller(),unsigned(identifier))!=nullptr;r.integer_destination(0)=present;break;}
    default:return std::nullopt;
    }
    return 0;
}
}
