#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../sprite_renderer/sprite.hpp"
#include "../game_session/session.hpp"
namespace th20::source::card {
struct alignas(8) CardInf final:runtime::CallbackOwner {
    std::uint32_t background_handle,info_handles[3],effect_handle;
    recovered::Timer age; //24
    char name[64]; //34
    std::int32_t spell_index;
    std::uint32_t flags;
    std::int32_t bonus,initial_bonus,duration;
    std::uint32_t capture_index,frames,last_frames,padding_94;
    double start_time,elapsed;
    std::int32_t encoded_time;
    sprite::Vec3 position;
    std::uint32_t field_b8;
    std::int32_t view_index;
    game_session::Context* context;
    std::uint32_t padding_c4;
    CardInf(); //486bb0
    ~CardInf() override; //486d20
};
static_assert(sizeof(CardInf)==0xc8&&offsetof(CardInf,age)==0x24&&offsetof(CardInf,flags)==0x78&&offsetof(CardInf,start_time)==0x98&&offsetof(CardInf,context)==0xc0);
CardInf* controller(int index=0) noexcept;
CardInf* create(int); //488b20
int initialize(CardInf&,int); //487b10
int update(CardInf&); //486f60
int draw(CardInf&); //487590
void post_frame(CardInf&); //4872d0
void start(CardInf&,int spell,const char* name,int duration,int portrait); //487c30
void finish(CardInf&); //4878b0
int encode_time(int seconds,int hundredths) noexcept; //488990
bool invalid_encoded_time(int) noexcept; //4885e0
}
