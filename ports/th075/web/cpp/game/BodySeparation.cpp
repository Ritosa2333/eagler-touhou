#include "BodySeparation.hpp"
namespace th075 {
namespace {
i32 signed_bits(u32 bits){i32 value;std::memcpy(&value,&bits,4);return value;}
i32 span(i32 right,i32 left){return signed_bits(u32(right)-u32(left)-1u);}
i32 sum(i32 first,i32 second){return signed_bits(u32(first)+u32(second));}
}
bool BodySeparation::advance(FighterMotion& first,const CollisionRectangle& firstBody,FighterMotion& second,const CollisionRectangle& secondBody){
    const float av=first.velocityX,bv=second.velocityX;const i8 al=first.horizontal_limit(second),bl=second.horizontal_limit(first);
    if((rightOccupant==0&&al!=-1)||(rightOccupant==1&&bl!=-1))rightOccupant=-1;
    if((leftOccupant==0&&al!=1)||(leftOccupant==1&&bl!=1))leftOccupant=-1;
    if(al==-1&&rightOccupant<0)rightOccupant=0;else if(bl==-1&&rightOccupant<0)rightOccupant=1;
    if(al==1&&leftOccupant<0)leftOccupant=0;else if(bl==1&&leftOccupant<0)leftOccupant=1;
    first.pushX=second.pushX=0;
    const auto a=transform_bounds(firstBody,first),b=transform_bounds(secondBody,second);if(!overlaps(a,b))return false;
    const double wa=double(first.facing)*double(av),wb=double(second.facing)*double(bv);
    const auto cancel=[&]{first.pushX=-first.velocityX;second.pushX=-second.velocityX;};
    const auto share=[&]{const float world=float((wa+wb)/2.0),af=float(double(first.facing)*double(world)),bf=float(double(second.facing)*double(world));first.pushX=float(double(af)-double(first.velocityX));second.pushX=float(double(bf)-double(second.velocityX));};
    if(al==-1&&rightOccupant==0){second.x=float(double(second.x)-span(b.right,a.left));if(wa+wb>0&&wb>0)cancel();}
    else if(bl==-1&&rightOccupant==1){first.x=float(double(first.x)-span(a.right,b.left));if(wa+wb>0&&wa>0)cancel();}
    else if(bl==1&&leftOccupant==1){first.x=float(double(first.x)+span(b.right,a.left));if(wa+wb<0&&wa<0)cancel();}
    else if(al==1&&leftOccupant==0){second.x=float(double(second.x)+span(a.right,b.left));if(wa+wb<0&&wb<0)cancel();}
    else if(sum(b.left,b.right)<sum(a.left,a.right)||(sum(b.left,b.right)==sum(a.left,a.right)&&sum(a.top,a.bottom)<sum(b.top,b.bottom))){
        if(wa<=wb){const double distance=double(span(b.right,a.left))/2.0;first.x=float(double(first.x)+distance);second.x=float(double(second.x)-distance);share();}
    }else if(wb<=wa){const double distance=double(span(a.right,b.left))/2.0;first.x=float(double(first.x)-distance);second.x=float(double(second.x)+distance);share();}
    return true;
}
}
