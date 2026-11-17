#pragma once
#include "GraphicsDevice.hpp"
#include "../game/PlayerMotion.hpp"
#include <algorithm>
#include <cstring>
#include <variant>
#include <type_traits>

namespace th09::sdl {
// GameWorld::draw also updates original game state. Repeat only its captured
// GPU commands, changing the local ship's two triangles for the extra frame.
class MotionCapture final : public ZunGraphics {
    struct Transform { MatrixKind kind; Matrix4 matrix; };
    struct Clear { bool color, depth; u32 rgba; float z; };
    struct Draw {
        PipelineState pipeline;
        u32 texture;
        Topology topology;
        VertexLayout layout;
        u32 count;
        size_t offset;
        bool batch;
    };
    using Command = std::variant<Viewport, Transform, Clear, Draw>;
    GraphicsDevice& output;
    touhou::sdl::State initial;
    std::vector<Command> commands;
    std::vector<u32> vertices;
    bool recording = false;
    bool valid = false;
    size_t player = size_t(-1);

    template<class T> void record(T command) {
        if (!valid || !recording) return;
        if (commands.size() >= 4096) { invalidate(); return; }
        commands.emplace_back(command);
    }
    void record_draw(Draw command, const void* data) {
        if (!valid || !recording) return;
        constexpr size_t max_words = 2 * 1024 * 1024 / sizeof(u32);
        const size_t words = size_t(command.count) * stride(command.layout) / sizeof(u32);
        if (words > max_words - vertices.size()) { invalidate(); return; }
        command.offset = vertices.size();
        vertices.resize(vertices.size() + words);
        std::memcpy(vertices.data() + command.offset, data, words * sizeof(u32));
        if (body_batch && command.batch && command.count == 6) player = commands.size();
        record(command);
    }

public:
    const AnmVm* body = nullptr;
    bool body_batch = false;
    explicit MotionCapture(GraphicsDevice& target) : output(target) {}

    void begin(const AnmVm* sprite) {
        commands.clear();
        vertices.clear();
        initial = output.backend.state;
        player = size_t(-1);
        body = sprite;
        recording = valid = sprite != nullptr;
    }
    void end() { recording = false; body = nullptr; }
    void invalidate() { valid = false; }
    bool ready() const { return valid && player < commands.size(); }
    void viewport(const Viewport& v) override { record(v); output.viewport(v); }
    void transform(MatrixKind kind, const Matrix4& m) override { record(Transform{kind, m}); output.transform(kind, m); }
    void clear(bool color, bool depth, u32 rgba, float z) override { record(Clear{color, depth, rgba, z}); output.clear(color, depth, rgba, z); }
    void draw(const PipelineState& p, u32 t, Topology topology, VertexLayout layout, const void* data, u32 count) override {
        record_draw({p, t, topology, layout, count, 0, false}, data);
        output.draw(p, t, topology, layout, data, count);
    }
    void triangles(const PipelineState& p, u32 t, const SpriteVertex* data, u32 count) override {
        record_draw({p, t, Topology::Triangles, VertexLayout::ScreenColorUv, count, 0, true}, data);
        output.triangles(p, t, data, count);
    }
    bool present(Vec2 delta) {
        if (!ready()) return false;
        const auto saved = output.backend.state;
        output.backend.state = initial;
        for (size_t i = 0; i < commands.size(); ++i) {
            std::visit([&](const auto& command) {
                using T = std::decay_t<decltype(command)>;
                if constexpr (std::is_same_v<T, Viewport>) output.viewport(command);
                else if constexpr (std::is_same_v<T, Transform>) output.transform(command.kind, command.matrix);
                else if constexpr (std::is_same_v<T, Clear>) output.clear(command.color, command.depth, command.rgba, command.z);
                else {
                    const auto* data = vertices.data() + command.offset;
                    if (i == player) {
                        SpriteVertex shifted[6];
                        std::memcpy(shifted, data, sizeof shifted);
                        for (auto& vertex : shifted) {
                            vertex.position.x += delta.x;
                            vertex.position.y += delta.y;
                        }
                        output.triangles(command.pipeline, command.texture, shifted, command.count);
                    } else if (command.batch) {
                        output.triangles(command.pipeline, command.texture, reinterpret_cast<const SpriteVertex*>(data), command.count);
                    } else {
                        output.draw(command.pipeline, command.texture, command.topology, command.layout, data, command.count);
                    }
                }
            }, commands[i]);
        }
        const auto before = output.backend.stats.presentations;
        output.present();
        output.backend.state = saved;
        return output.backend.stats.presentations != before;
    }
};

inline MotionCapture* active_motion_capture = nullptr;

inline Vec2 predict_sprite(const PlayerMotion& motion, u16 keys, bool automatic_focus, u16 fire_frames,
                           const MovementSpeeds& speeds, const PlayfieldLimits& limits, float rate) {
    struct NoEffects final : MotionEffects {
        void begin_focus(const Vec3&, u32, u32) override {}
        void end_focus() override {}
        void animation_interrupt(u32) override {}
        void update_options() override {}
    } effects;
    auto predicted = motion;
    predicted.update(keys, automatic_focus, fire_frames, speeds, limits, rate, effects);
    return {predicted.position.x - motion.position.x, predicted.position.y - motion.position.y};
}
}
