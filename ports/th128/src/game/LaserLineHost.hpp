#pragma once
#include "animation/AnmLogic.hpp"
#include "LaserSelector3Rules.hpp"
#include "BulletManager.hpp"
#include "PlayerHitRules.hpp"
#include "LaserWholeLineEffectsOwner.hpp"
#include "DeferredLineRetirementRules.hpp"
#include "LaserCircleCancellationRules.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace th128::laser {
struct LineConfiguration {
    float initialLength=0,maximumLength=0,travelLimit=0,width=0;
};
struct LineDescriptor {
    bullet::Position source;
    float angle=0,speed=0;
    LineConfiguration configuration;
    bullet::Emitter emitter;
};
struct LineSnapshot {
    std::uint32_t serial=0,epoch=0,ownerEntity=0,age=0;
    bullet::Position source,velocity;
    float angle=0,length=0,width=0,speed=0,travel=0,maximumLength=0,travelLimit=0;
    int type=0,color=0,shotSound=0,transformSound=0;
    int grazeClock=0;
    int boundsGrace=30;
    std::uint32_t nativeState=2,guard5b4=0;
    std::uint8_t retirement7c=0;
};
struct LineDraw {
    std::uint32_t id=0;
    anm::State pose;
    anm::SpriteInfo sprite;
};
struct Selector3Draw {std::uint32_t serial=0,ownerEntity=0;selector3::Quad quad;};
struct EffectSnapshot {
    std::uint32_t id=0,owner=0,parent=0;
    anm::State state;
};
struct CollisionContext {
    bullet::Position player;
    float halfWidth=0,halfHeight=0;
    th128::player::hit::PlayerGate gate;
    th128::player::hit::HitControl control;
};
enum class CollisionKind {Miss,Graze,HitCallRequired};
std::optional<CollisionKind> classifyOverlap(const LineSnapshot& line,const CollisionContext& context);
class LineHost {
public:
    static constexpr std::size_t Capacity=256;
    using CollisionOwner=std::function<bool(LineSnapshot&,std::string&)>;
    using SoundOwner=std::function<bool(int,float,std::string&)>;
    using RandomOwner=std::function<std::uint32_t()>;
    using RegionEffectOwner=std::function<bool(bullet::Position,std::string&)>;
    bool configure(const void* bytes,std::size_t size,CollisionOwner collision,SoundOwner sound,
                   RandomOwner gameRandom,std::string& error);
    void reset();
    bool birth(std::uint32_t ownerEntity,const LineDescriptor& descriptor,const bullet::Profile& profile,std::string& error);
    bool tick(std::string& error);
    bool setRegionEffectOwner(RegionEffectOwner owner,std::string& error);
    bool cancelRegion(LineSnapshot& line,bullet::Position center,float width,float height,float angle,
                      bool honorGuard,std::string& error);
    bool cancelLive(std::string& error,bool honorGuard=true,bool clearGuard=false,bool skipRetired=false);
    bool cancelEntryRadius(bullet::Position center,float radius,bool honorGuard,std::string& error);
    bool tickEffects(std::string& error);
    std::optional<std::vector<LineDraw>> effectDraws(std::string& error) const;
    std::vector<EffectSnapshot> effectSnapshots() const;
    std::optional<std::vector<LineDraw>> draws(std::string& error) const;
    std::vector<LineSnapshot> snapshots() const;
    std::uint32_t actualCount() const;
    bool healthy() const;
    using Selector3Begin=std::function<bool(std::string&)>;
    using Selector3Input=std::function<std::optional<selector3::FrameInput>(std::string&)>;
    using Selector3Contact=std::function<bool(unsigned,std::string&)>;
    bool setSelector3Services(Selector3Begin begin,Selector3Input input,Selector3Contact contact,Selector3Begin end,std::string& error);
    bool birthSelector3(std::uint32_t owner,const selector3::Descriptor& descriptor,std::string& error);
    using PositionReader=std::function<bool(float&,float&)>;
    bool setSourceFromIdentifier(int identifier,PositionReader reader,std::string& error);
    using AngleReader=std::function<bool(float&)>;
    bool setAngleFromIdentifier(int identifier,AngleReader reader,std::string& error);
    bool setSelector3AngularVelocity(int identifier,float value,std::string& error);
    bool setSelector3FollowBoss(int identifier,int value,std::string& error);
    std::vector<selector3::State> selector3Snapshots() const;
    std::optional<std::vector<Selector3Draw>> selector3Draws(std::string& error) const;
    static bool validConfiguration(const LineConfiguration& configuration);
private:
    struct Line {
        LineSnapshot state;
        LineDescriptor descriptor;
        bullet::Profile profile;
        std::optional<bullet::Extension4> acceleration;
        std::optional<selector3::State> selector3State;
        std::array<anm::VM,3> visuals;
        std::string failure;
    };
    std::unique_ptr<anm::Registry> registry_;
    std::unique_ptr<cancel::LaserWholeLineEffectsOwner> wholeEffects_;
    std::vector<Line> lines_;
    CollisionOwner collision_;
    SoundOwner sound_;
    RandomOwner random_;
    RegionEffectOwner regionEffect_;
    Selector3Begin selector3Begin_,selector3End_;
    Selector3Input selector3Input_;
    Selector3Contact selector3Contact_;
    std::uint32_t epoch_=1,serial_=0x10000;
    bool executing_=false;
    std::string failure_;
    anm::Runtime runtime(Line& line,bool remapped);
    bool check(const Line& line,std::string& error) const;
    bool recordFailure(std::string& error,const char* fallback);
    bool validateDeferred(std::size_t index,std::string& error) const;
    bool tickVisual(Line& line,std::size_t visual,std::string& error);
    bool bindVisuals(Line& line,std::string& error);
    bool appendRemainder(const Line& parent,const cancel::CircleRemainderPlan& plan,std::string& error);
    static bool advancePhysical(LineSnapshot& state);
};
}
