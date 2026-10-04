#pragma once

#include "GameSessionResources.hpp"
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "GameEconomy.hpp"
#include "GameBattle.hpp"
#include "SceneCompositor.hpp"
#include "ReplayRecorder.hpp"
#include "GameInput.hpp"
#include "ScoreFile.hpp"
#include "GameConfig.hpp"
#include "TitleMenu.hpp"
#include "PauseMenu.hpp"
#include "Ending.hpp"
#include <string>
#include <memory>

namespace th11 {

// High-level lifetime and frame boundary shared by the native and WebAssembly
// front ends.  Managers that implement bullets, enemies and players are
// attached at the battle boundary; this class owns the deterministic timing
// and resource transition so both front ends observe the same order.
enum class GameSessionPhase : u8 { title, stage, paused, game_over, finished, ending };

struct GameSessionInput {
    u32 held=0;
    u32 pressed=0;
    u32 released=0;
    bool pause=false;
    bool confirm=false;
    bool cancel=false;
    float recording_fps=60;
};

struct GameSessionState {
    GameSessionPhase phase=GameSessionPhase::title;
    u32 frame=0;
    u32 stage=0;
    i32 character=0;
    i32 subtype=0;
    i32 difficulty=1;
    bool focus=false;
    bool replay=false;
    bool demo=false;
    bool practice=false;
};

class GameSession final {
public:
    GameSession();
    GameSessionResources resources;
    AnmManager animations;
    SceneCompositor compositor{animations};
    GameEconomy economy;
    SpellRecords spell_records;
    ClearRecords clear_records;
    ScoreFile scores;
    GameConfig config;
    Replay playback;
    ReplayRecorder recording;
    GameInput game_input;
    u64 recording_timestamp=0;
    std::unique_ptr<GameBattle> battle;
    std::unique_ptr<TitleMenu> title;
    std::unique_ptr<PauseMenu> pause_menu;
    std::unique_ptr<Ending> ending;
    std::unique_ptr<AsciiText> title_ascii;
    ScreenFades title_fades;
    TitleSelection menu_selection;
    bool interactive=false;
    GameSessionState state;
    PracticeState practice;
    std::string error;

    bool begin(GameResources& source, u32 stage=1, i32 character=0,
               i32 subtype=0, i32 difficulty=1, bool demo=false, bool replay=false,bool practice=false,const PracticeConfig* parameters=nullptr);
    bool open_title(GameResources&,bool first=false,TitleScreen screen=TitleScreen::Main);
    bool begin_replay(GameResources&,const u8*,u32,u32 stage=0,bool demo=false);
    bool save_replay(const char* name,std::vector<u8>& output,bool terminal=true,float slowdown=0);
    bool load_scores(const u8*,u32);
    bool save_scores(std::vector<u8>&);
    bool update(const GameSessionInput& input);
    bool pause();
    bool resume();
    bool return_to_title();
    bool start_stage(GameResources& source, u32 stage);
    bool draw(AnmRenderer& renderer,AsciiText* overlay=nullptr);
    bool ready() const noexcept { return error.empty() && state.stage!=0; }
    bool battle_callbacks_attached() const noexcept { return battle_attached; }
    const char* phase_name() const noexcept;
private:
    bool battle_attached=false;
    u32 last_held=0;
    bool last_pause=false;
    bool recording_active=false;
    u32 recording_seed=0;
    bool record_stage_entry(bool initial);
    bool open_result(bool completed);
    bool restart_run(bool continuation);
    bool open_ending();
    bool open_results();
    bool completed_run=false;
    i32 completed_score=0,completed_continues=0;
    u32 initial_stage=1;
    i32 next_continues=0;
    float paused_rate=1;
    GameInput menu_input;
    u32 title_idle=0,demo_index=0;
    i32 last_replay_file=0;
    GameResources* source=nullptr;
    bool fail(const char* message) { error=message?message:"session error"; return false; }
    void reset_state() noexcept;
};

}
