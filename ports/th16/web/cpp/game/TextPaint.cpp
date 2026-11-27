#include "TextPaint.hpp"

#include "AnmManager.hpp"

namespace th16 {
namespace {

bool sprite_rect(AnmManager& animations, const AnmVm& vm, i32& left, i32& top, i32& right, i32& bottom, float* width_out) {
    const i32 slot = vm.sprite_source_slot >= 0 ? vm.sprite_source_slot : vm.file_slot;
    AnmFile* file = animations.file(slot);
    if (file == nullptr || vm.sprite_index < 0 || u32(vm.sprite_index) >= file->sprites.size()) return false;
    const AnmRuntimeSprite& s = file->sprites[u32(vm.sprite_index)];
    left = i32(s.x0);
    top = i32(s.y0);
    right = i32(s.x1);
    bottom = i32(s.y1);
    if (width_out) *width_out = s.width;
    return true;
}

TextIntoTextureRequest make_request(const AnmVm& vm, u32 color, u32 outline, i32 font, i32 height, const char* text,
                                    i32 left, i32 top, i32 right, i32 bottom) {
    TextIntoTextureRequest req;
    req.vm_id = vm.id;
    req.text = text;
    // flags_hi bit12：阴影色清零且不画阴影（param_10 = !bit）
    req.shadow = (vm.flags_hi & 0x1000u) == 0;
    req.color = color;
    req.outline = req.shadow ? outline : 0;
    req.font = font;
    req.height = height;
    req.left = left;
    req.top = top;
    req.right = right;
    req.bottom = bottom;
    return req;
}

}  // namespace

bool text_paint_vm(AnmManager& animations, AnmVm& vm, u32 color, u32 outline, i32 font, i32 x, i32 y,
                   const char* text, std::vector<TextIntoTextureRequest>* out) {
    if (text == nullptr) text = "";
    const i32 height = text_font_height(font);
    i32 left = 0, top = 0, right = 0, bottom = 0;
    const bool have_rect = sprite_rect(animations, vm, left, top, right, bottom, nullptr);
    vm.flags_lo |= 1u;
    if (!have_rect) return false;
    // 原版：字号 ∈ [1,8] 时跳过 GDI 写入，但仍置 flags_lo bit0
    if (height >= 1 && height < 9) return true;
    if (out == nullptr) return true;
    TextIntoTextureRequest req = make_request(vm, color, outline, font, height < 1 ? 0x11 : height, text, left, top,
                                              right, bottom);
    req.x = x * 2;
    req.spacing = y * 2;
    out->push_back(std::move(req));
    return true;
}

bool text_paint_vm_centered(AnmManager& animations, AnmVm& vm, u32 color, u32 outline, i32 font, i32 spacing,
                            const char* text, std::vector<TextIntoTextureRequest>* out) {
    if (text == nullptr) text = "";
    const i32 height = text_font_height(font);
    i32 left = 0, top = 0, right = 0, bottom = 0;
    float sprite_w = 0;
    const bool have_rect = sprite_rect(animations, vm, left, top, right, bottom, &sprite_w);
    vm.flags_lo |= 1u;
    if (!have_rect) return false;
    if (height >= 1 && height < 9) return true;
    if (out == nullptr) return true;
    TextIntoTextureRequest req = make_request(vm, color, outline, font, height < 1 ? 0x11 : height, text, left, top,
                                              right, bottom);
    const u32 half = u32((height * 2 - 1) * i32(req.text.size())) >> 1;
    req.x = i32(sprite_w - (float(half) + 0.0f));
    req.spacing = spacing * 2;
    out->push_back(std::move(req));
    return true;
}

bool text_paint_vm_middle(AnmManager& animations, AnmVm& vm, u32 color, u32 outline, i32 font, i32 spacing,
                          const char* text, std::vector<TextIntoTextureRequest>* out) {
    if (text == nullptr) text = "";
    const i32 height = text_font_height(font);
    i32 left = 0, top = 0, right = 0, bottom = 0;
    float sprite_w = 0;
    const bool have_rect = sprite_rect(animations, vm, left, top, right, bottom, &sprite_w);
    vm.flags_lo |= 1u;
    if (!have_rect) return false;
    if (height >= 1 && height < 9) return true;
    if (out == nullptr) return true;
    TextIntoTextureRequest req = make_request(vm, color, outline, font, height < 1 ? 0x11 : height, text, left, top,
                                              right, bottom);
    const u32 quarter = u32((height * 2 - 1) * i32(req.text.size())) >> 2;
    req.x = i32(u32(i32(sprite_w) / 2) - quarter);
    req.spacing = spacing * 2;
    out->push_back(std::move(req));
    return true;
}

}  // namespace th16
