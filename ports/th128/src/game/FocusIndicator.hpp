#pragma once
#include "animation/AnmScene.hpp"
#include <string>
namespace th128::player {
struct FocusDraw {std::uint32_t id=0,parent=0;anm::State state;anm::SpriteInfo sprite;float x=0,y=0,z=0;};
class FocusIndicator {
public:
    bool configure(const anm::Registry& actualBulletBank,std::string& error);
    bool update(bool focused,float horizontal,float vertical,float depth,std::string& error);
    bool tick(std::string& error);
    std::vector<FocusDraw> draws() const;
    bool focused() const{return active_!=0;}
private:
    anm::Scene scene_;
    std::uint32_t active_=0,next_=1;
    bool configured_=false,executing_=false;
    std::string failure_;
    bool fail(std::string reason,std::string& error);
    bool checked(std::string& error);
    bool pose(const anm::Node& node,FocusDraw& draw,std::size_t depth=0) const;
};
}
