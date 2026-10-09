#pragma once
#include "SessionState.hpp"
namespace th15 {
struct ChapterCheckpointServices {
    virtual ~ChapterCheckpointServices()=default;
    virtual bool synchronize_resources()=0;
    virtual bool clear_saved_animations()=0;
    virtual bool save_player()=0;virtual bool save_enemies()=0;virtual bool save_background()=0;
    virtual bool save_bullets()=0;virtual bool save_items()=0;virtual bool save_effects()=0;virtual bool save_popups()=0;virtual bool save_bomb()=0;
    virtual bool retire_reward()=0;virtual bool retire_message()=0;virtual bool retire_scene_effect()=0;
    virtual bool restore_player()=0;virtual bool restore_enemies()=0;virtual bool restore_background()=0;
    virtual bool restore_bullets()=0;virtual bool restore_items()=0;virtual bool reset_spell()=0;virtual bool clear_lasers()=0;virtual bool restore_effects()=0;virtual bool restore_popups()=0;virtual bool restore_bomb()=0;
    virtual bool life_hud(i32,i32)=0;virtual bool bomb_hud(i32,i32)=0;
    virtual bool reset_gui()=0;virtual bool stop_sounds()=0;
    virtual bool begin_restart_effect()=0;virtual bool begin_restart_overlay()=0;
    virtual bool checkpoint_file(bool restoring)=0;
};
class ChapterCheckpoint {
    SessionState& progress;PlayerLifeSession& player;ItemScoreState& score;EnemyWorldState& enemies;std::string& music;ChapterCheckpointServices& host;
    SessionState saved_progress;PlayerLifeSession saved_player;ItemScoreState saved_score;i32 saved_total=0,saved_defeated=0,saved_rank=0;std::string saved_music;bool available=false;
    std::array<u8,0x1cc> file_metadata{};
    bool check(bool,const char*);void count_next_retry();void restore_progress();
public:
    std::string error;
    ChapterCheckpoint(SessionState& s,PlayerLifeSession& p,ItemScoreState& v,EnemyWorldState& e,std::string& wave,ChapterCheckpointServices& h):progress(s),player(p),score(v),enemies(e),music(wave),host(h){}
    bool capture(i32 chapter,PracticePatchEffects* practice=nullptr);bool restore();bool ready()const noexcept{return available;}
    bool write_file(std::vector<u8>&);
    bool read_file(const u8*,u32,u32& consumed);
    const SessionState& state()const noexcept{return saved_progress;}
    const PlayerLifeSession& player_state()const noexcept{return saved_player;}
    const ItemScoreState& score_state()const noexcept{return saved_score;}
};
}
