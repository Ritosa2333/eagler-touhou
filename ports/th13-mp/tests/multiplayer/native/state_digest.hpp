#pragma once

#include "game/Types.hpp"
#include "game/Timer.hpp"
#include "game/Interpolation.hpp"
#include "game/AnmVisualState.hpp"
#include "game/AnmVariables.hpp"
#include "game/AnmInterpolators.hpp"
#include "game/AnmVm.hpp"
#include "game/AnmRegistry.hpp"
#include "game/AnmManager.hpp"
#include "game/ReplayRecorder.hpp"
#include "sdl/ApplicationState.hpp"
#include "sdl/AudioDevice.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

namespace mpdigest {

class DigestSink {
public:
    void tag(const char* text) {
        mix(0x9e3779b97f4a7c15ull ^ static_cast<uint64_t>(std::strlen(text)));
        for (const char* p = text; *p; ++p) mix(static_cast<uint8_t>(*p));
        mix(0);
    }
    void u8(uint8_t value) { mix(value); }
    void u32(uint32_t value) {
        mix(value & 0xffu);
        mix((value >> 8) & 0xffu);
        mix((value >> 16) & 0xffu);
        mix((value >> 24) & 0xffu);
    }
    void i32(int32_t value) { u32(static_cast<uint32_t>(value)); }
    void u64(uint64_t value) {
        for (unsigned shift = 0; shift < 64; shift += 8)
            mix(static_cast<uint8_t>(value >> shift));
    }
    void f32(float value) {
        static_assert(sizeof(float) == 4, "digest requires float32");
        uint32_t bits = 0;
        std::memcpy(&bits, &value, sizeof bits);
        u32(bits);
    }
    void f64(double value) {
        static_assert(sizeof(double) == 8, "digest requires float64");
        uint64_t bits = 0;
        std::memcpy(&bits, &value, sizeof bits);
        u64(bits);
    }
    void bytes(const void* data, std::size_t size) {
        const auto* p = static_cast<const uint8_t*>(data);
        u64(size);
        for (std::size_t i = 0; i < size; ++i) mix(p[i]);
    }
    uint64_t value() const { return state_; }

private:
    void mix(uint8_t byte) {
        state_ ^= byte;
        state_ *= 0x100000001b3ull;
    }
    uint64_t state_ = 0xcbf29ce484222325ull;
};

inline void digest_timer(DigestSink& d, const char* tag, const th13::Timer& timer) {
    d.tag(tag);
    d.i32(timer.previous);
    d.i32(timer.current);
    d.f32(timer.fractional);
    d.u32(timer.rate_index);
    d.u32(timer.flags);
}

inline void digest_variables(DigestSink& d, const th13::AnmVariables& v) {
    d.tag("vars.int");
    for (int i = 0; i < 4; ++i) d.i32(v.integers[i]);
    d.tag("vars.extra");
    for (int i = 0; i < 2; ++i) d.i32(v.extra_integers[i]);
    d.tag("vars.float");
    for (int i = 0; i < 4; ++i) d.f32(v.floats[i]);
    d.tag("vars.rand");
    d.f32(v.random_scale);
    d.f32(v.random_angle);
    d.u32(v.random_bound);
    d.tag("vars.pos");
    d.f32(v.position.x);
    d.f32(v.position.y);
    d.f32(v.position.z);
    d.tag("vars.rot");
    d.f32(th13::normalize_angle(v.rotation.x));
    d.f32(th13::normalize_angle(v.rotation.y));
    d.f32(th13::normalize_angle(v.rotation.z));
}

inline void digest_vec2(DigestSink& d, const char* tag, const th13::Vec2& v) {
    d.tag(tag);
    d.f32(v.x);
    d.f32(v.y);
}

inline void digest_vec3(DigestSink& d, const char* tag, const th13::Vec3& v) {
    d.tag(tag);
    d.f32(v.x);
    d.f32(v.y);
    d.f32(v.z);
}

inline void digest_matrix(DigestSink& d, const char* tag, const th13::Matrix4& m) {
    d.tag(tag);
    for (int i = 0; i < 16; ++i) d.f32(m.m[i]);
}

inline void digest_visual(DigestSink& d, const th13::AnmVisualState& v) {
    d.tag("vis.flags");
    d.u32(v.flags & ~0xCu);
    d.u32(v.render_flags);
    d.i32(v.layer);
    d.i32(v.sprite);
    digest_vec3(d, "vis.child_anchor", v.child_anchor);
    digest_vec3(d, "vis.angular_velocity", v.angular_velocity);
    digest_vec2(d, "vis.scale", v.scale);
    digest_vec2(d, "vis.secondary_scale", v.secondary_scale);
    digest_vec2(d, "vis.scale_velocity", v.scale_velocity);
    digest_vec2(d, "vis.uv_scale", v.uv_scale);
    digest_vec2(d, "vis.uv_offset", v.uv_offset);
    digest_vec2(d, "vis.uv_velocity", v.uv_velocity);
    digest_vec2(d, "vis.size", v.size);
    d.tag("vis.colors");
    d.u32(v.color);
    d.u32(v.secondary_color);
    d.tag("vis.uv4");
    for (int i = 0; i < 4; ++i) {
        d.f32(v.uv[i].x);
        d.f32(v.uv[i].y);
    }
    digest_vec2(d, "vis.sprite_size", v.sprite_size);
    digest_matrix(d, "vis.sprite_matrix", v.sprite_matrix);
    digest_matrix(d, "vis.uv_matrix", v.uv_matrix);
    d.tag("vis.rest");
    d.i32(v.sprite_frame);
    digest_vec3(d, "vis.translation", v.translation);
}

template <class Channel>
void digest_interpolation(DigestSink& d, const char* tag, const Channel& ch) {
    using Sample = decltype(ch.start[0]);
    constexpr bool kFloat = std::is_floating_point_v<Sample>;
    d.tag(tag);
    for (std::size_t i = 0; i < ch.start.size(); ++i)
        kFloat ? d.f32(static_cast<float>(ch.start[i]))
               : d.i32(static_cast<int32_t>(ch.start[i]));
    for (std::size_t i = 0; i < ch.end.size(); ++i)
        kFloat ? d.f32(static_cast<float>(ch.end[i]))
               : d.i32(static_cast<int32_t>(ch.end[i]));
    for (std::size_t i = 0; i < ch.control1.size(); ++i)
        kFloat ? d.f32(static_cast<float>(ch.control1[i]))
               : d.i32(static_cast<int32_t>(ch.control1[i]));
    for (std::size_t i = 0; i < ch.control2.size(); ++i)
        kFloat ? d.f32(static_cast<float>(ch.control2[i]))
               : d.i32(static_cast<int32_t>(ch.control2[i]));
    for (std::size_t i = 0; i < ch.current.size(); ++i)
        kFloat ? d.f32(static_cast<float>(ch.current[i]))
               : d.i32(static_cast<int32_t>(ch.current[i]));
    digest_timer(d, "interp.timer", ch.timer);
    d.i32(ch.duration);
    d.i32(ch.mode);
}

inline void digest_interpolators(DigestSink& d, const th13::AnmInterpolators& it) {
    digest_interpolation(d, "it.position", it.position);
    digest_interpolation(d, "it.color", it.color);
    digest_interpolation(d, "it.alpha", it.alpha);
    digest_interpolation(d, "it.rotation", it.rotation);
    digest_interpolation(d, "it.angle", it.angle);
    digest_interpolation(d, "it.scale", it.scale);
    digest_interpolation(d, "it.secondary_scale", it.secondary_scale);
    digest_interpolation(d, "it.uv_scale", it.uv_scale);
    digest_interpolation(d, "it.secondary_color", it.secondary_color);
    digest_interpolation(d, "it.secondary_alpha", it.secondary_alpha);
    digest_interpolation(d, "it.uv_x", it.uv_x);
    digest_interpolation(d, "it.uv_y", it.uv_y);
}

inline void digest_vm(DigestSink& d, const th13::AnmVm& vm) {
    digest_timer(d, "vm.timer", vm.timer);
    digest_timer(d, "vm.age", vm.age);
    d.tag("vm.head");
    d.i32(vm.instruction_offset);
    d.i32(vm.source_script);
    d.u32(vm.visible ? 1u : 0u);
    d.i32(vm.pending_interrupt);
    d.f32(vm.slowdown);
    d.u32(vm.ordering);
    d.u32(vm.presentation_motion ? 1u : 0u);
    d.u32(vm.presentation_part);
    d.u64(vm.error.size());
    digest_variables(d, vm.variables);
    digest_visual(d, vm.visual);
    digest_interpolators(d, vm.interpolators);
}

inline uint64_t digest_registry(th13::AnmRegistry& registry, const char* tag) {
    DigestSink d;
    d.tag(tag);
    d.tag("reg.meta");
    d.u32(registry.count());
    d.u32(registry.generation_counter());
    d.u32(registry.ordered_count(false));
    d.u32(registry.ordered_count(true));
    for (int alternate = 0; alternate < 2; ++alternate) {
        const th13::u32 chain = registry.ordered_count(alternate != 0);
        for (th13::u32 i = 0; i < chain; ++i) {
            d.tag(alternate == 0 ? "chain.main" : "chain.alt");
            d.u32(i);
            const th13::u32 handle = registry.ordered_handle(alternate != 0, i);
            d.u32(handle);
            const th13::AnmVm* vm = registry.find(handle);
            d.u32(vm != nullptr ? 1u : 0u);
            if (vm) digest_vm(d, *vm);
        }
    }
    for (th13::u32 layer = 0; layer < 33; ++layer) {
        const std::vector<th13::AnmVm*>& bucket = registry.layer(layer);
        d.tag("layer");
        d.u32(layer);
        d.u32(static_cast<th13::u32>(bucket.size()));
        for (const th13::AnmVm* vm : bucket) d.u32(vm ? registry.handle(*vm) : 0u);
    }
    return d.value();
}

inline uint64_t digest_loading_caption(th13::sdl::ApplicationState& app) {
    return digest_registry(app.loading_animations.registry, "loading.registry");
}

inline int battle_frame_of(const th13::sdl::ApplicationState& app) {
    return app.progress.stage_frame - 1;
}

inline const th13::AnmVm* find_fore_vm(const th13::sdl::ApplicationState& app,
                                       int script) {
    for (const th13::AnmVm* vm : app.loading_animations.registry.layer(31))
        if (vm && vm->source_script == script) return vm;
    return nullptr;
}

struct FrameDigest {
    uint64_t projection = 0;
    uint64_t rng = 0;
    uint64_t audio = 0;
    uint64_t files = 0;
    uint64_t animations = 0;
    uint64_t loading = 0;
    uint64_t session = 0;
    uint64_t recorder = 0;

