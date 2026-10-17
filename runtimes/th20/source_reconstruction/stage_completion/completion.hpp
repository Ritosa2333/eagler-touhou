#pragma once
#include "../gameplay/gameplay.hpp"
#include "../game_session/session.hpp"
#include "../progress_state/profile.hpp"
namespace th20::source::gameplay {
class CompletionEnvironment {
public:
 virtual ~CompletionEnvironment()=default;
 virtual GameController& game()=0;
 virtual game_session::Session& session()=0;
 virtual progress::Profile& profile()=0;
 virtual bool replay_has_stage(int)=0; //488770
 virtual void finish_player()=0; //4bd0c0
 virtual void update_playtime()=0; //4bce10
 virtual void finish_practice()=0; //4e59e0
 virtual void finish_replay()=0; //4e5f30
 virtual void mark_hud_clear()=0; //4bdbd0
 virtual void achievement(int)=0; //52f900, actual announcement owner still external
 virtual int stone_count(unsigned)=0; //4bd610
 virtual void notify_unlock(unsigned)=0; //4bddc0
 virtual void grant_stone(unsigned)=0; //4bcf60
 virtual bool unlock_flag(unsigned)=0; //4bd780
 virtual int& replay_selection()=0; //5afcfc
 virtual void select_scene(int,bool guarded)=0; //4be250 /4a0fb0
 virtual void advance_stage()=0; //4bd8a0
};
void mark_game_complete(GameController&) noexcept; //4bd980
int complete_stage(GameController&,CompletionEnvironment&); //4bc570
CompletionEnvironment& completion_environment();
inline int complete_stage(GameController& game){return complete_stage(game,completion_environment());}
}
