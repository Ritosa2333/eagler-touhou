#include "BulletCreation.hpp"
#include "GameMath.hpp"
namespace th08 {
static constexpr i32 palette16[]{0,1,1,1,1,2,2,2,2,3,3,3,4,4,4,0};
static constexpr i32 palette32[]{0,1,1,2,2,3,4,0};
static void direction(Vec3& vector,float angle,float speed){
    vector.x=(cosine(angle)*number(speed)).to_float();vector.y=(sine(angle)*number(speed)).to_float();
}
bool BulletCreation::set_sprite(AnmVm& vm,i32 index){
    if(!state.animation){failure=Failure::MissingAnimation;return false;}
    if(state.animation->SetSprite(&vm,index)!=0){failure=Failure::MissingAnimation;return false;}return true;
}
bool BulletCreation::set_palette(AnmVm& vm,const AnmVm& source,const BulletState& bullet,i32 color){
    if(vm.activeSpriteIndex==wrapping_add(source.activeSpriteIndex,color))return true;
    if(!bullet.sprites.animation[0].loadedSprite){failure=Failure::MissingAnimation;return false;}
    const float height=bullet.sprites.animation[0].loadedSprite->heightPx;
    i32 offset=color;
    if(height<=16){if(u32(color)>=16){failure=Failure::InvalidTemplate;return false;}offset=palette16[color];}
    else if(height<=32){if(u32(color)>=8){failure=Failure::InvalidTemplate;return false;}offset=palette32[color];}
    return set_sprite(vm,wrapping_add(source.activeSpriteIndex,offset));
}
i32 BulletCreation::create(const BulletEmission& e,i32 index,i32 layer,float aim){
    failure=Failure::None;
    BulletState* slot=state.next_slot;
    for(i32 checked=0;checked<1536;++checked){
        if(slot->state==0)break;
        if((++slot)->state==6)slot=state.bullets;
        if(checked==1535)return 1;
    }
    if(!e.type){failure=Failure::InvalidTemplate;return 1;}
    const auto values=bullet_pattern(e,index,layer,aim,random);auto& b=*slot;const auto& type=*e.type;
    b.state=1;b.unknown_dbc=1;b.grazed=0;b.since_fired.set(0);b.reisen_illusion=0;b.active_time.set(0);
    b.speed=values.speed;b.angle=add_angle(values.angle,0);b.position=e.position;b.position.z=.1f;
    direction(b.velocity,values.angle,Scalar::mul(values.speed,timing.rate));
    b.extra_flags=e.flags;b.sprite_offset=e.color;b.barrier_cooldown=0;b.unknown_dbe=0;
    b.sprites.animation[0]=type.animation[0];b.sprites.animation[4]=type.animation[4];
    b.sprites.hitbox=type.hitbox;b.sprites.reserved_d40=type.reserved_d40;b.sprites.height=type.height;b.sprites.layer=type.layer;
    b.transform_sound=e.transform_sound;b.despawn_protection=0;
    auto& main=b.sprites.animation[0];const i32 sprite=wrapping_add(type.animation[0].activeSpriteIndex,e.color);
    if(main.activeSpriteIndex!=sprite&&!set_sprite(main,sprite))return 1;
    if(!set_palette(b.sprites.animation[4],type.animation[4],b,e.color))return 1;
    const u32 spawn=(e.flags&2)?1:(e.flags&4)?2:(e.flags&8)?3:0;
    if(spawn){
        b.sprites.animation[spawn]=type.animation[spawn];
        if(!set_palette(b.sprites.animation[spawn],type.animation[spawn],b,e.color))return 1;
        b.state=u16(spawn+1);
        b.position.x=(number(b.position.x)-number(Scalar::mul(b.velocity.x,4))).to_float();
        b.position.y=(number(b.position.y)-number(Scalar::mul(b.velocity.y,4))).to_float();
        b.position.z=(number(b.position.z)-number(Scalar::mul(b.velocity.z,4))).to_float();
    }
    std::memcpy(b.extras,e.extras,sizeof(b.extras));b.flags=e.flags;b.extra_flags=0;b.current_extra=e.extra_index;
    if(!initialize_extra(b))return 1;
    if(state.cancel_frames!=0&&!(b.flags&0x1000))b.state=5;
    state.next_slot=(slot+1)->state==6?state.bullets:slot+1;
    return 0;
}
bool BulletCreation::initialize_extra(BulletState& b){
    while(b.current_extra<18){
        if(b.current_extra<0){failure=Failure::InvalidExtra;return false;}
        const auto& e=b.extras[b.current_extra];const u32 flag=e.flags;
        if(!flag||(e.mode==0&&b.extra_flags))return true;
        if(!(b.flags&flag)){++b.current_extra;continue;}
        switch(flag){
        case 1:b.extra_flags|=1;b.extra_state[0].timer.set(0);b.extra_state[0].vector.z=0;break;
        case 0x10:{
            b.extra_flags|=flag;auto& s=b.extra_state[1];s.float_a=e.float_a;s.float_b=e.float_b>-990?e.float_b:b.angle;s.timer.set(0);s.integer_a=e.integer_a;
            direction(s.vector,s.float_b,Scalar::mul(timing.rate,s.float_a));
            if(b.current_extra!=0&&b.transform_sound>=0&&actions)actions->sound(b.transform_sound,0,false);break;
        }
        case 0x20:{
            b.extra_flags|=flag;auto& s=b.extra_state[2];s.float_a=e.float_a;s.float_b=e.float_b;s.timer.set(0);s.integer_a=e.integer_a;
            if(b.current_extra!=0&&b.transform_sound>=0&&actions)actions->sound(b.transform_sound,0,false);break;
        }
        case 0x40:case 0x80:case 0x100:{
            b.extra_flags|=flag;auto& s=b.extra_state[3];s.float_b=e.float_a;s.float_a=e.float_b>-999?e.float_b:b.speed;s.timer.set(0);s.integer_a=e.integer_a;s.integer_b=e.integer_b;s.integer_c=0;break;
        }
        case 0x400:case 0x800:{
            b.extra_flags|=flag;auto& s=b.extra_state[4];s.float_a=e.float_a>=0?e.float_a:b.speed;s.integer_b=e.integer_a;s.integer_a=0;break;
        }
        case 0x2000:b.despawn_protection=e.integer_a;++b.current_extra;continue;
        case 0x4000:
            if(u32(e.integer_a)>=32){failure=Failure::InvalidTemplate;return false;}
            b.sprites=state.templates.types[e.integer_a];
            if(!set_sprite(b.sprites.animation[0],wrapping_add(b.sprites.animation[0].activeSpriteIndex,e.integer_b)))return false;
            ++b.current_extra;continue;
        case 0x20000:b.extra_flags|=flag;b.extra_state[5].timer.set(e.integer_a);break;
        case 0x40000:b.state=5;break;
        case 0x80000:if(actions)actions->sound(e.integer_a,b.position.x,true);++b.current_extra;continue;
        case 0x400000:case 0x800000:b.extra_flags|=flag;b.extra_state[6].timer.set(e.integer_a);break;
        case 0x1000000:{
            if(b.current_extra>=17){failure=Failure::InvalidExtra;return false;}
            BulletEmission shot;shot.transform_sound=-1;shot.position=b.position;
            const u32 packed=u32(e.integer_a);shot.pattern=i16((packed>>24)&127);shot.sprite=i16((packed>>16)&255);shot.color=i16((packed>>8)&255);shot.extra_index=i32(packed&255);
            shot.count=i16(e.integer_b);shot.speed=e.float_a;shot.ending_speed=e.float_b;
            const auto& next=b.extras[++b.current_extra];shot.layers=i16(next.integer_a);shot.flags=u32(next.integer_b);shot.angle=next.float_a;shot.spread=next.float_b;
            std::memcpy(shot.extras,b.extras,sizeof(shot.extras));if(actions)actions->reemit(shot);++b.current_extra;
            if(packed&0x80000000){b.state=5;break;}continue;
        }
        default:break;
        }
        ++b.current_extra;return true;
    }
    return true;
}
void BulletCreation::emit(BulletEmission& e,float aim,u16* replay_flags){
    if(replay_flags)*replay_flags|=0x800;
    if(state.active_count>=1536)return;
    if(u32(e.sprite)>=32){failure=Failure::InvalidTemplate;return;}
    e.type=&state.templates.types[e.sprite];
    bool full=false;for(i32 layer=0;layer<e.layers&&!full;++layer)for(i32 index=0;index<e.count;++index)if(create(e,index,layer,aim)){full=true;break;}
    if((e.flags&0x200)&&actions)actions->sound(e.sound,e.position.x,true);
}
}
