// Recovered behavior cross-checked with the MIT GensokyoClub/th08 reference.
#include "GuiController.hpp"
#include "Presentation.hpp"
#include "PresentationVisual.hpp"
#include "PresentationAudit.hpp"
#include <cstdio>
namespace th08 {
namespace {
i32 product(i32 a,i32 b){return signed_bits(u32(a)*u32(b));}
float add(float a,float b){return Scalar::add(a,b);}
float sub(float a,float b){return Scalar::sub(a,b);}
Extended integer(i32 n){return Extended::from_int(n);}
}
void GuiController::snapshot_presentation(){
    if(!presentation_marker.capture())return;
    if(!presentation_state.display)presentation_state.display=std::make_unique<GuiImplState>();
    *presentation_state.display=display;presentation_state.bonus=display.bonus;presentation_state.popup=display.popup;presentation_state.spell_bonus=display.spell_bonus;
    presentation_state.boss_life=gui.boss_life;presentation_state.boss_opacity=gui.boss_opacity;presentation_state.boss_present=gui.boss_present;presentation_state.boss_life_state=display.boss_life_state;presentation_state.valid=true;
}
AnmVm GuiController::presentation_vm(const AnmVm& source)const{
    AnmVm draw=source;if(!presentation::active||!presentation_state.valid||!presentation_state.display)return draw;
    const auto* begin=reinterpret_cast<const u8*>(&display);const auto* end=begin+sizeof(display);const auto* at=reinterpret_cast<const u8*>(&source);
    if(at<begin||at+sizeof(AnmVm)>end)return draw;const size_t offset=size_t(at-begin);const auto* before=reinterpret_cast<const AnmVm*>(reinterpret_cast<const u8*>(presentation_state.display.get())+offset);
    if(before->scriptIndex!=source.scriptIndex||before->visible!=source.visible)return draw;
    // The time-of-night clock has a second owner-driven fade after its ANM
    // script is advanced: update_stage() changes color1.a by four each 60 Hz
    // tick when the player enters/leaves the clock area. AlphaTime therefore
    // cannot describe the whole fade. Keep this extra ownership local to the
    // clock and to the same sprite lifecycle; other HUD color jumps remain
    // discrete unless their ANM explicitly marks them continuous.
    u32 owner_fields=0;
    if(&source==&display.clock&&before->activeSpriteIndex==source.activeSpriteIndex)owner_fields|=presentation::VisualSample::Opacity;
    presentation::VisualSample(*before).apply(source,draw,presentation::world_alpha,presentation::VisualSample::Attributes,owner_fields);
    const float dx=source.pos.x-before->pos.x,dy=source.pos.y-before->pos.y;if(dx*dx+dy*dy<16384.0f)draw.pos={presentation::lerp_world(before->pos.x,source.pos.x),presentation::lerp_world(before->pos.y,source.pos.y),presentation::lerp_world(before->pos.z,source.pos.z)};
    const float ox=source.pos2.x-before->pos2.x,oy=source.pos2.y-before->pos2.y;if(ox*ox+oy*oy<16384.0f)draw.pos2={presentation::lerp_world(before->pos2.x,source.pos2.x),presentation::lerp_world(before->pos2.y,source.pos2.y),presentation::lerp_world(before->pos2.z,source.pos2.z)};
    return draw;
}
void GuiController::draw_presented_no_rotation(AnmVm& vm){TH08_AUDIT_SCOPE(Gui,&vm,vm.currentTimeInScript.current,0);if(presentation::render_only){auto draw=presentation_vm(vm);renderer.draw_no_rotation(draw);}else renderer.draw_no_rotation(vm);}
void GuiController::draw_presented_2d(AnmVm& vm){TH08_AUDIT_SCOPE(Gui,&vm,vm.currentTimeInScript.current,0);if(presentation::render_only){auto draw=presentation_vm(vm);renderer.draw_2d(draw);}else renderer.draw_2d(vm);}
void GuiController::draw_presented_world(AnmVm& vm){TH08_AUDIT_SCOPE(Gui,&vm,vm.currentTimeInScript.current,0);if(presentation::render_only){auto draw=presentation_vm(vm);renderer.draw_world(draw);}else renderer.draw_world(vm);}
Vec3 GuiController::presentation_text_position(const GuiFormattedText& current,const GuiFormattedText& before)const{
    if(!presentation::active||!presentation_state.valid||current.display!=before.display||current.timer.current<before.timer.current)return current.position;
    const float dx=current.position.x-before.position.x,dy=current.position.y-before.position.y;if(dx*dx+dy*dy>=16384.0f)return current.position;
    return {presentation::lerp_world(before.position.x,current.position.x),presentation::lerp_world(before.position.y,current.position.y),presentation::lerp_world(before.position.z,current.position.z)};
}
bool GuiController::start(AnmVm& vm,AnmLoaded* file,i32 script,bool reset_position){
    if(!file||script<0||u32(script)>=file->scriptCount)return false;
    vm.scriptIndex=i16(script);if(reset_position){vm.pos={};vm.pos2={};vm.fontWidth=vm.fontHeight=15;}executor.start(*file,vm,file->scripts[script]);return true;
}
void GuiController::reset_clear(){display.loading_portrait.activeSpriteIndex=display.unknown3a1c.activeSpriteIndex=display.arcade.activeSpriteIndex=-1;display.transition_count=0;}
void GuiController::show_bonus(i32 n){auto& p=display.bonus;p.position={416,48,0};p.display=1;p.timer.set(0);p.argument=n;scene.hud_redraw=2;}
void GuiController::show_popup(i32 n,i32 type){auto& p=display.popup;p.position={416,168,0};p.display=type;p.timer.set(0);p.argument=n;scene.hud_redraw=2;}
void GuiController::show_spell_bonus(i32 n){auto& p=display.spell_bonus;p.position={224,16,0};p.display=1;p.timer.set(0);p.argument=n;scene.hud_redraw=2;}
bool GuiController::clock(i32 action){
    if(action==3){display.clock.color1.a=0;return true;}
    if(!gui.times)return false;if(action==0&&!start(display.clock,gui.times,2,true))return false;
    if(gui.times->SetSprite(&display.clock,globals.clock_time))return false;
    if(action==1||action==2)display.clock.pendingInterrupt=i16(action);return true;
}
bool GuiController::capture(){if(!start(display.arcade,scene.capture,1)||!display.arcade.loadedSprite)return false;actions.capture_arcade(*display.arcade.loadedSprite);return true;}
void GuiController::update_stage(){
    if(display.dialogue.message<0){
        if(gui.boss_present){if(!display.boss_life_state){display.front[12].pendingInterrupt=1;display.boss_life_state=1;gui.boss_opacity=0;}
            else{if(display.front[12].stopped)display.boss_life_state=2;gui.boss_opacity=gui.boss_opacity<252?gui.boss_opacity+4:255;}}
        else if(display.boss_life_state){if(display.boss_life_state<=2){display.front[12].pendingInterrupt=2;display.boss_life_state=3;}gui.boss_opacity=gui.boss_opacity?gui.boss_opacity-4:0;
            if(display.front[12].stopped){display.boss_life_state=0;gui.boss_life=0;gui.boss_opacity=0;}}
        if(display.boss_life_state>=2){if(gui.boss_life_max>gui.boss_life){gui.boss_life=add(gui.boss_life,.01f);if(gui.boss_life_max<gui.boss_life)gui.boss_life=gui.boss_life_max;}
            else if(gui.boss_life_max<gui.boss_life){gui.boss_life=sub(gui.boss_life,.02f);if(gui.boss_life_max>gui.boss_life)gui.boss_life=gui.boss_life_max;}}
    }
    for(auto& vm:display.front)executor.execute(vm);for(auto& vm:display.stage_text)executor.execute(vm);
    if(clock_intro_enabled&&display.stage_text[0].color1.a)executor.execute(display.clock_intro);
    executor.execute(display.stage_rank);executor.execute(display.clock);
    auto& alpha=display.clock.color1.a;if(alpha){if(context.player.x>=64&&context.player.y<128){if(alpha>64)alpha-=4;}else if(alpha<255)alpha=alpha<=251?alpha+4:255;}
    executor.execute(display.nullify);executor.execute(display.difficulty);
    if(display.loading_portrait.activeSpriteIndex>=0){if(executor.execute(display.loading_portrait))display.loading_portrait.activeSpriteIndex=-1;if(executor.execute(display.arcade))display.arcade.activeSpriteIndex=-1;for(auto& vm:display.arcade_blur)executor.execute(vm);}
    if(display.transition_count){i32 active=168;for(auto& vm:display.transition)if(executor.execute(vm))--active;display.transition_count=active;}
    for(auto* p:{&display.bonus,&display.popup})if(p->display){p->position.x=p->timer.current<30?(number(p->timer.value().to_float())*number(-312)/number(30)+number(416)).to_float():104;
        if(p->timer.current>=(p==&display.bonus?250:180))p->display=0;p->timer.tick(executor.timing);}
    if(display.spell_bonus.display){if(display.spell_bonus.timer.current>=280)display.spell_bonus.display=0;display.spell_bonus.timer.tick(executor.timing);}
    if(display.clear_frames==1){
        i32 score=display.clear_stage;for(const auto term:{product(display.clear_graze,50),product(display.clear_points,5000),product(display.clear_time,100)})score=wrapping_add(score,term);
        if(scene.stage>=6&&!(scene.flags&1)){score=wrapping_add(score,product(Scalar::truncate(globals.lives),2500000));score=wrapping_add(score,product(Scalar::truncate(globals.bombs),500000));}
        if(scene.stage==7)score=wrapping_add(score,product(12-globals.clock_time,2000000));
        switch(context.difficulty){case 0:score/=2;break;case 2:score=product(score,12)/10;break;case 3:score=product(score,15)/10;break;case 4:score=product(score,2);break;}
        switch(config.lives){case 3:score=product(score,5)/10;break;case 4:score=product(score,2)/10;break;case 5:score/=10;break;case 6:score/=20;break;}
        display.clear_total=score;for(i32 i=0;i<10;++i)values.add_score(score);++display.clear_frames;
    }
    if(scene.stage<6&&display.clear_clock_display&&display.clear_clock_display>=display.clear_clock&&!(scene.flags&0x60))scene.flags=(scene.flags&~0x60u)|0x40;
    if(display.clear_clock_display&&display.clear_clock_display!=display.clear_clock){
        if(display.clear_clock_delay>=60){if(display.clear_clock_display<display.clear_clock){display.clear_clock_display=wrapping_add(display.clear_clock_display,scene.input&257?4:1);if(display.clear_clock_display>display.clear_clock)display.clear_clock_display=display.clear_clock;}else display.clear_clock_delay=wrapping_add(display.clear_clock_delay,1);}
        else display.clear_clock_delay=wrapping_add(display.clear_clock_delay,1);
    }
}
void GuiController::draw_clear(){
    auto& a=ascii.state;Vec3 pos{120,96,0};a.color=0xffffff40;
    if(scene.stage<6)ascii.add_format(pos,software(),"Stage Clear");else{if(scene.stage>=7)pos.y-=16;ascii.add_format(pos,software(),"All Clear!");}
    pos.y+=32;a.color=-1;ascii.add_format(pos,software(),"Clear = %8d0",display.clear_stage);
    pos.y+=16;a.color=0xffe0e0ff;ascii.add_format(pos,software(),"Point = %8d0",product(display.clear_points,5000));
    pos.y+=16;a.color=0xffd0d0ff;ascii.add_format(pos,software(),"Graze = %8d0",product(display.clear_graze,50));
    pos.y+=16;ascii.add_format(pos,software(),"Time  = %8d0",product(display.clear_time,100));pos.y+=32;
    // A valid stage has at least one recorded frame. Retain a defined result
    // for an empty imported state instead of dividing by zero.
    const i32 total=context.stage_frames?context.stage_frames:1;
    ascii.add_format(pos,software(),"over-80%% = %3d.%.2d%%",product(context.human_frames,100)/total,(product(context.human_frames,10000)/total)%100);pos.y+=16;
    ascii.add_format(pos,software(),"over 80%% = %3d.%.2d%%",product(context.youkai_frames,100)/total,(product(context.youkai_frames,10000)/total)%100);
    if(scene.stage>=6&&!(scene.flags&1)&&!context.practice_replay){
        pos.y+=16;a.color=0xffffff80;ascii.add_format(pos,software(),"Player =%8d0",product(Scalar::truncate(globals.lives),2500000));pos.y+=16;ascii.add_format(pos,software(),"Bomb   = %7d0",product(Scalar::truncate(globals.bombs),500000));
        if(scene.stage==7){pos.y+=16;ascii.add_format(pos,software(),"Last Time = %2d:%.2d",(display.clear_clock/60)%12,display.clear_clock%60);pos.y+=16;ascii.add_format(pos,software(),"Night Bonus");pos.y+=16;ascii.add_format(pos,software(),"        %8d0",product(12-globals.clock_time,2000000));}
    }
    pos.y+=32;constexpr const char* ranks[]={"Easy Rank    *0.5","Normal Rank  *1.0","Hard Rank    *1.2","Lunatic Rank *1.5","Extra Rank   *2.0","Phantasm Rank*2.0"};
    if(context.difficulty>=0&&context.difficulty<6){a.color=0xffff8080;ascii.add_format(pos,software(),ranks[context.difficulty]);}
    if(context.difficulty<4&&!(scene.flags&1)){pos.y+=16;constexpr const char* penalties[]={"Player Penalty*0.5","Player Penalty*0.2","Player Penalty*0.1","Player Penalty*0.05"};if(config.lives>=3&&config.lives<=6){a.color=0xffff8080;ascii.add_format(pos,software(),penalties[config.lives-3]);pos.y+=16;}}
    pos.y+=16;a.color=-1;ascii.add_format(pos,software(),"Total = %8d0",display.clear_total);a.color=-1;
    if(scene.stage<=5){pos.y+=40;pos.x=120;a.color=0xffdfdfdf;ascii.add_format(pos,software(),"%s%2d:%.2d",display.clear_clock_old/60<12?"PM":"AM",(display.clear_clock_old/60)%12,display.clear_clock_old%60);pos.x+=99;a.color=0xffafafaf;ascii.add_format(pos,software(),">>");pos.x+=34;a.color=0xffff8f8f;ascii.add_format(pos,software(),"%s%2d:%.2d",display.clear_clock_display/60<12?"PM":"AM",(display.clear_clock_display/60)%12,display.clear_clock_display%60);a.color=-1;}
}
void GuiController::draw_popups(){
    auto& a=ascii.state;a.gui=1;
    if(display.bonus.display){a.color=0xffffff80;ascii.add_format(presentation_text_position(display.bonus,presentation_state.bonus),software()," BONUS %8d",display.bonus.argument);a.color=-1;}
    const i32 kind=display.popup.display;const bool compressed=kind==2||kind==4||kind==5||kind==6;
    if(compressed){a.scale_x=.9f;a.scale_y=1;a.space_width=11;a.color=0xffe0b0ff;}
    else if(kind==1||kind==3)a.color=0xffc0b0ff;
    constexpr const char* formats[]={"","Full Power Mode!","Supernatural Border!!","CherryPoint Max!","Border Bonus %7d","Spell Bonus Failed","Last Spell Failed"};
    if(kind>=1&&kind<=6){ascii.add_format(presentation_text_position(display.popup,presentation_state.popup),software(),formats[kind],display.popup.argument);a.color=-1;}
    if(compressed){a.scale_x=a.scale_y=1;a.space_width=13;}
    if(display.spell_bonus.display){Vec3 local=display.spell_bonus.position;auto& pos=presentation::render_only?local:display.spell_bonus.position;pos.x=105;pos.y=80;a.color=0xffff0000;ascii.add_format(pos,software(),"Spell Card Bonus!");pos.y+=16;char text[32];std::snprintf(text,sizeof(text),"+%d",display.spell_bonus.argument);pos.x=(number(384)-Extended::from_int64(std::strlen(text))*number(28)).to_float()/2+32;a.scale_x=a.scale_y=2;a.color=0xffff8080;ascii.add_string(pos,text,software());a.scale_x=a.scale_y=1;a.color=-1;}
    a.gui=0;
}
}
