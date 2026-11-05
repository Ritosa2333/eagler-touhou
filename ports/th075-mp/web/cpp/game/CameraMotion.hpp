#pragma once
#include "BattleGeometry.hpp"
#include "FighterCombat.hpp"
namespace th075 {
class CameraMotion {
public:
    float fixedY=0,targetSpan=0,y=300,shakeY=0,span=0,shakeX=0,smoothX=0,fixedX=0,biasX=0,targetX=0,fixedSpan=0,amplitude=0,angleY=0,targetY=0,x=0,smoothY=0,biasY=20,angleX=0;
    bool fixed=false;
    void reset_parameters();void initialize(const FighterMotion&,const FighterMotion&);void advance(const FighterMotion&,const FighterMotion&);
    void snap();void override_view(bool enabled,float horizontal,float vertical,float spanOffset){fixed=enabled;fixedX=horizontal;fixedY=vertical;fixedSpan=spanOffset;}
    void shake(float value){amplitude=value;}
    CombatView combat_view()const{return {x,y,span};}
};
}
