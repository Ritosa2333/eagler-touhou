#include "GuiController.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
namespace th08 {
namespace {
float add(float a,float b){return Scalar::add(a,b);}
Extended integer(i32 n){return Extended::from_int(n);}
}
void GuiController::draw_hud(){
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
    if(gui.flags.lives)for(i32 i=0;i<Scalar::truncate(globals.lives);++i)positioned(display.front[10],float(488+i*16),88,.46f);
    if(gui.flags.bombs)for(i32 i=0;i<Scalar::truncate(globals.bombs);++i)positioned(display.front[11],float(488+i*16),104,.46f);
    if((gui.flags.bombs||gui.flags.lives)&&((scene.flags>>7)&3)==1&&context.spell_active)draw_presented_no_rotation(display.nullify);
    for(i32 x=32;x<368;x+=128)positioned(display.front[14],float(x),464,.49f);
    Vec3 pos{488,56,0};ascii.add_format(pos,software(),"%.9d",signed_bits(globals.display_score));pos.x+=117;ascii.add_format(pos,software(),"%1d",globals.retries>9?9:globals.retries);a.scale_x=a.scale_y=1;
    pos={488,40,0};ascii.add_format(pos,software(),"%.9d",signed_bits(globals.high_score));pos.x+=117;ascii.add_format(pos,software(),"%1d",globals.high_score_retries>9?9:globals.high_score_retries);a.scale_x=a.scale_y=1;
    if(gui.flags.graze||minimal)ascii.add_format({488,152,0},software(),"%d",globals.graze);
    if(gui.flags.points||minimal){pos={488,168,0};pos.x=add(pos.x,float(ascii.add_format(pos,software(),"%d",globals.points)*13));a.scale_x=.5f;a.scale_y=1;ascii.add_format(pos,software(),"/");a.scale_x=a.scale_y=1;pos.x+=6;ascii.add_format(pos,software(),"%d",globals.next_point_extend);}
    if(gui.flags.time||minimal){if(globals.time_orbs>=globals.last_spell_requirement)a.color=0xfffff0c0;pos={488,184,0};pos.x=add(pos.x,float(ascii.add_format(pos,software(),"%d",globals.time_orbs)*13));a.scale_x=.5f;a.scale_y=1;ascii.add_format(pos,software(),"/");a.scale_x=a.scale_y=1;pos.x+=6;ascii.add_format(pos,software(),"%d",globals.last_spell_requirement);a.color=-1;}
    renderer.flush();if(gui.flags.power||minimal){const i32 power=Scalar::truncate(globals.power);if(power>0){const float right=integer(wrapping_add(power,488)).to_float();const UntexturedVertex vertices[]={{{488,136,.1f},1,0xe0e0e0ff},{{right,136,.1f},1,0x80e0e0ff},{{488,152,.1f},1,0xe0e0e0ff},{{right,152,.1f},1,0x80e0e0ff}};renderer.draw_gui_strip(vertices);}if(power<128)ascii.add_format({488,136,0},software(),"%d",power);else ascii.add_format({488,136,0},software(),"MAX");}
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
