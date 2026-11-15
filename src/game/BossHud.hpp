#pragma once
#include "animation/AnmScene.hpp"
#include "EnemyRules.hpp"
#include <span>

namespace th128::hud {
struct TimerNotice {int interrupt=0,sound=-1;};
TimerNotice bossTimerNotice(int seconds,int previous,bool present,bool message,bool hidden) noexcept;
std::optional<std::array<float,2>> bossFill(int life,int maximum,float previous) noexcept;
std::array<float,3> bossMarkerPosition(float angle,float x,float y,float z) noexcept;
struct BossInput {
    struct Slot {std::uint32_t owner=0;int life=0,maximum=0;std::uint32_t flags=0;int protection=0;float x=0,y=0,z=0;};
    std::array<Slot,3> slots{};
    std::array<std::array<enemy::BossGaugeState::Segment,4>,3> markers{};
    std::array<int,2> countdown{-1,0};
    bool message=false,hidden=false;
    float playerX=0,playerY=0;
};
struct BossHudState {
    std::array<float,3> fills{},targets{};
    std::array<bool,3> visible{};
    int seconds=-1,hundredths=0,previousSeconds=-1,sound=-1;
    bool timerVisible=false;
    std::uint32_t timerColor=0xffffffffu;
    float timerScale=1;
};
struct BossHudSnapshot {std::uint32_t id=0;anm::State state;anm::SpriteInfo sprite;float x=0,y=0,z=0;};
struct TimerGlyph {int sprite=0;float x=0,y=0,scale=1;std::uint32_t color=0xffffffffu;};
class BossHud {
public:
    bool load(std::span<const std::uint8_t> bytes,std::string& error);
    bool boot(std::string& error);
    bool tick(const BossInput& input,std::string& error);
    const BossHudState& state() const;
    std::vector<BossHudSnapshot> snapshots() const;
    std::vector<TimerGlyph> glyphs() const;
private:
    anm::Scene scene_;
    BossHudState state_;
    std::array<bool,3> dimmed_{};
    bool booted_=false,executing_=false;
    std::string failure_;
    bool fail(std::string reason,std::string& error);
    bool checked(std::string& error);
};
}
