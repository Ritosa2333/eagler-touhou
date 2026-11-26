// 原60E890..60E8F9；按入口关卡选择视频槽，后续句柄在每次服务前重读。
#pragma once
#include "core/stages/StageBackgroundServices.hpp"
namespace kaikeidou {
inline void pause_stage_background(const StageResourceContext& context,StageBackgroundServices& services){
    const auto stage=context.stage;
    if(stage==0||stage==2){services.movie_pause(context.handles[0],0);return;}
    if(stage==1){services.movie_pause(context.handles[2],0);services.movie_pause(context.handles[3],0);services.movie_pause(context.handles[4],0);}
}
}
