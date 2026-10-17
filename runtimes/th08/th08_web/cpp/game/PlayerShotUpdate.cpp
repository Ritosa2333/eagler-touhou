#include "PlayerShots.hpp"
#include "BulletMotion.hpp"
#include <cmath>
namespace th08 {
void PlayerShots::snapshot_presentation(){
    if(!presentation_marker.capture())return;
    for(size_t i=0;i<presentation_previous.size();++i){const auto& shot=state.shots[i];auto& before=presentation_previous[i];before.active=shot.state!=0;
        if(before.active){before.position=shot.position;before.angle=shot.angle;before.age=shot.timer.current;before.state=shot.state;before.kind=shot.kind;before.visual.capture(shot.animation);
            const auto& drawn=authored_colors[i];
            if(drawn.valid&&drawn.state==shot.state&&drawn.kind==shot.kind&&drawn.script==shot.animation.scriptIndex&&shot.timer.current>=drawn.age)before.visual.color1=drawn.color;
            if(shot.draw==ShotDraw::Laser)std::copy(std::begin(shot.history),std::end(shot.history),std::begin(before.history));
        }
    }
}
namespace {
Extended length(float x,float y){return number((number(x)*number(x)+number(y)*number(y)).to_float()).square_root();}
void color(PlayerShot& shot,bool bonus){shot.animation.color1.r=255;shot.animation.color1.g=bonus?208:255;shot.animation.color1.b=bonus?176:255;}
}
bool PlayerShots::update_callback(PlayerShot& shot,ShotUpdate kind){
    failure=Failure::None;
    switch(kind){
    case ShotUpdate::Homing:
        if(shot.state==1){
            if(state.homing_target.x>-100&&shot.timer.current<40&&shot.timer.changed()){
                float x=Scalar::sub(state.homing_target.x,shot.position.x),y=Scalar::sub(state.homing_target.y,shot.position.y);
                float time=(length(x,y)/(number(shot.speed)/number(4))).to_float();if(time<1)time=1;
                x=(number(x)/number(time)+number(shot.velocity.x)).to_float();y=(number(y)/number(time)+number(shot.velocity.y)).to_float();
                const Extended magnitude=length(x,y);const float divisor=magnitude.to_float();shot.speed=number(10)<magnitude?10:divisor;if(shot.speed<1)shot.speed=1;
                shot.velocity.x=(number(x)*number(shot.speed)/number(divisor)).to_float();shot.velocity.y=(number(y)*number(shot.speed)/number(divisor)).to_float();
            }else if(shot.speed<10){
                shot.speed=Scalar::add(shot.speed,.3333333432674408f);const float x=shot.velocity.x,y=shot.velocity.y,divisor=length(x,y).to_float();
                shot.velocity.x=(number(x)*number(shot.speed)/number(divisor)).to_float();shot.velocity.y=(number(y)*number(shot.speed)/number(divisor)).to_float();
            }
        }
        shot.angle=Extended::from_double(std::atan2(double(shot.velocity.y),double(shot.velocity.x))).to_float();return false;
    case ShotUpdate::Accelerating:
        if(shot.state==1)shot.velocity.y=(number(shot.velocity.y)-(rng.range(.1f)+number(.27f))).to_float();return false;
    case ShotUpdate::PlayerLaser:{
        if(shot.laser_slot<0||shot.laser_slot>=3){failure=Failure::InvalidLaserSlot;return true;}auto& slot=state.lasers[shot.laser_slot];
        if(slot.shot!=&shot&&shot.animation.stopped)shot.animation.SetInterrupt(1);
        if((state.gui_blocked||state.bomb||(state.game_flags&0x2000))&&slot.timer.current>20)slot.timer.set(20);
        if(slot.timer.current<=0){slot.timer.set(0);slot.shot=nullptr;shot.state=0;return true;}
        if(slot.timer.current<=70&&shot.animation.stopped)shot.animation.SetInterrupt(1);
        shot.position.x=Scalar::add(shot.position.x,shot.velocity.z);shot.position.z=.44f;if(state.player_state==2)return true;
        shot.animation.scale.y=Scalar::div(shot.position.y,14);shot.size.y=shot.position.y;shot.position.y=Scalar::div(shot.position.y,2);
        if(slot.timer.current<100)slot.timer.decrement(1,timing);color(shot,state.youkai_bonus);return false;
    }
    case ShotUpdate::OptionLaser:{
        if(shot.laser_slot<0||shot.laser_slot>=3){failure=Failure::InvalidLaserSlot;return true;}auto& slot=state.lasers[shot.laser_slot];
        if(slot.shot!=&shot||state.gui_blocked||state.shooting_timer.current<0||state.player_state==2||state.bomb||(state.game_flags&0x2000)){
            shot.animation.SetInterrupt(1);slot.shot=nullptr;shot.update=ShotUpdate::None;
        }
        if(!state.option_active){slot.shot=nullptr;return true;}
        if(shot.interval>16){failure=Failure::InvalidInterval;return true;}
        for(i32 i=0;i<shot.interval;++i)if(shot.history[i*2].x>=-900){if(!actions){failure=Failure::MissingActions;return true;}actions->damage_region(shot.history[i*2],{16,448},1,0,true);}
        for(i32 i=31;i>0;--i){shot.history[i]=shot.history[i-1];shot.history[i].y=Scalar::sub(shot.history[i].y,1);}
        shot.history[0]=shot.position;shot.position=state.options[0];shot.position.z=.44f;shot.size.y=448;shot.position.y=Scalar::sub(shot.position.y,208);color(shot,state.youkai_bonus);return false;
    }
    default:return false;
    }
}
bool PlayerShots::update(){
    failure=Failure::None;snapshot_presentation();if(state.game_flags&0x400)return true;
    for(auto& shot:state.shots){if(!shot.state)continue;
        if(update_callback(shot,shot.update)){if(failure!=Failure::None)return false;shot.state=0;continue;}
        shot.position.x=(number(timing.rate)*number(shot.velocity.x)+number(shot.position.x)).to_float();shot.position.y=(number(timing.rate)*number(shot.velocity.y)+number(shot.position.y)).to_float();
        if(shot.kind!=4&&shot.kind!=5){if(!shot.animation.loadedSprite){failure=Failure::MissingSprite;return false;}const auto& sprite=*shot.animation.loadedSprite;if(!bullet_in_view(shot.position,sprite.widthPx,sprite.heightPx))shot.state=0;}
        if(!actions){failure=Failure::MissingActions;return false;}if(actions->step_animation(shot.animation))shot.state=0;shot.timer.tick(timing);
    }
    return true;
}
}
