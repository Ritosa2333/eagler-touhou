#include "card.hpp"
#include "records.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../gameplay/enemy_variables.hpp"
#include "../gameplay/stage_data.hpp"
#include "../stage_completion/replay_access.hpp"
#include "../bomb_system/bomb.hpp"
#include "../hud_system/hud.hpp"
#include "../effect_system/effect.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../text_renderer/centered.hpp"
#include <cstring>
#include <string>
namespace th20::source::card {
void start(CardInf& o,int spell,const char* name,int duration,int portrait){
    recovered::timer_set(o.age,0);o.spell_index=spell;strcpy_s(o.name,sizeof(o.name),name);o.flags|=3u;o.flags&=~0x18u;o.flags|=0x200u;o.flags&=~0x80u;
    if(!replay::is_playback()){
        const unsigned mode=game_session::mode()==2?1:0;
        auto* entry=record(*progress::current_profile(),spell);strcpy_s(reinterpret_cast<char*>(entry),0xc0,name);increment_record(entry,0xc8,mode);
        entry=record(progress::fallback_profile(),spell);strcpy_s(reinterpret_cast<char*>(entry),0xc0,name);increment_record(entry,0xc8,mode);
    }
    for(auto* a:hud::controller->number_animations){a->base.field_438=2;sprite::execute_animation(*a);}
    o.flags&=~0x20u;if(bomb::controller()->active())o.flags|=0x20u;o.frames=1;o.flags&=~0x40u;
    auto& sprites=*program_entry::sprite_controller;auto& text=*text::renderer;
    o.info_handles[0]=sprite::spawn_named_animation(sprites,*text.animation_file,nullptr,0);
    o.info_handles[1]=sprite::spawn_named_animation(sprites,*program_entry::graphics_state.surface_animation,"text",o.view_index+22);
    o.info_handles[2]=sprite::spawn_named_animation(sprites,*text.animation_file,nullptr,1);
    std::string owned_name(name);auto* name_animation=sprite::resolve_animation_handle(sprites,o.info_handles[1]);
    text.enqueue_task([&o,name_animation,name=std::move(owned_name)]{
        text::write_centered_animation_text(*program_entry::sprite_controller,*name_animation,0xffffff,0xff000000,4,0,nullptr,[&o]{sprite::execute_animation_interrupt(*program_entry::sprite_controller,o.info_handles[1],4);},name.c_str());
    });
    program_entry::thread_registry.request_effect(33,0);
    auto& effects=*static_cast<effects::Controller*>(o.context->objects_04[7]);
    o.effect_handle=sprite::spawn_named_animation(sprites,*effects.files[0],"effect",6);
    auto* boss=gameplay::selected_enemy(o.context->objects_04[1],0);o.position=boss?gameplay::enemy_position(boss):sprite::Vec3{};
    if(auto* a=sprite::find_animation(sprites,o.effect_handle))a->vector_5bc=o.position;
    for(int script:{4,5})sprite::find_animation_child(sprites,o.effect_handle,script,0)->base.fields_444[2]=duration;
    o.duration=duration;
    constexpr unsigned base[]{500000,1000000,1500000,2000000,1000000};auto& table=game_session::session.player_table;const int difficulty=gameplay::player_state::difficulty(table);
    if(difficulty<0||difficulty>=5)throw std::out_of_range("Card initial bonus difficulty outside original table");
    o.bonus=recovered::signed_bits(static_cast<unsigned>(gameplay::player_state::stage(table))*base[difficulty]);o.initial_bonus=o.bonus;if(o.initial_bonus>999999999)o.initial_bonus=999999999;
    sprite::spawn_named_animation(sprites,*effects.files[0],"effect",13);
    const unsigned special=spell>=88&&spell<=92?1:0;
    // Original stage fields select the two portrait variants, 52 bytes apart.
    auto field=[&](unsigned offset){if(offset+4>sizeof(gameplay::StageDefinition))throw std::out_of_range("Card portrait outside stage data");return gameplay::player_state::read<int>(*gameplay::selected_stage,offset);};
    auto& enemies=gameplay::enemy_controller();const unsigned offset=static_cast<unsigned>(portrait)*0x34+special*0xc;
    if(const int file=field(0x64+offset);file>=0)o.background_handle=sprite::spawn_named_animation(sprites,*enemies.services->existing_animation(enemies,file),nullptr,field(0x68+offset));
    o.flags=(o.flags&~0x200u)|((static_cast<unsigned>(field(0x6c+offset))&1u)<<9);
    const unsigned second=static_cast<unsigned>(portrait)*0x34;
    if(const int file=field(0x7c+second);file!=-1)sprite::spawn_named_animation(sprites,*enemies.services->existing_animation(enemies,file),nullptr,field(0x80+second));
}
}
