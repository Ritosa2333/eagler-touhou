#pragma once
#include "weapon.hpp"
#include "../sprite_renderer/render_mesh.hpp"
namespace th20::source::overlay {
struct Counter {std::int32_t current=0,threshold=1500;}; //532850
class Environment;
Environment& environment();
struct WeaponStoneInf final:runtime::CallbackOwner {
    recovered::Timer age;
    std::uint32_t animation_handle;
    sprite::AnimationFile* file;
    Weapon* main_weapon;
    Weapon* focused_weapon;
    Weapon* unfocused_weapon;
    Weapon* passive_weapon;
    Counter counter;
    sprite::RenderMesh* mesh;
    recovered::Timer mesh_age;
    std::int32_t phase;
    sprite::Vec3 position;
    recovered::Timer cancellation_age;
    float cancellation_radius;
    std::uint8_t main_shooting,padding_79[3];
    std::int32_t view_index;
    game_session::Context* context;
    WeaponStoneInf();                                      //532880
    ~WeaponStoneInf() override;                             //532970
    void enable_callbacks() override;                      //5332b0
    void disable_callbacks() override;                     //533700
    void select_context(int) noexcept;                     //5345d0
};
static_assert(sizeof(WeaponStoneInf)==0x84&&offsetof(WeaponStoneInf,main_weapon)==0x28&&offsetof(WeaponStoneInf,phase)==0x54&&offsetof(WeaponStoneInf,main_shooting)==0x78);
WeaponStoneInf* controller(int index=0) noexcept;
void select_main(WeaponStoneInf&,int character,int stone,Environment&); //534420
void select_focused(WeaponStoneInf&,int character,int stone,Environment&); //5344d0
void select_unfocused(WeaponStoneInf&,int character,int stone,Environment&); //5347d0
void select_passive(WeaponStoneInf&,int character,int stone,Environment&); //534600
void refresh_selection(WeaponStoneInf&,int character,Environment&); //534080
void initialize_option(WeaponStoneInf&,player_entity::Option&,int); //533180
int script_variant(WeaponStoneInf&); //534130
const sprite::Vec2* option_offset(WeaponStoneInf&,int level,int index,bool focused); //4ff630/4ff760
int selected_stone(const game_session::Player&,int slot) noexcept; //4641d0
std::uint8_t inherited_stone(const game_session::Player&,int slot) noexcept; //464420
bool main_active(const WeaponStoneInf&); //464270
bool unfocused_active(const WeaponStoneInf&); //464480
bool focused_active(const WeaponStoneInf&); //4642e0
bool passive_active(const WeaponStoneInf&); //4643b0
void reset(WeaponStoneInf&,int stage_id); //5333b0
int initialize(WeaponStoneInf&,int,Environment&); //532fb0
int initialize(WeaponStoneInf&,int);
WeaponStoneInf* create_controller(int); //534dd0
void release(int); //534110/4bd390
int clear(WeaponStoneInf&,Environment&); //532f40
int clear(WeaponStoneInf&);
void activate_weapons(WeaponStoneInf&,bool); //5330a0
void shoot(WeaponStoneInf&,int,int,int); //5331e0
int shot_script_index(WeaponStoneInf&); //534210
int update(WeaponStoneInf&); //532b20
}
