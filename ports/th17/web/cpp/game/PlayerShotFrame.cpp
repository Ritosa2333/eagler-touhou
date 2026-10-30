#include "PlayerShots.hpp"
#include "AnmVisualState.hpp"
#include "AnmCoordinates.hpp"
#include <cmath>
namespace th17 {
namespace {
bool targetable(const PlayerShotTarget& e){return !(e.flags&0xc000021);}
float difference(float target,float angle){const float d=float(target-angle);return d>3.1415927f?float(target-float(angle+6.2831855f)):float(angle-target)>3.1415927f?float(target-float(angle-6.2831855f)):d;}
}
bool PlayerShots::laser_update(PlayerShot& shot,const ShotSpec& s,bool focus_turn){
    const u32 low=u8(s.option)&15,lock=s.option>=16?u32(s.option)>>4:low;
    if(lock>=context.laser_power.size()){error="Laser option unavailable";return false;}
    const Vec3 origin=(!focus_turn&&low==0)?context.player.position:option_origin(s.option);shot.motion.position=origin;
    const float old=shot.motion.angle,target=normalize_angle(focus_turn&&context.player.focus?-1.5707964f:s.angle);
    shot.motion.angle=std::abs(normalize_angle(difference(old,target)))>=.001f?normalize_angle(target):normalize_angle(float(old+normalize_angle(float(normalize_angle(difference(target,old))*.1f))));
    if(shot.state==2)return true;
    if(sound&&!sound(20,context.player.position.x,SoundAction::pan)){error="Laser pan update failed";return false;}
    auto* source=damage.find(shot.damage_source);if(!source){error="Laser damage source unavailable";return false;}
    if(shot.hitbox.x<512){shot.hitbox.x=float(shot.hitbox.x+18);source->size.x=shot.hitbox.x;}
    const float distance=float(shot.hitbox.x*.5f),x=float(std::cos(double(shot.motion.angle))*double(distance)),y=float(std::sin(double(shot.motion.angle))*double(distance));source->motion.position={float(x+origin.x),float(y+origin.y),0};
    if(auto* vm=animations.registry.find(shot.animation)){vm->visual.flags|=8|16;vm->visual.sprite_size.x=shot.hitbox.x;vm->visual.uv_scale.x=float(shot.hitbox.x*.001953125f);}else shot.animation=0;
    if(shot.contact_this_frame==0){if(shot.state!=1)return true;if(shot.contact_effect==1){if(!animations.interrupt(shot.animation,3)){error=animations.error;return false;}shot.contact_effect=0;}}
    const bool roaring=context.roaring_flags&2,focus_allowed=roaring&&(context.roaring_kind==1||context.roaring_kind==3);
    const bool option_available=roaring||low==0||s.option-1<context.player.option_count;
    const bool keep= context.shoot_frame>=0&&option_available&&context.player_state!=2&&context.player_state!=4&&
        (focus_allowed||context.player.focus==0)&&!(context.player.behavior_flags&0x40)&&!context.bomb_active&&context.enemy_scene&&
        (!focus_turn||(context.power_step!=0&&context.laser_power[lock]==context.power/context.power_step&&context.player.focus==0));
    if(shot.state==1&&!keep){source->flags&=~1u;shot.state=2;if(!animations.interrupt(shot.animation,1)){error=animations.error;return false;}context.laser_power[lock]=0;if(sound&&!sound(20,0,SoundAction::stop)){error="Laser audio stop failed";return false;}}
    shot.contact_this_frame=0;return true;
}
bool PlayerShots::update_behavior(PlayerShot& shot,const ShotSpec& s){
    switch(s.update_kind){
    case 0:return true;
    case 1:{
        if(shot.state==2)return true;
        if(!context.enemy_scene)shot.target=0;
        else if(shot.target==0){float nearest=65536.f;for(auto* enemy:context.targets)if(enemy&&targetable(*enemy)){const float dx=float(shot.motion.position.x-enemy->position.x),dy=float(shot.motion.position.y-enemy->position.y),distance=float(float(dy*dy)+float(dx*dx));if(distance<nearest){nearest=distance;shot.target=i32(enemy->id);}}}
        PlayerShotTarget* target=nullptr;for(auto* e:context.targets)if(e&&e->id==u32(shot.target)){target=e;break;}if(!target)shot.target=0;
        if(target&&targetable(*target)){
            const float dx=float(target->position.x-shot.motion.position.x),dy=float(target->position.y-shot.motion.position.y),angle=float(std::atan2(double(dy),double(dx))),d=difference(angle,shot.motion.angle);
            if(shot.age.current>=60){shot.motion.speed=float(shot.motion.speed+.2f);return true;}
            float speed=shot.motion.speed;if(std::abs(d)>=.7853982f){speed=float(speed-.2f);if(speed<4)speed=4;}else if(std::abs(d)<.2617994f){speed=float(speed+.2f);if(speed>16)speed=16;}shot.motion.set_angle(normalize_angle(float(shot.motion.angle+float(d*.08f))));shot.motion.speed=speed;
        }else{shot.motion.speed=float(shot.motion.speed+.1f);if(shot.motion.speed>16)shot.motion.speed=16;}return true;
    }
    case 2:return laser_update(shot,s,true);
    case 3:if(shot.state==1)shot.motion.speed=float(shot.motion.speed+.3f);return true;
    case 4:if(shot.state==1)shot.motion.speed=float(shot.motion.speed*.8f);return true;
    case 5:return laser_update(shot,s,false);
    case 6:shot.damage=wrapping_add(shot.damage,2);return true;
    default:error="Unsupported player shot update callback";return false;
    }
}
bool PlayerShots::update_one(PlayerShot& shot){
    const auto* spec=specification(shot.specification);if(!spec){error="Active shot specification unavailable";return false;}if(!update_behavior(shot,*spec))return false;shot.motion.integrate(rate);shot.motion.advance(rate);auto* vm=animations.registry.find(shot.animation);auto* source=damage.find(shot.damage_source);
    if(!vm){shot.state=0;shot.animation=0;if(source)source->flags&=~1u;return true;}bool inside=spec->type==2||shot.age.current<15;
    if(!inside){Vec3 p[4];if(!anm_quad_positions(*vm,p)){error="Player shot geometry unavailable";return false;}for(const auto& point:p)if(context.screen_origin.x<point.x&&point.x<float(context.screen_origin.x+384)&&context.screen_origin.y<point.y&&point.y<float(context.screen_origin.y+448))inside=true;}
    if(!inside){if(!animations.retire(shot.animation)){error=animations.error;return false;}shot.state=0;if(source)source->flags&=~1u;return true;}
    if(source&&(shot.flags&1)){source->motion.position=shot.motion.position;source->angle=shot.motion.angle;source->size=shot.hitbox;source->damage=shot.damage;}vm->visual.translation=shot.motion.position;if(vm->visual.render_flags&0x80){vm->visual.flags|=4;vm->variables.rotation.z=shot.motion.angle;}shot.age.tick(&rate);return true;
}
bool PlayerShots::update(){for(auto& shot:shots)if(shot.state!=0&&!update_one(shot))return false;return true;}
}
