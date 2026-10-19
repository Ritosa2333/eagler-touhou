#pragma once
#include <cstddef>
#include <cstdint>
namespace th20::source::runtime {class CallbackOwner;}
namespace th20::source::game_session {
// Field names remain offsets until their owning gameplay logic is recovered.
struct Player {
    std::uint32_t fields_00[11];
    std::uint8_t bytes_2c[4];
    std::uint32_t fields_30[29];
    std::uint8_t bytes_a4[2],padding_a6[2];
    std::uint32_t fields_a8[2];
    std::uint8_t byte_b0,padding_b1[3];
    std::uint32_t fields_b4[15];
};
struct Context {
    runtime::CallbackOwner* primary_owner; // +00, 478e80
    void* objects_04[8];
    Player* current_player;                // +24, 464080
    void* object_28;                       // +28, 478ec0
    runtime::CallbackOwner* overlay_owner; // +2c, 464230
};
struct PlayerTable {
    Player players[2];
    std::uint32_t field_1e0,field_1e4;
    std::int32_t continue_count;
    std::uint32_t fields_1ec[14],padding_224;
};
struct alignas(8) Session {
    Context contexts[2];
    std::uint32_t field_60,field_64,field_68,flags;
    std::int32_t mode;
    std::uint32_t fields_74[4],padding_84;
    PlayerTable player_table;
    std::uint64_t field_2b0;
    std::uint32_t field_2b8,field_2bc;
    Session();                             // 422e40, static initializer401180
};
static_assert(sizeof(void*)==4);
static_assert(sizeof(Player)==0xf0 && offsetof(Player,bytes_a4)==0xa4 && offsetof(Player,byte_b0)==0xb0);
static_assert(sizeof(Context)==0x30 && offsetof(Context,current_player)==0x24);
static_assert(sizeof(PlayerTable)==0x228 && offsetof(PlayerTable,continue_count)==0x1e8);
static_assert(sizeof(Session)==0x2c0 && offsetof(Session,player_table)==0x88 && offsetof(Session,field_2b0)==0x2b0);
void construct_context(Context&) noexcept;   // 423320
void construct_player(Player&) noexcept;     // 423050
void construct_player_table(PlayerTable&) noexcept; //422f20
void construct_session(Session&) noexcept;  // 422e40, preserves original padding
extern Session session;                     // unique storage originally5ba568
Context& context(std::int32_t index) noexcept;//40bbc0
Player& player(std::int32_t index) noexcept; //4989f0 /488720
std::uint32_t& flags() noexcept;
std::int32_t& mode() noexcept;
runtime::CallbackOwner*& overlay_owner(std::int32_t index=0) noexcept; //464230/464250
runtime::CallbackOwner*& primary_owner(std::int32_t index=0) noexcept; //478e80/40c300
void bind_default_player() noexcept;         // startup40bbc0→498f40→488720→41df50
void set_flag0(Session&,std::uint32_t) noexcept; //4be220
void set_flag1(Session&,std::uint32_t) noexcept; //4bdd60
void clear_game_mode_flags() noexcept;
void add_continue_count(PlayerTable&,std::int32_t) noexcept; //4bccf0
void increment_continue_count() noexcept;   //4bccc0
}
