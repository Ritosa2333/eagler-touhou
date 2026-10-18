#pragma once
#include "Player.hpp"
namespace th10 {
struct PlayerResourceEnvironment {
    GameEconomy* game;
    Player** current;
    PlayerProfile** cached_profile;
    const char* const* profile_names;
    const float* hitbox_sizes;
    const float* attraction_speeds;
    const float* pickup_sizes;
    const float* focused_pickup_sizes;
    float* rate;
    AnmRegistry* registry;
    AnmFile** animation_slot;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken update_callback,draw_callback;
    virtual Player* allocate()=0;
    virtual AnmFile* load_animations(const char* name)=0;
    virtual i32 load_profile(Player& player,const char* name)=0;
    virtual void initialize_animation(Player& player)=0;
    virtual void configure_options(Player& player)=0;
    virtual void display_lives(i32 lives)=0;
    virtual void report_error()=0;
    virtual void release_animations(AnmFile& file)=0;
    virtual void delete_object(void* object)=0;
    virtual void free_bytes(void* memory)=0;
};
struct PlayerResources {
    Player& player;
    PlayerResourceEnvironment& environment;
    void initialize() noexcept;
    i32 start();
    void activate();
    void shutdown();
    static Player* create(PlayerResourceEnvironment& environment);
};
}
