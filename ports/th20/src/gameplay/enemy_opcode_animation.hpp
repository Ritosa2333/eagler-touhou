#pragma once
#include "enemy_opcode.hpp"
namespace th20::source::gameplay {
class EnemyAnimationOpcodeServices {
public:
    virtual ~EnemyAnimationOpcodeServices()=default;
    virtual sprite::AnimationFile& file(EnemyController&,unsigned)=0;
    virtual std::uint32_t spawn(sprite::AnimationFile&,int,const sprite::Vec3*,float,int,unsigned)=0;
    virtual void delete_animation(std::uint32_t&)=0;
    virtual sprite::Animation* animation(std::uint32_t&)=0;
    virtual float height(sprite::Animation&)=0;
    virtual float width(sprite::Animation&)=0;
    virtual void hide(std::uint32_t)=0;
    virtual void interrupt(std::uint32_t,int)=0;
    virtual void layer(sprite::Animation&,int)=0;
    virtual void execute(sprite::Animation&)=0;
    virtual void remember(game_session::Context&,std::uint32_t)=0;
    virtual void effect(game_session::Context&,int,const sprite::Vec3&)=0;
    virtual void create_enemy(EnemyController&,const char*,const SpawnParameters&,Enemy*)=0;
    virtual Enemy* selected(EnemyController&,unsigned)=0;
    virtual Enemy* find(EnemyController&,unsigned)=0;
};
EnemyAnimationOpcodeServices& enemy_animation_opcode_services();
EnemyOpcodeResult execute_enemy_animation_opcode(EnemyOpcodeReader&,EnemyAnimationOpcodeServices&);
void change_enemy_animation(EnemyOpcodeReader&,EnemyAnimationOpcodeServices&); //496b90
void spawn_enemy_from_opcode(EnemyOpcodeReader&,EnemyAnimationOpcodeServices&); //496fb0
}
