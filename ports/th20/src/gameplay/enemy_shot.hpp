#pragma once
#include "enemy_opcode.hpp"
#include "../bullet_system/command.hpp"
namespace th20::source::gameplay {
// Exact value after the shared_ptr in EnemyQueuedRecord (node payload+8).
struct EnemyShotValues {
    bullet::ShotParameters parameters;                       //record+8
    std::int32_t command_cursor;                             //30
    sprite::Vec3 offset,absolute;                            //34/40, absolute.z doubles as enabled flag
};
static_assert(sizeof(EnemyShotValues)==0x44);
EnemyShotValues shot_values(const EnemyQueuedRecord&) noexcept;
void store_shot_values(EnemyQueuedRecord&,const EnemyShotValues&) noexcept;
class EnemyShotOpcodeServices {
public:
    virtual ~EnemyShotOpcodeServices()=default;
    virtual std::shared_ptr<void> metadata(const bullet::ShotMetadata* copy)=0;
    virtual void fire(game_session::Context&,const bullet::ShotParameters&,const std::shared_ptr<void>&,std::uint32_t)=0;
    virtual void cancel_all(game_session::Context&)=0;
    virtual void cancel_circle(game_session::Context&,const sprite::Vec3&,float,bool nearby)=0;
    virtual sprite::Vec3 player_position(game_session::Context&)=0;
    virtual void mesh(EnemyState&,float,std::uint32_t)=0;
    virtual void background_event(int)=0;
    virtual std::uint32_t callback(unsigned table,int index)=0;
    virtual void invoke_callback(EnemyState&,int)=0;
    virtual void add_score(game_session::Context&,const sprite::Vec3&,int)=0;
};
EnemyQueuedRecord& queued_shot(EnemyState&,int,EnemyShotOpcodeServices&,bool unique); //498b80(copy on write)/498d10(shared)
sprite::Vec3 shot_origin(const EnemyState&,const EnemyShotValues&); //48c010 repeated original position selection
EnemyShotOpcodeServices& enemy_shot_opcode_services();
EnemyOpcodeResult execute_enemy_bullet_opcode(EnemyOpcodeReader&,EnemyShotOpcodeServices&);
}
