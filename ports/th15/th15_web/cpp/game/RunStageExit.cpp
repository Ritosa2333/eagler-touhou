#include "RunStageExit.hpp"
namespace th15 {
bool RunStageExit::fail(const std::string& reason){if(error.empty())error=reason.empty()?"Run stage exit failed":reason;return false;}
bool RunStageExit::finish(bool restart_audio){
 if(!error.empty()||!run.scene()||!driver.driver())return fail("Stage exit requires a scheduled scene");
 outgoing=run.scene();if(outgoing->battle.spell.flags&1)return fail("Stage exit requires completed active spell");
 bomb_owned=outgoing->battle.bomb!=nullptr;spell_owned=true;carried=false;presentation={};
 SessionTeardown controller(progress,outgoing->battle.session,presentation,*this);
 const bool ok=controller.finish(12,restart_audio);if(!ok)return fail(controller.error);
 return carried&&!run.scene()&&run.previous_scene()==outgoing&&!driver.driver()||fail("Stage exit did not transfer its background and detach its callbacks");
}
bool RunStageExit::owns(SessionObject kind)const noexcept{
 switch(kind){case SessionObject::PreviousBackground:return run.previous_scene()!=nullptr;
 case SessionObject::Replay:return replay.live()||replay.replay();
 case SessionObject::Enemies:return outgoing&&outgoing->battle.enemies!=nullptr;
 case SessionObject::Bomb:return bomb_owned;case SessionObject::Spell:return spell_owned;
 default:return outgoing!=nullptr;}
}
bool RunStageExit::stop_checkpoint_worker(){return platform.stop_checkpoint_worker();}
bool RunStageExit::save_records(){return platform.save_records();}
bool RunStageExit::clear_session_links(){return platform.clear_session_links();}
bool RunStageExit::reset_transition(){return platform.reset_transition();}
bool RunStageExit::release(SessionObject kind){
 auto& battle=outgoing->battle;
 switch(kind){
 case SessionObject::PreviousBackground:run.release_previous();return true;
 case SessionObject::Enemies:return battle.release_stage_enemies()||fail(battle.error);
 case SessionObject::Bomb:if(!battle.retire_bomb_visuals())return fail(battle.error);battle.bomb.reset();bomb_owned=false;return true;
 case SessionObject::Spell:if(!battle.retire_spell_visuals())return fail(battle.error);spell_owned=false;return true;
 default:return fail("Full run object release requested during normal stage exit");
 }
}
bool RunStageExit::clear_checkpoint_data(){return fail("Normal stage exit must retain checkpoint storage");}
bool RunStageExit::reset_gui_for_stage(){return outgoing->hud.reset_for_retry()||fail(outgoing->hud.error);}
bool RunStageExit::carry_background(){if(!run.carry_current_background())return fail(run.error);carried=true;return true;}
bool RunStageExit::disable_callbacks(SessionObject kind){
 switch(kind){case SessionObject::PauseMenu:return platform.disable_pause_callbacks();
 case SessionObject::Bullets:outgoing->battle.disable_bullet_callback();return true;
 case SessionObject::Replay:driver.driver()->enable_input(false);return true;
 default:return fail("Unknown retained stage callback owner");}
}
bool RunStageExit::reset_items(){return outgoing->battle.items->reset()||fail(outgoing->battle.items->error);}
bool RunStageExit::clear_enemies(){return outgoing->battle.enemies&&outgoing->battle.enemies->clear()||fail("Retained enemy clear failed");}
bool RunStageExit::retire_stage_animations(){outgoing->retire_projectile_effect_animations();return true;}
bool RunStageExit::detach_scene_callbacks(){if(!outgoing->suspend_for_transition())return fail(outgoing->error);driver.detach();return true;}
bool RunStageExit::queue_music(i32 mode){return platform.queue_exit_music(mode);}
bool RunStageExit::reset_audio_slots(){return platform.reset_audio_slots();}
}
