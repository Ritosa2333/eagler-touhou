#include "TitlePlayerDataView.hpp"

#include <cstdio>
#include <ctime>

namespace th13 {

bool TitlePlayerDataView::fail(const std::string& reason) {
    if (error.empty()) error = reason;
    return false;
}
bool TitlePlayerDataView::create(u32& handle, i32 bank, i32 script) {
    handle = manager_.create(bank, script, -1, 0);
    if (!handle) return fail("Player data animation unavailable: " + manager_.error);
    return true;
}
bool TitlePlayerDataView::leave() {
    for (u32 handle : roots_) if (!manager_.interrupt(handle, 1)) return fail(manager_.error);
    for (u32& handle : rows_) {
        if (handle && !manager_.interrupt(handle, 1)) return fail(manager_.error);
        handle = 0;
    }
    for (u32 handle : {character_, difficulty_})
        if (handle && !manager_.interrupt(handle, 1)) return fail(manager_.error);
    roots_.clear();
    character_ = difficulty_ = 0;
    closing_ = true;
    return true;
}

bool TitlePlayerDataView::update(const TitlePlayerDataMenu& menu, const Rasterizer& rasterize) {
    if (!error.empty()) return false;
    if (!menu.opened()) return true;
    if (!opened_) {
        // 00453259 ascii19; 004532c1 title105; then character/rank and the
        // exact fixed title script sequence 155..158,152..154,159.
        u32 handle = 0;
        if (!create(handle, banks_.ascii, 19)) return false;
        roots_.push_back(handle);
        for (i32 script : {105}) {
            if (!create(handle, banks_.title, script)) return false;
            roots_.push_back(handle);
        }
        opened_ = true;
    }
    if (menu.phase() == 3 && !closing_) return leave();
    if (closing_) return true;
    if (old_character_ != menu.character()) {
        if (old_character_ >= 0 && roots_.size() == 10) {
            const size_t arrow = menu.character() == (old_character_ + 1) % 4 ? 3 : 2;
            if (!manager_.interrupt(roots_[arrow], 2, true)) return fail(manager_.error);
        }
        if (character_ && !manager_.interrupt(character_, 1)) return fail(manager_.error);
        if (!create(character_, banks_.title, 143 + menu.character())) return false;
    }
    if (old_difficulty_ != menu.difficulty()) {
        if (old_difficulty_ >= 0 && roots_.size() == 10) {
            const size_t arrow = menu.difficulty() == (old_difficulty_ + 1) % 5 ? 5 : 4;
            if (!manager_.interrupt(roots_[arrow], 2, true)) return fail(manager_.error);
        }
        if (difficulty_ && !manager_.interrupt(difficulty_, 1)) return fail(manager_.error);
        if (!create(difficulty_, banks_.title, 147 + menu.difficulty())) return false;
    }
    if (roots_.size() == 2) {
        u32 handle = 0;
        for (i32 script : {155, 156, 157, 158, 152, 153, 154, 159}) {
            if (!create(handle, banks_.title, script)) return false;
            roots_.push_back(handle);
        }
    }
    const bool changed = old_character_ != menu.character() || old_difficulty_ != menu.difficulty() ||
                         old_page_ != menu.page();
    if (menu.page() == 0) {
        for (u32& handle : rows_) {
            if (handle && !manager_.interrupt(handle, 1)) return fail(manager_.error);
            handle = 0;
        }
    } else if (changed) {
        if (!rasterize) return fail("Player data card rasterizer unavailable");
        const auto records = menu.visible_records();
        for (size_t i = 0; i < rows_.size(); ++i) {
            if (!rows_[i] && !create(rows_[i], banks_.text, 25 + i32(i))) return false;
            auto* vm = manager_.registry.find(rows_[i]);
            if (!vm) return fail("Player data card row VM unavailable");
            std::string text = " ";
            u32 color = 0x808080;
            if (i < records.size()) {
                const auto& record = records[i];
                char prefix[16], suffix[32];
                std::snprintf(prefix, sizeof prefix, "No.%3d ", record.slot + 1);
                std::snprintf(suffix, sizeof suffix, " %4d/%4d", record.normal_captures,
                              record.normal_attempts);
                std::string name = record.name;
                if (!record.recorded) {
                    name.clear();
                    for (i32 n = 0; n < 21; ++n) name += "\x81\x48";
                } else if (name.size() < 42) {
                    name.append(42 - name.size(), ' ');
                }
                text = prefix + name + suffix;
                if (record.recorded) color = record.character_flagged ? 0xffff80 : 0xefefef;
            }
            if (!rasterize(*vm, color, text, AnmTextAlignment::Center))
                return fail("Player data card row rasterization failed");
        }
    }
    old_character_ = menu.character();
    old_difficulty_ = menu.difficulty();
    old_page_ = menu.page();
    return true;
}

bool TitlePlayerDataView::draw(const TitlePlayerDataMenu& menu, HudDrawServices& services) {
    if (!error.empty()) return false;
    if (menu.phase() != 2) return true;
    HudTextStyle style;
    // 00453de8 sets +0x19174, which is the shadow flag (00403da0 +0x124,
    // consumed by 004042a0). Font +0x19178 remains the default0; alignment
    // +0x19184 remains1 (left), coordinate-space +0x1917c remains0.
    style.font = 0;
    style.shadow = true;
    style.alignment = 1;
    const auto output = [&](float x, float y, const std::string& text) {
        return services.hud_text({{x, y, 0}, style, text}) || fail("Player data text service failed");
    };
    const auto& summary = menu.summary();
    char text[256];
    if (menu.page() == 0) {
        for (size_t i = 0; i < summary.ranking.size(); ++i) {
            const auto& record = summary.ranking[i];
            const u32 shade = 255u - u32(i) * 16u;
            style.color = 0xff0000ffu | (shade << 16) | (shade << 8);
            if (!summary.available || !record.available) {
                std::snprintf(text, sizeof text, "%2d  --------  -----------  ----/--/-- --:--  Stage -  ---%%", i32(i) + 1);
            } else if (record.timestamp == 0) {
                std::snprintf(text, sizeof text, "%2d  %s  %9d%d  ----/--/-- --:--  Stage -  ---%%",
                              i32(i) + 1, record.name.c_str(), record.score, record.continues);
            } else {
                const std::time_t clock = std::time_t(record.timestamp);
                const std::tm* date = std::localtime(&clock);
                if (!date) return fail("Player data score timestamp unavailable");
                static constexpr const char* stages[] = {"test   ", "Stage 1", "Stage 2", "Stage 3",
                    "Stage 4", "Stage 5", "Stage 6", "Extra  ", "Clear  ", "ExClear"};
                if (record.stage < 0 || record.stage >= 10) return fail("Player data score stage out of range");
                std::snprintf(text, sizeof text, "%2d  %s  %9d%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%",
                    i32(i) + 1, record.name.c_str(), record.score, record.continues,
                    date->tm_year + 1900, date->tm_mon + 1, date->tm_mday, date->tm_hour, date->tm_min,
                    stages[record.stage], double(record.progress_percent));
            }
            if (!output(48, 160 + float(i) * 18, text)) return false;
        }
    }
    style.color = 0xffffffff;
    if (!summary.available || !summary.history_complete) {
        if (!output(328,378,"    -----") || !output(328,396,"---:--:--") ||
            !output(328,414,"    -----")) return false;
    } else {
        std::snprintf(text, sizeof text, "    %5d", summary.play_count);
        if (!output(328,378,text)) return false;
        std::snprintf(text, sizeof text, "%3d:%.2d:%.2d", summary.total_frames / 216000,
                      (summary.total_frames / 3600) % 60, (summary.total_frames / 60) % 60);
        if (!output(328,396,text)) return false;
        std::snprintf(text, sizeof text, "    %5d", summary.clear_count);
        if (!output(328,414,text)) return false;
    }
    return true;
}

void TitlePlayerDataView::release() {
    for (u32& handle : roots_) manager_.retire(handle);
    for (u32& handle : rows_) manager_.retire(handle);
    for (u32* handle : {&character_, &difficulty_}) manager_.retire(*handle);
    roots_.clear();
    opened_ = closing_ = false;
    old_character_ = old_difficulty_ = old_page_ = -1;
}

} // namespace th13
