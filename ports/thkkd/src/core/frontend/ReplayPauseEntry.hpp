// 原5E3652..5E36A4、B42AB1..B42AF2及652ED0；调用前scene必须为0。
#pragma once
#include "core/frontend/ReplayPauseEntryContext.hpp"
#include "core/frontend/ReplayPauseEntryServices.hpp"
#include "core/shared/BackgroundMoviePause.hpp"
#include "core/shared/MusicPresentationContext.hpp"
namespace kaikeidou {
inline bool enter_pause_from_scene_zero(ReplayPauseEntryContext& context,bool replay_variant,
    const StageResourceContext& background,const MusicPresentationContext& music,ReplayPauseEntryServices& services){
    const auto physical=services.physical_input_count(context.pause_binding);
    if(replay_variant){
        if(physical!=1){
            if(context.replay_cursor+1u!=context.replay_count)return false;
            if((context.replay_mode&0xffu)!=1)return false;
            context.eof_flags=(context.eof_flags&0xffffff00u)|1u;
        }
        context.selection=(context.selection&0xffffff00u)|(context.replay_mode&0xffu);
    }else if(physical!=1)return false;
    context.scene=1;
    context.phase=0;
    if(!replay_variant)context.selection=0;
    pause_stage_background(background,services);
    services.stop_sound(music.audio_handle);
    services.draw_pause_interface();
    return true;
}
}
