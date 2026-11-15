#pragma once
#include "IceBirthRules.hpp"
#include "IceMaskRules.hpp"
#include "BulletManager.hpp"
#include "BulletVisuals.hpp"
#include "animation/AnmScene.hpp"
#include <list>
#include <span>

namespace th128::ice {
struct Completion {int power=0;std::uint32_t pixels=0;float percent=0;};
struct PerfectFreezeResult {std::uint32_t frozen=0,skipped=0,pixels=0;bool completed=false;};
struct Draw {std::uint32_t id=0;int bank=36;anm::State state;anm::SpriteInfo sprite;float x=0,y=0,z=0;};
struct Statistics {int busy=0,power=0,elapsed=0;std::uint32_t objects=0,converted=0,shattered=0,pixels=0,completions=0;};
struct Services {
    std::function<std::uint32_t()> random;
    std::function<std::uint32_t()> displayRandom;
    std::function<const bullet::BulletVisuals::Visual*(std::uint32_t)> visual;
    std::function<bool(const bullet::Birth&,std::string&)> consume;
    std::function<bool(int,float,std::string&)> sound;
    std::function<bool(Position,float,std::string&)> damage;
    std::function<bool(Position,std::string&)> expire;
    std::function<bool(Completion,std::string&)> complete;
    std::function<bool(Completion,std::string&)> perfectComplete;
};
class Host {
public:
    bool configure(std::span<const std::uint8_t> animation,std::span<const std::uint8_t> masks,
                   bullet::BulletVisuals& bullets,Services services,std::string& error);
    bool activate(int power,Position position,std::string& error);
    bool charge(int held,Position position,std::string& error);
    bool tick(std::span<const bullet::Birth> bullets,std::string& error);
    bool freezePerfect(std::span<const bullet::Birth> bullets,PerfectFreezeResult& result,std::string& error);
    bool tickAnimations(std::string& error);
    bool shatterPerfect(std::string& error);
    Statistics statistics() const;
    std::vector<IceObjectState> objects() const;
    std::vector<Draw> draws() const;
private:
    struct Slot {IceObjectState state;std::uint32_t bucket=0;std::optional<bullet::BulletVisuals::Visual> visual;std::list<std::uint32_t>::iterator globalLink;};
    std::vector<Slot> slots_=std::vector<Slot>(SlotCount);
    std::array<std::list<std::uint32_t>,80> buckets_;
    std::list<std::uint32_t> global_;
    std::array<std::int32_t,SlotCount> occupancy_{};
    std::array<std::uint8_t,IceMaskBytes> mask_{};
    mutable std::uint32_t cachedMaskPixels_=0;
    mutable bool maskPixelsDirty_=false;
    std::vector<std::uint8_t> bitmaps_,scratchMask_;
    anm::Scene scene_{anm::Scene::AllocationPolicy::Native4096PoolWithHeapFallback};
    bullet::BulletVisuals* visuals_=nullptr;
    Services services_;
    Statistics stats_;
    std::uint32_t cursor_=0,nextAnimation_=1;
    struct Activation {std::uint64_t animation=0;int elapsed=0;bool active=false;};
    std::array<Activation,2> activations_{};
    std::uint64_t charge_=0;
    bool configured_=false,executing_=false,perfectBatch_=false;
    std::string failure_;
    bool fail(std::string reason,std::string& error);
    bool checked(std::string& error);
    std::optional<std::uint32_t> birth(IceBirthInput input,std::string& error);
    bool bind(std::uint64_t key,int script,Position position,std::string& error);
    bool contact(const bullet::Birth& bullet,std::string& error) const;
    bool convert(const bullet::Birth& bullet,std::span<std::uint8_t> mask,std::string& error);
    bool shatter(std::uint64_t animation,std::string& error);
    bool shatterBatch(std::string& error);
    bool pose(const anm::Node& node,Draw& draw,std::size_t depth=0) const;
    bool scratchMask(IceMaskOperation operation,const Position& position,std::string& error);
};
}