    bool operator==(const FrameDigest& other) const {
        return projection == other.projection && rng == other.rng &&
               audio == other.audio && files == other.files &&
               animations == other.animations && loading == other.loading &&
               session == other.session && recorder == other.recorder;
    }
    bool operator!=(const FrameDigest& other) const { return !(*this == other); }

    std::string diff_against(const FrameDigest& other) const {
        std::string out;
        const auto add = [&](const char* name, bool differs) {
            if (differs) {
                if (!out.empty()) out += ',';
                out += name;
            }
        };
        add("projection", projection != other.projection);
        add("rng", rng != other.rng);
        add("audio", audio != other.audio);
        add("files", files != other.files);
        add("animations", animations != other.animations);
        add("loading", loading != other.loading);
        add("session", session != other.session);
        add("recorder", recorder != other.recorder);
        return out;
    }
};

inline FrameDigest capture(th13::sdl::ApplicationState& app) {
    FrameDigest out;
    {
        DigestSink d;
        d.tag("projection");
        const th13::i32* items = app.projection();
        for (int i = 0; i < 16; ++i) d.i32(items[i]);
        out.projection = d.value();
    }
    {
        DigestSink d;
        d.tag("rng");
        d.u32(app.game_random.seed());
        d.u32(app.game_random.index());
        d.u32(app.visual.seed());
        d.u32(app.visual.index());
        out.rng = d.value();
    }
    {
        DigestSink d;
        d.tag("audio");
        const th13::sdl::AudioDevice::Statistics s = app.audio_device.statistics();
        d.u64(s.mixed_frames);
        d.u64(s.nonzero_samples);
        d.u64(s.requests);
        d.u64(s.started);
        d.u64(s.music_loops);
        d.u64(s.submitted_frames);
        d.u64(s.dropped_requests);
        d.u64(s.stopped);
        d.u32(s.music_cursor);
        d.u32(s.queued_frames);
        d.u32(s.sample_rate);
        d.u32(s.active_voices);
        d.u32(s.pending_voices);
        d.i32(s.track);
        d.u32(s.ready ? 1u : 0u);
        d.u32(s.device ? 1u : 0u);
        d.u32(s.suspended ? 1u : 0u);
        d.u32(s.music_paused ? 1u : 0u);
        d.i32(app.audio_device.music_volume);
        d.i32(app.audio_device.effect_volume);
        out.audio = d.value();
    }
    {
        DigestSink d;
        d.tag("files");
        for (const auto& records : app.files.ordinary_records) {
            d.i32(records.play_count);
            d.i32(records.total_frames);
            d.u32(records.history_complete ? 1u : 0u);
        }
        d.u32(app.ordinary_records_dirty ? 1u : 0u);
        out.files = d.value();
    }
    out.animations = digest_registry(app.animations.registry, "animations.registry");
    out.loading = digest_registry(app.loading_animations.registry, "loading.registry");
    {
        DigestSink d;
        d.tag("session");
        d.u32(app.replay_consumed);
        d.u32(app.replay_finished ? 1u : 0u);
        d.u32(app.environment.paused ? 1u : 0u);
        d.i32(app.pending_destination);
        d.u32(app.pending_load);
        d.u32(app.stage_transition_loading ? 1u : 0u);
        d.u32(app.stage_transition_active ? 1u : 0u);
        d.i32(app.progress.stage_frame);
        d.u32(app.frames);
        d.i32(app.run ? app.run->battle.touch.mode : -1);
        out.session = d.value();
    }
    {
        DigestSink d;
        d.tag("recorder");
        if (app.recorder.ready()) {
            th13::ReplaySaveMeta pinned;
            pinned.date = 1234567;
            pinned.slow_numerator = 0.0;
            pinned.slow_denominator = 1099511627776.0;
            pinned.stage_field = 8;
            pinned.append_eof = false;
            pinned.version = "1.00c";
            std::vector<th13::u8> bytes;
            if (app.recorder.encode(bytes, "t2-digest", app.selection_score.score, pinned)) {
                d.bytes(bytes.data(), bytes.size());
            } else {
                d.tag("encode-error");
            }
        } else {
            d.tag("not-ready");
        }
        out.recorder = d.value();
    }
    return out;
}

}
