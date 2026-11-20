#pragma once
#include "GraphicsDevice.hpp"
#include "../game/PlayerMotion.hpp"
#include <algorithm>
#include <array>
#include <cstring>
#include <variant>
#include <type_traits>

namespace th09::sdl {
// GameWorld::draw also updates original game state. Repeat only its captured
// GPU commands, moving sprites for intermediate presentations.
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
        size_t motion_first = 0, motion_end = 0;
    };
    struct QuadMotion { u32 first; Vec2 velocity; };
    using Command = std::variant<Viewport, Transform, Clear, Draw>;
    GraphicsDevice& output;
    touhou::sdl::State initial;
    std::vector<Command> commands;
    std::vector<u32> vertices;
    std::vector<QuadMotion> motions;
    std::vector<SpriteVertex> shifted_batch;
    size_t motion_first = 0;
    bool recording = false;
    bool valid = false;
    std::array<size_t,2> players{};

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
        if (body_batch >= 0 && command.batch && command.count == 6) players[body_batch] = commands.size();
        record(command);
    }

public:
    std::array<const AnmVm*,2> bodies{};
    int body_batch = -1;
    explicit MotionCapture(GraphicsDevice& target) : output(target) {}

    int slot(const AnmVm& sprite) const {
        for (int side = 0; side < 2; ++side) if (bodies[side] == &sprite) return side;
        return -1;
    }
    void begin(const AnmVm* sprite) { begin({sprite,nullptr}); }
    void begin(std::array<const AnmVm*,2> sprites) {
        commands.clear();
        vertices.clear();
        motions.clear();
        motion_first = 0;
        initial = output.backend.state;
        players.fill(size_t(-1));
        bodies = sprites;
        recording = valid = bodies[0] || bodies[1];
    }
    void end() { recording = false; bodies.fill(nullptr); body_batch = -1; }
    void invalidate() { valid = false; }
    bool ready(int side) const { return valid && players[side] < commands.size(); }
    bool bullets_ready() const { return valid && motion_first != 0; }
    bool ready() const { return ready(0) || ready(1) || bullets_ready(); }
    void viewport(const Viewport& v) override { record(v); output.viewport(v); }
    void transform(MatrixKind kind, const Matrix4& m) override { record(Transform{kind, m}); output.transform(kind, m); }
    void clear(bool color, bool depth, u32 rgba, float z) override { record(Clear{color, depth, rgba, z}); output.clear(color, depth, rgba, z); }
    void draw(const PipelineState& p, u32 t, Topology topology, VertexLayout layout, const void* data, u32 count) override {
        record_draw({p, t, topology, layout, count, 0, false}, data);
        output.draw(p, t, topology, layout, data, count);
    }
    void triangles(const PipelineState& p, u32 t, const SpriteVertex* data, u32 count) override {
        record_draw({p, t, Topology::Triangles, VertexLayout::ScreenColorUv, count, 0, true, motion_first, motions.size()}, data);
        motion_first = motions.size();
        output.triangles(p, t, data, count);
    }
    void sprite_motion(u32 first, Vec2 velocity) override {
        if(valid&&recording)motions.push_back({first,velocity});
    }
    bool present(Vec2 delta) { return present({delta,Vec2{}},0.f); }
    bool present(const std::array<Vec2,2>& deltas,float phase) {
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
                    const int side = i == players[0] ? 0 : i == players[1] ? 1 : -1;
                    if (side >= 0) {
                        const auto delta = deltas[side];
                        SpriteVertex shifted[6];
                        std::memcpy(shifted, data, sizeof shifted);
                        for (auto& vertex : shifted) {
                            vertex.position.x += delta.x;
                            vertex.position.y += delta.y;
                        }
                        output.triangles(command.pipeline, command.texture, shifted, command.count);
                    } else if (command.motion_first!=command.motion_end) {
                        shifted_batch.resize(command.count);
                        std::memcpy(shifted_batch.data(),data,command.count*sizeof(SpriteVertex));
                        for(auto n=command.motion_first;n<command.motion_end;++n){
                            const auto& motion=motions[n];
                            for(auto v=motion.first;v<motion.first+6;++v){
                                shifted_batch[v].position.x+=motion.velocity.x*phase;
                                shifted_batch[v].position.y+=motion.velocity.y*phase;
                            }
                        }
                        output.triangles(command.pipeline,command.texture,shifted_batch.data(),command.count);
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
