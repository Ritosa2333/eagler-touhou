#pragma once
#include "MessageHost.hpp"
#include "animation/AnmScene.hpp"
#include <optional>

namespace th128::message {
struct TextPresentation {bool visible=false;float alpha=0,sx=1,sy=1,offsetY=0;};
struct VisualSnapshot {std::uint32_t id=0;anm::State state;std::optional<anm::SpriteInfo> sprite;float x=0,y=0,z=0;};
class Visuals {
public:
    bool load(int bank,std::span<const std::uint8_t> bytes,std::string& error);
    bool event(const Event& event,std::string& error);
    bool tick(std::string& error);
    std::vector<VisualSnapshot> snapshots() const;
    std::array<TextPresentation,4> textPresentation() const;
private:
    anm::Scene scene_;
    std::string failure_;
    bool executing_=false;
    bool extraBossPortrait_=false;
    unsigned textLine_=0;
    std::optional<std::uint32_t> auxiliaryFront_;
    float auxiliaryExtent_=0;
    int textSpeaker_=0,textStyle_=0,rightFamily_=0;
    std::array<float,2> textPosition_{400,480};
    bool retireAuxiliary(std::string& error);
    bool constructAuxiliary(std::string& error);
    bool setAuxiliaryExtent(float extent,std::string& error);
    void runtime();
    bool checked(std::string& error);
    bool expression(int side,int value,std::string& error);
    bool pose(const anm::Node& node,VisualSnapshot& result,unsigned depth=0) const;
};
}
