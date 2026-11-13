#include "GameSessionResources.hpp"
#include "ShotFiles.hpp"
#if PORT_TITLE == 14
#include "AnmManager.hpp"
#include "SceneCamera.hpp"
#include <cstring>
#endif

namespace th11 {

namespace {
bool core_error(std::string& error,const char* message) { error=message; return false; }
}

bool GameSessionResources::load_core(GameResources& source) {
#if PORT_TITLE == 14
    if(incoming_stage)return core_error(error,"cancel pending TH14 stage before core replacement");
#endif
    if(core_source==&source)return true;
    error.clear(); stage_number=0;
    // The resource carries the reason it failed, which for an ANM is the opcode of the title's numbering the
    // measured table has no row for. An empty reason leaves the message exactly as it was.
    auto anm=[&](const char* message,const char* file,AnmResource& output){
        if(source.open_anm(file,output))return true;
        return core_error(error,(std::string(message)+output.detail).c_str());
    };
#if PORT_TITLE == 14
    core_deferred=effects&&!effects->core_ready();core_loaded_jobs=0;selected_player_pending=-1;
    if(core_deferred){
        // Only the original loading screen's resources are needed now. Each
        // remaining decode is a real frame job before its upload/templates.
        core={};
#if (defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)) || defined(TH14_CPP_BOOT_WORKER)
        if(effects&&effects->defer_initial_stage_decode()){
            core_source=&source;return true;
        }
#endif
        if(!anm("ascii ANM","ascii.anm",core.ascii)||!anm("text ANM","text.anm",core.text))return false;
        if(animation_manager){auto& a=*animation_manager;a.fallback_sprites=&core.ascii;
            if(!a.prepare_resource(core.ascii,2)||!a.prepare_resource(core.text,0))return core_error(error,a.last_error_text().c_str());}
        core_source=&source;return true;
    }
#endif
    if(!anm("ascii ANM","ascii.anm",core.ascii))return false;
    if(!anm("bullet ANM","bullet.anm",core.bullet))return false;
    if(!anm("enemy ANM","enemy.anm",core.enemy))return false;
#if PORT_TITLE == 14 || PORT_TITLE == 16
    if(!anm("effect ANM","effect.anm",core.effect))return false;
#endif
    if(!anm("front ANM","front.anm",core.front))return false;
    if(!anm("text ANM","text.anm",core.text))return false;
    if(!anm("title ANM","title.anm",core.title))return false;
    if(!anm("title variant ANM","title_v.anm",core.title_variant))return false;
    for(u32 i=0;i<player_file_count;++i)
        if(!anm("player ANM",player_file_name(i),core.players[i]))return false;
    // The name goes into the message: a wrong cipher, a wrong table row and a file the title does not ship all
    // arrive here, and the name is what tells them apart.
    for(u32 i=0;i<shot_file_count;++i)if(!source.open_sht(shot_file_name(i),core.shots[i]))return core_error(error,(std::string("SHT resource ")+shot_file_name(i)+": "+source.error()).c_str());
#if PORT_TITLE == 16
    // Retail 492cb8 / 492cdc, in selection order rather than filename order.
    constexpr const char* season_anm[]={"pl00sub.anm","pl02sub.anm","pl03sub.anm","pl01sub.anm","pl04sub.anm"};
    constexpr const char* season_sht[]={"pl00sub.sht","pl02sub.sht","pl03sub.sht","pl01sub.sht","pl04sub.sht"};
    for(u32 i=0;i<5;++i){
        if(!anm("season player ANM",season_anm[i],core.season_players[i]))return false;
        if(!source.open_sht(season_sht[i],core.season_shots[i]))return core_error(error,(std::string("season SHT ")+season_sht[i]+": "+source.error()).c_str());
    }
#endif
    if(!source.load_ecl("default.ecl",core.defaults))return core_error(error,"default ECL");
#if PORT_TITLE == 14
    if(animation_manager){auto& a=*animation_manager;a.fallback_sprites=&core.ascii;
        if(!a.prepare_resource(core.ascii,2)||!a.prepare_resource(core.text,0))return core_error(error,a.last_error_text().c_str());}
    if((!effects||effects->core_ready())&&!prepare_core())return false;
#endif
    core_source=&source;return true;
}
#if PORT_TITLE == 14
#if (defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)) || defined(TH14_CPP_BOOT_WORKER)
bool GameSessionResources::load_core_bootstrap(){
    if(!core_deferred||(core_loaded_jobs&(1u<<9)))return true;
    if(!core_source)return core_error(error,"TH14 bootstrap resource source missing");
    if(!core_source->open_anm("ascii.anm",core.ascii))return core_error(error,(std::string("ascii ANM: ")+core.ascii.detail).c_str());
    if(!core_source->open_anm("text.anm",core.text))return core_error(error,(std::string("text ANM: ")+core.text.detail).c_str());
    if(animation_manager){auto& a=*animation_manager;a.fallback_sprites=&core.ascii;
        if(!a.prepare_resource(core.ascii,2)||!a.prepare_resource(core.text,0))return core_error(error,a.last_error_text().c_str());}
    core_loaded_jobs|=u16(1u<<9);return true;
}
#endif
#if defined(TH14_CPP_BOOT_WORKER)
bool GameSessionResources::complete_cpp_core_bootstrap(){
    if(!core_deferred||!core_source||!core.ascii.templates||!core.text.templates
       ||!core.front.templates||!core.bullet.templates||!core.effect.templates)
        return core_error(error,"TH14 C++ bootstrap owners not prepared");
    // Jobs0..2 are the exact front/bullet/effect owners already completed.
    core_loaded_jobs|=u16((1u<<9)|7u);
    if(animation_manager)animation_manager->fallback_sprites=&core.ascii;
    return true;
}
bool GameSessionResources::complete_cpp_title_resources(){
    if(!core_deferred||!core_source||!core.title.templates||!core.title_variant.templates)
        return core_error(error,"TH14 C++ title owners not prepared");
    core_loaded_jobs|=u16((1u<<4)|(1u<<5));return true;
}
#endif
bool GameSessionResources::load_core_job(u32 index){
    if(index>=9)return core_error(error,"TH14 core resource job outside 0..8");
    if(!core_deferred||(core_loaded_jobs&(1u<<index)))return true;
    if(!core_source)return core_error(error,"TH14 core resource source missing");
    // The actual TH14 boot/title workers already own common/title files.
    // Player file9 belongs to the selected initial worker. Generic enemy
    // and the legacy all-player/default-ECL preloads have no live TH14 owner.
    if(effects&&effects->initial_cpp_worker_enabled()&&(index==3||index>=6)){
        core_loaded_jobs|=u16(1u<<index);return true;
    }
    // Original 444050 prepares front, bullet, then effect before constructing
    // their users. Keep the deferred path in the same order as prepare_core.
    AnmResource* files[]={&core.front,&core.bullet,&core.effect,&core.enemy,&core.title,&core.title_variant,&core.players[0],&core.players[1],&core.players[2]};
    const char* names[]={"front.anm","bullet.anm","effect.anm","enemy.anm","title.anm","title_v.anm","pl00.anm","pl01.anm","pl02.anm"};
    if(!core_source->open_anm(names[index],*files[index]))return core_error(error,(std::string(names[index])+": "+files[index]->detail).c_str());
    // The actual initial worker reads only its selected SHT after file9 ACK.
    // Keep the synchronous/legacy core path unchanged.
    if(index>=6&&!(effects&&effects->initial_cpp_worker_enabled())){const u32 character=index-6;
        for(u32 loadout=0;loadout<shot_loadout_count;++loadout){const u32 slot=shot_file_slot(character,loadout);
            if(!core_source->open_sht(shot_file_name(slot),core.shots[slot]))return core_error(error,(std::string("SHT resource ")+shot_file_name(slot)+": "+core_source->error()).c_str());}
    }
    if(index==8&&!core_source->load_ecl("default.ecl",core.defaults))return core_error(error,"default ECL");
    core_loaded_jobs|=u16(1u<<index);return true;
}
bool GameSessionResources::prepare_core(){
    const bool selected_worker=effects&&effects->initial_cpp_worker_enabled();
#if (defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)) || defined(TH14_CPP_BOOT_WORKER)
    if(core_deferred&&effects&&effects->defer_initial_stage_decode()&&!(core_loaded_jobs&(1u<<9)))
        return core_error(error,"TH14 bootstrap resources still pending");
#endif
    if(core_deferred){for(u32 index=0;index<9;++index)if(!load_core_job(index))return false;core_deferred=false;}
    if(animation_manager){
        auto& a=*animation_manager;a.fallback_sprites=&core.ascii;
        if(!a.prepare_resource(core.ascii,2)||!a.prepare_resource(core.text,0)||!a.prepare_resource(core.front,5)
           ||!a.prepare_resource(core.bullet,7)||!a.prepare_resource(core.effect,8)
           ||(!selected_worker&&!a.prepare_resource(core.enemy,9))
           ||!a.prepare_resource(core.title,16)||!a.prepare_resource(core.title_variant,17))return core_error(error,a.last_error_text().c_str());
        if(!selected_worker)for(u32 i=0;i<player_file_count;++i)if(!a.prepare_resource(core.players[i],9))return core_error(error,a.last_error_text().c_str());
    }
    return true;
}
bool GameSessionResources::select_player(u32 character,u32 subtype){
    if(!core_source||!player_selection_valid(i32(character),i32(subtype)))return core_error(error,"invalid TH14 player resource selection");
    auto& file=core.players[character];
    // GameSession clears the previous world's VMs before replacing this file.
    // Release the old GPU generation before changing data at its stable address.
    if(animation_manager&&animation_manager->references(file))return core_error(error,"TH14 selected player resource still referenced");
    if(effects)effects->release_animation(file);
    file={};
    selected_player_pending=i32(character*2+subtype);
    if(effects&&effects->defer_initial_stage_decode())return true;
    return load_selected_player(character,subtype);
}
bool GameSessionResources::load_selected_player(u32 character,u32 subtype){
    if(!core_source)return core_error(error,"TH14 pending player resource owner changed");
    return load_selected_player_from(*core_source,character,subtype);
}
bool GameSessionResources::load_selected_player_from(GameResources& reader,u32 character,u32 subtype){
    if(!core_source||!player_selection_valid(i32(character),i32(subtype))||selected_player_pending!=i32(character*2+subtype))
        return core_error(error,"TH14 pending player resource owner changed");
    auto& file=core.players[character];
    if(animation_manager&&animation_manager->references(file))return core_error(error,"TH14 pending player resource still referenced");
    if(!reader.open_anm(player_file_name(character,subtype),file))return core_error(error,(std::string("player ANM: ")+file.detail).c_str());
    if(animation_manager&&(!effects||!effects->defer_initial_stage_decode())&&!animation_manager->prepare_resource(file,9))return core_error(error,animation_manager->last_error_text().c_str());
    selected_player_pending=-1;return true;
}
#endif

bool GameSessionResources::load_stage(GameResources& source,u32 number) {
#if PORT_TITLE == 14
    if(incoming_stage)return core_error(error,"cancel pending TH14 stage before resource replacement");
#endif
    auto next=std::make_unique<StageResources>();
#if PORT_TITLE == 14
    if(effects&&effects->defer_initial_stage_decode()){
        if(number<1||number>7)return core_error(error,"initial TH14 stage outside1..7");
        next->stage_number=number;
    }else
#endif
    if(!source.load_stage(number,*next)){error="stage resource "+last_resource+": "+source.error();return false;}
    if(effects&&!effects->prepare(*next)){effects->release(*next);error="stage graphics preparation: "+effects->last_error();return false;}
    retire_previous();previous_stage=std::move(stage);stage=std::move(next);
    stage_number=number;error.clear();
#if PORT_TITLE == 14
    if((!effects||effects->stage_ready())&&!prepare_stage())return false;
#endif
    return true;
}
#if PORT_TITLE == 14
bool GameSessionResources::begin_stage_loading(u32 number){
    if(number<2||number>6||incoming_stage)return core_error(error,"TH14 pending stage outside next-stage boundary");
    pending_stage=std::make_unique<StageResources>();incoming_stage=pending_stage.get();
    pending_number=number;stage_number=number;incoming_stage->stage_number=number;
    decoded_phases=requested_phases=ready_phases=0;error.clear();return true;
}
StageResourceEffects::DecodeResult StageResourceEffects::decode_stage_phase(GameResources& source,StageResources& stage,Phase phase,u32 selection){
    const bool decoded=phase==Phase::Hud?source.load_stage_hud(stage.stage_number,stage,selection):
        phase==Phase::Scene?source.load_stage_scene(stage.stage_number,stage):source.load_stage_timeline(stage.stage_number,stage);
    return decoded?DecodeResult::Ready:DecodeResult::Failed;
}
StageResourceEffects::CameraReadResult StageResourceEffects::read_stage_camera_word(const SceneCamera& source,u32 word,u32 offset,u32& value){
    if(word>=75||offset>sizeof(SceneCamera)-4||(offset&3u))return CameraReadResult::Failed;
    std::memcpy(&value,reinterpret_cast<const u8*>(&source)+offset,4);return CameraReadResult::Ready;
}
GameSessionResources::PhaseResult GameSessionResources::advance_stage_phase(GameResources& source,StageResourceEffects::Phase phase,u32 selection){
    if(!incoming_stage){error="TH14 incoming resource owner missing";return PhaseResult::Failed;}
    const u32 mask=1u<<u32(phase);
    if(ready_phases&mask)return PhaseResult::Ready;
    if(!(decoded_phases&mask)){
        StageResourceEffects synchronous;
        const auto decoded=(effects?effects:&synchronous)->decode_stage_phase(source,*incoming_stage,phase,selection);
        if(decoded==PhaseResult::Waiting)return decoded;
        if(decoded==PhaseResult::Failed){error="stage resource "+last_resource+": "+source.error();
            if(effects&&!effects->last_error().empty())error+="; "+effects->last_error();return decoded;}
        decoded_phases|=mask;
    }
    if(!(requested_phases&mask)){
        // A failing preparation may already own partial textures or queued
        // references; cancellation must include that attempted phase.
        requested_phases|=mask;
        if(effects&&!effects->prepare_stage_phase(*incoming_stage,phase)){error="stage graphics phase preparation: "+effects->last_error();return PhaseResult::Failed;}
    }
    if(effects&&!effects->stage_phase_ready(phase))return PhaseResult::Waiting;
    if(animation_manager){auto& a=*animation_manager;bool prepared=true;
        if(phase==StageResourceEffects::Phase::Hud)prepared=a.prepare_resource(incoming_stage->logo,6);
        else if(phase==StageResourceEffects::Phase::Scene)prepared=a.prepare_resource(incoming_stage->background,u16((pending_number&1)+3));
        else{
            prepared=a.prepare_resource(incoming_stage->enemies,9);
            for(u32 i=0;prepared&&i<incoming_stage->ecl_animations.size();++i)
                if(!incoming_stage->ecl_animation_names[i].empty())prepared=a.prepare_resource(incoming_stage->ecl_animations[i],u16(10+i));
        }
        if(!prepared){error=a.last_error_text();return PhaseResult::Failed;}
    }
    ready_phases|=mask;return PhaseResult::Ready;
}
bool GameSessionResources::publish_stage_scene(){
    if(!pending_stage||!(ready_phases&(1u<<u32(StageResourceEffects::Phase::Scene))))return core_error(error,"TH14 incoming scene not prepared");
    retire_previous();previous_stage=std::move(stage);stage=std::move(pending_stage);
    return incoming_stage==stage.get();
}
void GameSessionResources::finish_stage_parts(){
    if(incoming_stage&&effects)effects->finish_stage_phases(*incoming_stage,requested_phases);
    incoming_stage=nullptr;pending_number=0;
}
void GameSessionResources::cancel_stage_loading(){
    if(!incoming_stage)return;
    if(effects)effects->cancel_stage_decode(*incoming_stage);
    // The host wait already owns a generation before any graphics phase is
    // requested. A zero mask must still cancel that work before owner teardown.
    if(effects)effects->cancel_stage_phases(*incoming_stage,requested_phases);
    incoming_stage=nullptr;pending_stage.reset();pending_number=0;
    decoded_phases=requested_phases=ready_phases=0;
}
bool GameSessionResources::prepare_stage(){
    if(animation_manager){auto& a=*animation_manager;
        if(!a.prepare_resource(stage->background,u16((stage_number&1)+3))||!a.prepare_resource(stage->logo,6)||!a.prepare_resource(stage->enemies,9)){error=a.last_error_text();return false;}
        for(u32 i=0;i<stage->ecl_animations.size();++i)if(!a.prepare_resource(stage->ecl_animations[i],u16(10+i))){error=a.last_error_text();return false;}
    }
    return true;
}
#endif
void GameSessionResources::retire_previous(){if(previous_stage&&effects)effects->release(*previous_stage);previous_stage.reset();}

}
