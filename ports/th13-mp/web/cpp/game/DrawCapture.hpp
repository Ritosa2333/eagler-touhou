#pragma once
#include "AnmResource.hpp"
#include "ZunGraphics.hpp"

#include <cstdio>
#include <map>
#include <string>
#include <vector>

namespace th13 {

class DrawCapture final : public ZunGraphics {
public:
    void register_texture(const AnmResource* file, u32 first, u32 count, i32 slot);

    bool open_output(const char* path);
    bool close_output() noexcept;
    bool legacy_harness() const noexcept {
#ifdef __EMSCRIPTEN__
        return false;
#else
        return true;
#endif
    }
    void begin_frame(u32 frame);
    bool end_frame();
    void clear_device_texture() noexcept {
        bound_set_ = false;
        bound_ = Identity{};
    }
    void sprite(i32 layer, u32 flags) noexcept;
    void set_stale_layer(i32 layer) noexcept { layer_ = layer; }

    u64 frames_out() const noexcept { return frames_out_; }
    u64 quads() const noexcept { return quads_; }
    u64 ups() const noexcept { return ups_; }
    u32 zero_op_frames() const noexcept { return zero_frames_; }
    const std::string& error() const noexcept { return error_; }
    long output_position() const noexcept { return out_ ? std::ftell(out_) : 0; }

    touhou::graphics::PipelineState& pipeline() override { return pipeline_; }
    u32 texture(const AnmResource& file, u32 index) override;
    void bind_texture(u32 handle) override;
    void set_layout(touhou::graphics::VertexLayout layout) override;
    void set_matrix(touhou::graphics::MatrixKind kind, const Matrix4& matrix) override;
    void triangles(u32 count, const void* data, u32 stride) override;
    void primitives(touhou::graphics::Topology kind, u32 count, const void* data, u32 stride) override;
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport& value) override {
        viewport_ = value;
        viewport_set_ = true;
    }
    void presentation_offset(Vec2) override {}
    void presentation_range(const AnmVm&, u32, u32, u32 = 0) override {}
    void presentation_camera(bool) override {}

private:
    struct Identity {
        i32 slot = -1, entry = -1;
        std::string name;
    };
    i32 layer_ = -100;
    u32 anchor_ = 0;
    touhou::graphics::PipelineState pipeline_{};
    touhou::graphics::VertexLayout layout_ = touhou::graphics::VertexLayout::ScreenColorUv;
    Matrix4 matrices_[4]{};
    Identity bound_{};
    bool bound_set_ = false;
    GraphicsViewport viewport_{};
    bool viewport_set_ = false;
    std::vector<Identity> identities_;
    std::map<std::pair<const AnmResource*, u32>, u32> handles_;
    std::FILE* out_ = nullptr;
    std::string line_;
    u32 frame_ = 0, seq_ = 0, zero_frames_ = 0;
    u64 frames_out_ = 0, quads_ = 0, ups_ = 0;
    std::string error_;

    void fail(const char* why) {
        if (error_.empty()) error_ = why;
    }
    __attribute__((noinline)) void emit_quad(u32 prim, u32 count, u32 stride, const void* data);
    __attribute__((noinline)) void emit_up(u32 prim, u32 count, u32 stride, const void* data);
    __attribute__((noinline)) void append_tex(const Identity& identity);
    __attribute__((noinline)) void append_blend();
    __attribute__((noinline)) void append_matrix(const Matrix4& matrix);
    __attribute__((noinline)) static void append_float(std::string& out, float value);
};

extern DrawCapture* draw_capture;

}
