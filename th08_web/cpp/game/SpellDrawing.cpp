#include "SpellDrawing.hpp"
#include "Presentation.hpp"
#include "PresentationVisual.hpp"
#include "PresentationAudit.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th08 {
namespace {
float add(float x,float delta){return Scalar::add(x,delta);}
float angle(float before,float current){
    if(presentation::alpha>=1)return current;if(presentation::alpha<=0)return before;
    constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;float delta=current-before;if(delta>pi)delta-=tau;else if(delta<-pi)delta+=tau;return add_angle(before+delta*presentation::alpha,0);
}
u8 channel(u8 before,u8 current){return u8(std::clamp(presentation::lerp(float(before),float(current)),0.0f,255.0f));}
}
void SpellDrawing::snapshot_presentation(){if(!presentation_marker.capture())return;for(u32 i=0;i<presentation_previous.size();++i)presentation_previous[i]=state.spell_vms[i];presentation_previous_panel_color=state.spell_panel_color;presentation_previous_spell_flags=state.spell_flags;presentation_previous_spell_number=state.spell_number;presentation_valid=true;}
AnmVm SpellDrawing::presentation_vm(u32 index)const{
    const auto& source=state.spell_vms[index];AnmVm draw=source;if(!presentation::active||!presentation_valid||index>=presentation_previous.size())return draw;const auto& before=presentation_previous[index];
    if(before.anmFile!=source.anmFile||before.scriptIndex!=source.scriptIndex||before.visible!=source.visible||source.currentTimeInScript.current<before.currentTimeInScript.current)return draw;
    // face_cdbg's spell banners scroll their texture with an authored AddV
    // loop (scripts 0/2 use +/-1/120 every logical frame). AddV changes the
    // accumulated UV directly, so it has no uvScrollVel for the generic ANM
    // sidecar to discover. Within the same sprite lifecycle, SpellDrawing owns
    // that accumulated offset and may present the two 60 Hz endpoints smoothly.
    const presentation::VisualSample sample(before);
    sample.apply(source,draw,presentation::alpha,presentation::VisualSample::Uv,sample.authored_uv_fields(source));
    const float dx=source.pos.x-before.pos.x,dy=source.pos.y-before.pos.y;if(dx*dx+dy*dy<16384.0f)draw.pos={presentation::lerp(before.pos.x,source.pos.x),presentation::lerp(before.pos.y,source.pos.y),presentation::lerp(before.pos.z,source.pos.z)};
    const float ox=source.pos2.x-before.pos2.x,oy=source.pos2.y-before.pos2.y;if(ox*ox+oy*oy<16384.0f)draw.pos2={presentation::lerp(before.pos2.x,source.pos2.x),presentation::lerp(before.pos2.y,source.pos2.y),presentation::lerp(before.pos2.z,source.pos2.z)};
    draw.rotation={angle(before.rotation.x,source.rotation.x),angle(before.rotation.y,source.rotation.y),angle(before.rotation.z,source.rotation.z)};
    if(before.scale.x*source.scale.x>=0&&before.scale.y*source.scale.y>=0)draw.scale={presentation::lerp(before.scale.x,source.scale.x),presentation::lerp(before.scale.y,source.scale.y)};
    draw.color1={};draw.color1.b=channel(before.color1.b,source.color1.b);draw.color1.g=channel(before.color1.g,source.color1.g);draw.color1.r=channel(before.color1.r,source.color1.r);draw.color1.a=channel(before.color1.a,source.color1.a);
    draw.color2={};draw.color2.b=channel(before.color2.b,source.color2.b);draw.color2.g=channel(before.color2.g,source.color2.g);draw.color2.r=channel(before.color2.r,source.color2.r);draw.color2.a=channel(before.color2.a,source.color2.a);return draw;
}
bool SpellDrawing::digit(i32 value,AnmVm& vm){
    const i32 index=wrapping_add(value,136);if(!digits||!digits->sprites||index<0||u32(index)>=digits->spriteCount)return false;
    vm.loadedSprite=&digits->sprites[index];
    // One VM is reused for every displayed spell digit. Key diagnostics by
    // the authored screen slot, not by submission ordinal, so changing a
    // number cannot masquerade as a moving sprite between logical frames.
    const i32 px=Scalar::truncate(vm.pos.x),py=Scalar::truncate(vm.pos.y);
    const u32 part=0x80000000u|(u32(px)&0x3ffu)|((u32(py)&0x3ffu)<<10);
    TH08_AUDIT_SCOPE(Spell,&state.spell_vms[12],state.spell_vms[12].currentTimeInScript.current,part);
    renderer.draw_no_rotation(vm);return true;
}
bool SpellDrawing::draw(){
    auto& v=state.spell_vms;auto& r=renderer;
    std::array<AnmVm,14> presented{};if(presentation::render_only)for(u32 i=0;i<presented.size();++i)presented[i]=presentation_vm(i);
    const auto vm=[&](u32 i)->AnmVm&{return presentation::render_only?presented[i]:v[i];};
    const auto no_rotation=[&](u32 i){TH08_AUDIT_SCOPE(Spell,&v[i],v[i].currentTimeInScript.current,i);r.draw_no_rotation(vm(i));};
    const auto draw_2d=[&](u32 i){TH08_AUDIT_SCOPE(Spell,&v[i],v[i].currentTimeInScript.current,i);r.draw_2d(vm(i));};
    if(vm(0).visible){no_rotation(0);no_rotation(2);draw_2d(4);}
    if(vm(1).visible){
        const Vec3 saved=vm(1).pos;vm(1).pos={add(vm(1).pos.x,vm(1).pos2.x),add(vm(1).pos.y,vm(1).pos2.y),add(vm(1).pos.z,vm(1).pos2.z)};
        no_rotation(1);vm(1).pos=saved;no_rotation(3);draw_2d(5);
    }
    if(vm(6).visible){vm(10).pos=vm(6).pos;vm(10).pos.x=add(vm(10).pos.x,-32);no_rotation(10);draw_2d(6);}
    if(!vm(7).visible)return true;
    r.mix_enabled=true;
    u32 panel_color=state.spell_panel_color;
    // SpellUpdate owns a proximity fade on the whole panel and changes only
    // its mix alpha by four per fixed tick. Interpolate that owner value for
    // presentation-only draws while the same spell/panel lifecycle is active.
    if(presentation::render_only&&presentation::active&&presentation_valid&&presentation_previous_spell_number==state.spell_number&&
       bool(presentation_previous_spell_flags&1)==bool(state.spell_flags&1)&&presentation_previous[7].visible==state.spell_vms[7].visible){
        const u8 before=u8(presentation_previous_panel_color>>24),current=u8(state.spell_panel_color>>24);
        const u8 mixed=u8(std::clamp(std::lround(presentation::lerp(float(before),float(current))),0l,255l));
        panel_color=(panel_color&0xffffff)|(u32(mixed)<<24);
    }
    r.mix_color=panel_color;vm(11).pos=vm(7).pos;
    no_rotation(11);draw_2d(7);draw_2d(8);draw_2d(9);no_rotation(13);
    bool valid=true;
    if(!(state.spell_flags&1024)){
        if(!records||state.spell_number>=spell_count||state.shot<0||state.shot>=12)valid=false;
        else {
            auto& digit_vm=vm(12);auto& pos=digit_vm.pos;pos=vm(13).pos;pos.x=add(pos.x,-40);pos.y=add(pos.y,1);
            i32 value=(state.spell_flags&4)?signed_bits(state.spell_bonus):0,divisor=10000000;bool leading=false;
            for(i32 i=0;i<8;i++){
                const i32 n=value/divisor;if(n)leading=true;if(leading||divisor==1)valid=digit(n,digit_vm)&&valid;pos.x=add(pos.x,7);value%=divisor;divisor/=10;
            }
            const auto& history=(state.game_flags&0x4000)?records[state.spell_number].practice:records[state.spell_number].game;
            leading=false;
            const auto three=[&](i32 n){
                if(n>999)n=999;if(n/100){valid=digit(n/100,digit_vm)&&valid;n%=100;leading=true;}pos.x=add(pos.x,7);
                if(n/10||leading){valid=digit(n/10,digit_vm)&&valid;n%=10;}pos.x=add(pos.x,7);valid=digit(n%10,digit_vm)&&valid;
            };
            pos.x=add(pos.x,32);three(signed_bits(history.captures[state.shot]));pos.x=add(pos.x,13);three(signed_bits(history.attempts[state.shot]));
        }
    }
    r.mix_enabled=false;r.mix_color=0x80808080;return valid;
}
}
