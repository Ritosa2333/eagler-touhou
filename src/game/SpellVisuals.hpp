#pragma once
#include "animation/AnmScene.hpp"
#include <span>
namespace th128::spell {
struct Visual {unsigned id=0;anm::State state;std::optional<anm::SpriteInfo> sprite;float x=0,y=0,z=0;};
class Visuals {
public:
 bool load(int bank,std::span<const std::uint8_t> bytes,std::string& error);
 bool start(std::array<float,3> position,std::string& error,std::array<bool,3> medals={});
 bool finish(std::string& error);
 bool retireFollowEffect(std::string& error);
 bool levelUp(int level,std::array<float,3> player,std::string& error);
 bool tick(std::array<float,3> position,bool active,std::string& error);
 std::vector<Visual> snapshots() const;
 std::optional<Visual> titlePose() const;
 std::vector<Visual> bonusGlyphs(std::uint32_t flags,std::uint32_t value) const;
private:
 anm::Scene scene_;
 std::array<float,3> position_{};
 std::string failure_;
 bool executing_=false;
 bool checked(std::string& error);
 bool pose(const anm::Node& node,Visual& output,unsigned depth=0) const;
};
}
