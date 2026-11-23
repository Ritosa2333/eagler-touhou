// th16 harness 录制设备——RenderDevice 的"假显卡"：不画任何东西，只把每次绘制 / 清屏 / 切目标时生效的
// 状态与顶点字节记下来，供 tools/oracle/draw-compare.py 与 game-oracle.py --draw 录下的原版 D3D 调用流对拍，
// 也供 scene-boot / anm-draw 命令以可读形式打印四边形。
//
// 与原版调用流的比较口径：只比较"每次绘制时生效的状态"，不比较冗余的 SetRenderState 调用次数与顺序。
#pragma once
#include "../../web/cpp/game/Camera.hpp"
#include "../../web/cpp/game/RenderDevice.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace th16 {

class RecordingDevice final : public RenderDevice {
public:
    using PipelineState = touhou::graphics::PipelineState;

    enum class OpKind { Target, Viewport, Clear, Draw };

    struct Op {
        explicit Op(OpKind k) : kind(k) {}
        OpKind kind;
        TextureRef target;          // Target：切到的目标；Draw / Clear：当时的目标
        Viewport viewport;          // Viewport：设置值；Draw / Clear：当时的视口
        // Clear
        u32 clear_flags = 0;
        u32 clear_color = 0;
        bool has_rect = false;
        ScreenRect rect;
        // Draw
        Topology topology = Topology::Triangles;
        VertexLayout layout = VertexLayout::ScreenColorUv;
        u32 primitives = 0;
        std::vector<u32> words;     // 顶点原始 32 位字
        PipelineState state;
        TextureRef texture;
        float world[16] = {}, view[16] = {}, projection[16] = {}, texture_matrix[16] = {};
    };

    std::vector<Op> ops;            // 当前帧累计
    u32 draws_total = 0;

    RecordingDevice() {
        apply_original_device_defaults(state_);
        matrix::identity(world_);
        matrix::identity(view_);
        matrix::identity(projection_);
        matrix::identity(texture_matrix_);
    }

    PipelineState& pipeline() override { return state_; }

    void select_target(TextureRef target) override {
        target_ = target;
        viewport_ = Viewport{0, 0, 640, 480};
        Op op(OpKind::Target);
        op.target = target;
        ops.push_back(std::move(op));
    }

    void clear(u32 flags, u32 argb, const ScreenRect* rect) override {
        Op op(OpKind::Clear);
        op.target = target_;
        op.viewport = viewport_;
        op.clear_flags = flags;
        op.clear_color = argb;
        if (rect != nullptr) { op.has_rect = true; op.rect = *rect; }
        ops.push_back(std::move(op));
    }

    void set_viewport(const Viewport& viewport) override {
        viewport_ = viewport;
        Op op(OpKind::Viewport);
        op.viewport = viewport;
        ops.push_back(std::move(op));
    }

    void set_matrix(MatrixKind kind, const float m[16]) override {
        float* dst = kind == MatrixKind::World ? world_ : kind == MatrixKind::View ? view_
                   : kind == MatrixKind::Projection ? projection_ : texture_matrix_;
        std::memcpy(dst, m, sizeof(float) * 16);
    }

    void bind_texture(TextureRef texture) override { texture_ = texture; }

    void draw(Topology topology, VertexLayout layout, u32 primitive_count, const void* vertices, u32 stride) override {
        Op op(OpKind::Draw);
        op.target = target_;
        op.viewport = viewport_;
        op.topology = topology;
        op.layout = layout;
        op.primitives = primitive_count;
        op.state = state_;
        op.texture = texture_;
        std::memcpy(op.world, world_, sizeof world_);
        std::memcpy(op.view, view_, sizeof view_);
        std::memcpy(op.projection, projection_, sizeof projection_);
        std::memcpy(op.texture_matrix, texture_matrix_, sizeof texture_matrix_);
        const u32 vertex_count = touhou::graphics::vertex_count(topology, primitive_count);
        const u32 words = vertex_count * stride / 4;
        op.words.resize(words);
        std::memcpy(op.words.data(), vertices, size_t(words) * 4);
        ops.push_back(std::move(op));
        ++draws_total;
    }

    void begin_frame() { ops.clear(); }

    // 一帧的 JSON 行：{"frame":n,"ops":[…]}
    void write_frame(std::FILE* out, int frame) const {
        std::fprintf(out, "{\"frame\":%d,\"ops\":[", frame);
        bool first = true;
        for (const Op& op : ops) {
            std::fputs(first ? "" : ",", out);
            first = false;
            write_op(out, op);
        }
        std::fputs("]}\n", out);
    }

