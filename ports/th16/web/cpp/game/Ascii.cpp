// th16 AsciiManager 实现（地址见 Ascii.hpp）。
#include "Ascii.hpp"

#include <cstdio>
#include <cstring>

#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "Camera.hpp"

namespace th16 {

namespace {

// 0x408650：各字体的字宽（乘缩放前）与行高
struct FontMetrics { float glyph_width; float line_height; };

FontMetrics metrics_for(AsciiManager::Font font, float scale_x, i32 default_glyph_width) {
    switch (font) {
    case AsciiManager::Font::Small: return {6.0f * scale_x, 9.0f};
    case AsciiManager::Font::Medium:
    case AsciiManager::Font::MediumNearest: return {7.0f * scale_x, 10.0f};
    case AsciiManager::Font::Large:
    case AsciiManager::Font::LargeNearest: return {12.0f * scale_x, 16.0f};
    default: return {float(default_glyph_width) * scale_x, 14.0f};
    }
}

// 字体 2/3 用 '.'、字体 4/5 用 ',' 当窄字（宽 4 × 缩放）
char narrow_glyph_for(AsciiManager::Font font) {
    switch (font) {
    case AsciiManager::Font::Medium:
    case AsciiManager::Font::MediumNearest: return '.';
    case AsciiManager::Font::Large:
    case AsciiManager::Font::LargeNearest: return ',';
    default: return '\0';
    }
}

// 0x408650 字体 2/3：字母 +0x74 / +0x94，专用符号表；'.' 由调用方特殊处理
i32 medium_sprite(u8 c) {
    if (c >= 'a' && c <= 'z') return c + 0x74;
    if (c >= 'A' && c <= 'Z') return c + 0x94;
    switch (c) {
    case '/': return 0xce;
    case ':': return 0xcf;
    case '-': return 0xd0;
    case '*': return 0xd1;
    case '%': return 0xd2;
    case '$': return 0x101;
    case '+': return 0xd4;
    default: return c + 0x94;
    }
}

// 0x408650 字体 4/5：字符 +0xbf，专用符号表；',' 由调用方特殊处理
i32 large_sprite(u8 c) {
    switch (c) {
    case '/': return 0xf9;
    case '.': return 0xfa;
    case 's': return 0xfb;
    case '*': return 0xfc;
    default: return c + 0xbf;
    }
}

bool uses_nearest_sampling(AsciiManager::Font font, float scale_x) {
    switch (font) {
    case AsciiManager::Font::MediumNearest:
    case AsciiManager::Font::LargeNearest: return true;
    case AsciiManager::Font::Default: return scale_x != 1.0f;
    default: return false;
    }
}

}  // namespace

void AsciiManager::create(AnmManager& animations, AnmFile& ascii) {
    animations_ = &animations;
    ascii_ = &ascii;
    for (AnmVm* vm : {&glyph_vm_, &glyph_vm_alt_}) {
        AnmManager::reset_vm(*vm);
        vm->file_slot = ascii.slot;
    }
    anm_set_sprite(ascii, glyph_vm_, 0);
    anm_set_sprite(ascii, glyph_vm_alt_, 0x62);
    count_ = 0;
    frame = 0;
}

// 0x408140
void AsciiManager::print(const Vec3& pos, const char* text) {
    if (count_ >= kMaxEntries) return;
    Entry& e = entries_[count_++];
    std::strncpy(e.text, text, sizeof e.text - 1);
    e.text[sizeof e.text - 1] = '\0';
    e.pos = {pos.x * window_scale, pos.y * window_scale, pos.z * window_scale};
    e.group = style.group;
    e.color = style.color;
    e.scale = style.scale;
    e.reserved = style.reserved;
    e.font = style.font;
    e.shadow = style.shadow;
    e.lifetime = style.lifetime;
    e.h_align = style.h_align;
    e.v_align = style.v_align;
}

void AsciiManager::vprintf(const Vec3& pos, const char* format, va_list args) {
    char buffer[260];
    std::vsnprintf(buffer, sizeof buffer, format, args);
    print(pos, buffer);
}

// 0x408260
void AsciiManager::printf(const Vec3& pos, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(pos, format, args);
    va_end(args);
}

// 0x4084f0：入队后把刚入队的条目改成字体 1
void AsciiManager::printf_small(const Vec3& pos, const char* format, ...) {
    va_list args;
    va_start(args, format);
    const i32 before = count_;
    vprintf(pos, format, args);
    va_end(args);
    if (count_ > before) entries_[count_ - 1].font = Font::Small;
}

// 0x4082b0：按千分位分组（无符号）
void AsciiManager::print_grouped(const Vec3& pos, u32 value) {
    char buffer[260];
    if (value < 1000u) {
        std::snprintf(buffer, sizeof buffer, "%u", value);
    } else if (value < 1000000u) {
        std::snprintf(buffer, sizeof buffer, "%u,%.3u", value / 1000u, value % 1000u);
    } else if (value < 1000000000u) {
        std::snprintf(buffer, sizeof buffer, "%u,%.3u,%.3u", value / 1000000u, (value / 1000u) % 1000u, value % 1000u);
    } else {
        std::snprintf(buffer, sizeof buffer, "%u,%.3u,%.3u,%.3u", value / 1000000000u, (value / 1000000u) % 1000u,
                      (value / 1000u) % 1000u, value % 1000u);
    }
    print(pos, buffer);
}

// 0x4083b0：分数值（已除 10）按千分位分组，末尾再接一位——原版用它显示续关次数
void AsciiManager::print_score(const Vec3& pos, u32 value, u32 last_digit) {
    char buffer[260];
    if (value < 100u) {
        std::snprintf(buffer, sizeof buffer, "%u", value * 10u + last_digit);
    } else if (value < 100000u) {
        std::snprintf(buffer, sizeof buffer, "%u,%.2u%u", value / 100u, value % 100u, last_digit);
    } else if (value < 100000000u) {
        std::snprintf(buffer, sizeof buffer, "%u,%.3u,%.2u%u", value / 100000u, (value / 100u) % 1000u, value % 100u,
                      last_digit);
    } else {
        std::snprintf(buffer, sizeof buffer, "%u,%.3u,%.3u,%.2u%u", value / 100000000u, (value / 100000u) % 1000u,
                      (value / 100u) % 1000u, value % 100u, last_digit);
    }
    print(pos, buffer);
}

// 0x408070 → 0x408fb0：寿命 -1，仍 ≥ 0 的条目依次前移
void AsciiManager::update() {
    i32 kept = 0;
    for (i32 i = 0; i < count_; ++i) {
        Entry& e = entries_[i];
        e.lifetime -= 1;
        if (e.lifetime < 0) continue;
        if (kept != i) entries_[kept] = e;
        ++kept;
    }
    count_ = kept;
    ++frame;
}

// 0x408560
void AsciiManager::draw_group(Group group, AnmRenderer& renderer, const Camera& ui_camera) {
    for (i32 i = 0; i < count_; ++i) {
        if (entries_[i].group == group) draw_entry(entries_[i], renderer);
    }
    renderer.flush();
    renderer.use_camera(ui_camera);
}

// 0x408ef0（组 1）/ 0x4080b0（组 2）
void AsciiManager::draw_field_group(Group group, AnmRenderer& renderer, const Camera& field_camera,
                                    const Camera& ui_camera) {
    renderer.use_camera(field_camera);
    if (group == Group::Field) {
        // 组 1 的文字坐标以场地左上为原点：层组位 0x80000 让 0x406a70 加上 (320, 16) 屏幕偏移
        renderer.flush();
        glyph_vm_.flags_hi = (glyph_vm_.flags_hi & ~0x40000u) | 0x80000u;
        draw_group(group, renderer, ui_camera);
        glyph_vm_.flags_hi &= ~0xc0000u;
        return;
    }
    draw_group(group, renderer, ui_camera);
}

// 0x409040：只换精灵号与 UV（尺寸由调用方按精灵表另取）
void AsciiManager::select_glyph(i32 sprite) {
    const AnmRuntimeSprite& s = ascii_->sprites[u32(sprite)];
    glyph_vm_.sprite_index = sprite;
    glyph_vm_.uv[0] = {s.u0, s.v0};
    glyph_vm_.uv[1] = {s.u1, s.v0};
    glyph_vm_.uv[2] = {s.u0, s.v1};
    glyph_vm_.uv[3] = {s.u1, s.v1};
}

// 0x408650
void AsciiManager::draw_entry(const Entry& entry, AnmRenderer& renderer) {
    if (ascii_ == nullptr) return;
    AnmVm& vm = glyph_vm_;
    const float window = window_scale;
    const float scale_x = entry.scale.x;

    vm.flags_hi &= ~AnmVm::kHiScaleModeMask;
    // 可见、左上锚点（anchor x / y = 1）
    vm.flags_lo = (vm.flags_lo & 0xfebfffffu) | 0x00a00001u;
    vm.pos = entry.pos;
    vm.flags_lo |= AnmVm::kScaleDirty;
    vm.scale = entry.scale;
    if (uses_nearest_sampling(entry.font, scale_x)) vm.flags_hi |= AnmVm::kHi311;
    else vm.flags_hi &= ~AnmVm::kHi311;

    const FontMetrics metrics = metrics_for(entry.font, scale_x, kDefaultGlyphWidth);
    const float glyph_width = metrics.glyph_width;
    const float line_height = metrics.line_height;
    const char narrow = narrow_glyph_for(entry.font);
    const size_t length = std::strlen(entry.text);

    // 水平对齐：按整行宽度预退起点（窄字按 4 × 缩放算）
    if (entry.h_align == HAlign::Center) {
        if (narrow != '\0') {
            for (const char* p = entry.text; *p != '\0'; ++p) {
                const float step = (*p == narrow) ? scale_x * -4.0f * 0.5f : glyph_width * -0.5f;
                vm.pos.x = step * window + vm.pos.x;
            }
        } else {
            vm.pos.x = (float(-i32(length)) * glyph_width * 0.5f) * window + vm.pos.x;
        }
    } else if (entry.h_align == HAlign::Right) {
        if (narrow != '\0') {
            for (const char* p = entry.text; *p != '\0'; ++p) {
                const float step = (*p == narrow) ? scale_x * -4.0f * window : -(window * glyph_width);
                vm.pos.x = step + vm.pos.x;
            }
        } else {
            vm.pos.x = (float(-i32(length)) * glyph_width) * window + vm.pos.x;
        }
    }
    // 垂直对齐
    if (entry.v_align == VAlign::Middle) vm.pos.y = line_height * -0.5f * window + vm.pos.y;
    else if (entry.v_align == VAlign::Bottom) vm.pos.y = -(window * line_height) + vm.pos.y;

    float advance = glyph_width;   // 空格沿用上一个字形的步进
    for (const u8* p = reinterpret_cast<const u8*>(entry.text); *p != '\0'; ++p) {
        const u8 c = *p;
        if (c == '\n') {
            vm.pos.y = entry.scale.y * line_height * window + vm.pos.y;
            vm.pos.x = entry.pos.x;
            continue;
        }
        if (c == ' ') {
            vm.pos.x = window * advance + vm.pos.x;
            continue;
        }
        switch (entry.font) {
        case Font::Default: select_glyph(c - 0x20); break;
        case Font::Small: select_glyph(c + 0x42); break;
        case Font::Medium:
        case Font::MediumNearest:
            advance = scale_x * 7.0f;
            if (c == '.') { select_glyph(0xd3); advance = scale_x * 4.0f; }
            else select_glyph(medium_sprite(c));
            break;
        case Font::Large:
        case Font::LargeNearest:
            advance = scale_x * 12.0f;
            vm.pos.y = entry.pos.y;
            if (c == ',') {
                select_glyph(0xfd);
                advance = scale_x * 4.0f;
                vm.pos.y = window * 3.0f + entry.pos.y;
            } else {
                select_glyph(large_sprite(c));
            }
            break;
        }
        const AnmRuntimeSprite& sprite = ascii_->sprites[u32(vm.sprite_index)];
        vm.flags_lo |= AnmVm::kScaleDirty;
        vm.sprite_size = {sprite.width, sprite.height};

        if (entry.shadow) {
            // 半透明黑影：alpha 减半，右下偏 2 像素
            vm.color[0] = vm.color[1] = vm.color[2] = 0;
            vm.color[3] = u8(entry.color >> 25);
            vm.pos.x = window * 2.0f + vm.pos.x;
            vm.pos.y = window * 2.0f + vm.pos.y;
            renderer.draw_screen_sprite(vm);
            vm.pos.x = window * -2.0f + vm.pos.x;
            vm.pos.y = window * -2.0f + vm.pos.y;
        }
        vm.color[0] = u8(entry.color);
        vm.color[1] = u8(entry.color >> 8);
        vm.color[2] = u8(entry.color >> 16);
        vm.color[3] = u8(entry.color >> 24);
        renderer.draw_screen_sprite(vm);
        vm.pos.x = window * advance + vm.pos.x;
    }
}

}  // namespace th16
