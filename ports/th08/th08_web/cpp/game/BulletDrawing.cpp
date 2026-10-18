#include "BulletDrawing.hpp"
#include "GameMath.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
#include <cmath>
namespace th08 {
namespace {
Vec3 presentation_lerp(const Vec3& previous,const Vec3& current){return {presentation::lerp_world(previous.x,current.x),presentation::lerp_world(previous.y,current.y),presentation::lerp_world(previous.z,current.z)};}
float presentation_angle(float previous,float current){
    if(presentation::world_alpha>=1)return current;if(presentation::world_alpha<=0)return previous;
    constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;float delta=current-previous;if(delta>pi)delta-=tau;else if(delta<-pi)delta+=tau;return add_angle(previous+delta*presentation::world_alpha,0);
}
bool close_enough(const Vec3& a,const Vec3& b){const float dx=a.x-b.x,dy=a.y-b.y;return dx*dx+dy*dy<16384.0f;}
}
void BulletDrawing::snapshot(){
    if(!presentation_marker.capture())return;
    previous_submitted_opacity_factor=last_submitted_opacity_factor;
    for(size_t i=0;i<previous_bullets.size();++i){const auto& b=state.bullets[i];auto& p=previous_bullets[i];p.active=b.state!=0;if(p.active){const auto& vm=b.sprites.animation[b.state>=2&&b.state<=5?b.state-1:0];p.position=b.position;p.angle=b.angle;p.age=b.active_time.current;p.state=b.state;p.script=vm.scriptIndex;p.sprite=vm.activeSpriteIndex;p.visual.capture(vm);}}
    for(size_t i=0;i<previous_lasers.size();++i){const auto& l=state.lasers[i];auto& p=previous_lasers[i];p.active=l.in_use!=0;if(p.active){const auto& vm=l.animation[0];p.position=l.position;p.angle=l.angle;p.start_offset=l.start_offset;p.end_offset=l.end_offset;p.scale_x=vm.scale.x;p.scale_y=vm.scale.y;p.age=l.timer.current;p.state=l.state;p.script=vm.scriptIndex;p.color=l.color;for(u32 n=0;n<2;++n)p.visual[n].capture(l.animation[n]);}}
}
void BulletDrawing::bullet(BulletState& b,const Vec2& origin,u32 previous_opacity_factor,u32 current_opacity_factor){
    TH08_AUDIT_SCOPE(Bullet,&b,b.active_time.current,u32(b.state));
    auto& source=b.sprites.animation[b.state>=2&&b.state<=5?b.state-1:0];
    if(!presentation::render_only){
        source.pos={Scalar::add(origin.x,b.position.x),Scalar::add(origin.y,b.position.y),.05f};
        source.color1.d3dColor=i32((u32(source.color1.d3dColor)&0xff000000)|0xffffff);
        if(source.type){source.rotation.z=add_angle(Scalar::add(1.5707964f,b.angle),0);source.updateRotation=true;}
    }
    AnmVm copy;AnmVm* vm=&source;if(presentation::render_only){copy=source;vm=&copy;}Vec3 position=b.position;float angle=b.angle;
    if(presentation::active){const size_t index=size_t(&b-state.bullets);const auto& p=previous_bullets[index];if(p.active&&p.state==b.state&&p.script==source.scriptIndex&&b.active_time.current>=p.age&&close_enough(p.position,b.position)){position=presentation_lerp(p.position,b.position);angle=presentation_angle(p.angle,b.angle);p.visual.apply(source,*vm,presentation::world_alpha);p.visual.apply_active_opacity(source,*vm,presentation::world_alpha,presentation::VisualSample::Opacity,previous_opacity_factor,current_opacity_factor);}}
    vm->pos={Scalar::add(origin.x,position.x),Scalar::add(origin.y,position.y),.05f};
    vm->color1.d3dColor=i32((u32(vm->color1.d3dColor)&0xff000000)|0xffffff);
    if(vm->type){vm->rotation.z=add_angle(Scalar::add(1.5707964f,angle),0);vm->updateRotation=true;}
    actions.draw(*vm);
}
void BulletDrawing::laser(LaserState& l,const Vec2& origin){
    TH08_AUDIT_SCOPE(Laser,&l,l.timer.current,u32(l.state));
    const auto current_position=[&](float distance,float depth){const float c=cosine(l.angle).to_float(),s=sine(l.angle).to_float();return Vec3{(number(c)*number(distance)+number(l.position.x)).to_float(),(number(s)*number(distance)+number(l.position.y)).to_float(),depth};};
    const auto current_offset=[&](AnmVm& vm){vm.pos.x=Scalar::add(origin.x,vm.pos.x);vm.pos.y=Scalar::add(origin.y,vm.pos.y);};
    if(!presentation::render_only){
        const float midpoint=((number(l.end_offset)-number(l.start_offset))/number(2)+number(l.start_offset)).to_float();auto& body=l.animation[0];body.pos=current_position(midpoint,.06f);l.color=-1;current_offset(body);
        if((l.start_offset<16||l.speed==0)&&(!l.unknown599||l.state)){auto& cap=l.animation[1];cap.pos=current_position(l.start_offset,.05f);cap.color1=body.color1;cap.flag6=1;cap.color1.a=255;cap.scale.x=((number(l.width)/number(10))*((number(16)-number(l.start_offset))/number(16))).to_float();cap.scale.y=cap.scale.x;if(cap.scale.y<=0){cap.scale.x=Scalar::div(l.width,10);cap.scale.y=cap.scale.x;}current_offset(cap);}
    }
    Vec3 laser_position=l.position;float angle=l.angle,start_offset=l.start_offset,end_offset=l.end_offset,scale_x=l.animation[0].scale.x,scale_y=l.animation[0].scale.y;bool smooth_laser=false;
    if(presentation::active){const size_t index=size_t(&l-state.lasers);const auto& p=previous_lasers[index];smooth_laser=p.active&&p.state==l.state&&p.script==l.animation[0].scriptIndex&&p.color==l.color&&l.timer.current>=p.age&&close_enough(p.position,l.position);if(smooth_laser){laser_position=presentation_lerp(p.position,l.position);angle=presentation_angle(p.angle,l.angle);start_offset=presentation::lerp_world(p.start_offset,l.start_offset);end_offset=presentation::lerp_world(p.end_offset,l.end_offset);if(std::fabs(scale_x-p.scale_x)<8.0f)scale_x=presentation::lerp_world(p.scale_x,scale_x);if(std::fabs(scale_y-p.scale_y)<64.0f)scale_y=presentation::lerp_world(p.scale_y,scale_y);}}
    const float cosine_value=cosine(angle).to_float(),sine_value=sine(angle).to_float();
    const float midpoint=((number(end_offset)-number(start_offset))/number(2)+number(start_offset)).to_float();
    const auto position=[&](float distance,float depth){return Vec3{(number(cosine_value)*number(distance)+number(laser_position.x)).to_float(),(number(sine_value)*number(distance)+number(laser_position.y)).to_float(),depth};};
    const auto offset=[&](AnmVm& vm){vm.pos.x=Scalar::add(origin.x,vm.pos.x);vm.pos.y=Scalar::add(origin.y,vm.pos.y);};
    AnmVm body_copy,cap_copy;AnmVm* body=&l.animation[0];if(presentation::render_only){body_copy=*body;body=&body_copy;
        // LaserRuntime directly owns the beam's angle. ANM angular-velocity
        // flags alone cannot authorize this continuous owner-driven rotation.
        if(smooth_laser)previous_lasers[size_t(&l-state.lasers)].visual[0].apply(l.animation[0],*body,presentation::world_alpha,presentation::VisualSample::Attributes,presentation::VisualSample::Rotation);
    }body->pos=position(midpoint,.06f);if(smooth_laser){body->scale.x=scale_x;body->scale.y=scale_y;body->updateScale=true;}if(!presentation::render_only)l.color=-1;offset(*body);actions.draw(*body);
    if((start_offset<16||l.speed==0)&&(!l.unknown599||l.state)){
        AnmVm* cap=&l.animation[1];if(presentation::render_only){cap_copy=*cap;cap=&cap_copy;if(smooth_laser)previous_lasers[size_t(&l-state.lasers)].visual[1].apply(l.animation[1],*cap,presentation::world_alpha);}cap->pos=position(start_offset,.05f);cap->color1=body->color1;cap->flag6=1;cap->color1.a=255;
        cap->scale.x=((number(l.width)/number(10))*((number(16)-number(start_offset))/number(16))).to_float();cap->scale.y=cap->scale.x;
        if(cap->scale.y<=0){cap->scale.x=Scalar::div(l.width,10);cap->scale.y=cap->scale.x;}
        offset(*cap);actions.draw(*cap);
    }
}
bool BulletDrawing::draw(u32 flags,const Vec2& origin){
    const bool tint=flags&1024;const u32 current_opacity_factor=tint?255u:128u;if(tint)actions.tint(0xfff01010);actions.items();
    for(auto& l:state.lasers)if(l.in_use)laser(l,origin);
    for(auto* layer:state.layers){u32 count=0;for(auto* b=layer;b;b=b->next_in_layer){if(++count>1536){if(tint)actions.clear_tint();return false;}bullet(*b,origin,previous_submitted_opacity_factor,current_opacity_factor);}}
    actions.effects();if(tint)actions.clear_tint();if(!presentation::render_only)last_submitted_opacity_factor=current_opacity_factor;return true;
}
}
