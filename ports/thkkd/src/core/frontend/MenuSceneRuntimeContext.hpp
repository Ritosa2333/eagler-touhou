// 菜单独占状态与额外平台状态；导航、存档和游戏对象由GameStartEntryRefs共享。
#pragma once
#include "core/frontend/MenuOptionsContext.hpp"
#include "core/frontend/MenuKeyConfigContext.hpp"
#include "core/frontend/MenuMusicContext.hpp"
#include "core/frontend/MenuCharacterPracticeContext.hpp"
#include "core/frontend/MenuTitleReturnContext.hpp"
#include "core/frontend/MenuPresentationContext.hpp"
#include "core/frontend/MenuReplayInitialData.hpp"
#include <array>
namespace kaikeidou {
struct MenuSceneRuntimeContext {
    MenuOptionsContext options;
    MenuKeyConfigContext keys;
    MenuMusicContext music_room;
    MenuCharacterPracticeContext selection;
    MenuTitleReturnContext title;
    MenuPresentationAssets assets;
    MenuPresentationTiming timing;
    std::array<std::uint32_t,100> sound_handles{}; // A78E48。
    std::array<std::uint32_t,256> keyboard_counts{}; // A71E08。
    std::array<std::uint32_t,16> logical_input{}; // A72208。
    std::uint32_t default_font=0; // 8B63F4，由字体准备服务更新。
    MenuReplayStorage replay; // B43000..B43C17及原静态文字，冷启动仅初始化一次。
};
}
