#include "BattleGeometry.hpp"
#include <algorithm>
#include <cmath>
namespace th075 {
void CollisionBounds::include_pair(const CollisionRectangle& a,const CollisionRectangle& b){
    intersection.left=std::max({intersection.left,a.left,b.left});intersection.top=std::max({intersection.top,a.top,b.top});
    intersection.right=std::min({intersection.right,a.right,b.right});intersection.bottom=std::min({intersection.bottom,a.bottom,b.bottom});
}
i8 FighterMotion::horizontal_limit(const FighterMotion& other)const{
    if(double(x)-40.0<=0.0||double(other.x)-double(x)>880.0)return 1;
    if(double(x)+40.0>=1280.0||double(x)-double(other.x)>880.0)return -1;
    return 0;
}
void FighterMotion::advance_position(const FighterMotion& other){
    if(freeze||other.hitFreeze)return;const auto limit=horizontal_limit(other);
    const double movement=(double(velocityX)+double(pushX))+double(other.opponentPush);
    if(movement*double(int(limit)*int(facing))>=0.0)x=float(movement*double(facing)+double(x));
    if(x<40.0f)x=40.0f;if(x>1240.0f)x=1240.0f;
    y=float(double(y)+double(velocityY));if(y<0.0f)y=0.0f;
}
bool FighterMotion::update_facing(const FighterMotion& other){const auto previous=facing;if(other.x>x)facing=1;else if(other.x<x)facing=-1;return previous!=facing;}
static i32 truncate(double value){
    if(!std::isfinite(value)||value>=2147483648.0||value<-2147483648.0)return std::numeric_limits<i32>::min();
    return static_cast<i32>(value);
}
CollisionRectangle transform_bounds(const CollisionRectangle& bounds,const FighterMotion& motion){
    CollisionRectangle result;
    if(motion.facing==1){result.left=truncate(std::ceil(double(motion.x))+bounds.left);result.right=truncate(std::ceil(double(motion.x))+bounds.right);}
    else{result.left=truncate(std::ceil(double(motion.x))-bounds.right);result.right=truncate(std::ceil(double(motion.x))-bounds.left);}
    result.top=truncate(std::ceil(double(motion.y))-bounds.bottom);result.bottom=truncate(std::ceil(double(motion.y))-bounds.top);return result;
}
bool overlaps(const CollisionRectangle& a,const CollisionRectangle& b){return ((u32(b.left)-u32(a.right))&(u32(a.left)-u32(b.right))&(u32(b.top)-u32(a.bottom))&(u32(a.top)-u32(b.bottom))&0x80000000u)!=0;}
i32 ReplayRandom::range(i32 low,i32 high){const u32 delta=u32(high)-u32(low),product=delta*u32(next());i32 signedProduct;std::memcpy(&signedProduct,&product,4);return increment_wrapped(low,signedProduct/32768);}
}

