#include "PlayerShots.hpp"
#include "AnmVisualState.hpp"
#include "AnmCoordinates.hpp"
#include <cmath>
namespace th15 {
namespace {bool targetable(const EnemyState& e){return !(e.flags&0x21)&&!(e.flags&0xc000000);}}
bool PlayerShots::update_behavior(PlayerShot& shot,const ShotSpec& s){
    switch(s.update_kind){
    case 0:return true;
    case 1:{
        if(shot.state==2)return true;auto* world=context.enemies;if(!world)shot.target=0;else if(shot.target==0){float nearest=65536.f;for(auto* enemy:world->enemies)if(targetable(*enemy)){const float dx=float(shot.motion.position.x-enemy->motion.position.x),dy=float(shot.motion.position.y-enemy->motion.position.y),distance=float(float(dx*dx)+float(dy*dy));if(distance<nearest){nearest=distance;shot.target=i32(enemy->id);}}}
        auto* target=world?world->find(u32(shot.target)):nullptr;if(!target)shot.target=0;
        if(target&&targetable(*target)){
            const float dx=float(target->motion.position.x-shot.motion.position.x),dy=float(target->motion.position.y-shot.motion.position.y),angle=float(std::atan2(double(dy),double(dx)));float difference=float(angle-shot.motion.angle);if(difference>3.1415927f)difference=float(angle-float(shot.motion.angle+6.2831855f));else if(float(shot.motion.angle-angle)>3.1415927f)difference=float(angle-float(shot.motion.angle-6.2831855f));
            if(shot.age.current>=60){shot.motion.speed=float(shot.motion.speed+.2f);return true;}float speed=shot.motion.speed;if(std::abs(difference)>=.7853982f){speed=float(speed-.2f);if(speed<4)speed=4;}else if(std::abs(difference)<.2617994f){speed=float(speed+.2f);if(speed>16)speed=16;}shot.motion.set_angle(float(shot.motion.angle+float(difference*.08f)));shot.motion.speed=speed;
        }else{shot.motion.speed=float(shot.motion.speed+.1f);if(shot.motion.speed>16)shot.motion.speed=16;}return true;
    }
    case 2:{
        if(s.option<1||s.option>8){error="Laser option unavailable";return false;}const auto& fixed=context.player.options[u32(s.option-1)].position;const Vec3 origin={float(fixed.x)*.0078125f,float(fixed.y)*.0078125f,0};shot.motion.position=origin;shot.motion.angle=normalize_angle(s.angle);if(shot.state==2)return true;
        if(sound&&!sound(20,context.player.position.x,SoundAction::pan)){error="Laser pan update failed";return false;}auto* source=damage.find(shot.damage_source);if(!source){error="Laser damage source unavailable";return false;}if(shot.hitbox.x<512){shot.hitbox.x=float(shot.hitbox.x+18);source->size.x=shot.hitbox.x;}
        const float distance=float(shot.hitbox.x*.5f),x=float(std::cos(double(shot.motion.angle))*double(distance)),y=float(std::sin(double(shot.motion.angle))*double(distance));source->motion.position={float(x+origin.x),float(y+origin.y),0};
        if(auto* vm=animations.registry.find(shot.animation)){vm->visual.flags|=8|16;vm->visual.sprite_size.x=shot.hitbox.x;vm->visual.uv_scale.x=float(shot.hitbox.x*.001953125f);}else shot.animation=0;
        if(shot.contact_this_frame==0){if(shot.state!=1)return true;if(shot.contact_effect==1){if(!animations.interrupt(shot.animation,3)){error=animations.error;return false;}shot.contact_effect=0;}}
        if(shot.state==1&&(context.shoot_frame<0||context.laser_power[0]<=s.option-1||context.power_step==0||context.laser_power[u32(s.option)]!=context.power/context.power_step||context.player.focus!=0||context.bomb_active||!context.enemy_scene)){source->flags&=~1u;shot.state=2;if(!animations.interrupt(shot.animation,1)){error=animations.error;return false;}context.laser_power[u32(s.option)]=0;if(sound&&!sound(20,0,SoundAction::stop)){error="Laser audio stop failed";return false;}}shot.contact_this_frame=0;return true;
    }
    case 3:if(shot.state==1)shot.motion.speed=float(shot.motion.speed+1);return true;
    case 4:{
        if(shot.state==2)return true;auto* world=context.enemies;if(!(shot.flags&0x3c)){if(!world)shot.target=0;else if(shot.target==0)for(auto* enemy:world->enemies)if(targetable(*enemy)){const auto& p=enemy->motion.position;if(float(p.y-16)<=shot.motion.position.y&&shot.motion.position.y<=float(p.y+16)&&(shot.motion.position.x<=float(p.x-16)||float(p.x+16)<=shot.motion.position.x)){shot.flags=(shot.flags&~0x38u)|4;if(!animations.interrupt(shot.animation,2)){error=animations.error;return false;}shot.behavior.set(0);shot.motion.speed=0;shot.target_position=p;break;}}}
        if((shot.flags&0x3c)==4){if(shot.behavior.current==4){shot.motion.set_angle(shot.motion.position.x>shot.target_position.x?-3.1415927f:0.f);shot.motion.speed=14;shot.flags=(shot.flags&~0x34u)|8;}shot.behavior.tick(&rate);}return true;
    }
    default:error="Unsupported player shot update callback";return false;
    }
}
bool PlayerShots::update_one(PlayerShot& shot){
    const auto* spec=specification(shot.specification);if(!spec){error="Active shot specification unavailable";return false;}if(!update_behavior(shot,*spec))return false;shot.motion.integrate(rate);shot.motion.advance(rate);auto* vm=animations.registry.find(shot.animation);auto* source=damage.find(shot.damage_source);
    if(!vm){shot.state=0;shot.animation=0;if(source)source->flags&=~1u;return true;}bool inside=spec->type==2||shot.age.current<10;
    if(!inside){Vec3 p[4];if(!anm_quad_positions(*vm,p)){error="Player shot geometry unavailable";return false;}for(const auto& point:p)if(context.screen_origin.x<point.x&&point.x<float(context.screen_origin.x+384)&&context.screen_origin.y<point.y&&point.y<float(context.screen_origin.y+448))inside=true;}
    if(!inside){if(!animations.retire(shot.animation)){error=animations.error;return false;}shot.state=0;if(source)source->flags&=~1u;return true;}
    if(source&&(shot.flags&1)){source->motion.position=shot.motion.position;source->angle=shot.motion.angle;source->size=shot.hitbox;source->damage=shot.damage;}vm->presentation_motion=true;vm->visual.translation=shot.motion.position;if(vm->visual.render_flags&0x80){vm->visual.flags|=4;vm->variables.rotation.z=shot.motion.angle;}shot.age.tick(&rate);return true;
}
bool PlayerShots::update(){for(auto& shot:shots)if(shot.state!=0&&!update_one(shot))return false;return true;}
}