    // 可读的四边形摘要：屏幕批每 6 顶点一个四边形；世界条带（4 顶点）经 world·view·projection 与视口投影。
    // 每行：kind slot entry x0 y0 x1 y1 x2 y2 x3 y3 color z0..z3 rhw0..rhw3 fog fog0..fog3 zt zw u0 v0 u3 v3
    struct QuadSummary {
        char kind;                 // 's' 屏幕四边形 / 'w' 世界四边形
        TextureRef texture;
        float x[4], y[4], z[4], rhw[4];
        u32 color;
        bool fog = false;
        u32 fog_factor[4] = {};
        bool depth_test = false, depth_write = false;
        float u0, v0, u3, v3;
    };
    std::vector<QuadSummary> quads() const {
        std::vector<QuadSummary> out;
        for (const Op& op : ops) {
            if (op.kind != OpKind::Draw) continue;
            if (op.layout == VertexLayout::ScreenColorUv && op.topology == Topology::Triangles) {
                for (size_t base = 0; base + 42 <= op.words.size(); base += 42) {
                    QuadSummary q{};
                    q.kind = 's';
                    q.texture = op.texture;
                    // 顶点序 TL, TR, BL, TR, BL, BR → 取 0, 1, 2, 5
                    const size_t idx[4] = {0, 1, 2, 5};
                    for (int i = 0; i < 4; ++i) {
                        const u32* v = &op.words[base + idx[i] * 7];
                        q.x[i] = as_float(v[0]); q.y[i] = as_float(v[1]); q.z[i] = as_float(v[2]); q.rhw[i] = as_float(v[3]);
                        q.fog_factor[i] = 0;
                    }
                    q.color = op.words[base + 4];
                    q.u0 = as_float(op.words[base + 5]); q.v0 = as_float(op.words[base + 6]);
                    q.u3 = as_float(op.words[base + 5 * 7 + 5]); q.v3 = as_float(op.words[base + 5 * 7 + 6]);
                    q.fog = op.state.fog;
                    q.depth_test = op.state.depthTest && op.state.depthCompare != touhou::graphics::Compare::Always;
                    q.depth_write = op.state.depthWrite;
                    out.push_back(q);
                }
            } else if (op.layout == VertexLayout::WorldUv && op.topology == Topology::Strip && op.words.size() >= 20) {
                QuadSummary q{};
                q.kind = 'w';
                q.texture = op.texture;
                float wv[16], full[16];
                matrix::multiply(wv, op.world, op.view);
                matrix::multiply(full, wv, op.projection);
                for (int i = 0; i < 4; ++i) {
                    const float p[3] = {as_float(op.words[i * 5]), as_float(op.words[i * 5 + 1]), as_float(op.words[i * 5 + 2])};
                    float clip[4], eye[4];
                    matrix::transform_point(p, full, clip);
                    matrix::transform_point(p, wv, eye);
                    const float inv = 1.0f / clip[3];
                    q.x[i] = (1.0f + clip[0] * inv) * float(op.viewport.width) * 0.5f + float(op.viewport.x);
                    q.y[i] = (1.0f - clip[1] * inv) * float(op.viewport.height) * 0.5f + float(op.viewport.y);
                    q.z[i] = clip[2] * inv;
                    q.rhw[i] = inv;
                    // 顶点雾：线性 (far − z_eye) / (far − near)，钳 0..1，乘 255
                    float f = (op.state.fogFar - eye[2]) / (op.state.fogFar - op.state.fogNear);
                    if (f < 0.0f) f = 0.0f;
                    if (f > 1.0f) f = 1.0f;
                    q.fog_factor[i] = u32(f * 255.0f + 0.5f);
                    float uv[4];
                    const float tex[3] = {as_float(op.words[i * 5 + 3]), as_float(op.words[i * 5 + 4]), 1.0f};
                    if (op.state.textureTransform) matrix::transform_point(tex, op.texture_matrix, uv);
                    else { uv[0] = tex[0]; uv[1] = tex[1]; }
                    if (i == 0) { q.u0 = uv[0]; q.v0 = uv[1]; }
                    if (i == 3) { q.u3 = uv[0]; q.v3 = uv[1]; }
                }
                q.color = op.state.textureFactor;
                q.fog = op.state.fog;
                q.depth_test = op.state.depthTest && op.state.depthCompare != touhou::graphics::Compare::Always;
                q.depth_write = op.state.depthWrite;
                out.push_back(q);
            }
        }
        return out;
    }

    static void print_quad(const QuadSummary& q) {
        std::printf("quad %c slot=%d entry=%d %.8g %.8g %.8g %.8g %.8g %.8g %.8g %.8g %08x z=%.8g,%.8g,%.8g,%.8g "
                    "rhw=%.8g,%.8g,%.8g,%.8g fog=%d,%u,%u,%u,%u zt=%d zw=%d uv=%.6g,%.6g,%.6g,%.6g\n",
                    q.kind, q.texture.slot, q.texture.entry, q.x[0], q.y[0], q.x[1], q.y[1], q.x[2], q.y[2], q.x[3], q.y[3],
                    q.color, q.z[0], q.z[1], q.z[2], q.z[3], q.rhw[0], q.rhw[1], q.rhw[2], q.rhw[3],
                    q.fog ? 1 : 0, q.fog_factor[0], q.fog_factor[1], q.fog_factor[2], q.fog_factor[3],
                    q.depth_test ? 1 : 0, q.depth_write ? 1 : 0, q.u0, q.v0, q.u3, q.v3);
    }

private:
    PipelineState state_;
    TextureRef target_ = TextureRef::backbuffer();
    TextureRef texture_ = TextureRef::none();
    Viewport viewport_{0, 0, 640, 480};
    float world_[16], view_[16], projection_[16], texture_matrix_[16];

