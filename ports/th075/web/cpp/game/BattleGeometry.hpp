#pragma once
#include "Types.hpp"
namespace th075 {
struct CollisionRectangle {i32 left=0,top=0,right=0,bottom=0;};
struct CollisionBounds {
    CollisionRectangle intersection{};
    void include_pair(const CollisionRectangle&,const CollisionRectangle&);
};
struct FighterMotion {
    float x=0,y=0,velocityX=0,velocityY=0,accelerationX=0,accelerationY=0,pushX=0,opponentPush=0;
    i8 facing=1;i16 freeze=0,hitFreeze=0;
    i8 horizontal_limit(const FighterMotion& opponent)const;
    void advance_position(const FighterMotion& opponent);
    bool update_facing(const FighterMotion& opponent);
    void clear(){velocityX=velocityY=accelerationX=accelerationY=0;}
    void set_hit_freeze(i16 frames){if(frames>=0)hitFreeze=frames;}
};
CollisionRectangle transform_bounds(const CollisionRectangle&,const FighterMotion&);
bool overlaps(const CollisionRectangle&,const CollisionRectangle&);
class ReplayRandom {
public:
    u32 state=1;
    u16 next(){state=state*214013u+2531011u;return u16((state>>16)&32767);}
    i32 range(i32 low,i32 high);
};
}
