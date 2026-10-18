#include "rewards.hpp"
#include "../gameplay/enemy_frame.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include <cstring>
namespace th20::source::item {
namespace {
void* hud(){return gameplay::unrecovered::boss_hud_005c06a4();}
class GameRewards final:public RewardEnvironment {
public:
    bool hud_available() override{return hud()!=nullptr;}
    void hud_bombs(int a,int b,int c) override{unrecovered::hud_bombs_004b8650(hud(),a,b,c);}
    void hud_lives(int a,int b,int c) override{unrecovered::hud_lives_004b8bf0(hud(),a,b,c);}
    void hud_notice(int a,int b) override{unrecovered::hud_notice_004b90e0(hud(),a,b);}
    void sound(int id) override{program_entry::thread_registry.request_effect(id,0);}
    void sound_at(int id,float x) override{program_entry::thread_registry.request_effect_at(id,x);}
    void floating_score(void* owner,const sprite::Vec3& position,int amount,std::uint32_t color) override{unrecovered::floating_score_00510710(owner,position,amount,color);}
    void refresh_power(void* entity,int value) override{unrecovered::refresh_player_power_004faca0(*static_cast<runtime::CallbackOwner*>(entity),value);}
    bool boss_collecting() override{return environment().boss_collecting();}
    bool special_active() override{std::uint8_t value;std::memcpy(&value,reinterpret_cast<std::uint8_t*>(unrecovered::special_state_00513dd0())+0xb0,1);return value!=0;}
    void start_special_phase() override{unrecovered::start_special_phase_00534d00();}
};
}
RewardEnvironment& reward_environment(){static GameRewards host;return host;}
}
