#pragma once
#include "special.hpp"
#include "../gameplay/enemy_spawn.hpp"
#include "../effect_system/effect.hpp"
namespace th20::source::special_state {
class Environment {
public:
    virtual ~Environment()=default;
    virtual game_session::Player& player()=0;
    virtual int session_mode()=0;
    virtual bool enemies_present()=0; //478060 nonnull and49be20: any nonzero of 16 handles at+54
    virtual bool boss_collecting()=0; //478160
    virtual bool special_blocked()=0; //5c6114 !=0
    virtual void spawn_enemy(const char*,const gameplay::SpawnParameters&)=0; //4a8920
    virtual bool enemy_exists(unsigned& handle)=0; //4aaac0, does not clear stale handle
    virtual sprite::Vec3 enemy_position(unsigned& handle)=0; //4aadb0
    virtual void effect(int,const effects::Parameters&)=0; //49db70
    virtual void delete_animation(unsigned&)=0; //44fcd0
    virtual void free_entry(Entry&)=0; //413b70 (source allocator)
    virtual void initialize_entry(Entry&,unsigned,int)=0; //512fb0
    virtual void clear_bullets(const sprite::Vec3&)=0; //47cf60
    virtual void clear_lasers(const sprite::Vec3&)=0; //4caad0
    virtual sprite::Vec3 position_of(void*)=0; //47a2c0
    virtual void text_style(int,int)=0; //4b8620
    virtual void text_font(int)=0; //488b00
    virtual void text_color(unsigned)=0; //470ad0
    virtual void text_alpha(std::uint8_t)=0; //488940
    virtual void text_scale(float,float)=0; //4b8d90
    virtual void text_save(int)=0; //4e6920
    virtual void text_restore()=0; //4e67e0
    virtual void text_line(const sprite::Vec3&,const char*)=0; //46ce90
    virtual void text_level(const sprite::Vec3&,unsigned)=0; //46cfa0 "Level ",number
};
}
