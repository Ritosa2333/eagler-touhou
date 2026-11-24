#pragma once
#include "animation/AnmScene.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace th128::routechoice {

struct Input {
    std::uint32_t pressed = 0;
};

struct StartContext {
    int stage = 0;
    std::int32_t message78 = 0;
    std::int32_t message80 = 0;
};

enum class Phase { Idle, Intro, Select, Confirm, Releasing, Done, Failed };

struct Status {
    Phase phase = Phase::Idle;
    int stage = 0;
    std::uint32_t handle = 0;
    std::int32_t message70 = 0;
    std::int32_t message74 = 0;
    std::int32_t message78 = 0;
    std::int32_t message7c = 0;
    std::int32_t message80 = 0;
    std::uint32_t lastInterrupt = 0;
    std::size_t interruptCount = 0;
    std::uint32_t lastSound = 0;
    std::size_t soundCount = 0;
    bool confirmed = false;
};

struct Snapshot {
    std::uint32_t id = 0, parent = 0;
    anm::State state;
    std::optional<anm::SpriteInfo> sprite;
    float x = 0, y = 0, z = 0;
};

class Host {
public:
    static constexpr int BankId = 5;
    static constexpr std::uint32_t Up = 0x10u;
    static constexpr std::uint32_t Down = 0x20u;
    static constexpr std::uint32_t Confirm = 0x80201u;

    bool load(std::span<const std::uint8_t> bytes, std::string& error);
    bool start(const StartContext& context, std::string& error);
    bool tick(Input input, std::string& error);
    bool destroy(std::string& error);
    const std::vector<Snapshot>& snapshots() const { return snapshots_; }
    std::optional<std::int32_t> result() const { return result_; }
    const Status& status() const { return status_; }

private:
    static constexpr std::uint32_t RootKey = 1;
    anm::Scene scene_;
    std::vector<Snapshot> snapshots_;
    Status status_;
    std::optional<std::int32_t> result_;
    std::string failure_;
    bool loaded_ = false, started_ = false, executing_ = false;
    void runtime();
    void refresh();
    bool fail(std::string reason, std::string& error);
    bool checked(std::string& error);
    bool selectChild(int script, int sprite, std::string& error);
    bool enterSelect(std::string& error);
    void recordInterrupt(int code);
};

}
