#pragma once
#include "Geometry526.hpp"
#include "animation/AnmScene.hpp"
#include <map>
#include <span>

namespace th128::geometry526 {
struct MeshSnapshot {
    std::uint32_t id=0,owner=0,column=0;
    anm::State state;
    anm::SpriteInfo sprite;
    std::vector<Vertex> vertices;
};
struct MainSnapshot {anm::State state;anm::SpriteInfo sprite;};
class Host {
public:
    static constexpr int BankId=6;
    bool load(std::span<const std::uint8_t> bytes,std::string& error);
    bool create(std::uint32_t owner,float radius,std::uint32_t color,std::string& error);
    bool update(std::uint32_t owner,const std::array<float,3>& position,float delta,std::string& error);
    bool tickPrimary(std::string& error);
    bool removeOwner(std::uint32_t owner,std::string& error);
    std::optional<std::vector<MeshSnapshot>> snapshots(std::string& error) const;
    std::optional<MainSnapshot> mainSnapshot(std::string& error) const;
    std::optional<EffectState> effectState(std::uint32_t owner) const;
    std::size_t effectCount() const {return effects_.size();}
    std::size_t nodeCount() const {return scene_.nodes().size();}
private:
    struct Effect {
        EffectState state;
        std::optional<Grid> grid;
        std::array<std::uint64_t,16> rows{};
    };
    anm::Scene scene_;
    anm::VM main_;
    std::map<std::uint32_t,Effect> effects_;
    std::string failure_;
    bool loaded_=false,executing_=false;
    bool fail(std::string reason,std::string& error);
    bool ready(std::string& error) const;
    bool checked(std::string& error);
};
}
