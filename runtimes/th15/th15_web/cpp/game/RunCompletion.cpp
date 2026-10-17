#include "RunCompletion.hpp"
namespace th15 {
bool RunCompletion::fail(const std::string& reason){if(error.empty())error=reason.empty()?"Run completion failed":reason;return false;}
bool RunCompletion::complete(){if(!error.empty())return false;SessionCompletion controller(progress,scene.battle.session,scene.battle.score,scene.hud.flags,scene.hud.clear_bonus,runtime.ending_frames,records,*this);return controller.complete()||fail(controller.error);}
bool RunCompletion::clear_notice(){return scene.hud.stage_clear()||fail(scene.hud.error);}
bool RunCompletion::all_clear_bonus()const{const auto* p=scene.battle.enemy_world.practice;return p&&p->enabled&&p->all_clear_bonus;}
bool RunCompletion::synchronize_clear_score(){scene.hud.displayed_score=scene.battle.score.score;return true;}
bool RunCompletion::finish_player_options(){return scene.battle.player->finish_stage_options()||fail(scene.battle.player->error);}
bool RunCompletion::end_bomb(){return scene.battle.bomb->stop()||fail(scene.battle.bomb->error);}
bool RunCompletion::prepare_ending(){return platform.prepare_ending(scene);}
bool RunCompletion::finish_replay(){return platform.finish_replay();}
bool RunCompletion::finish_practice(){return platform.finish_practice();}
bool RunCompletion::queue_next_scene(){return platform.queue_next_stage();}
bool RunCompletion::select_stage_resources(i32 stage){return platform.prepare_next_stage(stage);}
}
