// 原 5E6DC0..5E6F6F：菜单进度解锁，保留索引乘加的32位回绕和读取顺序。
#pragma once
#include "core/frontend/SaveData.hpp"
#include <stdexcept>

namespace kaikeidou {
namespace menu_unlock_detail {
inline std::uint32_t progress_word(const SaveData& data, std::uint32_t difficulty,
    std::uint32_t character, std::uint32_t weapon, std::uint32_t stage) {
    const std::uint32_t offset = difficulty * 252u + character * 84u + weapon * 28u + stage * 4u;
    if (offset / 4u >= data.progress.size())
        throw std::out_of_range("Menu unlock read outside progress table");
    return data.progress[offset / 4u];
}
}

inline bool omniverse_unlocked(const SaveData& data) {
    for (std::uint32_t character = 0; character < 3; ++character)
        for (std::uint32_t weapon = 0; weapon < 3; ++weapon)
            if (menu_unlock_detail::progress_word(data, 4, character, weapon, 6) == 2) return true;
    return false;
}

// 菜单传入-1或0..2；扩展索引仅在原32位地址仍落于进度表时有定义。
inline bool extra_unlocked(const SaveData& data, std::int32_t character, std::int32_t weapon) {
    using menu_unlock_detail::progress_word;
    if (character == -1) {
        if (weapon != -1) return false;
        for (std::uint32_t difficulty = 0; difficulty < 4; ++difficulty)
            for (std::uint32_t selected_character = 0; selected_character < 3; ++selected_character)
                for (std::uint32_t selected_weapon = 0; selected_weapon < 3; ++selected_weapon)
                    if (progress_word(data, difficulty, selected_character, selected_weapon, 5) == 2)
                        return true;
    } else if (weapon == -1) {
        for (std::uint32_t difficulty = 0; difficulty < 4; ++difficulty)
            for (std::uint32_t selected_weapon = 0; selected_weapon < 3; ++selected_weapon)
                if (progress_word(data, difficulty, static_cast<std::uint32_t>(character), selected_weapon, 5) == 2)
                    return true;
    } else {
        for (std::uint32_t difficulty = 0; difficulty < 4; ++difficulty)
            if (progress_word(data, difficulty, static_cast<std::uint32_t>(character),
                static_cast<std::uint32_t>(weapon), 5) == 2) return true;
    }
    return false;
}
}
