#pragma once

#include "Rng.hpp"

#include <array>
#include <cstdint>
#include <optional>

namespace th13 {

class YoumuBomb {
public:
    static constexpr int kSlots = 60;
    static constexpr int kActiveFrames = 261;
    static constexpr float kRectLength = 96.0f;
    static constexpr float kRectGrowUnit = 128.0f;
    static constexpr float kDamagePerRect = 30.0f;
    static constexpr float kCenterOffset = 256.0f;
    static constexpr float kLaneJitter = 48.0f;
    static constexpr float kLanes[11] = {224.0f, 192.0f, 320.0f, 224.0f, 64.0f, 80.0f,
                                         384.0f, 320.0f, 96.0f,  128.0f, 224.0f};

    struct Spawn {
        int slot;
        int script;
        float y;
    };

    struct Rect {
        float cx, cy;
        float w, h;
        float angle;
    };

    void reset();

    std::optional<Spawn> step(int age, GameRandom& rng);

    void set_vm(int slot, bool alive, float rotation_z, float rate = 1.0f);

    bool active(int slot) const;
    bool has_anm(int slot) const;
    float scale(int slot) const;
    float rect_scale(int slot) const;
    float lane_y(int slot) const;
    float angle(int slot) const;
    int script(int slot) const;
    int cursor() const;
    template <class Visitor> void visit_state(Visitor&& visitor) const {
        visitor(cursor_, cooldown_, slots_left_);
        for (const auto& slot : slots_)
            visitor(slot.has_anm, slot.alive, slot.odd, slot.script, slot.counter, slot.y, slot.angle, slot.scale,
                    slot.rect_scale);
    }

    Rect rect(int slot) const;

    static bool rect_covers(const Rect& rc, float px, float py, float r);

    int cover_count(float px, float py, float r) const;

    bool clears_bullet(float px, float py, float r) const;

private:
    struct Slot {
        bool has_anm = false;
        bool alive = false;
        bool odd = false;
        int script = 0;
        int counter = 0;
        float y = 0.0f;
        float angle = 0.0f;
        float scale = 0.0f;
        float rect_scale = 0.0f;
    };
    std::array<Slot, kSlots> slots_{};
    int cursor_ = 0;
    int cooldown_ = 0;
    int slots_left_ = 25;
};

}
