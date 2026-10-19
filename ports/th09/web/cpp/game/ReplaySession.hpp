#pragma once
#include "ReplayFile.hpp"
#include "GameInput.hpp"
#include "Rng.hpp"
namespace th09 {
namespace multiplayer { class WorldState; }
struct ReplayPlayerSettings {
    u32 points=0;
    u16 seed=0;
    u8 character=0,cpu=0,lives=0;
    i8 selector=0;
    u16 cpu_level=0;
    u8 background=0;
    u32 extends=0;
};
struct ReplayRoundSettings {
    ReplayPlayerSettings players[2];
    std::array<u8,204> config{};
    u8 difficulty=0,settings[4]{};
};
class ReplayPlayback {
public:
    enum class Step {inactive,advanced,finished};
    bool begin(const ReplayFile& file,u32 round,ReplayRoundSettings& settings);
    Step advance(u32 game_flags,GameInput (&inputs)[3],const bool (&auto_focus)[2]);
    u32 frame() const noexcept{return frame_number;}
    u32 length() const noexcept{return frames;}
    u32 sample_cursor() const noexcept{return samples;}
    i32 frame_rate=0,slowdown=0;
    // Callback result 6 asks the scheduler to run another simulation update.
    u32 after_update(u32 game_flags,i32 dialogue_state) const noexcept;
private:
    std::vector<u8> bytes;
    u32 offsets[4]{},frames=0,frame_number=0,samples=0;
};
struct InputCapture {
    u16 seed=0,flags=0;
    void sample(GameInput (&inputs)[3],const bool (&auto_focus)[2],Rng& rng,i32& external_event) noexcept;
};
class ReplayRecording {
    friend class multiplayer::WorldState;
public:
    static constexpr u32 chunk_frames=3598;
    struct Chunk {std::array<std::vector<u16>,3> inputs;std::vector<u8> rates;};
    void begin(u32 prior_ending_frames=0);
    bool advance(u32 game_flags,bool paused,const GameInput (&inputs)[3],const bool (&cpu)[2],u8 rate);
    u32 frame() const noexcept{return frame_number;}
    u32 ending_frames() const noexcept{return ending;}
    const std::vector<Chunk>& chunks() const noexcept{return parts;}
    std::vector<u8> stream(u32 group) const;
private:
    std::vector<Chunk> parts;
    u32 frame_number=0,ending=0;
};
}
