#include "PlayerBombPatterns.hpp"
#include "PlayerBombNames.hpp"
#include "AnmTransitions.hpp"
#include "GameMath.hpp"
#include "Localization.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
#include <algorithm>
namespace th08 {
namespace {
float raw_float(u32 bits){float value;std::memcpy(&value,&bits,4);return value;}Vec3 add(const Vec3& a,const Vec3& b){return {Scalar::add(a.x,b.x),Scalar::add(a.y,b.y),Scalar::add(a.z,b.z)};}
// thcrap stringdefs IDs for the player spellcard (bomb) cut-in names, indexed
// by PlayerBombKind. The Japanese names stay in PlayerBombNames.hpp as the
// fallback; only this display point is translated. PlayerBombKind::LastWord's
// "th08 Spell Dissolve" is the deathbomb Last Word name; upstream's separate
// "th08 Spell Resurrection" string has no counterpart in this engine's
// recovered data and is intentionally unused.
const char* const bomb_name_ids[]={
    "th08 Bomb Reimu","th08 Bomb Yukari","th08 Bomb Reimu Last","th08 Bomb Yukari Last",
    "th08 Spell Dissolve","th06 Bomb Marisa B","th08 Bomb Alice","th08 Bomb Marisa Last",
    "th08 Bomb Alice Last","th07 Bomb Sakuya A focused","th08 Bomb Remilia","th08 Bomb Sakuya Last",
    "th08 Bomb Remilia Last","th08 Bomb Youmu","th08 Bomb Yuyuko","th08 Bomb Youmu Last",
    "th08 Bomb Yuyuko Last",
};
const char* bomb_display_name(PlayerBombKind kind){const char* fallback=player_bomb_name(kind);return u32(kind)<17?Localization::StringById(bomb_name_ids[u32(kind)],fallback):fallback;}
}
void PlayerBombPatterns::snapshot_presentation(){
    if(!presentation_marker.capture())return;
    for(u32 i=0;i<128;++i){const auto& o=objects.objects[i];presentation_previous[i]={o.position,o.angle,o.state,o.animation[0].currentTimeInScript.current,o.animation[0].scriptIndex,presentation::VisualSample(o.animation[0])};}
    for(u32 i=0;i<7;++i){const auto& vm=objects.objects[0].animation[i+1];presentation_additional[i].capture(vm);presentation_additional_age[i]=vm.currentTimeInScript.current;}
}
void PlayerBombPatterns::presentation_visual(u32 index,u32 part,AnmVm& draw)const{
    if(index>=128||part>=8||(index!=0&&part!=0)||!presentation::render_only||!presentation::active)return;
    const auto& o=objects.objects[index];const auto& before=presentation_previous[index];
    const i32 previous_age=part?presentation_additional_age[part-1]:before.age;
    if(before.state!=o.state||o.animation[part].currentTimeInScript.current<previous_age)return;
    const auto& visual=part?presentation_additional[part-1]:before.visual;
    visual.apply(o.animation[part],draw,presentation::world_alpha,presentation::VisualSample::Attributes|presentation::VisualSample::Offset);
}
Vec3 PlayerBombPatterns::presentation_position(u32 index)const{
    if(index>=128||!presentation::active)return index<128?objects.objects[index].position:Vec3{};const auto& o=objects.objects[index];const auto& p=presentation_previous[index];
    const float dx=o.position.x-p.position.x,dy=o.position.y-p.position.y;if(p.state!=o.state||p.script!=o.animation[0].scriptIndex||o.animation[0].currentTimeInScript.current<p.age||dx*dx+dy*dy>=16384.0f)return o.position;
    return {presentation::lerp_world(p.position.x,o.position.x),presentation::lerp_world(p.position.y,o.position.y),presentation::lerp_world(p.position.z,o.position.z)};
}
float PlayerBombPatterns::presentation_angle(u32 index)const{
    if(index<128&&presentation::world_alpha>=1)return objects.objects[index].angle;
    if(index>=128||!presentation::active)return index<128?objects.objects[index].angle:0;const auto& o=objects.objects[index];const auto& p=presentation_previous[index];if(p.state!=o.state||p.script!=o.animation[0].scriptIndex||o.animation[0].currentTimeInScript.current<p.age)return o.angle;
    constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;float delta=o.angle-p.angle;if(delta>pi)delta-=tau;else if(delta<-pi)delta+=tau;return add_angle(p.angle+delta*presentation::world_alpha,0);
}
void PlayerBombPatterns::begin(PlayerBombKind kind,i32 sprite,i32 duration,i32 invincibility,i32 variant){begin_player_bomb(objects,bomb,life,movement.position,sprite,bomb_display_name(kind),duration,invincibility,variant,actions);}
void PlayerBombPatterns::step(AnmVm* vm,u32 count){for(u32 i=0;i<count;++i)if(vm[i].scriptIndex>=0)actions.step_animation(vm[i]);}
void PlayerBombPatterns::tint(u32 color){if(presentation::render_only){actions.reset_screen_color();return;}actions.background_color(player_bomb_color(color,bomb.timer,bomb.duration));}
bool PlayerBombPatterns::update(PlayerBombKind kind){snapshot_presentation();switch(kind){case PlayerBombKind::Youmu:if(!frame.main_animation)return false;youmu(false);break;case PlayerBombKind::YoumuLast:if(!frame.main_animation)return false;youmu(true);break;case PlayerBombKind::Yuyuko:yuyuko(false);break;case PlayerBombKind::YuyukoLast:yuyuko(true);break;case PlayerBombKind::Sakuya:sakuya(false);break;case PlayerBombKind::SakuyaLast:sakuya(true);break;case PlayerBombKind::Remilia:if(!frame.options)return false;remilia(false);break;case PlayerBombKind::RemiliaLast:if(!frame.options)return false;remilia(true);break;case PlayerBombKind::Alice:if(!frame.options)return false;alice(false);break;case PlayerBombKind::AliceLast:if(!frame.options)return false;alice(true);break;case PlayerBombKind::Reimu:reimu(false);break;case PlayerBombKind::ReimuLast:reimu(true);break;case PlayerBombKind::Marisa:marisa(false);break;case PlayerBombKind::MarisaLast:marisa(true);break;case PlayerBombKind::Yukari:yukari(false);break;case PlayerBombKind::YukariLast:yukari(true);break;case PlayerBombKind::LastWord:last_word();break;default:return false;}return true;}
bool PlayerBombPatterns::draw(PlayerBombKind kind,const Vec2& offset){switch(kind){case PlayerBombKind::Youmu:draw_youmu(false);break;case PlayerBombKind::YoumuLast:draw_youmu(true);break;case PlayerBombKind::Yuyuko:draw_yuyuko(false,offset);break;case PlayerBombKind::YuyukoLast:draw_yuyuko(true,offset);break;case PlayerBombKind::Sakuya:draw_sakuya(false,offset);break;case PlayerBombKind::SakuyaLast:draw_sakuya(true,offset);break;case PlayerBombKind::Remilia:tint(0x80d02020);break;case PlayerBombKind::RemiliaLast:tint(0x80f00000);break;case PlayerBombKind::Alice:tint(0x80404040);break;case PlayerBombKind::AliceLast:draw_alice();break;case PlayerBombKind::Reimu:draw_reimu(false,offset);break;case PlayerBombKind::ReimuLast:draw_reimu(true,offset);break;case PlayerBombKind::Marisa:case PlayerBombKind::MarisaLast:return draw_marisa(offset);case PlayerBombKind::Yukari:tint(0x80404040);break;case PlayerBombKind::YukariLast:draw_yukari(offset);break;case PlayerBombKind::LastWord:{u32 color=0x80404040;if(bomb.timer.current>=60){const u32 component=u32(wrapping_add(signed_bits(u32(wrapping_sub(bomb.timer.current,60))*176)/60,64));color=0x80000000|(component<<16)|(component<<8)|component;}tint(color);break;}default:return false;}return true;}
void PlayerBombPatterns::marisa(bool last){
    auto& object=objects.objects[0];
    if(bomb.timer.changed()&&bomb.timer.current==0){
        begin(last?PlayerBombKind::MarisaLast:PlayerBombKind::Marisa,0,last?350:300,last?380:350,last);
        if(last)actions.sound(13,0);actions.sound(19,0);object.position=movement.position;
        for(i32 i=0;i<5;++i)actions.animation(object.animation[i],(last?35:30)+i,false);
        if(!last)movement.multiplier={.2f,.2f};actions.screen(ScreenEffectType::EnvelopeShake,16,120,60,120,21);
        if(last){movement.multiplier={.2f,.2f};bomb.sequence=0;}
    }
    if(last&&bomb.timer.changed()&&bomb.timer.current%10==0){
        if(auto* e=actions.fixed_effect(53,movement.position,bomb.sequence%4+4,0xffffffff)){
            if(bomb.sequence&1)actions.animation(*e,92,true);e->segments=32;e->frequency=0;
            animation_position_transition(*e,30,4,{0,0,0},{128,0,0});animation_scale_transition(*e,30,1,{32,0},{64,0});
            animation_alpha_transition(*e,30,3,255,0);animation_rgb_transition(*e,30,0,0xffffffff,0xffff0000);actions.step_animation(*e);
        }
        bomb.sequence=wrapping_add(bomb.sequence,1);actions.panned_sound(17,movement.position.x);
    }
    if(bomb.timer.changed()&&bomb.timer.current%4!=0){
        const float y=Scalar::div(movement.position.y,2),height=Scalar::add(y,y);
        regions.rectangle(false,{192,y},384,height,6,0);
        regions.rectangle(true,{movement.position.x,y},128,height,12,0).suppress_effect=1;
        regions.rectangle(true,{192,y},384,height,last?7:6,0).suppress_effect=1;
    }
    step(object.animation,5);
}
void PlayerBombPatterns::yukari(bool last){
    const bool changed=bomb.timer.changed();auto& object=objects.objects[0];
    if(changed&&bomb.timer.current==0){
        begin(last?PlayerBombKind::YukariLast:PlayerBombKind::Yukari,1,last?250:150,last?300:200,last);actions.sound(13,0);object.position=movement.position;
        if(last){actions.animation(object.animation[0],21,false);actions.animation(object.animation[1],22,false);}
    }
    if(changed&&(bomb.timer.current==0||bomb.timer.current==10||bomb.timer.current==20||bomb.timer.current==30)){
        const i32 index=bomb.timer.current/10;const auto& pos=movement.position;
        regions.circle(false,{pos.x,pos.y},100,1,6,last&&index!=1?100:40);
        auto& damage=regions.circle(true,{pos.x,pos.y},100,1,70,40);damage.interval=5;
        static const u32 angles[]{0x3f490fdb,0x3f96cbe4,0x3fc90fdb,0x3ffb53d2};
        auto* effect=actions.parameter_effect(last?37:36,last?object.position:pos,{raw_float(angles[index]),1,4},index+4,0xffffffff);
        if(index&&effect)actions.animation(*effect,(last?92:88)+index,true);
        if(index)objects.objects[index].position=pos;
    }
    step(object.animation,2);
}
void PlayerBombPatterns::last_word(){
    if(!bomb.timer.changed()||bomb.timer.current!=0)return;const bool practice=context.game_flags&0x4000;
    begin(PlayerBombKind::LastWord,-1,practice?40:120,200,0);actions.spawn_effect(12,movement.position,1,0xff4040ff);
    if(auto* e=actions.fixed_effect(50,movement.position,4,0xff4040ff)){
        e->interpCurrentTimers[0].set(0);e->interpEndTimers[0].set(practice?30:90);e->interpModes[0]=5;
        e->posInitial.x=8;e->posInitial.y=64;e->posFinal.x=128;e->posFinal.y=0;e->pos.x=8;e->pos.y=64;
        e->segments=64;e->angle=0;e->radius=8;e->width=15;e->frequency=6;
    }
    actions.sound(13,0);movement.multiplier={0,0};for(auto* boss:input.bosses)if(boss)boss->flags&=~0x40u;
}
bool PlayerBombPatterns::draw_marisa(const Vec2& offset){
    tint(0x80404040);const float step=raw_float(0x3e567750);
    for(i32 i=0;i<5;++i){auto& source=objects.objects[0].animation[i];if(!source.loadedSprite)return false;AnmVm copy;if(presentation::render_only){copy=source;presentation_visual(0,u32(i),copy);}auto& vm=presentation::render_only?copy:source;
        TH08_AUDIT_SCOPE(PlayerBomb,&objects.objects[0],source.currentTimeInScript.current,(u32(objects.objects[0].state)<<8)|u32(i));
        float angle=(Extended::from_int(i)*number(step)-number(raw_float(0x3fc90fdb))-(number(step)+number(step))).to_float();if(angle<-3.1415927410125732f)angle=Scalar::add(angle,6.2831854820251465f);
        vm.pos=movement.position;vm.pos.x=(cosine(angle)*number(vm.loadedSprite->widthPx)*number(vm.scale.x)/number(2)+number(vm.pos.x)).to_float();vm.pos.y=(sine(angle)*number(vm.loadedSprite->widthPx)*number(vm.scale.x)/number(2)+number(vm.pos.y)).to_float();
        vm.rotation.z=angle;vm.updateRotation=1;vm.pos.x=Scalar::add(offset.x,vm.pos.x);vm.pos.y=Scalar::add(offset.y,vm.pos.y);vm.pos.z=0;actions.draw(vm,true);
    }return true;
}
void PlayerBombPatterns::draw_yukari(const Vec2& offset){
    tint(0x802020d0);auto& object=objects.objects[0];const Vec3 position=presentation_position(0);for(i32 i=0;i<2;++i){auto& source=object.animation[i];AnmVm copy;if(presentation::render_only){copy=source;presentation_visual(0,u32(i),copy);}auto& vm=presentation::render_only?copy:source;TH08_AUDIT_SCOPE(PlayerBomb,&object,source.currentTimeInScript.current,(u32(object.state)<<8)|u32(i));vm.pos=add(position,vm.pos2);vm.pos.x=Scalar::add(offset.x,vm.pos.x);vm.pos.y=Scalar::add(offset.y,vm.pos.y);vm.pos.z=i?0:.01f;actions.draw(vm,true);}
}
#if defined(TH_PRESENTATION_AUDIT)
const float* PlayerBombPatterns::audit_presentation_sample(uintptr_t object,u32 part)const{
    static float out[24];std::fill(out,out+24,0.0f);
    const auto* begin=objects.objects;const auto* end=objects.objects+128;const auto* current=reinterpret_cast<const PlayerBombObject*>(object);
    if(current<begin||current>=end||part>=8)return out;
    const size_t index=size_t(current-begin);const auto& before=presentation_previous[index];const auto& vm=current->animation[part];
    const auto& visual=part?presentation_additional[part-1]:before.visual;const i32 previous_age=part?presentation_additional_age[part-1]:before.age;
    out[0]=float(index);out[1]=float(before.state);out[2]=float(current->state);out[3]=float(before.script);out[4]=float(vm.scriptIndex);
    out[5]=float(previous_age);out[6]=float(vm.currentTimeInScript.current);out[7]=before.position.x;out[8]=before.position.y;out[9]=before.position.z;
    out[10]=current->position.x;out[11]=current->position.y;out[12]=current->position.z;out[13]=visual.scale.x;out[14]=visual.scale.y;
    out[15]=vm.scale.x;out[16]=vm.scale.y;out[17]=float(visual.color1.a);out[18]=float(vm.color1.a);out[19]=float(visual.continuous);
    out[20]=float(presentation::VisualSample::continuous_fields(vm));out[21]=float(presentation_marker.last_epoch&0xffffffu);out[22]=float(bomb.timer.current);out[23]=float(current->frame);
    return out;
}
#endif
}
