#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../game_session/session.hpp"
#include "../platform_services/configuration.hpp"
#include <cstdint>
namespace th20::source::replay {
#pragma pack(push,4)
struct FileHeader { //507480,30
    std::uint32_t magic;
    std::uint16_t version;
    std::uint8_t byte_06,byte_07,byte_08,padding_09[3];
    std::uint32_t field_0c,field_10,field_14;
    std::uint8_t byte_18,byte_19;
    std::uint16_t word_1a;
    std::uint32_t header_size,user_size,stage_size,packed_size,unpacked_size;
    FileHeader() noexcept;
};
struct UserHeader { //507360,100
    std::uint32_t field_00,field_04;
    std::uint8_t byte_08,byte_09,flags,byte_0b,padding_0c[4];
    std::int64_t timestamp;
    std::uint32_t fields_18[2];
    platform::Configuration configuration;
    std::uint32_t fields_d0[3],stones[4];
    std::uint8_t inherited[4];
    std::int32_t difficulty,finished_stage,field_f8,spell;
    UserHeader() noexcept;
};
struct StageRecord { //5077d0,2a0
    std::int32_t stage;
    std::uint32_t seed,frame_count,data_bytes;
    std::int32_t fixed_x,fixed_y;
    std::uint32_t field_18;
    std::int32_t capture_times[20];
    std::uint32_t padding_6c;
    game_session::PlayerTable player_table;
    std::uint32_t flags,padding_29c;
    StageRecord() noexcept;
};
struct InputFrame {std::uint16_t current,pressed,released;};
struct RecordingChunk { //507720,3d87c
    InputFrame inputs[36000];
    std::uint8_t fps[36000];
    InputFrame* input_cursor;
    std::uint8_t* fps_cursor;
    scheduler::Link link;
    RecordingChunk() noexcept;
    ~RecordingChunk();
    bool append(std::uint16_t,std::uint16_t,std::uint16_t); //509e40
    bool append_fps(std::uint8_t); //509ee0
    int frame_count() const noexcept; //50a0a0
    int fps_count() const noexcept; //50a170
};
struct PlaybackCursor { //5076b0,2c
    InputFrame* inputs;
    InputFrame* input_cursor;
    std::uint8_t* fps;
    std::uint8_t* fps_cursor;
    StageRecord* stage;
    std::int32_t frame;
    scheduler::Link link;
    PlaybackCursor() noexcept;
    ~PlaybackCursor();
    void rewind(bool active) noexcept; //509da0 /509e10
};
class ReplayInf final:public runtime::CallbackOwner { //507560,360
public:
    std::int32_t mode,fast_forward;
    FileHeader* header;
    UserHeader* user;
    StageRecord* stages[8];
    scheduler::Link recordings[8];
    scheduler::Link* active_chunk;
    std::uint32_t chunk_count;
    PlaybackCursor playback[8];
    std::uint8_t* decoded;
    std::uint8_t fps,padding_24d[3];
    std::int32_t frame;
    scheduler::Node* additional_update;
    std::int32_t active_stage;
    std::uint32_t flags;
    char filename[256];
    ReplayInf();
    ~ReplayInf() override; //5078b0
    void enable_callbacks() override; //508f70
    void disable_callbacks() override; //509dd0, only extra update and draw
    scheduler::Link* add_recording_chunk(int); //50a2e0
    void clear_recording(int); //509fa0
    void reset_stage(); //50a6b0
};
#pragma pack(pop)
static_assert(sizeof(FileHeader)==0x30&&offsetof(FileHeader,packed_size)==0x28);
static_assert(sizeof(UserHeader)==0x100&&offsetof(UserHeader,configuration)==0x20&&offsetof(UserHeader,stones)==0xdc);
static_assert(sizeof(StageRecord)==0x2a0&&offsetof(StageRecord,player_table)==0x70&&offsetof(StageRecord,flags)==0x298);
static_assert(sizeof(InputFrame)==6&&sizeof(RecordingChunk)==0x3d87c&&offsetof(RecordingChunk,input_cursor)==0x3d860);
static_assert(sizeof(PlaybackCursor)==0x2c&&offsetof(ReplayInf,playback)==0xe8&&offsetof(ReplayInf,decoded)==0x248&&sizeof(ReplayInf)==0x360);
ReplayInf* controller() noexcept;
ReplayInf* create(int,const char*); //50a930
ReplayInf* read_metadata(const char*); //50a8e0
int initialize(ReplayInf&,int,const char*); //508480
int load(ReplayInf&,const char*); //508b90
int update_recording(ReplayInf&); //507f80
int update_playback(ReplayInf&); //507b70
int update_fast_forward(ReplayInf&); //5081e0
int draw(ReplayInf&); //508360
int prepare_save(ReplayInf&,int); //508310
void save(ReplayInf&,const char* filename,const char* name,int,int); //509280
}
