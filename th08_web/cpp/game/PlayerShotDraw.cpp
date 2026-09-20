#include "PlayerShots.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
#include "GameMath.hpp"
namespace th08 {
namespace {
Vec3 lerp_position(const Vec3& before,const Vec3& current){return {presentation::lerp_world(before.x,current.x),presentation::lerp_world(before.y,current.y),presentation::lerp_world(before.z,current.z)};}
float lerp_angle(float before,float current){if(presentation::world_alpha>=1)return current;if(presentation::world_alpha<=0)return before;constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;float delta=current-before;if(delta>pi)delta-=tau;else if(delta<-pi)delta+=tau;return add_angle(before+delta*presentation::world_alpha,0);}
}
void PlayerShots::draw_trail(PlayerShot& shot,const Vec2& offset){
    const u8 alpha=shot.animation.color1.a;const u32 faded=(u32(alpha)*3)>>2;
    if(shot.interval>16){failure=Failure::InvalidInterval;return;}
    for(i32 i=0;i<shot.interval*2&&shot.history[i].x!=-999;i+=2){
        shot.animation.pos=shot.history[i];if(i)shot.animation.color1.a=u8(faded-((faded/2)*u32(i))/u32(shot.interval));
        shot.animation.pos.x=Scalar::add(offset.x,shot.animation.pos.x);shot.animation.pos.y=Scalar::add(offset.y,shot.animation.pos.y);
        if(state.youkai_bonus){shot.animation.color1.r=255;shot.animation.color1.g=shot.animation.color1.b=64;}
        if(actions)actions->draw(shot.animation,false);
    }
    shot.animation.color1.a=alpha;
}
void PlayerShots::draw(bool impact,const Vec2& offset){
    failure=Failure::None;if(!actions){failure=Failure::MissingActions;return;}
    for(size_t i=0;i<128;++i){auto& source=state.shots[i];if(source.state!=(impact?2:1))continue;
        auto& drawn=authored_colors[i];
        if(!presentation::render_only)drawn={source.animation.color1,source.timer.current,source.state,source.kind,source.animation.scriptIndex,true};
        PlayerShot copy;PlayerShot* shot=&source;
        if(presentation::render_only){copy=source;shot=&copy;if(drawn.valid&&drawn.state==source.state&&drawn.kind==source.kind&&drawn.script==source.animation.scriptIndex&&drawn.age==source.timer.current)copy.animation.color1=drawn.color;}
        TH08_AUDIT_SCOPE(PlayerShot,&source,source.timer.current,u32(source.state));
        if(presentation::active){const auto& before=presentation_previous[i];const float dx=source.position.x-before.position.x,dy=source.position.y-before.position.y;if(before.active&&before.state==source.state&&before.kind==source.kind&&source.timer.current>=before.age&&dx*dx+dy*dy<16384.0f){shot->position=lerp_position(before.position,source.position);shot->angle=lerp_angle(before.angle,source.angle);before.visual.apply(shot->animation,shot->animation,presentation::world_alpha);
            if(presentation::render_only&&source.draw==ShotDraw::Laser)for(u32 n=0;n<32;++n)
                if(before.history[n].x>=-900&&source.history[n].x>=-900&&presentation::VisualSample::near(before.history[n],source.history[n]))shot->history[n]=lerp_position(before.history[n],source.history[n]);
        }}
        if(shot->animation.type){shot->animation.rotation.z=shot->angle;shot->animation.updateRotation=true;}
        shot->animation.pos={Scalar::add(offset.x,shot->position.x),Scalar::add(offset.y,shot->position.y),impact?.2f:.4f};
        if(shot->gauge_bonus){shot->animation.color1.r=255;shot->animation.color1.g=shot->animation.color1.b=64;}
        actions->draw(shot->animation,impact);if(!impact&&shot->draw==ShotDraw::Laser)draw_trail(*shot,offset);
    }
}
}
