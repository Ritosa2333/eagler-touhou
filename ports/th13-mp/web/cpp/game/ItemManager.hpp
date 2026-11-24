#pragma once

#include "SessionState.hpp"
#include "StageSim.hpp"
#include "Types.hpp"

#include <array>
#include <functional>
#include <string>

namespace th13 {

namespace multiplayer {
class WorldState;
}

void set_item_trace(bool enabled);
void set_item_trace_frame(i32 frame);
void item_trace_note(const char* tag, i32 max_point, i32 score);
std::string take_item_trace();

class GameRandom;

enum ItemType : i32 {
    ItemPower = 1,
    ItemPoint = 2,
    ItemBigPower = 3,
    ItemLifePiece = 4,
    ItemBombPiece = 5,
    ItemExtraLife = 6,
    ItemBomb = 7,
    ItemBigPowerAlt = 8,
    ItemSpiritDrop = 9,
    ItemSpiritKind0 = 10,
    ItemSpiritKind1 = 11,
    ItemSpiritKind2 = 12,
    ItemSpiritKind3 = 13,
    ItemSpiritKind4 = 14,
};

struct Item {
    i32 state = 0;
    i32 type = 0;
    i32 type2 = 0;
    float x = 0, y = 0, z = 0;
    float vx = 0, vy = 0;
    float vz = 0;
    float homing_speed = 0;
    i32 sprite = 0;
    i32 anm_frame = 0;
    i32 anm_frame_prev = -1;
    float anm_time = 0;
    u32 flags = 0;
    bool alive = false;
    i32 target_seat = -1;
    i32 tracking_seat = -1;
};

struct Spirit {
    i32 state = 0;
    i32 kind = 0;
    u32 serial = 0;
    float speed = 0;
    float move_speed = 0;
    float x = 0, y = 0, z = 0;
    float angle = 0;
    i32 anm_frame = 0;
    i32 anm_frame_prev = 0;
    float anm_time = 0;
    i32 frame = -1;
    Timer lifetime_clock;
    u32 flags = 0;
    bool fade_requested = false;
    bool alive = false;
    i32 tracking_seat = -1;
};

class ItemManager;

struct ItemCollector {
    bool boss_present = false;
    float boss_x = 0, boss_y = 0;
    float x = 0, y = 0;
    i32 state = 1;
    bool focus = false;
    bool dialog = false;
    float homing_speed = 4.0f;
    std::array<float, 4> box{{0, 0, 0, 0}};
    std::array<float, 4> box_auto{{0, 0, 0, 0}};
    std::array<float, 4> box_auto_focus{{0, 0, 0, 0}};
    SessionState* progress = nullptr;
    HudPlayerState* player = nullptr;
    HudScoreState* score = nullptr;
    i32 seat = 0;
    bool eligible = true;
    i32 character = -1;
};

struct ItemEvent {
    enum class Kind : i32 {
        Collected = 0,
        ScorePopup = 1,
        PowerLevelUp = 2,
        FullPower = 3,
        Extend = 4,
        SpiritGained = 5,
        BombGained = 6,
        TranceGained = 7,
    };
    Kind kind = Kind::Collected;
    i32 value = 0;
    float x = 0, y = 0;
    u32 color = 0xffffffffu;
};

class ItemManager final : public StageItemHost {
public:
    std::function<bool(u32 slot, u32 serial)> spirit_animation_alive;
    std::function<bool(i32 script, float x, float y, float z)> spirit_pickup_effect;
    std::function<bool(u32 slot, u32 serial, i32 kind, u32 flags, float x, float y, float z)> spirit_spawn_animation;
    std::function<void()> option_power_changed;
    std::function<void(i32 seat, i32 stock, i32 pieces)> life_changed, bombs_changed;
    static constexpr i32 kItemCapacity = 2648;
    static constexpr i32 kSpiritCapacity = 256;

    explicit ItemManager(GameRandom* shared_random = nullptr) : random_(shared_random) {
        for (i32 i = 0; i < 600; ++i)
            ordinary_free_.push_back(i);
        for (i32 i = 600; i < kItemCapacity; ++i)
            cancel_free_.push_back(i);
    }

    void reset_items();
    bool spawn_targeted_item(float x, float y, i32 type, i32 target_seat);

    void bind_random(GameRandom& random) { random_ = &random; }

    void bind_session(const SessionState& session) { session_ = &session; }
    std::function<void(i32, float)> spirit_birth_sound;

    void spawn_item(float x, float y, float z, i32 kind, float angle, float speed) override;

    void spawn_value_item(i32 kind, float x, float y, float z, float base_speed) override;
    void spawn_value_item(i32 kind, float x, float y, float z, float base_speed, bool sound) override;

    void on_et_clear(float x, float y, float z) override;

    void spawn_item(const float pos[3], i32 type, float angle, float speed);

    void update(const ItemCollector& player);
    void update_seats(const ItemCollector* seats, i32 seat_count);

    void update_spirits(const ItemCollector& player);
    void update_spirits_seats(const ItemCollector* seats, i32 seat_count);
    void set_spirit_trance(bool active);

    std::vector<ItemEvent>& events() { return events_; }
    void clear_events() { events_.clear(); }

    std::array<Item, kItemCapacity>& items() { return items_; }
    std::array<Spirit, kSpiritCapacity>& spirits() { return spirits_; }

    bool challenge = false;
    bool legacy_pickup_rewards = false;
    bool collect_power_small(i32 amount = 1);
    bool collect_power_big(i32 amount);
    void collect_life_piece(i32 amount = 1);
    void collect_bomb_piece(i32 amount);
    void collect_extra_life();
    void collect_bomb();
    void collect_trance(i32 amount);
    i32 point_value(float player_y, bool item_homing) const;

private:
    void home_step(Item& item, const ItemCollector& player);
    void collect_check(Item& item, const ItemCollector& player, const ItemCollector* seats, i32 seat_count);
    struct ResourceGrant {
        i32 life_pieces = 0, bomb_pieces = 0;
        i32 lives = 0, bombs = 0;
        i32 bonus_bomb_piece_seat = -1;
    };
    void grant_resources(const ResourceGrant& grant, const ItemCollector* seats, i32 seat_count);
    static const ItemCollector& pick_collector(const ItemCollector* seats, i32 seat_count, float x, float y,
                                               const ItemCollector& fallback);
    static bool is_power_item(i32 type) { return type == ItemPower || type == ItemBigPower || type == ItemBigPowerAlt; }
    static i32 power_receiver(const ItemCollector* seats, i32 seat_count, float x, float y);
    i32 redirected_seat_ = -1;
    void push_level_up(const Item& item);
    float angle_to_player(const Spirit& s, const ItemCollector& player);
    void collect_spirit(Spirit& s, const ItemCollector& player, const ItemCollector* seats, i32 seat_count);
    void add_max_point(float displayed_increment);

    GameRandom* random_ = nullptr;
    const SessionState* session_ = nullptr;
    std::array<Item, kItemCapacity> items_{};
    std::array<Spirit, kSpiritCapacity> spirits_{};
    std::vector<ItemEvent> events_;
    ItemCollector last_player_{};
    u32 spirit_serial_ = 0;
    i32 spirit_overflow_ = 0;
    i32 spirit_window_ = 0;
    Timer spirit_window_clock_;
    std::vector<i32> ordinary_free_, cancel_free_;
    void retire_item(Item& item);
    i32 cancel_total_ = 0;
    i32 cancel_wave_ = 0;
    i32 point_items_ = 0;

    friend class multiplayer::WorldState;
};

}
