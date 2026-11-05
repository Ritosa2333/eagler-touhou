#pragma once
#include "RunSession.hpp"
#include "SessionInitialization.hpp"
#include "RecordStore.hpp"
namespace th15 {
struct RunConstructionServices {
 virtual ~RunConstructionServices()=default;
 virtual bool prepare_pause_menu()=0;virtual bool prepare_checkpoint_storage()=0;
 virtual bool restore_enemy_checkpoint(StageGameplay&)=0;
 virtual bool load_stage_theme(i32 stage)=0;virtual bool load_player_theme(i32 stage,bool boss)=0;
};
// Execute the original construction controller against real named run objects.
// Allocation of pure C++ owners precedes this; visual initialization, replay
// snapshots and callback activation remain in their original ordered phases.
class RunInitialization final:private SessionInitializationServices {
 RunGameplay& run;SessionState& progress;RecordStore& records;SessionReplay& replay;RunSession& driver;RunConstructionServices& platform;
 StageCamera camera;i32 destination=0;const u8* replay_bytes=nullptr;u32 replay_size=0;bool registered=false;
 StageGameplay& scene(){return *run.scene();}bool fail(const std::string&);ReplayRunState state();
 bool bomb_hud(i32,i32)override;bool create_player()override;bool configure_player()override;bool register_session()override;
 bool create_replay()override;bool reset_replay()override;bool create_background()override;bool create_gui()override;bool reset_gui()override;
 bool create_bullets()override;bool create_items()override;bool create_lasers()override;bool create_pause_menu()override;bool create_popups()override;bool create_checkpoint_storage()override;
 bool create_enemies()override;bool restore_enemies()override;bool create_bomb()override;bool create_spell()override;
 bool load_stage_music()override;bool load_player_music(bool)override;bool finish_scene()override;
public:
 std::string error;
 RunInitialization(RunGameplay& r,SessionState& p,RecordStore& v,SessionReplay& q,RunSession& d,RunConstructionServices& h):run(r),progress(p),records(v),replay(q),driver(d),platform(h){}
 bool initialize(const StageCamera&,i32 scene_destination,const u8* file=nullptr,u32 size=0);
};
}
