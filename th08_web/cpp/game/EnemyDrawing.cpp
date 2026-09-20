#include "EnemyDrawing.hpp"
#include "GameMath.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
#include <optional>
#include <cmath>
namespace th08 {
namespace {
void rotate(AnmVm& vm,float angle){vm.rotation.z=angle;vm.updateRotation=true;}
void place(AnmVm& vm,const Vec3& p,const Vec3& extra,const Vec2& offset,float depth){
    vm.pos={Scalar::add(p.x,extra.x),Scalar::add(p.y,extra.y),depth};
    vm.pos.x=Scalar::add(offset.x,vm.pos.x);vm.pos.y=Scalar::add(offset.y,vm.pos.y);
}
Extended middle_angle(float a,float b){
    float direct,wrapped;
    if(b<=a){direct=Scalar::sub(a,b);wrapped=(number(b)+number(6.283185482025147f)-number(a)).to_float();a=b;}
    else{direct=Scalar::sub(b,a);wrapped=(number(a)+number(6.283185482025147f)-number(b)).to_float();}
    return number(wrapped<=direct?wrapped:direct)*number(.5f)+number(a);
}
struct RenderOnlyEnemyRestore {
    EclVm& enemy;bool active=false,invalid=false;EclVm::Failure failure=EclVm::Failure::None;std::array<SpriteVertex,194> vertices{};
    explicit RenderOnlyEnemyRestore(EclVm& e):enemy(e),active(presentation::render_only){if(active){invalid=e.invalid;failure=e.failure;std::memcpy(vertices.data(),e.trail.vertices,sizeof(e.trail.vertices));}}
    ~RenderOnlyEnemyRestore(){if(active){enemy.invalid=invalid;enemy.failure=failure;std::memcpy(enemy.trail.vertices,vertices.data(),sizeof(enemy.trail.vertices));}}
};
void retain_enemy_draw_state(EclVm& enemy,const Vec2& offset){
    auto& main=enemy.animation[0];const auto satellite=[&](u32 index){auto& vm=enemy.animation[index];if(vm.scriptIndex<0)return;if(vm.type)rotate(vm,index==1?enemy.direction.z:-enemy.direction.z);place(vm,enemy.resolved_position,(enemy.flags2&0x100)?main.pos2:vm.pos2,offset,.3f);};
    satellite(1);if(enemy.flags&0x2000000)rotate(main,enemy.direction.z);place(main,enemy.resolved_position,main.pos2,offset,.25f);
    auto& trail=enemy.trail;if(trail.flags&&trail.length<=96&&trail.step>0&&!(trail.flags&8)){
        const Vec2 scale=main.scale;const ZunColor color=main.color1;
        for(i32 j=trail.length-1;j>0;j-=trail.step)if(!(trail.points[j].position.x<-990)){
            if(enemy.flags&0x2000000)rotate(main,trail.points[j].angle);
            if(trail.flags&2)main.scale.x=(number(scale.x)-Extended::from_int(j)*number(scale.x)/Extended::from_int(trail.length)).to_float();
            if(trail.flags&4)main.color1.a=u8(color.a-i32(color.a)*j/trail.length);
            place(main,trail.points[j].position,main.pos2,offset,.3f);
        }
        main.scale=scale;main.color1=color;
    }
    satellite(2);
}
bool trail_strip(EclVm& enemy,EnemyTrail& trail,AnmVm& vm,EnemyDrawActions& actions){
    i32 count=0;for(i32 j=0;j<trail.length&&!(trail.points[j].position.x<-990);j+=trail.step)count+=2;
    if(count<=2)return true;if(!vm.loadedSprite){enemy.invalid=true;enemy.failure=EclVm::Failure::MissingAnimation;return false;}
    const auto& sprite=*vm.loadedSprite;
    const float span=Scalar::sub(sprite.uvEnd.x,sprite.uvStart.x);
    const float increment=(number(span)/Extended::from_int((count+1)/2-1)).to_float();
    float u=Scalar::add(sprite.uvEnd.x,vm.uvScrollPos.x),previous=0;
    u32 written=0;
    for(i32 j=0;j<trail.length&&!(trail.points[j].position.x<-990);j+=trail.step,u=Scalar::sub(u,increment)){
        const auto& point=trail.points[j];const float angle=j?middle_angle(trail.points[j-1].angle,point.angle).to_float():point.angle;
        if((trail.flags&2)&&j>0&&j+trail.step<trail.length){
            // The original look-ahead's second operand is the fixed step-th
            // node, not j+step. Keep that resource-visible behavior.
            const float next=middle_angle(trail.points[j-1+trail.step].angle,trail.points[trail.step].angle).to_float();
            if(std::fabs(Scalar::sub(previous,angle))<1e-5f&&std::fabs(Scalar::sub(angle,next))<1e-5f){count-=2;continue;}
        }
        previous=angle;const float sin=sine(angle).to_float(),cos=cosine(angle).to_float();
        float x=0,y=(number(vm.scale.y)*number(sprite.heightPx)/number(2)).to_float();
        if(trail.flags&2){const float factor=(number(1)-Extended::from_int(j)/Extended::from_int(trail.length)).to_float();x=Scalar::mul(x,factor);y=Scalar::mul(y,factor);}
        if(written+2>194){enemy.invalid=true;return false;}auto& a=trail.vertices[written++];auto& b=trail.vertices[written++];
        a.color=b.color=u32(vm.color1.d3dColor);if(trail.flags&4){const u8 alpha=u8(vm.color1.a-i32(vm.color1.a)*j/trail.length);a.color=b.color=(a.color&0xffffff)|(u32(alpha)<<24);}
        a.pos=point.position;b.pos=point.position;
        a.pos.x=(number(cos)*number(x)-number(sin)*number(y)+number(32)+number(a.pos.x)).to_float();
        a.pos.y=(number(cos)*number(y)+number(sin)*number(x)+number(16)+number(a.pos.y)).to_float();
        b.pos.x=(number(sin)*number(y)+number(cos)*number(x)+number(32)+number(b.pos.x)).to_float();
        b.pos.y=(number(sin)*number(x)-number(cos)*number(y)+number(16)+number(b.pos.y)).to_float();
        a.uv={u,Scalar::add(sprite.uvStart.y,vm.uvScrollPos.y)};b.uv={u,Scalar::add(sprite.uvEnd.y,vm.uvScrollPos.y)};
    }
    if(count>2)actions.strip(vm,trail.vertices,count);return true;
}
bool draw_enemy(EclVm& enemy,const Vec2& offset,EnemyDrawActions& actions){
    TH08_AUDIT_SCOPE_FLAGS(Enemy,&enemy,enemy.lifetime.current,0,actions.discrete_motion(enemy)?audit::DiscreteMotion:0);
    RenderOnlyEnemyRestore restore(enemy);
    AnmVm copies[3];AnmVm* vms=enemy.animation;if(presentation::render_only){for(u32 i=0;i<3;++i){copies[i]=enemy.animation[i];actions.visual(enemy,i,copies[i]);}vms=copies;}
    auto& main=vms[0];const Vec3 draw_position=actions.position(enemy);const float draw_direction=actions.direction(enemy);
    const auto satellite=[&](u32 index){TH08_AUDIT_SCOPE(Enemy,&enemy,enemy.lifetime.current,index);auto& vm=vms[index];if(vm.scriptIndex<0)return;if(vm.type)rotate(vm,index==1?draw_direction:-draw_direction);place(vm,draw_position,(enemy.flags2&0x100)?main.pos2:vm.pos2,offset,.3f);actions.sprite(vm);};
    satellite(1);if(enemy.flags&0x2000000)rotate(main,draw_direction);
    place(main,draw_position,main.pos2,offset,.25f);
    std::optional<EnemyTrail> trail_copy;
    if(presentation::render_only&&enemy.trail.flags){trail_copy.emplace(enemy.trail);actions.trail(enemy,*trail_copy);}
    EnemyTrail& trail=trail_copy?*trail_copy:enemy.trail;
    if(trail.flags){
        if(trail.length>96||trail.step<=0){enemy.invalid=true;return false;}
        const Vec2 scale=main.scale;const ZunColor color=main.color1;
        if(!(trail.flags&8)){
            for(i32 j=trail.length-1;j>0;j-=trail.step)if(!(trail.points[j].position.x<-990)){
                TH08_AUDIT_SCOPE(Enemy,&enemy,enemy.lifetime.current,u32(100+j));
                if(enemy.flags&0x2000000)rotate(main,trail.points[j].angle);
                if(trail.flags&2)main.scale.x=(number(scale.x)-Extended::from_int(j)*number(scale.x)/Extended::from_int(trail.length)).to_float();
                if(trail.flags&4)main.color1.a=u8(color.a-i32(color.a)*j/trail.length);
                place(main,trail.points[j].position,main.pos2,offset,.3f);actions.sprite(main);
            }
        }else if(!trail_strip(enemy,trail,main,actions))return false;
        main.scale=scale;main.color1=color;
    }
    // Position and rotation deliberately retain the final trail sample.
    if(!(trail.flags&16)&&!(enemy.flags&32))actions.sprite(main);
    satellite(2);if(presentation::active&&!presentation::render_only)retain_enemy_draw_state(enemy,offset);return true;
}
}
void EnemyDrawing::snapshot(EclVm* const* layers){
    if(!presentation_marker.capture())return;
    previous.clear();for(i32 layer=0;layer<4;++layer){u32 count=0;for(auto* enemy=layers[layer];enemy&&++count<=480;enemy=enemy->next_in_layer){auto& sample=previous.emplace(enemy,PresentationSample{enemy->resolved_position,enemy->direction.z,enemy->lifetime.current,enemy->main_context.subroutine,true}).first->second;for(u32 i=0;i<3;++i)sample.visual[i].capture(enemy->animation[i]);
        const auto& trail=enemy->trail;sample.trail_flags=trail.flags;sample.trail_step=trail.step;
        if(trail.flags&&trail.length>0&&trail.length<=96){sample.trail.resize(trail.length);for(i32 i=0;i<trail.length;++i)sample.trail[i]={trail.points[i].position,trail.points[i].angle};}
    }}
}
void EnemyDrawing::visual(EclVm& enemy,u32 index,AnmVm& draw){
    if(index>=3||!presentation::active)return;const auto found=previous.find(&enemy);if(found==previous.end())return;
    // An ECL subroutine transition does not replace an enemy's ANM VM.  Gate
    // visual continuity on the enemy lifetime plus VisualSample's own
    // file/script/visibility identity instead; otherwise an in-flight ANM
    // position interpolation snaps when the same enemy changes ECL phases.
    const auto& before=found->second;if(enemy.lifetime.current<before.age)return;
    before.visual[index].apply(enemy.animation[index],draw,presentation::world_alpha,presentation::VisualSample::Attributes|presentation::VisualSample::Offset);
    before.visual[index].apply_active_opacity(enemy.animation[index],draw,presentation::world_alpha);
}
void EnemyDrawing::trail(EclVm& enemy,EnemyTrail& draw){
    if(!presentation::active||presentation::world_alpha>=1)return;const auto found=previous.find(&enemy);if(found==previous.end())return;
    const auto& before=found->second;const auto& current=enemy.trail;
    if(enemy.lifetime.current<before.age||enemy.main_context.subroutine!=before.subroutine||before.trail_flags!=current.flags||before.trail_step!=current.step||before.trail.size()!=u32(current.length))return;
    for(size_t i=0;i<before.trail.size();++i){const auto& a=before.trail[i];const auto& b=current.points[i];if(a.position.x<-990||b.position.x<-990||!presentation::VisualSample::near(a.position,b.position))continue;
        draw.points[i].position={presentation::lerp_world(a.position.x,b.position.x),presentation::lerp_world(a.position.y,b.position.y),presentation::lerp_world(a.position.z,b.position.z)};
        draw.points[i].angle=presentation::VisualSample::cyclic(a.angle,b.angle,presentation::world_alpha,6.2831854820251465f);
    }
}
bool EnemyDrawing::discrete_motion(EclVm& enemy){
    const auto found=previous.find(&enemy);
    return found!=previous.end()&&enemy.lifetime.current>=found->second.age&&enemy.main_context.subroutine!=found->second.subroutine;
}
Vec3 EnemyDrawing::position(EclVm& enemy){
    if(!presentation::active)return enemy.resolved_position;const auto found=previous.find(&enemy);if(found==previous.end())return enemy.resolved_position;
    const auto& before=found->second;const float dx=enemy.resolved_position.x-before.position.x,dy=enemy.resolved_position.y-before.position.y;if(enemy.lifetime.current<before.age||enemy.main_context.subroutine!=before.subroutine||dx*dx+dy*dy>=16384.0f)return enemy.resolved_position;
    return {presentation::lerp_world(before.position.x,enemy.resolved_position.x),presentation::lerp_world(before.position.y,enemy.resolved_position.y),presentation::lerp_world(before.position.z,enemy.resolved_position.z)};
}
float EnemyDrawing::direction(EclVm& enemy){
    if(presentation::world_alpha>=1)return enemy.direction.z;
    if(!presentation::active)return enemy.direction.z;const auto found=previous.find(&enemy);if(found==previous.end()||enemy.lifetime.current<found->second.age||enemy.main_context.subroutine!=found->second.subroutine)return enemy.direction.z;
    constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;float delta=enemy.direction.z-found->second.direction;if(delta>pi)delta-=tau;else if(delta<-pi)delta+=tau;return add_angle(found->second.direction+delta*presentation::world_alpha,0);
}
#if defined(TH_PRESENTATION_AUDIT)
const float* EnemyDrawing::audit_presentation_sample(uintptr_t object,u32 index)const{
    static float out[28];std::fill(out,out+28,0.0f);if(index>=3)return out;
    auto* enemy=reinterpret_cast<EclVm*>(object);const auto found=previous.find(enemy);if(found==previous.end())return out;
    const auto& sample=found->second;const auto& before=sample.visual[index];const auto& current=enemy->animation[index];
    out[0]=1;out[1]=float(sample.age);out[2]=float(enemy->lifetime.current);out[3]=float(sample.subroutine);out[4]=float(enemy->main_context.subroutine);out[5]=float(index);
    out[6]=float(before.continuous);out[7]=float(presentation::VisualSample::continuous_fields(current));out[8]=current.usePosOffset?1.0f:0.0f;
    out[9]=before.visible?1.0f:0.0f;out[10]=current.visible?1.0f:0.0f;out[11]=before.matches(current)?1.0f:0.0f;
    out[12]=before.pos2.x;out[13]=before.pos2.y;out[14]=before.pos2.z;out[15]=current.pos2.x;out[16]=current.pos2.y;out[17]=current.pos2.z;
    out[18]=before.pos.x;out[19]=before.pos.y;out[20]=before.pos.z;out[21]=current.pos.x;out[22]=current.pos.y;out[23]=current.pos.z;
    out[24]=float(current.interpCurrentTimers[AnmInterp_Pos].current);out[25]=float(current.interpEndTimers[AnmInterp_Pos].current);out[26]=float(current.currentTimeInScript.current);out[27]=float(current.scriptIndex);
    return out;
}
#endif
bool draw_enemy_layers(EclVm* const* layers,i32 first,i32 last,const Vec2& offset,EnemyDrawActions& actions){
    if(first<0||last>4)return false;
    for(i32 layer=first;layer<last;++layer){u32 count=0;for(auto* enemy=layers[layer];enemy;enemy=enemy->next_in_layer){if(++count>480||!draw_enemy(*enemy,offset,actions))return false;}}
    return true;
}
}
