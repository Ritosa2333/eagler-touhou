// 原5E2A90启动、5E2910更新与613260标题绘制块，保留加载和回调后的字段重读。
#pragma once
#include "core/stages/StageTitleContext.hpp"
#include "core/stages/StageTitlePresentationServices.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/stages/StageTitleArithmetic.hpp"
namespace kaikeidou {
inline constexpr std::array<std::string_view,7> stage_title_paths{{"dat/img/board/title0.png","dat/img/board/title1.png","dat/img/board/title2_0.png","dat/img/board/title3.png","dat/img/board/title4.png","dat/img/board/title5.png","dat/img/board/title6.png"}};
inline constexpr std::array<std::string_view,6> stage_title_animation_paths{{"dat/img/board/title2_0.png","dat/img/board/title2_1.png","dat/img/board/title2_2.png","dat/img/board/title2_3.png","dat/img/board/title2_4.png","dat/img/board/title2_5.png"}};
inline void start_stage_title(StageTitleContext& title,const std::uint32_t& stage,StageResourceServices& resources){title.active=1;title.age=0;if(stage<stage_title_paths.size())title.graph_handle=resources.load_graph(stage_title_paths[stage],0);}
inline void update_stage_title(StageTitleContext& title,const std::uint32_t& stage,StageResourceServices& resources){
 if(!title.active)return;
 if(stage==2&&stage_title_signed(title.age)%6==0){const auto frame=(stage_title_signed(title.age)%36)/6;if(frame>=0&&frame<6)title.graph_handle=resources.load_graph(stage_title_animation_paths[static_cast<std::size_t>(frame)],0);}
 if(stage_title_signed(title.age)<64)title.opacity+=4;
 if(stage_title_signed(title.age)>192)title.opacity-=4;
 if(stage_title_signed(title.age)>256){title.age=0;title.opacity=0;title.active=0;resources.delete_graph(title.graph_handle,0);}else ++title.age;
}
inline void present_stage_title(const StageTitleContext& title,StageTitlePresentationServices& graphics,unsigned precision){
 if(!title.active)return;
 if(stage_title_signed(title.age)<=128)graphics.blend(1,192);else graphics.blend(1,stage_title_integer(binary_float::multiply(binary_float::integer(title.opacity),binary_float::binary64(0x3fe8000000000000ull),precision)));
 const auto color=graphics.color(16,16,16);const auto square=title.age*title.age;const auto right=32u+stage_title_integer(binary_float::multiply(binary_float::integer(square),binary_float::binary64(0x3fb47ae147ae147bull),precision));graphics.box(32,64,right,192,color,1);
 graphics.blend(1,title.opacity);graphics.sprite(0x4068000000000000ull,0x4064000000000000ull,0x3ff0000000000000ull,0,title.graph_handle,1,0);graphics.blend(0,255);
}
}
