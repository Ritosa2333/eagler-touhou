#pragma once
#include "GameBattle.hpp"
#include "ChapterCheckpoint.hpp"
#include "PlayerCheckpoint.hpp"
#include "EnemyCheckpoint.hpp"
#include "StageCheckpoint.hpp"
#include "BulletCheckpoint.hpp"
#include "ItemCheckpoint.hpp"
#include "EffectCheckpoint.hpp"
#include "BombCheckpoint.hpp"
#include "PopupManager.hpp"
#include "CheckpointFile.hpp"
namespace th15 {
// GUI, asynchronous resource work and file persistence remain owned by the
// session. All gameplay snapshots below are real, typed owners.
struct CheckpointSceneServices {
    virtual ~CheckpointSceneServices()=default;
    virtual bool synchronize_resources()=0;
    virtual bool retire_reward()=0;virtual bool retire_message()=0;virtual bool retire_scene_effect()=0;
    virtual bool life_hud(i32,i32)=0;virtual bool bomb_hud(i32,i32)=0;
    virtual bool reset_gui()=0;virtual bool stop_sounds()=0;
    virtual bool begin_restart_effect()=0;virtual bool begin_restart_overlay()=0;
    virtual bool checkpoint_file(bool)=0;
};
class BattleCheckpoint final:private ChapterCheckpointServices {
    GameBattle& game;AnmManager& animations;SessionState& progress;PopupManager& popups;CheckpointSceneServices& scene;
    i32* previous_alternating_counter=nullptr;bool counter_linked=true;
    AnmCheckpoint animation_pool;PlayerCheckpoint player;EnemyCheckpoint enemies;StageCheckpoint background;
    BulletCheckpoint bullets;ItemCheckpoint items;EffectCheckpoint effects;std::unique_ptr<BombCheckpoint> bomb;
    ChapterCheckpoint chapter;
    bool check(bool,const std::string&);
    bool synchronize_resources()override{return scene.synchronize_resources();}
    bool clear_saved_animations()override{animation_pool.clear();return true;}
    bool save_player()override;bool save_enemies()override;bool save_background()override;bool save_bullets()override;bool save_items()override;bool save_effects()override;bool save_popups()override;bool save_bomb()override;
    bool restore_player()override;bool restore_enemies()override;bool restore_background()override;bool restore_bullets()override;bool restore_items()override;bool restore_effects()override;bool restore_popups()override;bool restore_bomb()override;
    bool reset_spell()override;bool clear_lasers()override;
    bool retire_reward()override{return scene.retire_reward();}bool retire_message()override{return scene.retire_message();}bool retire_scene_effect()override{return scene.retire_scene_effect();}
    bool life_hud(i32 a,i32 b)override{return scene.life_hud(a,b);}bool bomb_hud(i32 a,i32 b)override{return scene.bomb_hud(a,b);}
    bool reset_gui()override{return scene.reset_gui();}bool stop_sounds()override{return scene.stop_sounds();}
    bool begin_restart_effect()override{return scene.begin_restart_effect();}bool begin_restart_overlay()override{return scene.begin_restart_overlay();}
    bool checkpoint_file(bool restoring)override{return scene.checkpoint_file(restoring);}
public:
    std::string error;
    BattleCheckpoint(GameBattle&,StageScene&,AnmManager&,SessionState&,PopupManager&,std::string& music,CheckpointSceneServices&,const std::array<i32,6>& banks,i32 bullet_bank);
    ~BattleCheckpoint(){release_binding();}
    void release_binding()noexcept{if(counter_linked&&game.items->alternating_counter==&progress.alternating_pieces)game.items->alternating_counter=previous_alternating_counter;counter_linked=false;}
    bool capture(i32 chapter);bool restore();bool ready()const noexcept{return chapter.ready();}
    CheckpointHeader file_header(i64 timestamp,u32 display_flags)const noexcept;
    bool write_file(std::vector<u8>&,i64 timestamp,u32 display_flags);
    bool prepare_file(CheckpointFile&,i64 timestamp,u32 display_flags);
    bool read_file(const u8*,u32,u32 display_flags);
    const ChapterCheckpoint& state()const noexcept{return chapter;}
    u32 animation_count()const noexcept{return animation_pool.count();}
};
}
