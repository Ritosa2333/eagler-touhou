#pragma once

#include "AnmManager.hpp"
#include "MsgPlayer.hpp"
#include "TextureImage.hpp"

#include <array>
#include <functional>
#include <memory>
#include <string>

namespace th13 {

struct DialogueTextureSink {
    virtual ~DialogueTextureSink() = default;
    virtual u32 texture(const AnmResource& resource, u32 index) = 0;
    virtual TextureImage* pixels(u32 handle) = 0;
    virtual void changed(u32 handle, const std::array<u32, 4>& region) = 0;
};

using DialogueRasterizer =
    std::function<bool(TextureImage& image, i32 x, i32 y, bool ruby, u32 color, const std::string& cp932_text)>;

class DialogueText {
public:
    static constexpr i32 kLineCount = 2;
    static constexpr i32 kVmCount = 4;

    bool begin(AnmManager& manager, DialogueTextureSink& sink, std::string& error) {
        return ensure(manager, sink, error);
    }
    bool submit(AnmManager& manager, DialogueTextureSink& sink, i32 row, const MsgLine& line, float box_x, float box_y,
                const DialogueRasterizer& rasterizer, std::string& error);
    bool clear(DialogueTextureSink& sink, std::string& error);
    bool fade(i32 label, std::string& error);
    bool set_visible(AnmManager& manager, bool visible, std::string& error);
    bool set_anchor_y(AnmManager& manager, float value, std::string& error);
    void release(AnmManager& manager) noexcept;
    bool active() const noexcept { return handles_[0] != 0; }
    void bind_bubble_bank(i32 bank) noexcept { bubble_bank_ = bank; }
    u32 bubble_handle() const noexcept { return box_; }

    struct Region {
        i32 x = 0, y = 0, width = 0, height = 0;
    };
    struct RollbackState {
        std::array<u32, kVmCount> handles;
        std::array<Region, kVmCount> regions;
        std::array<std::string, kLineCount> text;
        std::array<u32, kLineCount> block;
        std::array<bool, kLineCount> cached;
        std::array<MsgLine, kLineCount> lines;
        u32 box, texture;
        float anchor_y;
        bool anchor_y_set;
        u32 pixel_texture;
        std::shared_ptr<const std::vector<u8>> pixels;
    };
    RollbackState capture_rollback(AnmManager&, DialogueTextureSink&) const;
    bool restore_rollback(const RollbackState&, AnmManager&, DialogueTextureSink&, std::string& error);

private:
    bool ensure(AnmManager& manager, DialogueTextureSink& sink, std::string& error);
    bool blank(DialogueTextureSink& sink, const Region& region, std::string& error);
    bool write_row(AnmManager& manager, DialogueTextureSink& sink, i32 row, const MsgLine& line,
                   const DialogueRasterizer& rasterizer, std::string& error);
    bool request_label(AnmManager& manager, u32 handle, i32 label, std::string& error);
    bool blank_all(DialogueTextureSink& sink, std::string& error, i32 label);
    bool copy_strip(DialogueTextureSink& sink, const Region& region, std::string& error);
    bool rasterize_ruby(const MsgLine& line, const DialogueRasterizer& rasterizer, std::string& error);
    bool update_bubble(AnmManager& manager, i32 row, const MsgLine& line, float box_x, float box_y, std::string& error);
    bool retire_bubble(AnmManager& manager, std::string& error);
    void place_rows(AnmManager& manager, float box_x, float box_y);
    static float row_x(const MsgLine& line, float box_x) noexcept {
        return line.side > 0 ? float(box_x - (line.bubble_width + 30.0f)) : box_x;
    }

    std::array<u32, kVmCount> handles_{};
    std::array<Region, kVmCount> regions_{};
    std::array<std::string, kLineCount> cached_text_{};
    std::array<u32, kLineCount> cached_block_{};
    std::array<bool, kLineCount> cached_{};
    std::array<MsgLine, kLineCount> cached_line_{};
    AnmManager* manager_ = nullptr;
    i32 bubble_bank_ = 3;
    u32 box_ = 0;
    u32 texture_ = 0;
    float anchor_y_ = 0;
    bool anchor_y_set_ = false;
    TextureImage scratch_;
    mutable std::shared_ptr<const std::vector<u8>> rollback_pixels_;
    mutable u32 rollback_texture_ = 0;
};

}
