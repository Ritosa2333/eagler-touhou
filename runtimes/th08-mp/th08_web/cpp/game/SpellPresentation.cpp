#include "SpellPresentation.hpp"
#include "Localization.hpp"
namespace th08 {
namespace {
// The vanilla width formula counts CP932 bytes (2 per full-width glyph). For
// UTF-8 translations count display columns per code point instead; the CP932
// path is byte-identical to the original.
float name_width(const char* name){
    u32 units=u32(std::strlen(name));
    const auto* bytes=reinterpret_cast<const u8*>(name);
    if(Localization::Active()&&utf8_valid(bytes,units))units=utf8_display_columns(bytes,units);
    return (Extended::from_int64(i64(units)*15)/number(2)+number(16)).to_float();
}
}
bool SpellPresentation::start(i32 slot,AnmLoaded* file,i32 script){
    if(!file||!file->scripts||u32(script)>=file->scriptCount)return false;
    auto& vm=state.spell_vms[slot];vm.anmFile=file;vm.scriptIndex=i16(script);anm.start(*file,vm,file->scripts[script]);return !anm.invalid;
}
bool SpellPresentation::player(i32 form,const char* name,i32 style){
    if(!name)return false;
    if(form==0||form==1){auto* file=form?state.spell_youkai_face:state.spell_human_face;if(!start(0,file,0)||file->SetSprite(&state.spell_vms[0],0))return false;}
    if(!start(2,state.spell_banners,0)||!start(4,state.spell_banners,2))return false;
    if(state.spell_banners->SetSprite(&state.spell_vms[2],style)||state.spell_banners->SetSprite(&state.spell_vms[4],style))return false;
    if(!start(6,context.text,4)||!text.draw(state.spell_vms[6],TextAlignment::Left,0x00f0f0ff,0,name))return false;
    state.spell_player_name_width=name_width(name);state.spell_vms[10].pendingInterrupt=1;
    actions.sound(14,0);redraw();return true;
}
bool SpellPresentation::enemy(i32 portrait,const char* name,i32 style){
    if(!name)return false;
    if(portrait>=0){if(!start(1,state.spell_enemy_face,0)||state.spell_enemy_face->SetSprite(&state.spell_vms[1],portrait))return false;}
    if(!start(2,state.spell_banners,1)||state.spell_banners->SetSprite(&state.spell_vms[2],style))return false;
    if(!start(4,state.spell_banners,3)||state.spell_banners->SetSprite(&state.spell_vms[4],style))return false;
    const bool last=(context.game_flags&0x4000)&&context.current_spell>=205&&context.current_spell<=221;
    if(last){
        if(!start(7,context.text,6)||!start(8,context.text,7)||!start(9,context.text,8))return false;
        for(i32 slot:{8,9})if(!text.draw(state.spell_vms[slot],TextAlignment::Right,0x00fff0f0,0,name))return false;
    }else if(!start(7,context.text,5))return false;
    if(!text.draw(state.spell_vms[7],TextAlignment::Right,0x00fff0f0,0,name))return false;
    state.spell_enemy_name_width=name_width(name);state.spell_vms[11].pendingInterrupt=1;
    if(!(state.spell_flags&1024))state.spell_vms[13].pendingInterrupt=1;
    actions.sound(14,0);redraw();return true;
}
}
