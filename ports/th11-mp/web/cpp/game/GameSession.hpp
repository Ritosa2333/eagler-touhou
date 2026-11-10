#pragma once

#include "GameSessionResources.hpp"
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "GameEconomy.hpp"
#include "GameBattle.hpp"
#include "SceneCompositor.hpp"
#include "ReplayRecorder.hpp"
#include "GameInput.hpp"
#ifdef TH11_MULTIPLAYER
#include <eagler/netplay/CooperativeMenuInput.hpp>
#endif
#include "ScoreFile.hpp"
#include "GameConfig.hpp"
#include "TitleMenu.hpp"
#include "PauseMenu.hpp"
#include "Ending.hpp"
#ifdef TH11_MULTIPLAYER
#include "Multiplayer.hpp"
#endif
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
    bool always_hitbox=false;
    bool interactive=false;
    GameSessionState state;
    PracticeState practice;
    std::string error;
#ifdef TH11_MULTIPLAYER
    u32 replay_stage_seed()const noexcept{return recording_seed;}
    std::array<ReplayStageState,3> multiplayer_stream_entries{};
    bool multiplayer_stream_pending=false;
    bool multiplayer_active=false;
    // Transaction control, not rewindable game state. A requested resource
    // boundary is undone, then retried with confirmed inputs and no history.
    bool multiplayer_rollback_boundary_requested=false;
    bool multiplayer_rollback_boundary();
    bool multiplayer_always_hitbox=false,multiplayer_local_visibility=false;
    MultiplayerOptions multiplayer_options{};
    u32 multiplayer_frame=0;
    std::array<bool,3> multiplayer_pause_held{};
    Netplay::CooperativeMenuInput<u32> multiplayer_menu_input;
    bool begin_multiplayer(GameResources&,const MultiplayerOptions&,const std::array<ReplayStageState,3>* stream=nullptr);
    bool update_multiplayer(const std::array<MultiplayerInput,3>&,const std::function<void()>& replay_entry={});
    MultiplayerSeatView multiplayer_seat(unsigned)const noexcept;
    u32 multiplayer_hash()const noexcept;
    bool multiplayer_open_result(bool completed,u32 held);
    bool multiplayer_open_ending(u32 held);
    bool multiplayer_open_clear_results(u32 held);
#endif

    void suppress_title_demo() noexcept { title_idle=0; }
    bool begin(GameResources& source, u32 stage=1, i32 character=0,
               i32 subtype=0, i32 difficulty=1, bool demo=false, bool replay=false,bool practice=false,const PracticeConfig* parameters=nullptr);
    bool open_title(GameResources&,bool first=false,TitleScreen screen=TitleScreen::Main);
    bool begin_replay(GameResources&,const u8*,u32,u32 stage=0,bool demo=false);
    // Diagnostic entry point: start one of the archive demos without the title
    // menu. Used by the Presentation Lab campaign runner.
    bool load_demo(u32 index);
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
    friend class WorldRollback;
    friend struct WorldRollbackTestAccess;
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
