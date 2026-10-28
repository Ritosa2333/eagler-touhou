#include "DialogueText.hpp"

#include <algorithm>
#include <cstring>

namespace th13 {

namespace {
// 0x45aef0 的 GDI 暂存面为 1024x129；对话每行只消费顶部 ~44 行，再缩入
// text.anm 512x20 条带（0x45aef0 D3DXLoadSurfaceFromMemory 缩放语义的 port）。
constexpr u32 kScratchWidth = 1024;
constexpr u32 kScratchHeight = 48;
constexpr u32 kScratchRows = 44;
constexpr u32 kArgb4444 = 5;

bool read_region(const AnmResource& resource, i32 script, i32& x, i32& y, i32& width,
                 i32& height) {
    if (script < 0 || u32(script) >= resource.sprites.size()) return false;
    const AnmSprite& sprite = resource.sprites[u32(script)];
    if (sprite.texture != 0 || !(sprite.width > 0.f) || !(sprite.height > 0.f)) return false;
    x = i32(sprite.x);
    y = i32(sprite.y);
    width = i32(sprite.width);
    height = i32(sprite.height);
    return x >= 0 && y >= 0 && width > 0 && height > 0;
}
}  // namespace

bool DialogueText::ensure(AnmManager& manager, DialogueTextureSink& sink, std::string& error) {
    AnmResource* resource = manager.resource(0);
    if (!resource) {
        error = "text.anm bank 0 is not loaded";
        return false;
    }
    manager_ = &manager;
    if (resource->scripts.size() < u32(kVmCount) || resource->sprites.size() < u32(kVmCount)) {
        error = "text.anm needs scripts/sprites 0..3 for dialogue";
        return false;
    }
    if (resource->textures.empty() || resource->textures[0].format != kArgb4444 ||
        resource->textures[0].kind == AnmTexture::Kind::External ||
        resource->textures[0].kind == AnmTexture::Kind::RenderTarget) {
        error = "text.anm texture0 is not the ARGB4444 dialogue target";
        return false;
    }
    if (!texture_) texture_ = sink.texture(*resource, 0);
    if (!texture_) {
        error = "text.anm texture0 was not preloaded";
        return false;
    }
    if (!scratch_.width) {
        scratch_.width = kScratchWidth;
        scratch_.height = kScratchHeight;
        scratch_.pitch = kScratchWidth * 2;
        scratch_.packed_format = u16(kArgb4444);
        scratch_.format = touhou::graphics::PixelFormat::Argb4444;
        scratch_.pixels.assign(size_t(scratch_.pitch) * scratch_.height, 0);
    }
    for (i32 script = 0; script < kVmCount; script++) {
        if (!read_region(*resource, script, regions_[u32(script)].x, regions_[u32(script)].y,
                         regions_[u32(script)].width, regions_[u32(script)].height)) {
            error = "text.anm sprite rect unavailable for script " + std::to_string(script);
            return false;
        }
        if (!handles_[u32(script)]) {
            // 0x4282b0 四条文本 VM 创建位置为 0；正文行再按 0x428ae0:499
            // 写 box_x / box_x-(width+30)，脚本自带 (2,22/42)/(2,16/36)。
            handles_[u32(script)] =
                manager.create(0, script, -1, 2, {}, 0, false);
            if (!handles_[u32(script)]) {
                error = manager.error.empty() ? "text.anm dialogue VM creation failed"
                                              : manager.error;
                release(manager);
                return false;
            }
            if (anchor_y_set_)
                if (auto* vm = manager.registry.find(handles_[u32(script)]))
                    vm->visual.child_anchor.y = anchor_y_;
        }
    }
    return true;
}

bool DialogueText::blank(DialogueTextureSink& sink, const Region& region, std::string& error) {
    TextureImage* image = sink.pixels(texture_);
    if (!image) {
        error = "text.anm texture0 pixels unavailable";
        return false;
    }
    if (image->format != touhou::graphics::PixelFormat::Argb4444 ||
        image->pitch < image->width * 2) {
        error = "text.anm texture0 is not ARGB4444";
        return false;
    }
    if (region.x < 0 || region.y < 0 || region.width <= 0 || region.height <= 0 ||
        u32(region.x + region.width) > image->width ||
        u32(region.y + region.height) > image->height) {
        error = "text.anm dialogue sprite rect outside texture0";
        return false;
    }
    for (i32 row = 0; row < region.height; row++) {
        auto* target = image->pixels.data() + size_t(region.y + row) * image->pitch +
                       size_t(region.x) * 2;
        std::memset(target, 0, size_t(region.width) * 2);
    }
    return true;
}

bool DialogueText::blank_all(DialogueTextureSink& sink, std::string& error, i32 label) {
    if (!texture_) return true;
    for (const Region& region : regions_) {
        if (!blank(sink, region, error)) return false;
        sink.changed(texture_, {u32(region.x), u32(region.y),
                                u32(region.x + region.width), u32(region.y + region.height)});
    }
    cached_.fill(false);
    // 0x428ae0 块首清四槽 / opcode 6 释放：原版在 0x46fc30 前把 EBX
    // 置为 label（块首 3、释放 1），由下一次 ANM tick 消费。
    for (u32 handle : handles_)
        if (!manager_ || !request_label(*manager_, handle, label, error)) {
            if (!manager_) error = "text.anm dialogue manager is not bound";
            return false;
        }
    return true;
}

bool DialogueText::request_label(AnmManager& manager, u32 handle, i32 label,
                                 std::string& error) {
    if (!handle) return true;
    // 0x46fc30 只写 VM+0x4a0（deferred）；不在此 immediate tick，等本帧
    // AnmManager::update（Application Update 优先级 34 > RunScene 20）消费。
    if (!manager.interrupt(handle, label, false)) {
        error = manager.error.empty() ? "text.anm dialogue interrupt failed" : manager.error;
        return false;
    }
    return true;
}

bool DialogueText::copy_strip(DialogueTextureSink& sink, const Region& region,
                              std::string& error) {
    TextureImage* image = sink.pixels(texture_);
    if (!image) {
        error = "text.anm texture0 pixels unavailable";
        return false;
    }
    if (image->format != touhou::graphics::PixelFormat::Argb4444 ||
        image->pitch < image->width * 2 || !scratch_.pitch) {
        error = "text.anm texture0 is not ARGB4444";
        return false;
    }
    // 0x45aef0 的 D3DX 缩放：1024 宽暂存 → 512 宽 sprite，源高 44
    // （0x4282b0 的 +0x51c=0x15 → 0x45aef0 param_5*2+2）→ 20 像素条带。
    // dot sampling 先接线；滤镜/字形资格待独立认证。
    for (i32 y = 0; y < region.height; y++) {
        const u32 source_y = u32(y) * kScratchRows / u32(region.height);
        if (source_y >= scratch_.height) return false;
        const u8* source = scratch_.pixels.data() + size_t(source_y) * scratch_.pitch;
        auto* target = image->pixels.data() + size_t(region.y + y) * image->pitch +
                       size_t(region.x) * 2;
        for (i32 x = 0; x < region.width; x++) {
            const u32 source_x = u32(x) * scratch_.width / u32(region.width);
            std::memcpy(target + size_t(x) * 2, source + size_t(source_x) * 2, 2);
        }
    }
    sink.changed(texture_, {u32(region.x), u32(region.y),
                            u32(region.x + region.width), u32(region.y + region.height)});
    return true;
}

bool DialogueText::rasterize_ruby(const MsgLine& line, const DialogueRasterizer& rasterizer,
                                  std::string& error) {
    std::fill(scratch_.pixels.begin(), scratch_.pixels.end(), 0);
    // 0x45aef0：正文/ruby 都从 TextOut(2*x,2) 起始；ruby 第二参数是逐字
    // 步进（param_9），0 = 连续排版，非 0 = 每字后 x += value*2。
    // 颜色与正文同源：fg=EDI[0x62+side]=0（见 write_row 注），描边=0。
    const i32 base_x = line.ruby_x * 2;
    if (line.ruby_spacing == 0) {
        if (!rasterizer(scratch_, base_x, 2, true, 0x00000000u, line.ruby_cp932)) {
            error = "dialogue ruby rasterization failed";
            return false;
        }
        return true;
    }
    const std::string& text = line.ruby_cp932;
    size_t index = 0;
    i32 step = 0;
    while (index < text.size() && text[index]) {
        const u8 lead = u8(text[index]);
        size_t length = 1;
        if (((lead >= 0x81 && lead <= 0x9F) || (lead >= 0xE0 && lead <= 0xFC)) &&
            index + 1 < text.size() && text[index + 1])
            length = 2;
        if (!rasterizer(scratch_, base_x + step * line.ruby_spacing * 2, 2, true,
                        0x00000000u, text.substr(index, length))) {
            error = "dialogue ruby rasterization failed";
            return false;
        }
        index += length;
        ++step;
    }
    return true;
}

bool DialogueText::write_row(AnmManager& manager, DialogueTextureSink& sink, i32 row,
                             const MsgLine& line, const DialogueRasterizer& rasterizer,
                             std::string& error) {
    const Region& region = regions_[u32(row)];
    const Region& ruby_region = regions_[u32(kLineCount + row)];
    if (!blank(sink, region, error) || !blank(sink, ruby_region, error)) return false;

    // 正文条带。0x428ae0 case 0x11（反汇编 429198-4291bb）：0046e8f0 调用为
    // (fg=EDI[0x62+side], stroke=0, style=(flags>>1)&1, 0, 0, text)；MSG VM 的
    // +0x188/+0x18c 在 004282b0 memset(0) 后无任何非零写入 → fg=黑、描边=黑。
    // 原版 v1.00c 对话帧（original-dialogue f0030）气泡正文核心像素实测 (0,0,0)。
    std::fill(scratch_.pixels.begin(), scratch_.pixels.end(), 0);
    if (!rasterizer(scratch_, 0, 2, false, 0x00000000u, line.text_cp932)) {
        error = "dialogue body rasterization failed";
        return false;
    }
    if (!copy_strip(sink, region, error)) return false;

    // ruby 条带（sprite2/3）。无注记时保持清空并提交 changed。
    if (line.has_ruby && !line.ruby_cp932.empty()) {
        if (!rasterize_ruby(line, rasterizer, error)) return false;
        if (!copy_strip(sink, ruby_region, error)) return false;
    } else {
        sink.changed(texture_, {u32(ruby_region.x), u32(ruby_region.y),
                                u32(ruby_region.x + ruby_region.width),
                                u32(ruby_region.y + ruby_region.height)});
    }
    // 0x428ae0:489-493/554-558：写入后 0x46fc30(handle, 2) 让 VM 离开
    // ins_4 隐藏等待并跳到 label 2 显示分支（延迟到 ANM tick 消费）。
    if (!request_label(manager, handles_[u32(row)], 2, error)) return false;
    if (line.has_ruby && !line.ruby_cp932.empty() &&
        !request_label(manager, handles_[u32(kLineCount + row)], 2, error))
        return false;
    return true;
}

bool DialogueText::submit(AnmManager& manager, DialogueTextureSink& sink, i32 row,
                          const MsgLine& line, float box_x, float box_y,
                          const DialogueRasterizer& rasterizer, std::string& error) {
    if (row < 0 || row >= kLineCount) {
        error = "dialogue row outside text.anm scripts 0..1";
        return false;
    }
    if (!ensure(manager, sink, error)) return false;

    const bool unchanged = cached_[u32(row)] && cached_text_[u32(row)] == line.text_cp932 &&
                           cached_block_[u32(row)] == line.block;
    if (row == 0 && unchanged) {
        place_rows(manager, box_x, box_y);
        return true;
    }
    if (row == 0) {
        // 0x428ae0:426-452 块首：先清四槽（0x46fc30 label 3），再写当前行。
        if (!blank_all(sink, error, 3)) return false;
    }
    if (unchanged) {
        place_rows(manager, box_x, box_y);
        return true;
    }
    // 0x428ae0:485-553：每条正文行先重建对话框（0042a120），再画文字。
    if (!update_bubble(manager, row, line, box_x, box_y, error)) return false;
    if (!write_row(manager, sink, row, line, rasterizer, error)) return false;
    cached_[u32(row)] = true;
    cached_text_[u32(row)] = line.text_cp932;
    cached_block_[u32(row)] = line.block;
    cached_line_[u32(row)] = line;
    if (row == 1 && cached_[0] && cached_block_[0] == line.block)
        cached_line_[0].bubble_width = line.bubble_width;
    place_rows(manager, box_x, box_y);
    return true;
}

bool DialogueText::fade(i32 label, std::string& error) {
    if (!manager_) return true;  // 尚未建立文本 VM：无 fade 目标
    // 0x424ba0：释放对话框根 VM（opcode 6/18 均在文本 label 前/后销毁气泡）。
    if (!retire_bubble(*manager_, error)) return false;
    // 0x42960c/0x429250：只对四条文本 VM 发 label；不改 texture0 像素，
    // 也不重置 cached_，否则 RunScene 的重复提交会用 label2 把淡出打断。
    for (u32 handle : handles_)
        if (!request_label(*manager_, handle, label, error)) return false;
    return true;
}

bool DialogueText::clear(DialogueTextureSink& sink, std::string& error) {
    (void)sink;
    // RunScene rows==0 兜底：与原版 opcode 6 相同，只淡出，不清像素。
    return fade(1, error);
}

bool DialogueText::set_anchor_y(AnmManager& manager, float value, std::string& error) {
    anchor_y_ = value;
    anchor_y_set_ = true;
    for (u32 handle : handles_) {
        if (!handle) continue; // VMs are allocated lazily in the web host.
        auto* vm = manager.registry.find(handle);
        if (!vm) { error = "text.anm dialogue VM unavailable"; return false; }
        vm->visual.child_anchor.y = value;
    }
    return true;
}

void DialogueText::release(AnmManager& manager) noexcept {
    for (u32& handle : handles_) manager.retire(handle);
    handles_.fill(0);
    if (box_) manager.retire(box_);
    box_ = 0;
    regions_.fill(Region{});
    cached_.fill(false);
    for (auto& text : cached_text_) text.clear();
    for (auto& line : cached_line_) line = MsgLine{};
    manager_ = nullptr;
    texture_ = 0;
    anchor_y_ = 0;
    anchor_y_set_ = false;
}

bool DialogueText::retire_bubble(AnmManager& manager, std::string& error) {
    if (!box_) return true;
    if (!manager.retire(box_)) {
        error = manager.error.empty() ? "front.anm dialogue bubble retire failed" : manager.error;
        return false;
    }
    box_ = 0;
    return true;
}

bool DialogueText::update_bubble(AnmManager& manager, i32 row, const MsgLine& line, float box_x,
                                 float box_y, std::string& error) {
    if (bubble_bank_ < 0) return true;
    AnmResource* front = manager.resource(bubble_bank_);
    if (!front) return true;  // 单测无 front.anm：显式降级为无气泡
    // 0042a120：param = side + 2*((+0x178>>2)&0xf)；根 224+param，
    // 左右端帽 176+param / 208+param（192+param 为固定中段）。
    const i32 variant = i32((line.text_attr >> 2) & 0xFu);
    const i32 param = (line.side > 0 ? 1 : 0) + variant * 2 + row * 8;
    const i32 root_script = 224 + param;
    const i32 left_script = 176 + param;
    const i32 right_script = 208 + param;
    if (root_script < 0 || u32(root_script) >= front->scripts.size()) {
        error = "front.anm dialogue bubble script missing: " + std::to_string(root_script);
        return false;
    }
    if (!retire_bubble(manager, error)) return false;
    box_ = manager.create(bubble_bank_, root_script, -1, 2, {box_x, box_y}, 0, false);
    if (!box_) {
        error = manager.error.empty() ? "front.anm dialogue bubble creation failed"
                                      : manager.error;
        return false;
    }
    AnmVm* root = manager.registry.find(box_);
    if (!root) {
        error = "front.anm dialogue bubble VM unavailable";
        return false;
    }
    // 0x428ae0:464/529 后续 0042a120/0042a280：两端帽 variables.floats[0]
    // (= VM+0x4bc = 10004) 写当前行块宽度；脚本 label2 用它算拉伸/右端位置。
    const auto set_width = [&](i32 script) -> bool {
        for (AnmVm* child : manager.registry.children(*root)) {
            if (child->source_script == script) {
                child->variables.floats[0] = line.bubble_width;
                return true;
            }
        }
        error = "front.anm dialogue bubble child missing: " + std::to_string(script);
        return false;
    };
    if (!set_width(left_script) || !set_width(right_script)) {
        manager.retire(box_);
        box_ = 0;
        return false;
    }
    return true;
}

void DialogueText::place_rows(AnmManager& manager, float box_x, float box_y) {
    for (i32 row = 0; row < kLineCount; row++) {
        if (!cached_[u32(row)]) continue;
        const MsgLine& line = cached_line_[u32(row)];
        const float x = row_x(line, box_x);
        for (i32 slot = row; slot < kVmCount; slot += kLineCount) {
            if (AnmVm* vm = manager.registry.find(handles_[u32(slot)]))
                vm->visual.translation = {x, box_y, 0.0f};
        }
    }
}

}  // namespace th13
