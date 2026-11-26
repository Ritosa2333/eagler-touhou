// 标题沿用原图形资源、平台随机和音频服务，返回值由宿主提供。
#pragma once
#include "core/stages/StageResourceContext.hpp"
#include "core/shared/MusicPresentationServices.hpp"
namespace kaikeidou {
struct MenuTitleReturnServices:StageResourceServices,MusicPresentationServices {};
}
