#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>

namespace th128::enemy {

inline constexpr std::size_t AnimationSlotCount = 16;
inline constexpr float UnchangedPolar = -999999.0f;
inline constexpr float Pi = 3.1415927410125732f;
inline constexpr float Tau = 6.2831854820251465f;

struct OriginalIdentityState {
    std::uint32_t next=0,lastCreated=0;
    std::uint32_t construct() noexcept {
        lastCreated=next;
        ++next;
        if(next==0)++next;
        return lastCreated;
    }
    std::int32_t integer() const noexcept {return std::bit_cast<std::int32_t>(lastCreated);}
    float floating() const noexcept {return float(integer());}
};

enum class VisibilityEffect { None, Hide, Show };

struct HitFeedbackInput {
    std::int32_t soundOverride=-1,counter=0,lifeWord500c=0,phaseClock=0;
    std::uint32_t enemyFlags=0,auxiliaryFlags=0;
    std::optional<std::uint32_t> phaseFlags;
    bool primaryPresent=false;
};
struct HitFeedbackPlan {
    std::int32_t counter=0;
    std::uint32_t primaryFlags=0,primaryColor=0;
    std::optional<std::int32_t> sound;
};
std::optional<HitFeedbackPlan> planHitFeedback(const HitFeedbackInput& input,
    std::uint32_t primaryFlags,std::uint32_t primaryColor) noexcept;

struct FlagsChange {
    std::uint32_t flags = 0;
    VisibilityEffect visibility = VisibilityEffect::None;
};

FlagsChange setFlags(std::uint32_t flags, std::uint32_t mask) noexcept;
FlagsChange clearFlags(std::uint32_t flags, std::uint32_t mask) noexcept;

struct BossSlots {
    std::array<std::uint32_t,8> owners{};
    std::uint32_t flags=0;
};
bool bossWaitPending(const BossSlots& manager) noexcept;

struct BossGaugeState {
    struct Segment {float ratio=0;std::uint32_t color=0;bool configured=false;};
    std::array<std::array<Segment,4>,8> values{};
};
bool setBossGauge(BossGaugeState& state,int boss,int slot,float life,std::int32_t maximum,std::uint32_t color) noexcept;
void clearUnownedBossGauges(BossGaugeState& state,const BossSlots& owners) noexcept;

bool setBossSlot(BossSlots& manager,std::uint32_t& flags,std::int32_t& retainedSlot,
                 std::uint32_t owner,std::int32_t slot) noexcept;
void setLife(std::array<std::uint32_t,6>& words,std::uint32_t& flags,std::int32_t life) noexcept;

struct Bounds {
    float width = 0;
    float height = 0;
};

bool setBounds(Bounds& bounds, float width, float height) noexcept;

struct Position {
    float x = 0;
    float y = 0;
    float z = 0;
};

struct Polar {
    float angle = 0;
    float speed = 0;
    std::uint32_t modeFlags = 0;
    Position origin{};
    float radius = 0;
    float radialSpeed = 0;
};

struct PolarInterpolation {
    float startAngle = 0;
    float startSpeed = 0;
    float targetAngle = 0;
    float targetSpeed = 0;
    std::int32_t duration = 0;
    std::int32_t elapsed = 0;
    float elapsedFloat = 0;
    std::int32_t mode=0,previous=-1;
    bool circular=false;
    float startRadius=0,startRadialSpeed=0,targetRadius=0,targetRadialSpeed=0;
};

bool setPolar(Polar& polar, float angle, float speed, std::uint32_t enemyFlags = 0) noexcept;
bool beginPolarInterpolation(Polar& polar, PolarInterpolation& interpolation,
                             std::int32_t duration, std::int32_t mode,
                             float targetAngle, float targetSpeed,
                             std::uint32_t enemyFlags = 0) noexcept;
bool stepPolarInterpolation(Polar& polar, PolarInterpolation& interpolation) noexcept;
bool beginCircularMotion(Position& position,Polar& polar,float angle,float angularSpeed,float radius,float radialSpeed) noexcept;
bool beginCircularInterpolation(Polar& polar,PolarInterpolation& interpolation,std::int32_t duration,std::int32_t mode,
                                float angularSpeed,float radius,float radialSpeed) noexcept;
bool advanceCircularPosition(Position& position,Position& velocity,Polar& polar) noexcept;

struct AnimationState {
    std::int32_t bank = 0;
    std::int32_t layerOffset = 0;
    std::int32_t baseScript = 0;
    std::int32_t motionSelector = 0;
};

struct AnimationRequest {
    std::int32_t bank = 0;
    std::int32_t slot = 0;
    std::int32_t script = 0;
    std::int32_t layer = 7;
    VisibilityEffect visibility = VisibilityEffect::None;
    bool primary = false;
};

void setAnimationBank(AnimationState& animation, std::int32_t bank) noexcept;
std::optional<AnimationRequest> animationSlot(const AnimationState& animation,
                                              std::int32_t slot, std::int32_t script,
                                              std::uint32_t enemyFlags = 0) noexcept;

struct MovementArea {float centerX=0,centerY=0,width=0,height=0;};
std::optional<MovementArea> prepareMovementArea(float centerX,float centerY,float width,float height) noexcept;
bool combinePositionWithArea(Position& combined,Position& velocity,Position& absolute,const Position& relative,
                             const MovementArea& area) noexcept;

struct PositionInterpolation {
    Position start,target;
    std::int32_t duration=0,mode=0,previous=0,elapsed=0;
    float elapsedFloat=0;
};
bool beginPositionInterpolation(PositionInterpolation& interpolation,const Position& position,
                                std::int32_t duration,std::int32_t mode,float x,float y) noexcept;
std::optional<Position> stepPositionInterpolation(PositionInterpolation& interpolation) noexcept;
bool advanceInterpolatedPosition(Position& position,Position& velocity,PositionInterpolation& interpolation) noexcept;

bool advancePolarPosition(Position& position, Position& velocity, const Polar& polar) noexcept;
bool combinePosition(Position& combined, Position& velocity, const Position& absolute,
                     const Position& relative, std::uint32_t enemyFlags = 0) noexcept;

struct SpawnParent {
    Position combinedPosition;
    std::uint32_t flags = 0;
    std::array<std::uint32_t, 12> inheritedWords{};
    std::uint32_t contextTag = 0;
};

struct SpawnRequest {
    std::string_view subroutine;
    Position position;
    std::int32_t life = 0;
    std::int32_t score = 0;
    std::int32_t drop = 0;
    std::uint32_t mode = 0;
    std::array<std::uint32_t, 12> inheritedWords{};
    std::uint32_t contextTag = 0;
};

std::optional<SpawnRequest> prepareSpawn(std::uint16_t opcode, std::string_view subroutine,
                                        const SpawnParent& parent, float x, float y,
                                        std::int32_t life, std::int32_t score,
                                        std::int32_t drop) noexcept;

}
