#include "ShotTh16.hpp"
#include "ShotManager.hpp"
#include <algorithm>
#include <cmath>
#if PORT_TITLE == 16
namespace th11 {
bool shot_th16_spawn_callback(Th16ShotSpawn& shot,const ShotSpec& spec,Th16DamageSources& sources){
    switch(u32(spec.spawn)){
    case 0:return true;
    case 1:shot.target_id=0;return true;
    case 2:if(!shot.damage_source)return false;shot.hitbox.x=0;sources.records[shot.damage_source].words[32]=1;sources.records[shot.damage_source].words[5]=0;shot.flags&=~1u;return true;
    case 4:if(!shot.damage_source)return false;sources.records[shot.damage_source].words[36]=2;return true;
    default:return false;
    }
}
bool Th16DamageSources::update(const float* rate){
    const auto floating=[](u32 word){float value;std::memcpy(&value,&word,4);return value;};const float scale=rate?*rate:1.f;
    // Entire256-slot loop442a20..442b47, before the remaining player update.
    for(u32 id=1;id<=256;++id){auto& w=records[id].words;if(!(w[0]&1))continue;if(w[23]&15)return false;
        const auto velocity=polar(floating(w[14]),floating(w[13])*scale);w[20]=float_bits(velocity.x);w[21]=float_bits(velocity.y);
        w[7]=float_bits(float(std::floor(double((velocity.x+floating(w[7]))*100.f)))/100.f);
        w[8]=float_bits(float(std::floor(double((velocity.y+floating(w[8]))*100.f)))/100.f);
        w[9]=float_bits(floating(w[22])+floating(w[9]));
        w[1]=float_bits(floating(w[2])+floating(w[1]));w[3]=float_bits(normalize_angle(floating(w[4])+floating(w[3])));w[33]=0;
        if(w[27]>=1)w[27]=0;w[24]=w[25];const float clock=floating(w[26])-(scale>.99f&&scale<1.01f?1.f:scale);w[26]=float_bits(clock);w[25]=u32(truncate_int(clock));if(signed_bits(w[25])<=0)w[0]&=~1u;
    }return true;
}
bool shot_th16_beam_update(Th16ShotSpawn& shot,const ShotSpec& spec,const ShotPlayer& player,Th16DamageSource& area,AnmVm* vm,bool firing,u32& cache,Th16BeamEffects& out){
    out={};const i32 raw=i32(spec.option)-1,index=raw>=100?raw-100+4:raw;
    if(index<0||index>=12)return false;
    const auto& option=player.options[index];auto& movement=shot.movement;
    movement.position={float(option.x)*.0078125f,float(option.y)*.0078125f,0};
    const float desired=normalize_angle(player.focused?-1.5707963705062866f:spec.angle),current=movement.angle;
    float delta=current-desired;
    if(delta>3.1415927410125732f)delta=current-(desired+6.2831854820251465f);
    else if(desired-current>3.1415927410125732f)delta=current-(desired-6.2831854820251465f);
    if(std::abs(normalize_angle(delta))<.001f){
        float turn=desired-current;
        if(turn>3.1415927410125732f)turn=desired-(current+6.2831854820251465f);
        else if(current-desired>3.1415927410125732f)turn=desired-(current-6.2831854820251465f);
        movement.angle=normalize_angle(normalize_angle(current+normalize_angle(normalize_angle(turn)*.1f)));
    }else movement.angle=normalize_angle(desired);
    if(shot.state==2)return true;
    out.pan=truncate_int((player.position.x*1000.f)/192.f);out.pan_active=true;
    if(shot.hitbox.x<512.f){shot.hitbox.x+=18.f;area.words[5]=float_bits(shot.hitbox.x);}
    const auto offset=polar(movement.angle,shot.hitbox.x*.5f);
    area.words[7]=float_bits(offset.x+movement.position.x);area.words[8]=float_bits(offset.y+movement.position.y);area.words[9]=0;
    if(vm){vm->flags|=8;vm->sprite_size.x=shot.hitbox.x;vm->flags|=16;vm->texture_scale.x=shot.hitbox.x*.001953125f;}else shot.animation=0;
    if(!shot.hit_this_frame&&shot.state==1&&shot.hit_last_frame==1){out.interrupts[out.count++]=3;shot.hit_last_frame=0;}
    if(shot.state==1){
        const bool season=shot.packed&0xf0000;
        const i32 slot=season?i32(spec.option)-101:raw;
        if(!player.power_step||(player.power==INT32_MIN&&player.power_step==-1))return false;
        if(!firing||slot>=(season?player.th16_season_count:player.option_count)||player.state==2||player.state==4||cache!=u32(player.power/player.power_step)||player.th16_dialogue||!player.th16_enemy_manager){
            area.words[0]&=~1u;shot.state=2;out.interrupts[out.count++]=1;cache=0;out.stop=true;
        }
    }
    shot.hit_this_frame=0;return true;
}
bool shot_th16_default_hit(Th16ShotSpawn& shot,Th16DamageSources& sources,AnmVm& animation,i32& damage){
    // Complete445e20 with the optional VM interrupt callback absent.
    shot.movement.position.z=.1f;
    if(animation.th16_callbacks[3]||!shot.damage_source)return false;
    animation.interrupt(1);shot.movement.speed*=.125f;shot.state=2;animation.position=shot.movement.position;
    sources.records[shot.damage_source].words[0]&=~1u;shot.damage_source=0;damage=shot.damage;return true;
}
bool shot_th16_collect(Th16DamageSources& sources,const Th16DamageQuery& query,Th16DamageDelivery& delivery,i32& damage,bool& bomb,Vec3* last_hit){
    damage=0;if(!query.advanced)return true;
    if(!delivery.bomb(query,damage))return false;bomb=damage>0;
    for(u32 id=1;id<=256;++id){auto& record=sources.records[id];auto& w=record.words;
        if(!(w[0]&1)||w[25]==w[24])continue;
        const i32 clock=signed_bits(w[25]),period=signed_bits(w[32]);
        if(!period||(clock==INT32_MIN&&period==-1))return false;
        if(clock%period||!delivery.overlaps(record,query)||(query.stamp&&w[33]==query.stamp))continue;
        if(query.stamp)w[33]=query.stamp;if(w[0]&4)bomb=true;
        i32 hit=signed_bits(w[29]);
        if(!query.probe){if(w[36]){w[35]=query.stamp;if(!delivery.hit(id,record,query,hit))return false;}w[30]=u32(wrapping_add(signed_bits(w[30]),signed_bits(w[29])));}
        damage=wrapping_add(damage,hit);
        if(last_hit){std::memcpy(&last_hit->x,&w[7],4);std::memcpy(&last_hit->y,&w[8],4);std::memcpy(&last_hit->z,&w[9],4);}
        if(signed_bits(w[31])<9999999&&signed_bits(w[31])<=signed_bits(w[30])){w[0]&=~1u;w[29]=0;}
    }
    if(damage>query.cap)damage=query.cap;
    return query.probe||!damage||delivery.score(wrapping_add(damage/10,10));
}
bool shot_th16_update(Th16ShotSpawn& shot,const ShotSpec& spec,Th16DamageSources& sources,const float* rate,Th16ShotUpdateDelivery& delivery){
    if(!shot.state)return true;
    const i32 callback=delivery.update_callback(shot,spec);
    if(callback)return callback>0;
    auto& movement=shot.movement;
    if(movement.flags&15)return false;
    const auto velocity=polar(movement.angle,movement.speed*(rate?*rate:1.f));
    movement.velocity.x=velocity.x;movement.velocity.y=velocity.y;
    movement.position.x=velocity.x+movement.position.x;movement.position.y=velocity.y+movement.position.y;movement.position.z=movement.velocity.z+movement.position.z;
    movement.position.x=float(std::floor(double(movement.position.x*100.f)))/100.f;
    movement.position.y=float(std::floor(double(movement.position.y*100.f)))/100.f;
    const auto retire=[&](){shot.state=0;shot.animation=0;if(shot.damage_source)sources.records[shot.damage_source].words[0]&=~1u;};
    u32 flags2=0;
    if(!delivery.find(shot.animation,flags2)){retire();return true;}
    if(spec.type!=2){
        std::array<Vec3,4> corners;
        if(!delivery.quad(shot.animation,corners))return false;
        if(shot.timer.current>=15){
            bool visible=false;for(const auto p:corners)if(p.x>delivery.play_origin.x&&p.x<delivery.play_origin.x+384.f&&p.y>delivery.play_origin.y&&p.y<delivery.play_origin.y+448.f){visible=true;break;}
            if(!visible){delivery.erase(shot.animation);retire();return true;}
        }
    }
    if(shot.damage_source&&(shot.flags&1)){
        auto& words=sources.records[shot.damage_source].words;
        words[7]=float_bits(movement.position.x);words[8]=float_bits(movement.position.y);words[9]=float_bits(movement.position.z);
        words[3]=float_bits(movement.angle);words[5]=float_bits(shot.hitbox.x);words[6]=float_bits(shot.hitbox.y);words[29]=u32(shot.damage);
    }
    delivery.position(shot.animation,movement.position);
    if(flags2&0x80)delivery.orient(shot.animation,movement.angle);
    auto& timer=shot.timer;timer.previous=timer.current;const float speed=rate?*rate:1.f;
    if(speed>.99f&&speed<1.01f){timer.current=wrapping_add(timer.current,1);timer.fractional+=1.f;}
    else{timer.fractional+=speed;timer.current=truncate_int(timer.fractional);}
    return true;
}
void shot_th16_homing(Th16ShotSpawn& shot,Th16ShotTargets& targets){
    if(shot.state==2)return;
    auto& movement=shot.movement;
    if(!targets.present())shot.target_id=0;
    else if(!shot.target_id)shot.target_id=targets.nearest(movement.position,256.f);
    if(shot.target_id&&!targets.valid(shot.target_id))shot.target_id=0;
    if(!shot.target_id){movement.speed=std::min(16.f,movement.speed+.1f);return;}
    const auto enemy=targets.target(shot.target_id);
    if(enemy.flags&0xc000021u){movement.speed=std::min(16.f,movement.speed+.1f);return;}
    const float dy=enemy.position.y-movement.position.y,dx=enemy.position.x-movement.position.x;
    const float desired=float(std::atan2(double(dy),double(dx))),current=movement.angle;
    float delta=desired-current;
    if(delta>3.1415927410125732f)delta=desired-(current+6.2831854820251465f);
    else if(current-desired>3.1415927410125732f)delta=desired-(current-6.2831854820251465f);
    if(shot.timer.current>=60){movement.speed+=.2f;return;}
    const float absolute=std::abs(delta);
    if(absolute>=.7853981852531433f)movement.speed=std::max(4.f,movement.speed-.2f);
    else if(absolute<.2617993950843811f)movement.speed=std::min(16.f,movement.speed+.2f);
    movement.angle=normalize_angle(normalize_angle(current+delta*.07999999821186066f));
}
i32 shot_th16_spawn(Th16ShotSpawn& out,const ShotSpec& spec,u32 packed,u32 index,i32 frame,const ShotPlayer& player,Vec3 origin,Rng& random,std::array<u32,16>& lasers,Th16DamageSources& sources,const float* rate,Th16ShotDelivery& delivery){
    if(!shot_th16_spawn_prepare(out,spec,packed,player,origin,random,lasers,rate))return -2;
    auto& p=out.movement.position;
    p.x=(spec.offset.x-out.movement.velocity.x)+p.x;
    p.y=(spec.offset.y-out.movement.velocity.y)+p.y;
    out.animation=delivery.create(bool(packed&0xf0000),i32(spec.animation)+((packed&0xf0000)?0:5));
    if(!out.animation||!delivery.orient(out.animation,spec.angle))return -2;
    out.damage_source=shot_th16_spawn_damage(out,index,sources);
    if(!out.damage_source)return -2;
    out.flags|=1;
    const i32 callback=delivery.callback(out,spec,frame);
    if(callback){out.state=0;delivery.erase(out.animation);out.animation=0;sources.records[out.damage_source].words[0]&=~1u;return callback<0?-2:-1;}
    if(spec.hit_animation>=0&&!delivery.sound(spec.hit_animation,p.x))return -2;
    return delivery.position(out.animation,p)?0:-2;
}
u32 Th16DamageSources::rectangle(Vec3 position,float angle,Vec2 size,i32 duration,i32 damage){
    // Full finite444b20; reserved record0 precedes the256 circulating slots.
    i32 candidate=cursor;
    for(u32 attempts=0;attempts<256;++attempts){candidate=wrapping_add(candidate,1);if(candidate>=256)candidate=0;
        if(candidate<0)return 0;
        auto& words=records[candidate+1].words;if(words[0]&1)continue;
        words[0]=(words[0]&~6u)|1;for(u32 i=7;i<24;++i)words[i]=0;
        words[7]=float_bits(position.x);words[8]=float_bits(position.y);words[9]=float_bits(position.z);
        words[5]=float_bits(size.x);words[6]=float_bits(size.y);words[3]=float_bits(normalize_angle(angle));words[4]=0;
        if(!(words[28]&1)){words[28]|=1;words[27]=0;}
        words[25]=u32(duration);words[24]=u32(wrapping_sub(duration,1));words[26]=float_bits(float(duration));
        words[29]=u32(damage);words[30]=0;words[31]=9999999;words[32]=1;words[33]=words[36]=0;
        cursor=candidate;return u32(candidate+1);
    }
    cursor=candidate;return u32(candidate+1);
}
u32 shot_th16_spawn_damage(const Th16ShotSpawn& shot,u32 index,Th16DamageSources& sources){
    const u32 id=sources.rectangle(shot.movement.position,shot.movement.angle,shot.hitbox,9999999,shot.damage);
    if(!id)return 0;auto& words=sources.records[id].words;words[36]=1;words[31]=10000000;words[34]=index;return id;
}
u32 Th16DamageSources::circle(Vec3 position,float radius,float growth,i32 duration,i32 damage){
    // Complete4449b0, including native pool-exhaustion return and preservation.
    i32 candidate=cursor;
    for(u32 attempts=0;attempts<256;++attempts){candidate=wrapping_add(candidate,1);if(candidate>=256)candidate=0;if(candidate<0)return 0;
        auto& words=records[candidate+1].words;if(words[0]&1)continue;
        words[0]=(words[0]&~4u)|3;for(u32 i=7;i<24;++i)words[i]=0;
        words[7]=float_bits(position.x);words[8]=float_bits(position.y);words[9]=float_bits(position.z);words[1]=float_bits(radius);words[2]=float_bits(growth);
        if(!(words[28]&1)){words[28]|=1;words[27]=0;}
        words[25]=u32(duration);words[24]=u32(wrapping_sub(duration,1));words[26]=float_bits(float(duration));
        words[29]=u32(damage);words[30]=0;words[31]=9999999;words[32]=1;words[33]=words[36]=0;cursor=candidate;return u32(candidate+1);
    }cursor=candidate;return u32(candidate+1);
}
bool shot_th16_spawn_prepare(Th16ShotSpawn& out,const ShotSpec& spec,u32 packed,const ShotPlayer& player,const Vec3& origin,Rng& random,std::array<u32,16>& lasers,const float* rate){
    const i32 number=spec.option,index=number-1;
    i32 option=-1,cache=-1;
    if(number){
        option=index>=100?index-100+4:index;
        cache=index>=100?index-100:index;
        if(option<0||option>=12||cache<0||cache>=8)return false;
    }
    out.state=1;out.packed=packed;out.timer.set(0,rate);out.damage=spec.damage;out.hitbox=spec.hitbox;
    out.flags=(out.flags&~2u)|((player.focused&1u)<<1);out.movement={};
    const auto* attachment=option>=0?&player.options[option]:nullptr;
    out.movement.position=attachment?Vec3{float(attachment->x)*.0078125f,float(attachment->y)*.0078125f,0}:origin;
    if(spec.type==2){if(cache<0||player.power_step<=0)return false;lasers[cache+((packed&0xf0000)?8:0)]=u32(player.power/player.power_step);}
    out.movement.speed=spec.speed;float angle=spec.angle;
    if(angle>=1000.f&&attachment){
        angle=normalize_angle((random.signed_unit()*3.1415927410125732f)/12.f+attachment->angle);
        out.movement.speed=spec.speed+random.signed_unit()*2.f;
    }else angle=normalize_angle(angle>=995.f&&attachment?attachment->angle:angle);
    out.movement.angle=angle;
    const auto velocity=polar(angle,out.movement.speed*(rate?*rate:1.f));out.movement.velocity={velocity.x,velocity.y,0};
    return true;
}
}
#endif
