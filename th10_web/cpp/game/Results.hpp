#pragma once
#include "MenuCursor.hpp"
#include "ScoreData.hpp"
#include "GameProgression.hpp"
#include "Replay.hpp"
namespace th10 {
struct ResultsEnvironment;
struct ResultsDrawEnvironment;
// Shared pause, result, score-name and replay-save interface (0x2c8 bytes).
struct Results {
    u32 flags;
    i32 state;
    UpdateChainEntry *update_entry,*draw_entry;
    Timer elapsed;
    u32 timer_flags;
    MenuCursor menu,keyboard;
    u32 menu_animation,background_animation,auxiliary_animation;
    i32 name_length,cleared,no_rank;
    Replay* previews[25];
    u8 reserved_250[0x64];
    char name[9];
    u8 reserved_2bd[3];
    float saved_rate;
    AnmFile* animation_file;
    void initialize(Results** current) noexcept;
    void reset_timer(const float* rate) noexcept;
    void activate() noexcept;
    void pause(ResultsEnvironment& environment);
    void resume(ResultsEnvironment& environment);
    void show_result(bool clear,ResultsEnvironment& environment);
    void dismiss(ResultsEnvironment& environment);
    void register_score(ResultsEnvironment& environment);
    i32 update(ResultsEnvironment& environment);
    void update_pause(ResultsEnvironment& environment);
    void update_results(ResultsEnvironment& environment);
    void initialize_name(ResultsEnvironment& environment,bool replay);
    void move_keyboard(ResultsEnvironment& environment);
    bool enter_character(ResultsEnvironment& environment,bool replay);
    void draw_name(Vec3 position,ResultsDrawEnvironment& environment) const;
    i32 draw(ResultsDrawEnvironment& environment) const;
};
static_assert(offsetof(Results,menu)==0x24 && offsetof(Results,keyboard)==0xfc);
static_assert(offsetof(Results,previews)==0x1ec && offsetof(Results,name)==0x2b4 && sizeof(Results)==0x2c8);
struct ResultsEnvironment {
    GameEconomy* game;
    Gui** gui;
    ScoreData** scores;
    Replay** replay;
    const StageConfiguration* stages;
    const StageConfiguration** current_stage;
    UpdateChainEntry** controller_update;
    const Timer* controller_elapsed;
    u32* controller_flags;
    const i32* replay_mode;
    const u32* engine_flags;
    const u32* display_flags;
    i32* pending_screen;
    const u16* pressed;
    const u16* repeated;
    float* rate;
    AnmFile** background_file;
    const char* alphabet;
    const double *active_time,*total_time;
    const bool* cheat_movement_used;
    bool repeat(u16 bits) const noexcept {return ((*pressed|*repeated)&bits)!=0;}
    virtual void sound(i32 id)=0;
    virtual void music_command(i32 command,const char* label)=0;
    virtual void result_music()=0;
    virtual u32 create_animation(AnmFile& file,i32 script)=0;
    virtual void capture_background(u32 animation)=0;
    virtual void interrupt(u32 animation,i32 label)=0;
    virtual u32 child(u32& animation,i32 script)=0;
    virtual void visible(u32 animation,bool value)=0;
    virtual void delete_animation(u32 animation)=0;
    virtual void select_screen(i32 screen)=0;
    virtual Replay* preview(const char* filename)=0;
    virtual void delete_replay(Replay* replay)=0;
    virtual void save_replay(const char* filename,const char* name)=0;
    virtual void timestamp(i32& destination)=0;
};
i32 insert_high_score(CharacterRecord& record,ResultsEnvironment& environment);
void finalize_score_display(Gui& gui,GameEconomy& game) noexcept;
}
