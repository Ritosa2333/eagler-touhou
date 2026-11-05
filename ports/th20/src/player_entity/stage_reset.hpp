#pragma once
#include "owner.hpp"
namespace th20::source::player_entity {
class StageResetServices {
public:
    virtual ~StageResetServices()=default;
    virtual game_session::Session& session()=0;
    virtual void refresh_power(Player&)=0;
    virtual void interrupt(std::uint32_t,int)=0;
    virtual void delete_animation(std::uint32_t&)=0;
    virtual void hide_animation(sprite::Animation&)=0;
};
StageResetServices& stage_reset_services();
void finish_stage_visibility(Player&,StageResetServices&); //4bd0c0
void finish_stage_visibility(Player&);
void restore_stage_visibility(Player&,StageResetServices&); //4ff8f0
void restore_stage_visibility(Player&);
void disable_for_stage(Player&,StageResetServices&); //4ffd20
void reset_for_stage(Player&,StageResetServices&); //4fb450
void reset_for_stage(Player&);
}
