#pragma once
#include "EffectPattern.hpp"
#include "FighterAnimation.hpp"
#include "SpriteRenderer.hpp"
#include <memory>
namespace th075 {
struct SharedEffect {
    const EffectPattern* pattern=nullptr;const ImageFrame* image=nullptr;AnimationCursor cursor;
    FighterMotion motion;bool alive=true,poseActive=false;u8 mode=0,alpha=255,red=255,green=255,blue=255,alternateDraw=0,shadow=0;
    std::array<i16,3> projection{};std::array<float,3> scale{1,1,1},rotation{},offset{};
    std::array<float,4> parameters{};i16 age=0,speedAngle=0,lifetime=0;
    void bind();void select(i16 action,i16 index=0,i16 elapsed=0);void frame(i16 index){select(cursor.action,index);}
    i16 advance();void destroy(){alive=false;image=nullptr;}
    void begin_pose(float x,float y,float z){poseActive=true;mode=0;alpha=red=green=blue=255;projection.fill(0);scale={1,1,1};rotation.fill(0);offset={x,y,z};}
    void polar(float degrees,float speed){motion.velocityX=float(double(angle_cos(degrees))*speed);motion.velocityY=float(-double(angle_sin(degrees))*speed);}
    SpritePose drawing_pose()const{SpritePose p;p.active=poseActive;p.flags=mode;p.color=(u32(alpha)<<24)|(u32(red)<<16)|(u32(green)<<8)|blue;p.projection=projection;p.scale=scale;p.rotation=rotation;p.offset=offset;return p;}
private:static void rebound(void*,AnimationCursor&);
};
struct SharedEffectRequest {i16 action=0;float x=0,y=0;i8 facing=1;u8 layer=1,alternateDraw=0,shadow=0;std::array<float,4> parameters{};};
struct SharedEffectEvents {virtual ~SharedEffectEvents()=default;virtual void spawn(i16,float,float,i8,u8,const std::array<float,4>&)=0;};
void advance_shared_effect(SharedEffect&,ReplayRandom&,SharedEffectEvents&);
class SharedEffects final:public SharedEffectEvents {
    const EffectPattern& pattern;ReplayRandom& random;std::array<std::deque<std::unique_ptr<SharedEffect>>,2> layers;
    friend class BattleSnapshot;
public:
    SharedEffects(const EffectPattern& pattern,ReplayRandom& random):pattern(pattern),random(random){}
    void spawn(i16,float,float,i8,u8,const std::array<float,4>& = {})override;
    SharedEffect* spawn(const SharedEffectRequest&);
    SharedEffect& get(u8 layer,u32 index){return *layers.at(layer).at(index);}
    void advance();void clear(){for(auto& layer:layers)layer.clear();}
    template<class Draw>void render(u8 layer,Draw&& draw){if(layer<layers.size())for(auto& effect:layers[layer])draw(*effect);}
    u32 size(u8 layer)const{return layer<layers.size()?u32(layers[layer].size()):0;}
};
}
