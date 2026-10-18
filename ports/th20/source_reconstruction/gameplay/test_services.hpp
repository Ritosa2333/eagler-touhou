#pragma once
#include "gameplay.hpp"
#include <array>
#include <vector>
#include <unordered_set>
#include <algorithm>

namespace gp=th20::source::gameplay;
namespace rt=th20::source::runtime;
struct RecordedServices;
struct RecordedOwner final:rt::CallbackOwner {
    RecordedServices* services;unsigned id;
    RecordedOwner(RecordedServices& env,unsigned value):services(&env),id(value) {}
    ~RecordedOwner() override;
    void disable_callbacks() override;
};
struct RecordedServices final:gp::Services {
    struct FakeDevice {std::uintptr_t* vtable;RecordedServices* owner;};
    std::array<std::uintptr_t,6> device_vtable{};
    FakeDevice fake_device{device_vtable.data(),this};
    rt::Worker loading_worker;
    std::uint32_t latch=1234,session_bits=0,color=0;
    std::int32_t mode_value=0,scene_value=0;
    bool change_selection=false,transition_changes_scene=false;
    bool ordering_valid=true;
    std::atomic<unsigned> load_calls{0};
    std::vector<int> events;
    std::array<rt::CallbackOwner*,9> owners{};
    static HRESULT WINAPI evict(void* raw) {
        auto& env=*static_cast<FakeDevice*>(raw)->owner;env.events.push_back(1);return S_OK;
    }
    RecordedServices() {
        device_vtable[5]=reinterpret_cast<std::uintptr_t>(evict);
        for(unsigned n=0;n<owners.size();++n) owners[n]=new RecordedOwner(*this,n);
    }
    ~RecordedServices() override {
        if(loading_worker.thread.joinable()) loading_worker.thread.join();
        std::unordered_set<rt::CallbackOwner*> remaining;
        for(auto* owner:owners) if(owner) remaining.insert(owner);
        for(auto* owner:remaining) delete owner;
    }
    std::uint32_t& input_latch() override {return latch;}
    IDirect3DDevice9& device() override {return *reinterpret_cast<IDirect3DDevice9*>(&fake_device);}
    rt::Worker& worker() override {return loading_worker;}
    std::uint32_t& session_flags() override {return session_bits;}
    std::int32_t session_mode() override {return mode_value;}
    std::int32_t scene() override {return scene_value;}
    rt::CallbackOwner* owner(gp::Owner which) override {return owners[static_cast<unsigned>(which)];}
    void preserve_background_as_secondary() override {owners[static_cast<unsigned>(gp::Owner::global_005c06a0)]=owners[static_cast<unsigned>(gp::Owner::global_005c069c)];}
    void load(gp::GameController& value) override {
        ordering_valid=ordering_valid && gp::controller==&value && (value.game_flags&4)!=0 && latch==0;
        ++load_calls;
    }
    void commit_progress_if_present() override {events.push_back(2);}
    void reset_clock_scale() override {events.push_back(3);}
    void clear_surface_callbacks() override {events.push_back(4);}
    void screen_transition(float x,float y) override {
        ordering_valid=ordering_valid && x==480.0f && y==392.0f;events.push_back(5);
        if(transition_changes_scene) scene_value=22;
    }
    bool stage_selection_changed() override {events.push_back(6);return change_selection;}
    void increment_continue_count() override {events.push_back(7);}
    void cleanup(gp::Cleanup operation) override {events.push_back(100+static_cast<int>(operation));}
    void remove_callback(th20::source::scheduler::Node*) override {events.push_back(8);}
    void stop_music() override {ordering_valid=ordering_valid && !gp::controller;events.push_back(9);}
    void clear_queued_music_name() override {events.push_back(10);}
    void stop_all_effects() override {events.push_back(11);}
    void set_final_clear_color(std::uint32_t value) override {color=value;events.push_back(12);}
};
inline RecordedOwner::~RecordedOwner() {
    services->events.push_back(200+static_cast<int>(id));
    for(auto& owner:services->owners) if(owner==this) owner=nullptr;
}
inline void RecordedOwner::disable_callbacks() {services->events.push_back(300+static_cast<int>(id));}