    static float as_float(u32 bits) { float f; std::memcpy(&f, &bits, 4); return f; }

    static void write_matrix(std::FILE* out, const char* name, const float m[16]) {
        std::fprintf(out, ",\"%s\":[", name);
        for (int i = 0; i < 16; ++i) {
            u32 bits; std::memcpy(&bits, &m[i], 4);
            std::fprintf(out, "%s%u", i ? "," : "", bits);
        }
        std::fputs("]", out);
    }

    static void write_op(std::FILE* out, const Op& op) {
        using namespace touhou::graphics;
        switch (op.kind) {
        case OpKind::Target:
            std::fprintf(out, "{\"op\":\"target\",\"slot\":%d,\"entry\":%d}", op.target.slot, op.target.entry);
            return;
        case OpKind::Viewport:
            std::fprintf(out, "{\"op\":\"viewport\",\"rect\":[%d,%d,%d,%d]}", op.viewport.x, op.viewport.y, op.viewport.width, op.viewport.height);
            return;
        case OpKind::Clear:
            std::fprintf(out, "{\"op\":\"clear\",\"flags\":%u,\"color\":%u,\"target\":[%d,%d],", op.clear_flags, op.clear_color,
                         op.target.slot, op.target.entry);
            if (op.has_rect) std::fprintf(out, "\"rect\":[%d,%d,%d,%d]}", op.rect.left, op.rect.top, op.rect.right, op.rect.bottom);
            else std::fputs("\"rect\":null}", out);
            return;
        case OpKind::Draw:
            break;
        }
        const PipelineState& s = op.state;
        static const char* const kTopology[] = {"points", "lines", "linestrip", "triangles", "strip", "fan"};
        static const char* const kLayout[] = {"WorldUv", "WorldColorUv", "ScreenColorUv", "ScreenColor", "ScreenUv"};
        std::fprintf(out, "{\"op\":\"draw\",\"topology\":\"%s\",\"layout\":\"%s\",\"primitives\":%u,\"target\":[%d,%d],"
                          "\"texture\":[%d,%d],\"viewport\":[%d,%d,%d,%d],\"state\":{",
                     kTopology[u32(op.topology)], kLayout[u32(op.layout)], op.primitives, op.target.slot, op.target.entry,
                     op.texture.slot, op.texture.entry, op.viewport.x, op.viewport.y, op.viewport.width, op.viewport.height);
        std::fprintf(out, "\"depthTest\":%d,\"depthWrite\":%d,\"depthCompare\":%u,\"blend\":%d,\"srcBlend\":%u,\"dstBlend\":%u,"
                          "\"blendOp\":%u,\"alphaTest\":%d,\"alphaRef\":%u,\"alphaCompare\":%u,\"fog\":%d,\"fogColor\":%u,"
                          "\"fogNear\":%u,\"fogFar\":%u,\"textureFactor\":%u,\"colorOp\":%u,\"colorArg1\":%u,\"colorArg2\":%u,"
                          "\"alphaOp\":%u,\"alphaArg1\":%u,\"alphaArg2\":%u,\"textureTransform\":%d,\"minFilter\":%u,"
                          "\"magFilter\":%u,\"addressU\":%u,\"addressV\":%u,\"cull\":%u}",
                     s.depthTest ? 1 : 0, s.depthWrite ? 1 : 0, u32(s.depthCompare), s.blend ? 1 : 0, u32(s.sourceBlend),
                     u32(s.destinationBlend), u32(s.blendEquation), s.alphaTest ? 1 : 0, s.alphaReference, u32(s.alphaCompare),
                     s.fog ? 1 : 0, s.fogColor, float_bits(s.fogNear), float_bits(s.fogFar), s.textureFactor,
                     u32(s.color.operation), u32(s.color.first.source), u32(s.color.second.source),
                     u32(s.alpha.operation), u32(s.alpha.first.source), u32(s.alpha.second.source),
                     s.textureTransform ? 1 : 0, u32(s.minFilter), u32(s.magFilter), u32(s.addressU), u32(s.addressV), u32(s.cull));
        const bool world_layout = op.layout == VertexLayout::WorldUv || op.layout == VertexLayout::WorldColorUv;
        if (world_layout) {
            write_matrix(out, "world", op.world);
            write_matrix(out, "view", op.view);
            write_matrix(out, "projection", op.projection);
        }
        if (s.textureTransform && world_layout) write_matrix(out, "textureMatrix", op.texture_matrix);
        std::fputs(",\"words\":[", out);
        for (size_t i = 0; i < op.words.size(); ++i) std::fprintf(out, "%s%u", i ? "," : "", op.words[i]);
        std::fputs("]}", out);
    }

    static u32 float_bits(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
};

}  // namespace th16
