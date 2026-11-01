#include "TitleMusicRoomView.hpp"

namespace th13 {

bool TitleMusicRoomView::fail(const std::string& why) {
    if (error.empty()) error = why.empty() ? "Music Room presentation failed" : why;
    return false;
}

bool TitleMusicRoomView::update(const TitleState& state, const TitleMusicRoomMenu& menu,
                                TitleMusicRoomPlatform& host, const Rasterizer& rasterize) {
    if (!error.empty()) return false;
    if (!opened) {
        // 00455574/004555e8/00455712/00455856: ascii19, title106,
        // text41..48 and title160+track own the original dynamic text regions.
        particles = animations.create(ascii_bank, 19);
        badge = animations.create(title_bank, 106);
        if (!particles || !badge) return fail(animations.error);
        for (i32 i = 0; i < 8; ++i) {
            comments[u32(i)] = animations.create(text_bank, 41 + i);
            if (!comments[u32(i)]) return fail(animations.error);
        }
        if (host.track_count() < 0 || host.track_count() > 22)
            return fail("Music Room track count exceeds original title text slots");
        for (i32 i = 0; i < host.track_count(); ++i) {
            const u32 handle = animations.create(title_bank, 160 + i);
            rows.push_back(handle);
            auto* vm = animations.registry.find(handle);
            if (!vm) return fail(animations.error);
            // 曲名色恒白；未听曲名 = 0x4ac930 占位 "No.%2d " + 11 全角？。
            if (!rasterize || !rasterize(*vm, 0xffffff,
                    host.track_heard(i) ? host.track_title(i) : host.locked_title(i),
                    AnmTextAlignment::Left)) return fail("Music Room title rasterization failed");
            if (!animations.interrupt(handle, i == menu.row() ? 2 : 3, true))
                return fail(animations.error);
        }
        opened = true;
    }
    if (state.substate == 3 && !closing) {
        for (u32 handle : rows) if (!animations.interrupt(handle, 1)) return fail(animations.error);
        for (u32 handle : comments) if (!animations.interrupt(handle, 1)) return fail(animations.error);
        if (!animations.interrupt(badge, 1) || !animations.interrupt(particles, 1))
            return fail(animations.error);
        closing = true;
    }
    if (closing) return true;
    if (menu.row() < top) top = menu.row();
    else if (menu.row() >= top + 10) top = menu.row() - 9;
    for (size_t i = 0; i < rows.size(); ++i) {
        auto* vm = animations.registry.find(rows[i]);
        if (!vm) return fail("Music Room row VM unavailable");
        const bool selected = i32(i) == menu.row();
        vm->visual.flags = (vm->visual.flags & ~2u) |
            (i32(i) >= top && i32(i) < top + 10 ? 2u : 0u);
        const Vec3 position{selected ? 60.f : 64.f, 96.f + 20.f * float(i32(i) - top), 0};
        if (previous_row != menu.row()) {
            vm->interpolators.position.begin(4, 0,
                {vm->variables.position.x, vm->variables.position.y, vm->variables.position.z},
                {position.x, position.y, position.z});
            if (!animations.interrupt(rows[i], selected ? 2 : 3, true)) return fail(animations.error);
        }
    }
    previous_row = menu.row();
    // 注释块 = +0x750（Z 时更新），不是光标行；Z 重置 +0x74c 时整体重挂。
    if (previous_comment != menu.comment_track() || menu.revealed_lines() < revealed) {
        for (u32 handle : comments)
            if (!animations.interrupt(handle, 3, true)) return fail(animations.error);
        previous_comment = menu.comment_track();
        revealed = 0;
    }
    const bool locked = !host.track_heard(menu.comment_track()) && menu.locked_armed();
    while (revealed < menu.revealed_lines()) {
        const u32 handle = comments[u32(revealed)];
        auto* vm = animations.registry.find(handle);
        const std::string text = locked ? host.locked_comment(revealed)
                                        : host.track_comment(menu.comment_track(), revealed);
        if (!vm || !rasterize || !rasterize(*vm, locked ? 0x8080ffu : 0xffffffu, text,
                                            AnmTextAlignment::Left))
            return fail("Music Room comment rasterization failed");
        if (!animations.interrupt(handle, 2, true)) return fail(animations.error);
        ++revealed;
    }
    return true;
}

void TitleMusicRoomView::release() {
    for (u32& handle : rows) animations.retire(handle);
    for (u32& handle : comments) animations.retire(handle);
    animations.retire(badge);
    animations.retire(particles);
    rows.clear();
    opened = closing = false;
    previous_row = previous_comment = -1;
    revealed = top = 0;
}

} // namespace th13
