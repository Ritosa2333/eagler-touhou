#include "GuiController.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
#include <algorithm>
namespace th08 {
namespace {
float add(float a,float b){return Scalar::add(a,b);}
Extended integer(i32 n){return Extended::from_int(n);}
}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
// Native TH08 artwork with equal per-seat rows. Resource counts stay readable
// at the authored 640x480 resolution; no separate card/panel UI is introduced.
void GuiController::draw_multiplayer_hud(){
    auto& a=ascii.state;auto viewport=renderer.viewport;viewport.x=viewport.y=0;viewport.width=640;viewport.height=480;renderer.set_viewport(viewport);
    const auto positioned=[&](const AnmVm& vm,float x,float y,float z){
        auto draw=vm;draw.pos={x,y,z};renderer.draw_no_rotation(draw);
    };
    // The rows are cleared on every draw so shrinking resources leave no
    // ghost icons. This is the same lifetime choice as TH07's compact HUD.
    auto& tile=display.front[13];
    for(i32 y=16;y<464;y+=32)for(i32 x=416;x<624;x+=32)positioned(tile,float(x),float(y),.49f);
    for(i32 y=0;y<464;y+=32)positioned(tile,0,float(y),.49f);
    for(i32 x=0;x<624;x+=128){positioned(display.front[14],float(x),0,.49f);positioned(display.front[14],float(x),464,.49f);}
    for(i32 i=0;i<4;++i){
        if(i==1){
            auto logo=presentation_vm(display.front[i]);
            if(pilot_count==3){logo.scale.x*=.65f;logo.scale.y*=.65f;logo.pos.y+=68.f;}
            renderer.draw_2d(logo,true);
        }else draw_presented_no_rotation(display.front[i]);
    }
    {auto difficulty=presentation_vm(display.difficulty);difficulty.pos={560,20,.47f};renderer.draw_no_rotation(difficulty);}
    const auto original_label=[&](u32 index,float x,float y){
        auto vm=presentation_vm(display.front[index]);vm.scale={1,1};vm.pos={x,y,.47f};
        renderer.draw_no_rotation(vm);
    };
    static constexpr const char* names[]{"Reimu/Yukari","Marisa/Alice","Sakuya/Remilia","Youmu/Yuyuko","Reimu","Yukari","Marisa","Alice","Sakuya","Remilia","Youmu","Yuyuko"};
    const Vec2 previous_scale{a.scale_x,a.scale_y};const u32 previous_color=a.color;const i32 previous_gui=a.gui,previous_selected=a.selected;
    a.gui=a.selected=0;a.color=0xffffffff;
    // Shared score retains the original TH08 digit size and positions.
    a.scale_x=a.scale_y=1.f;
    ascii.add_format({488,40,0},false,"%.9d",signed_bits(globals.high_score));
    ascii.add_format({488,56,0},false,"%.9d",signed_bits(globals.display_score));
    ascii.add_format({605,40,0},false,"%1d",globals.high_score_retries>9?9:globals.high_score_retries);
    ascii.add_format({605,56,0},false,"%1d",globals.retries>9?9:globals.retries);
    for(u32 seat=0;seat<pilot_count;++seat){
        const float y=80.f+54.f*float(seat);const auto& bank=pilot_resources[seat];
        const u8 character=roster&&roster->seats[seat].player?roster->seats[seat].player->status().context.character:0;
        a.scale_x=a.scale_y=.9f;a.color=seat==local_player?0xfffff0c0:0xffc8dcff;
        ascii.add_format({436,y,0},false,"P%u",seat+1);
        a.scale_x=a.scale_y=.85f;
        ascii.add_string({472,y,0},names[character<12?character:0],false);
        original_label(4,436.f,y+17.f);original_label(5,540.f,y+17.f);
        original_label(6,436.f,y+35.f);
        a.color=0xffffffff;a.scale_x=a.scale_y=1.f;
        const i32 lives=std::max(0,Scalar::truncate(bank.lives));
        const i32 bombs=std::max(0,Scalar::truncate(bank.bombs));
        const i32 power=Scalar::truncate(bank.power);
        // A native icon plus an unabridged count fits all legal resources in
        // both 2P and 3P. Cooperative/transient states are presented beside
        // the player in the playfield, as in TH07MP, not inside this resource HUD.
        positioned(display.front[10],480.f,y+17.f,.46f);
        positioned(display.front[11],583.f,y+17.f,.46f);
        ascii.add_format({499,y+17,0},false,"%d",lives);
        ascii.add_format({602,y+17,0},false,"%d",bombs);
        ascii.add_format({488,y+35,0},false,power>=128?"MAX":"%d",power);
    }
    const float statistics_y=82.f+54.f*float(pilot_count);
    // Retain TH08's own number-strip artwork instead of covering the logo
    // with a new opaque panel.
    for(u32 i=0;i<3;++i)positioned(display.front[15],480.f,statistics_y+16.f*float(i),.48f);
    a.color=0xffffffff;a.scale_x=a.scale_y=.9f;
    original_label(7,436.f,statistics_y);
    original_label(8,436.f,statistics_y+16.f);
    original_label(9,436.f,statistics_y+32.f);
    ascii.add_format({488,statistics_y,0},false,"%d",globals.graze);
    ascii.add_format({488,statistics_y+16.f,0},false,"%d/%d",globals.points,globals.next_point_extend);
    ascii.add_format({488,statistics_y+32.f,0},false,"%d/%d",globals.time_orbs,globals.last_spell_requirement);
    a.scale_x=previous_scale.x;a.scale_y=previous_scale.y;a.color=previous_color;a.gui=previous_gui;a.selected=previous_selected;
    renderer.flush();
}
#endif
void GuiController::draw_hud(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(pilot_resources){draw_multiplayer_hud();return;}
#endif
    auto& a=ascii.state;auto viewport=renderer.viewport;viewport.x=viewport.y=0;viewport.width=640;viewport.height=480;renderer.set_viewport(viewport);
    const bool minimal=context.graphics_options&16;
    auto positioned=[&](AnmVm& vm,float x,float y,float z){
        const u32 part=(u32(i32(x))&0x3ffu)|((u32(i32(y))&0x3ffu)<<10);
        TH08_AUDIT_SCOPE(Gui,&vm,vm.currentTimeInScript.current,part);
        if(presentation::render_only){auto draw=vm;draw.pos={x,y,z};renderer.draw_no_rotation(draw);}else{vm.pos={x,y,z};renderer.draw_no_rotation(vm);}
    };
    if(!minimal){auto& vm=display.front[15];positioned(vm,480,40,.49f);positioned(vm,480,56,.49f);
        for(const auto pair:{std::pair<u32,float>{u32(gui.flags.lives),88},{u32(gui.flags.bombs),104},{u32(gui.flags.power),136},{u32(gui.flags.graze),152},{u32(gui.flags.points),168},{u32(gui.flags.time),184}})if(pair.first)positioned(vm,480,pair.second,.48f);
        positioned(vm,512,464,.48f);
    }
    if(context.graphics_options&4096||display.front[13].currentInstruction||scene.hud_redraw){
        auto& vm=display.front[13];for(i32 y=0;y<464;y+=32)positioned(vm,0,float(y),.49f);
        for(i32 x=416;x<624;x+=32)for(i32 y=16;y<464;y+=32)positioned(vm,float(x),float(y),.49f);
        for(i32 x=0;x<624;x+=128){positioned(display.front[14],float(x),0,.49f);positioned(display.front[14],float(x),464,.49f);}
        for(i32 i=0;i<10;++i){if(i==1)draw_presented_2d(display.front[i]);else draw_presented_no_rotation(display.front[i]);}draw_presented_no_rotation(display.difficulty);
        if(!presentation::render_only)gui.flags.lives=gui.flags.bombs=gui.flags.power=gui.flags.graze=gui.flags.points=gui.flags.time=2;
    }
    if(gui.flags.lives)for(i32 i=0;i<Scalar::truncate(display_lives());++i)positioned(display.front[10],float(488+i*16),88,.46f);
    if(gui.flags.bombs)for(i32 i=0;i<Scalar::truncate(display_bombs());++i)positioned(display.front[11],float(488+i*16),104,.46f);
    if((gui.flags.bombs||gui.flags.lives)&&((scene.flags>>7)&3)==1&&context.spell_active)draw_presented_no_rotation(display.nullify);
    for(i32 x=32;x<368;x+=128)positioned(display.front[14],float(x),464,.49f);
    Vec3 pos{488,56,0};ascii.add_format(pos,software(),"%.9d",signed_bits(globals.display_score));pos.x+=117;ascii.add_format(pos,software(),"%1d",globals.retries>9?9:globals.retries);a.scale_x=a.scale_y=1;
    pos={488,40,0};ascii.add_format(pos,software(),"%.9d",signed_bits(globals.high_score));pos.x+=117;ascii.add_format(pos,software(),"%1d",globals.high_score_retries>9?9:globals.high_score_retries);a.scale_x=a.scale_y=1;
    if(gui.flags.graze||minimal)ascii.add_format({488,152,0},software(),"%d",globals.graze);
    if(gui.flags.points||minimal){pos={488,168,0};pos.x=add(pos.x,float(ascii.add_format(pos,software(),"%d",globals.points)*13));a.scale_x=.5f;a.scale_y=1;ascii.add_format(pos,software(),"/");a.scale_x=a.scale_y=1;pos.x+=6;ascii.add_format(pos,software(),"%d",globals.next_point_extend);}
    if(gui.flags.time||minimal){if(globals.time_orbs>=globals.last_spell_requirement)a.color=0xfffff0c0;pos={488,184,0};pos.x=add(pos.x,float(ascii.add_format(pos,software(),"%d",globals.time_orbs)*13));a.scale_x=.5f;a.scale_y=1;ascii.add_format(pos,software(),"/");a.scale_x=a.scale_y=1;pos.x+=6;ascii.add_format(pos,software(),"%d",globals.last_spell_requirement);a.color=-1;}
    renderer.flush();if(gui.flags.power||minimal){const i32 power=Scalar::truncate(display_power());if(power>0){const float right=integer(wrapping_add(power,488)).to_float();const UntexturedVertex vertices[]={{{488,136,.1f},1,0xe0e0e0ff},{{right,136,.1f},1,0x80e0e0ff},{{488,152,.1f},1,0xe0e0e0ff},{{right,152,.1f},1,0x80e0e0ff}};renderer.draw_gui_strip(vertices);}if(power<128)ascii.add_format({488,136,0},software(),"%d",power);else ascii.add_format({488,136,0},software(),"MAX");}
    if(!presentation::render_only){if(gui.flags.lives)--gui.flags.lives;if(gui.flags.power)--gui.flags.power;if(gui.flags.bombs)--gui.flags.bombs;if(gui.flags.graze)--gui.flags.graze;if(gui.flags.points)--gui.flags.points;if(gui.flags.time)--gui.flags.time;}
}
void GuiController::draw_stage(){
    for(auto& vm:display.stage_text)draw_presented_2d(vm);draw_presented_2d(display.clock_intro);draw_presented_2d(display.clock);
    if(display.loading_portrait.activeSpriteIndex>=0){draw_presented_no_rotation(display.loading_portrait);draw_presented_world(display.arcade);for(auto& vm:display.arcade_blur)draw_presented_world(vm);if(display.unknown3a1c.activeSpriteIndex>=0){TH08_AUDIT_SCOPE(Gui,&display.unknown3a1c,display.unknown3a1c.currentTimeInScript.current,0);if(presentation::render_only){auto draw=presentation_vm(display.unknown3a1c);draw.pos={304,448,0};renderer.draw_no_rotation(draw);}else{display.unknown3a1c.pos={304,448,0};renderer.draw_no_rotation(display.unknown3a1c);}}}
    if(display.transition_count)for(auto& vm:display.transition){draw_presented_world(vm);renderer.current_sprite=nullptr;}
    if(display.dialogue.message<0&&(u32(gui.boss_present)+display.boss_life_state)>0){
        auto rect=[&](float left,float right,u32 color1,u32 color2){const u32 colors[]={color1,color1,color2,color2};renderer.draw_rectangle(left,19,right,23,colors,true);};
        float boss_life=gui.boss_life;u32 boss_opacity=gui.boss_opacity;
        if(presentation::active&&presentation_state.valid&&presentation_state.boss_present==gui.boss_present&&presentation_state.boss_life_state==display.boss_life_state){
            boss_life=presentation::lerp_world(presentation_state.boss_life,gui.boss_life);boss_opacity=u32(std::clamp(presentation::lerp_world(float(presentation_state.boss_opacity),float(gui.boss_opacity)),0.0f,255.0f));
        }
        const u32 alpha=boss_opacity<<24;rect(64,(number(boss_life)*number(320)+number(64)).to_float(),alpha|0xffffff,alpha|0x202060);
        for(i32 j=0;j<8;++j){if(!gui.segment_end[j]||gui.segment_start[j]>=boss_life)continue;const float end=boss_life<gui.segment_end[j]?boss_life:gui.segment_end[j];
            rect((number(gui.segment_start[j])*number(320)+number(64)).to_float(),(number(end)*number(320)+number(64)).to_float(),alpha|(u32(gui.segment_colors[j])&0xffffff),alpha|(u32(gui.segment_colors[j]>>2)&0x3f3f3f));}
        draw_presented_no_rotation(display.front[12]);
        const i32 count=gui.ecl_lives,gap=(count<=5)+1;
        for(i32 j=0;j<count;++j){const float left=(integer(j)*number(26)/integer(count)+number(35)).to_float(),right=(integer(j+1)*number(26)/integer(count)+number(35)-integer(gap)).to_float();rect(left,right,alpha|u32(0xffffff-j*255/9),alpha|0x202020);}
        const u32 color=gui.spell_seconds>=20?0xa0d0ff:gui.spell_seconds>=10?0xa080ff:gui.spell_seconds>=5?0xe080c0:0xff4040;
        ascii.state.color=alpha|color;const i32 seconds=gui.spell_seconds>99?99:gui.spell_seconds;
        if(!presentation::render_only&&gui.previous_spell_seconds!=gui.spell_seconds){if(seconds<3)actions.sound(38);else if(seconds<10)actions.sound(29);}
        ascii.add_format({384,16,0},software(),"%.2d",seconds);ascii.state.color=-1;if(!presentation::render_only)gui.previous_spell_seconds=gui.spell_seconds;
        if(!presentation::render_only&&!context.paused&&!context.retry&&!(scene.flags&1024)&&context.boss_exists){ascii.state.scale_x=ascii.state.scale_y=1;AsciiContext popup;popup.player=context.player;ascii.create_time({2,29,0},context.familiar_count,context.familiar_multiplier,0xfff0f00f,popup,true);}
    }
    draw_presented_no_rotation(display.stage_rank);
}
}
