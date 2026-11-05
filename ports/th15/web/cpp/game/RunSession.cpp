#include "RunSession.hpp"
#include "PracticeSections.hpp"
namespace th15 {
bool RunSession::fail(const std::string& reason){if(error.empty())error=reason.empty()?"Run session operation failed":reason;return false;}
ReplayRunState RunSession::replay_state(){auto& s=scene();return {progress,s.battle.session,s.battle.score,s.battle.enemy_world,s.battle.player->motion,s.music.current_music_wave};}
bool RunSession::attach(const StageCamera& view,i32 destination,bool prepare_replay,bool reset_audio){
 if(gameplay||!run.scene()||!error.empty())return fail("Run session cannot attach this scene");camera=view;scene_destination=destination;restart_audio=reset_audio;
 if(!replay.live()&&!replay.replay())return fail("Scene entry requires initialized recording or playback");
 if(prepare_replay){auto state=replay_state();if(!replay.prepare_stage(state))return fail(replay.error);}
 if(!scene().enable_entry_updates())return fail(scene().error);
 gameplay=std::make_unique<SessionGameplay>(scene(),progress,static_cast<SessionGameplayServices&>(*this),replay.live(),replay.replay());if(!gameplay->error.empty())return fail(gameplay->error);
 gameplay->enable_input(false);activation=std::make_unique<SessionActivation>(progress,scene().battle.session,gameplay->runtime.age,static_cast<SceneActivationServices&>(*this));return true;
}
bool RunSession::step(const SessionGameplayInput& input){if(!gameplay||!activation)return fail("Run session is not attached");if(!gameplay->step(input))return fail(gameplay->error);return error.empty();}
bool RunSession::ending_fade(){return platform.ending_fade();}
bool RunSession::finish_replay(){return platform.finish_replay();}
bool RunSession::destination(SessionDestination d){return platform.destination(d);}
bool RunSession::load_scene(bool& waiting){const SceneActivationFrame frame{run.previous_scene()!=nullptr,false,restart_audio};return activation->load(frame,waiting)||fail(activation->error);}
bool RunSession::activate_scene(){const SceneActivationFrame frame{run.previous_scene()!=nullptr,false,restart_audio};return activation->activate(frame)||fail(activation->error);}
bool RunSession::release_background(){if(!run.previous_scene())return true;if(!platform.release_background())return false;run.release_previous();return true;}
bool RunSession::load_checkpoint_file(bool& restored){return platform.load_checkpoint_file(restored);}
bool RunSession::restart_overlay(i32 script){return platform.restart_overlay(script);}
bool RunSession::restart_effect(i32 label){return platform.restart_effect(label);}
bool RunSession::prepare_stage_music(){return platform.prepare_stage_music();}
bool RunSession::start_stage_music(){const auto* d=stage_definition(progress.stage);if(!d)return fail("Stage music definition unavailable");scene().music.current_music_wave=std::string(d->music[0])+".wav";return platform.start_stage_music();}
bool RunSession::start_entry_music(){
 // Purple 43d5c1 selects slot 1 iff THBGMTest() is nonzero. This hook
 // belongs to scene entrance, not ordinary MSG requests or chapter retries.
 const auto* p=run.practice;const int section=p&&p->active?p->run.section:0;
 return section>0&&section<int(std::size(practice_sections))&&practice_sections[section].bgm?start_boss_music():start_stage_music();
}
bool RunSession::start_boss_music(){const auto* d=stage_definition(progress.stage);if(!d)return fail("Boss music definition unavailable");scene().music.current_music_wave=std::string(d->music[1])+".wav";return platform.start_boss_music();}
bool RunSession::seek_stage_music(double seconds){return platform.seek_stage_music(seconds);}
bool RunSession::demo_fade(){return platform.demo_fade();}

bool RunSession::update_overlays(){return platform.update_overlays();}
bool RunSession::clear_stage_intro(){return scene().hud.clear_intro()||fail(scene().hud.error);}
bool RunSession::discard_stage_assets(){return entry.discard_stage_assets();}
bool RunSession::return_to_title(bool selection){return entry.return_to_title(selection);}
bool RunSession::initialize_background(){return scene().initialize_background(camera)||fail(scene().error);}
bool RunSession::capture_previous_background(){auto* previous=run.previous_scene();return previous?entry.capture_previous_background(*previous):fail("Previous stage capture has no scene");}
bool RunSession::prepare_background_transition(){return entry.prepare_background_transition();}
bool RunSession::transition_banner(){return entry.transition_banner();}
bool RunSession::reset_bullets(){auto& b=*scene().battle.bullet_scene;b.reset();return b.error.empty()||fail(b.error);}
bool RunSession::reset_player(){auto& p=*scene().battle.player;return p.reset_for_stage()||fail(p.error);}
bool RunSession::reset_items(){auto& i=*scene().battle.items;return i.reset()||fail(i.error);}
bool RunSession::reset_enemies(){auto& e=*scene().battle.enemies;return e.clear()||fail(e.error);}
bool RunSession::reset_lasers(){auto& l=scene().battle.laser_scene->manager;return l.clear()||fail(l.error);}
bool RunSession::prepare_replay_stage(){auto state=replay_state();if(!replay.activate_stage(state))return fail(replay.error);gameplay->reset_input();gameplay->enable_input(true);return true;}
bool RunSession::start_main_script(){return scene().start_main_script()||fail(scene().error);}
bool RunSession::initialize_hud(){return scene().initialize_hud(scene_destination)||fail(scene().error);}
bool RunSession::enable_game_callbacks(){return scene().enable_game_callbacks()||fail(scene().error);}
bool RunSession::configure_player_options(){auto& p=*scene().battle.player;return p.configure_options()||fail(p.error);}
bool RunSession::interrupt_entrance(){return entry.interrupt_entrance();}
bool RunSession::retire_restart_overlay(){return entry.retire_restart_overlay();}
bool RunSession::interrupt_resume_overlay(){return entry.interrupt_resume_overlay();}
bool RunSession::restore_pending_progress(){return entry.restore_pending_progress();}
bool RunSession::queue_music(i32 mode){if(mode==2){const auto* d=stage_definition(progress.stage);if(!d)return fail("Stage music definition unavailable");scene().music.current_music_wave=std::string(d->music[0])+".wav";}return entry.queue_music(mode);}
bool RunSession::unlock_current_music(){return entry.unlock_current_music();}
}
