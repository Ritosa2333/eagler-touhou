#pragma once
// Test-only required dependency fixture, never part of production libraries.
#include "enemy_damage.hpp"
#include <array>
#include <functional>
#include <stdexcept>
#include <vector>
struct EnemyDamageFixture final:th20::source::gameplay::EnemyDamageServices {
    using State=th20::source::gameplay::EnemyState;
    using Vec3=th20::source::sprite::Vec3;
    using Vec2=th20::source::sprite::Vec2;
    using Animation=th20::source::sprite::Animation;
    using Session=th20::source::game_session::Session;
    Session storage;Session* global=&storage;
    std::array<std::uint8_t,0x1cc> hud{};
    float scale=1;std::vector<int> events;
    std::function<Animation*(std::uint32_t)> lookup;
    std::function<void(void*)> timeout;
    int amount=0,defeat_result=0,script_result=0,collision_result=0;
    std::uint32_t hit_flag=0;
    bool rectangle=false;int preview=-1;float angle=0,radius=0;Vec2 size{};Vec3 origin{};
    std::vector<std::pair<int,float>> sounds;
    const float* timer_rate() override{return &scale;}
    Session& session() override{return *global;}
    void* boss_hud() override{return hud.data();}
    Animation* animation(std::uint32_t handle) override{if(!lookup)throw std::logic_error("Missing test animation lookup");return lookup(handle);}
    void replace_animation(std::uint32_t&,int script) override{events.push_back(1000+script);}
    int calculate_damage(th20::source::game_session::Context&,const Vec3& position,const Vec2* dimensions,float a,float r,std::uint32_t* flag,Vec3*,int p,std::uint32_t) override {
        events.push_back(10+p);origin=position;rectangle=dimensions!=nullptr;if(dimensions)size=*dimensions;angle=a;radius=r;*flag=hit_flag;preview=p;return amount;
    }
    int defeat(void*) override{events.push_back(20);return defeat_result;}
    void clear_scripts(void*) override{events.push_back(30);}
    void select_script(void*,const char*) override{events.push_back(31);}
    int run_scripts(void*,float rate) override{if(rate!=scale)throw std::logic_error("Wrong phase script clock");events.push_back(32);return script_result;}
    void notify_timeout(void* secondary) override{events.push_back(40);if(!timeout)throw std::logic_error("Missing timeout observer");timeout(secondary);}
    void sound(int id,float x) override{events.push_back(50);sounds.push_back({id,x});}
    int player_circle(void*,const Vec3& p,float r,int flag) override{if(flag)throw std::logic_error("Collision flag must be zero");events.push_back(60);origin=p;radius=r;return collision_result;}
    int player_rectangle(void*,const Vec3& p,float a,float w,float h,int flag) override{if(flag)throw std::logic_error("Collision flag must be zero");events.push_back(61);origin=p;angle=a;size={w,h};return collision_result;}
    void player_graze(void*,const Vec3& p,int flag) override{if(flag)throw std::logic_error("Graze flag must be zero");events.push_back(62);origin=p;}
};
