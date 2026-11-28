#pragma once
#include "AnmRegistry.hpp"
#include <functional>
#include <unordered_map>
namespace th13 {
using AnmSpawnObserver = void (*)(const AnmVm&, i32, u32);
extern AnmSpawnObserver anm_spawn_observer;
class AnmManager final : public AnmObjectHost {
    struct Bank {
        AnmResource data;
        std::vector<AnmVm> templates;
    };
    std::unordered_map<i32, std::unique_ptr<Bank>> banks;
    std::vector<std::unique_ptr<Bank>> retired_banks;
    GameRandom& random;
    AnmEnvironment& environment;
    AnmVm* instantiate(AnmResource&, i32);
    AnmVm* instantiate_preserve_ip(AnmResource&, i32);
    bool initial_tick(AnmVm&);
    bool adopt_bank(i32, std::unique_ptr<Bank>);

public:
    struct CaptureRequest {
        const AnmResource* resource = nullptr;
        u32 texture = 0;
        i32 source[4] = {32, 16, 384, 448};
        i32 destination[4] = {};
    };
    AnmRegistry registry;
    std::string error;
    float rate = 1;
    using AnmObjectHost::spawn_child;
    AnmVm* create_instance(AnmResource& resource, i32 script) { return instantiate(resource, script); }
    bool prime_instance(AnmVm& vm) { return initial_tick(vm); }
    GameRandom& random_source() const noexcept { return random; }
    std::unordered_map<i32, std::string> resource_names;
    bool name_resource(i32 id, const std::string& name) {
        if (!resource(id) || name.empty() || name.size() > 1024) return false;
        resource_names[id] = name;
        return true;
    }
    std::function<bool(AnmVm&, i32)> effect;
    std::function<i32(AnmVm&, float)> effect_update;
    std::function<bool(AnmVm&, i32, float)> effect_interrupt;
    std::function<bool()> after_main_update;
    const void* after_main_update_owner = nullptr;
    AnmManager(GameRandom& random, AnmEnvironment& environment) : random(random), environment(environment) {}
    AnmEnvironment& scene_environment() noexcept { return environment; }
    void set_paused(bool paused) noexcept { environment.paused = paused; }
    bool load(i32 id, const u8*, u32 size);
    bool adopt(i32 id, AnmResource&& data);
    void unload(i32 id);
    void collect_resources();
    u32 resource_count() const noexcept { return banks.size(); }
    u32 retired_resource_count() const noexcept { return retired_banks.size(); }
    std::function<void(const AnmResource&)> resource_release;
    AnmResource* resource(i32 id) const noexcept;
    i32 resource_id(const AnmResource* value) const noexcept;
    bool sprite_fallback(i32 id) noexcept {
        auto* bank = resource(id);
        if (!bank) return false;
        environment.fallback_sprite_resource = bank;
        return true;
    }
    AnmVm* script_template(i32 id, u32 script) const noexcept;
    bool bind_template(AnmVm&, i32 id, i32 script);
    bool bind_template_preserve_ip(AnmVm&, i32 id, i32 script);
    u32 create(i32 id, i32 script, i32 layer = -1, u32 ordering = 2, const Vec3& translation = {}, float anm_arg = 0,
               bool game_coordinates = false);
    u32 create_preserve_ip(i32 id, i32 script, i32 layer = -1, u32 ordering = 2, const Vec3& translation = {},
                           float anm_arg = 0, bool game_coordinates = false);
    u32 create_overlay(i32 resource, i32 script) { return create(resource, script, -1, 4); }
    AnmVm* spawn_child(AnmVm&, i32 script, u32 ordering) override;
    AnmVm* spawn_detached(AnmVm&, i32 script) override;
    bool spawn_effect(AnmVm&, i32 script) override;
    bool release_tree(AnmVm& vm) override { return registry.destroy_tree(vm); }
    i32 update_effect(AnmVm&, float rate) override;
    bool interrupt_effect(AnmVm&, i32 label, float rate) override;
    bool rebind(u32& handle, i32 script);
    bool retire(u32& handle);
    void retire_resource(i32 id) noexcept {
        if (auto* bank = resource(id)) registry.retire_resource(bank);
    }
    bool pause(u32 handle, bool paused = true);
    bool interrupt(u32 handle, i32 label, bool immediate = false);
    bool update();
    bool update(bool alternate);
    int tick_instance(AnmVm&);
    bool queue_capture(const AnmVm& vm);
    std::vector<CaptureRequest> take_captures();
    void clear_captures() noexcept { captures_.clear(); }
    bool captures_pending() const noexcept { return !captures_.empty(); }

private:
    std::vector<CaptureRequest> captures_;
};
}
