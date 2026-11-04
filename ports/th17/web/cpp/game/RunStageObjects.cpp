#include "RunStageObjects.hpp"
namespace th17 {
struct RunStageObjects::Services final:BattleWorldServices {
    BattleWorldServices* host;explicit Services(BattleWorldServices& value):host(&value){}
    bool audio(i32 id,float pan,PlayerShots::SoundAction action,bool queued)override{return host->audio(id,pan,action,queued);}
    bool life_hud(i32 count,i32 pieces)override{return host->life_hud(count,pieces);}
    bool bomb_hud(i32 count,i32 pieces)override{return host->bomb_hud(count,pieces);}
    bool game_over()override{return host->game_over();}bool notice(i32 kind)override{return host->notice(kind);}
    bool popup(const Vec3& p,i32 amount,u32 color)override{return host->popup(p,amount,color);}bool trophy(i32 id)override{return host->trophy(id);}
    bool scene_message(i32 id)override{return host->scene_message(id);}bool message_complete(bool& done)override{return host->message_complete(done);}
    bool dialogue_present()const noexcept override{return host->dialogue_present();}
    bool spell_background_visible(bool visible)override{return host->spell_background_visible(visible);}
    bool spell_prepare_hud(bool start)override{return host->spell_prepare_hud(start);}
    bool spell_title(AnmVm& vm,const std::string& text)override{return host->spell_title(vm,text);}
    bool spell_result(i32 bonus,bool failed)override{return host->spell_result(bonus,failed);}
    bool chapter(i32 id)override{return host->chapter(id);}bool boss_segment(i32 boss,i32 index,float value,u32 color)override{return host->boss_segment(boss,index,value,color);}
    bool boss_segments(i32 count)override{return host->boss_segments(count);}bool stage_logo()override{return host->stage_logo();}
    bool background_interrupt(i32 label)override{return host->background_interrupt(label);}
};
RunStageObjects::RunStageObjects(StageAssets& a,AnmManager& manager,AnmEnvironment& env,Rng& game,Rng& visual,BattleWorldServices& host):services(std::make_unique<Services>(host)),ascii_bank(a.ascii),animation_effects(manager,game,visual,a.effect),battle(manager,env,game,visual,*services,a.scripts,a.shots,a.character,a.battle_resources()),popups(manager){}
RunStageObjects::~RunStageObjects()=default;
void RunStageObjects::bind(BattleWorldServices& host)noexcept{services->host=&host;}
bool RunStageObjects::initialize(){if(initialized)return error.empty();if(!battle.initialize(false)){error=battle.error;return false;}initialized=true;return true;}
bool RunStageObjects::initialize_popups(){if(popups_ready)return error.empty();if(!popups.initialize(ascii_bank)){error=popups.error;return false;}popups_ready=true;return true;}
bool RunStageObjects::replace_stage(StageAssets& a){if(!initialized){error="Run objects unavailable during stage replacement";return false;}if(!battle.replace_stage(a)){error=battle.error;return false;}return true;}
}
