#include "PlayerShots.hpp"
#include "GameMath.hpp"
#include <cmath>
#include <algorithm>
namespace th09 {
namespace {
constexpr float half_pi=1.5707963705062866f;
Vec2 polar(float angle,float speed){return {float(std::cos(double(angle))*double(speed)),float(std::sin(double(angle))*double(speed))};}
float angle_to(const Vec3& from,const Vec3& to){const float x=to.x-from.x,y=to.y-from.y;return x==0&&y==0?half_pi:float(std::atan2(double(y),double(x)));}
bool overlap(const Vec3& p,const Vec3& extent,const Vec3& q,const Vec3& size){
    return q.y-size.y*.5f<=extent.y*.5f+p.y&&q.x-size.x*.5f<=extent.x*.5f+p.x&&p.y-extent.y*.5f<=size.y*.5f+q.y&&p.x-extent.x*.5f<=size.x*.5f+q.x;
}
}
void PlayerShots::initialize(PlayerShot& s,const ShotDefinition& d){
    s.position=d.option?option_positions[d.option-1]:player_position;s.position.x=d.offset.x+s.position.x;s.position.y=d.offset.y+s.position.y;s.position.z=.495f;
    s.hitbox={d.hitbox.x,d.hitbox.y,1};s.angle=d.angle;s.speed=d.speed;s.velocity=polar(d.angle,d.speed);s.time.reset();s.hits=0;s.type=d.type;s.damage=d.damage;s.animation_index=d.animation;
    if(d.sound>=0)actions.play_positioned_sound(d.sound,player_position.x);actions.start_animation(s.animation,d.animation+5);s.hit_flash=0;
}
bool PlayerShots::create(PlayerShot& s,const ShotDefinition& d,i32 frame){
    if(d.creation==ShotCreation::lyrica_normal){if(actions.effect_active(5)||frame!=d.frame)return false;before_write(s);initialize(s,d);return true;}
    if(frame!=d.frame)return false;
    if(d.creation==ShotCreation::lyrica_effect){actions.effect(35,player_position,5);return false;}
    // A dormant slot can still contain meaningful VM/trail bytes. Capture it
    // before the first actual write, including Marisa's pre-initialize fields.
    // Definitions that do not create a shot never pay for a cold-slot copy.
    before_write(s);
    switch(d.creation){
    case ShotCreation::marisa_beam:
        beam_time.reset(30);beam=&s;s.option=d.option;s.offset=d.offset;s.trail_length=15;initialize(s,d);s.type=2;for(auto& p:s.trail)p.x=-999;s.position.x=-999;return true;
    case ShotCreation::sakuya_delayed:initialize(s,d);s.velocity={0,0};s.angle=d.angle;return true;
    case ShotCreation::youmu_wave:initialize(s,d);s.velocity={0,-24};player_scale={.5f,.5f};return true;
    case ShotCreation::lyrica_effect_shot:initialize(s,d);s.position=actions.effect_position(5);s.angle=-half_pi;s.velocity={0,-d.speed};return true;
    case ShotCreation::komachi_aim:{const float heading=target.x<-144?-half_pi:angle_to(player_position,target)+d.angle;initialize(s,d);s.velocity=polar(heading,d.speed);s.angle=heading;return true;}
    default:initialize(s,d);return true;
    }
}
void PlayerShots::fire(const ShotResource& resource,u32 set,i32 frame){
    if(set>=resource.sets.size())return;u32 next=0;const auto& definitions=resource.sets[set];
    for(auto& s:shots){if(s.state)continue;
        while(next<definitions.size()){const auto& d=definitions[next++];if(create(s,d,frame)){s.animation.zWriteDisabled=1;s.state=1;s.definition=&d;break;}}
        if(next>=definitions.size())return;
    }
}
void PlayerShots::deactivate_all(){
    for(auto& s:shots){if(s.state)before_write(s);s.state=0;}
}
bool PlayerShots::update_special(PlayerShot& s,const FrameTiming& timing){
    if(!s.definition)return false;
    switch(s.definition->update){
    case ShotUpdate::reimu_homing:
        if(s.state==1&&s.time.current>39&&s.time.current!=s.time.previous){
            if(target.x<=-900){if(s.speed<10){const auto old=s.velocity;s.speed+=.3333333432674408f;const float inverse=1.f/std::sqrt(old.x*old.x+old.y*old.y);s.velocity={s.speed*inverse*old.x,s.speed*inverse*old.y};}}
            else {const float dx=target.x-s.position.x,dy=target.y-s.position.y;const float distance=std::max(std::sqrt(dx*dx+dy*dy)/(s.speed*.25f),1.f),inverse=1.f/distance;
                const Vec2 v{dx*inverse+s.velocity.x,inverse*dy+s.velocity.y};const float length=std::sqrt(v.x*v.x+v.y*v.y);s.speed=std::clamp(length,1.f,10.f);const float scale=(1.f/length)*s.speed;s.velocity={scale*v.x,scale*v.y};}
        }s.angle=float(std::atan2(double(s.velocity.y),double(s.velocity.x)));break;
    case ShotUpdate::marisa_beam:
        if(beam_time.current<=0){beam_time.reset();beam=nullptr;s.state=0;return true;}
        s.position=player_position;s.position.z=.44f;s.position.x=s.offset.x+s.position.x;s.hitbox.y=s.position.y;s.animation.scale.y=s.position.y*.0714285746216774f;s.position.y*=.5f;
        for(u32 i=31;i>0;--i){s.trail[i]=s.trail[i-1];s.trail[i].y-=1;}
        for(i32 i=0;i<s.trail_length&&i*2<32;++i)if(s.trail[i*2].x>=-900)areas.box(s.trail[i*2],4,448,1,1,0,AttackAreaKind::special_damage);
        s.trail[0]=s.position;beam_time.decrement(1,timing);break;
    case ShotUpdate::sakuya_turn:
        if(s.state==1&&s.time.current==30){float heading=-half_pi;if(target.x>=-144)heading=angle_to(s.position,target)+s.angle;s.angle=float(add_angle(heading,0));s.velocity=polar(s.angle,s.speed);}break;
    case ShotUpdate::youmu_wave:
        s.position=player_position;s.position.y+=s.velocity.y;s.velocity.y-=1;if(s.time.current>=15)player_scale={1,1};
        if(s.state==1){const float width=s.time.value()+80;const Vec3 p{s.position.x-(width-96.f)*.5f,s.position.y-16.f,s.position.z};areas.box(p,width,40,15,0,0,AttackAreaKind::damage);areas.box(p,width,30,0,0,0,AttackAreaKind::cancel);}break;
    case ShotUpdate::medicine_clockwise:case ShotUpdate::medicine_counterclockwise:
        s.angle=float(add_angle(s.angle,s.definition->update==ShotUpdate::medicine_clockwise?.07853981852531433f:-.07853981852531433f));s.speed*=.96f;s.velocity=polar(s.angle,s.speed);break;
    default:break;
    }return false;
}
void PlayerShots::update(const FrameTiming& timing,u32 field_flags){
    if(field_flags&1)return;
    for(auto& s:shots){if(!s.state)continue;if(update_special(s,timing)){s.state=0;continue;}
        s.position.x=timing.rate*s.velocity.x+s.position.x;s.position.y=timing.rate*s.velocity.y+s.position.y;
        if(s.type!=2&&s.animation.loadedSprite){const float w=s.animation.loadedSprite->widthPx,h=s.animation.loadedSprite->heightPx;
            if(s.position.x+w*.5f<-144||144<s.position.x-w*.5f||s.position.y+h*.5f<0||448<s.position.y-h*.5f)s.state=0;}
        if(actions.advance_animation(s.animation))s.state=0;s.time.tick(timing);
    }
}
void PlayerShots::draw_special(PlayerShot& s,const PlayfieldGeometry& geometry){
    if(s.definition&&s.definition->drawing==ShotDrawing::marisa_beam){
        const u8 alpha=s.animation.color1.a;const i32 count=i32(s.trail_length)*2;
        for(i32 i=0;i<count&&i<32;++i){if(s.trail[i].x==-999)break;s.animation.pos=geometry.to_screen(s.trail[i]);s.animation.pos.z=0;s.animation.color1.a=u8(i32(alpha)-i*i32(alpha)/count);actions.draw_animation(s.animation,false);}s.animation.color1.a=alpha;
    }else if(s.definition&&s.definition->drawing==ShotDrawing::layered_trail){
        const auto color=s.animation.color1;for(i32 i=0;i<4;++i){if(s.trail[i].x==-999)break;s.animation.color1.r=u8(i*i32(color.r)/4+63);s.animation.color1.g=u8(i*i32(color.g)/4+63);s.animation.color1.a=u8(i*i32(color.a)/4+63);s.animation.scale={3.f-float(i)*.3333333432674408f,4.f-float(i)*.3333333432674408f};actions.draw_animation(s.animation,false);}s.animation.color1=color;
    }else actions.draw_animation(s.animation,false);
}
void PlayerShots::draw(const PlayfieldGeometry& geometry,bool fading){
    for(auto& s:shots){if(s.state!=(fading?2:1))continue;auto& v=s.animation;if(v.type){v.rotation.z=s.angle;v.updateRotation=1;}v.pos=geometry.to_screen(s.position);v.pos.z=fading?.2f:.4f;
        if(s.hit_flash){v.color1.r=255;v.color1.g=v.color1.b=64;}if(fading)actions.draw_animation(v,true);else draw_special(s,geometry);}
}
i32 PlayerShots::hit(const Vec3& p,const Vec3& extent,const Timer& protection,i32& direct,u32* token,i32& special){
    if(protection.current==protection.previous)return 0;if(token)*token=0;i32 total=0;
    for(auto& s:shots){if(s.state!=1||!overlap(p,extent,s.position,s.hitbox)||(s.type==2&&s.time.current%2)||(s.definition&&s.definition->hit==ShotHit::area_only))continue;
        total=wrapping_add(total,s.damage);if(s.type==2||s.type==3)continue;
        const float rotation=s.animation.rotation.z;actions.start_animation(s.animation,s.animation_index+6);s.animation.rotation.z=rotation;s.position.z=.1f;s.state=2;s.velocity.x*=.125f;s.velocity.y*=.125f;
        if(s.type==4){const Vec3 origin=shots[0].position;actions.effect(34,origin,6);areas.circle(origin,0,1.6f,6,70,0,AttackAreaKind::damage);actions.play_sound(16,side?500:-500);}
    }
    direct=total;return wrapping_add(total,areas.damage(p,extent,special));
}
}
