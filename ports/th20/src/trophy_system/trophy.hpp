#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../sprite_renderer/sprite.hpp"
#include <span>
#include <string_view>
namespace th20::source::trophy {
struct Message {
    std::int32_t id;
    std::uint8_t title[256],description[2][3][256];
};
static_assert(sizeof(Message)==0x704);
extern Message* messages; //5c685c,128 entries; byte strings use52f100 codec
void encode_string(std::uint8_t* output,const char* input); //52f100
const char* decode_string(const std::uint8_t* input); //52f060, actual shared256 byte result
void parse_messages(std::span<Message,128>,std::string_view); //text portion52e210
int initialize_resources(); //52e210
void release_resources(); //52e730
// Original MSVC checked deque<int>: allocator, proxy, block map, map size,
// element offset and count. Each allocated block stores four integers.
struct Queue {
    std::pmr::memory_resource* allocator;
    struct Proxy {void* container;void* iterator;}* proxy;
    std::int32_t** map;
    std::uint32_t map_size,offset,count;
    Queue();~Queue();
    void push(std::int32_t);
    std::int32_t pop_front();
};
static_assert(sizeof(Queue)==0x18);
struct TrophyInf final:runtime::CallbackOwner {
    sprite::AnimationFile* animation_file; //10
    Queue pending; //14
    std::int32_t previous_state,state,state_frame; //2c
    recovered::Timer age; //38
    std::uint32_t handles[3]; //48
    TrophyInf(); //52da70
    ~TrophyInf() override; //52dc00
    void enable_callbacks() override {scheduler::enable(*update_node);scheduler::enable(*draw_node);} //4a0a70
};
static_assert(sizeof(TrophyInf)==0x54&&offsetof(TrophyInf,state)==0x30&&offsetof(TrophyInf,age)==0x38);
extern TrophyInf* controller; //5c6858
void change_state(TrophyInf&,int); //52f800
int initialize(TrophyInf&,int); //52e170
int update(TrophyInf&); //52df30
bool achieved(unsigned); //52ca70
void mark_achieved(unsigned); //52f840
TrophyInf* announce(unsigned); //52f900
}
