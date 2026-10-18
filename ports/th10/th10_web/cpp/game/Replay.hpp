#pragma once
#include "Player.hpp"
#include "GameInput.hpp"
#include "Rng.hpp"
namespace th10 {
struct ReplayEnvironment;
struct ReplayKeys {u16 held,pressed,released;};
static_assert(sizeof(ReplayKeys)==6);
struct ReplayBuffer {
    ReplayKeys inputs[3600];
    ReplayKeys* input_cursor;
    u8 frame_rates[3600];
    u8* rate_cursor;
    ListNode<ReplayBuffer> node;
    void initialize() noexcept;
    bool append(ReplayKeys keys) noexcept;
    bool append_rate(u8 rate) noexcept;
};
static_assert(offsetof(ReplayBuffer,node)==0x6278 && sizeof(ReplayBuffer)==0x6284);
struct ReplayStage {
    std::int16_t stage;
    u16 seed;
    i32 frames,stream_bytes,score;
    std::int16_t power;
    u16 reserved_012;
    i32 item_value,faith,lives,rank;
    PlayerOption::FixedPoint position,history[33];
    PlayerOption::FixedPoint option_targets[4],option_positions[4],option_offsets[4],option_focused_offsets[4];
    i32 score_units,extend_index,focused;
    u32 flags;
    void initialize() noexcept {std::memset(this,0,sizeof(*this));}
    void capture_game(const GameEconomy& game) noexcept;
    void restore_game(GameEconomy& game,Rng& random,float* rate) const noexcept;
};
static_assert(offsetof(ReplayStage,option_targets)==0x134 && offsetof(ReplayStage,score_units)==0x1b4 && sizeof(ReplayStage)==0x1c4);
struct ReplayReader {
    ReplayKeys *inputs,*input_cursor;
    u8 *frame_rates,*rate_cursor;
    ReplayStage* stage;
    i32 frame;
    ListNode<ReplayReader> node;
    void rewind() noexcept {input_cursor=inputs;rate_cursor=frame_rates;frame=0;}
};
static_assert(sizeof(ReplayReader)==0x24);
struct ReplayHeader {
    u32 signature;
    u16 version,reserved_006;
    u32 reserved_008,user_offset,version_code,reserved_014,reserved_018,packed_bytes,unpacked_bytes;
    void initialize() noexcept {std::memset(this,0,sizeof(*this));signature=0x72303174;version=5;version_code=0x100;}
};
static_assert(sizeof(ReplayHeader)==0x24);
struct ReplayInfo {
    char name[12];
    i32 timestamp,score;
    u8 configuration[52];
    float slow_rate;
    i32 stage_count,character,shot_type,difficulty,last_stage,score_units;
    void initialize() noexcept {std::memset(this,0,sizeof(*this));}
};
static_assert(sizeof(ReplayInfo)==0x64);
struct Replay {
    u32 flags,manager_state;
    UpdateChainEntry *update_entry,*draw_entry;
    i32 mode;
    ReplayHeader* header;
    ReplayInfo* info;
    ReplayStage* stages[8];
    ListNode<ReplayBuffer> buffers[8];
    ListNode<ReplayBuffer>* active_buffer;
    ReplayReader readers[8];
    u8* unpacked;
    u8 recorded_fps,reserved_1c5[3];
    i32 elapsed;
    UpdateChainEntry* end_frame_entry;
    i32 active_stage;
    char filename[256];
    void initialize() noexcept {std::memset(this,0,sizeof(*this));}
    ListNode<ReplayBuffer>* add_buffer(i32 stage,ReplayEnvironment& environment);
    void clear_buffers(i32 stage,ReplayEnvironment& environment);
    i32 update_input(ReplayEnvironment& environment);
    i32 frame_action(const ReplayEnvironment& environment,bool check_pause) const noexcept;
    i32 draw(ReplayEnvironment& environment,bool check_pause) const;
    void prepare_stage(ReplayEnvironment& environment);
    void activate_stage(ReplayEnvironment& environment);
    i32 finish_recording(i32 clear,ReplayEnvironment& environment);
};
static_assert(offsetof(Replay,buffers)==0x3c && offsetof(Replay,readers)==0xa0 && offsetof(Replay,elapsed)==0x1c8 && sizeof(Replay)==0x2d4);
struct ReplayEnvironment {
    GameEconomy* game;
    GameInput* input;
    Rng* random;
    float* rate;
    const u32* display_flags;
    const u32* recording_mode;
    const u32* controller_flags;
    const float* measured_fps;
    Player** player;
    virtual void* allocate(u32 bytes)=0;
    virtual void release(void* memory)=0;
    virtual void configure_options(Player& player)=0;
    virtual void activate_player(Player& player)=0;
    virtual void draw_rate(const Vec3& position,u32 color,u8 fps)=0;
    virtual void timestamp(i32& destination)=0;
};
void restore_faith(GameEconomy& game,i32 frames,float* rate) noexcept;
}
