// 物理暂停使用原656C70返回计数；61D690保留为本批界面服务边界。
#pragma once
#include "core/stages/StageBackgroundServices.hpp"
#include "core/shared/MusicPresentationServices.hpp"
namespace kaikeidou {
struct ReplayPauseEntryServices:StageBackgroundServices,MusicPresentationServices {
    virtual std::int32_t physical_input_count(std::uint32_t binding)=0;
    virtual void draw_pause_interface()=0;
};
}
