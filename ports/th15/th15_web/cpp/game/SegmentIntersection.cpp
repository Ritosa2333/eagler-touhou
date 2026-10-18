#include "SegmentIntersection.hpp"
#include <cmath>
namespace th15 {
bool segment_intersection(const Vec2& a,const Vec2& b,const Vec2& c,const Vec2& d,Vec2& hit){
    const float dx=float(a.x-b.x),dy=float(a.y-b.y),delta=float(a.x-c.x);
    const float u=float(float(float(c.y-a.y)*dx)+float(delta*dy)),v=float(float(float(a.x-d.x)*dy)+float(float(d.y-a.y)*dx));
    if(float(u*v)>0)return false;
    if(u==0&&v==0){Vec2 lo=a,hi=b,e0=c,e1=d;if(a.x>b.x){lo=b;hi=a;}if(c.x>d.x){e0=d;e1=c;}if(lo.x>e1.x||lo.y>e1.y||hi.x<e0.x||hi.y<e0.y)return false;}
    else{const float ex=float(c.x-d.x),ey=float(c.y-d.y),f=float(float(float(c.x-b.x)*ey)+float(float(b.y-c.y)*ex)),g=float(float(float(c.x-a.x)*ey)+float(float(a.y-c.y)*ex));if(float(f*g)>0)return false;}
    const float ax=float(b.x-a.x),ay=float(b.y-a.y),cx=float(d.x-c.x),cy=float(d.y-c.y);
    const bool av=std::abs(double(ax))<.01,cv=std::abs(double(cx))<.01;
    const float slope=av?0:float(ay/ax),constant=av?a.x:float(a.y-float(float(ay*a.x)/ax));
    const float other=cv?0:float(cy/cx),offset=cv?c.x:float(c.y-float(float(cy*c.x)/cx));
    if(av){if(cv){if(std::abs(double(delta))>=.001)return false;hit=a;}else hit={a.x,float(float(other*a.x)+offset)};}
    else if(cv)hit={c.x,float(float(slope*c.x)+constant)};
    else{const float difference=float(offset-constant),denominator=float(slope-other);hit={float(difference/denominator),float(float(float(difference*slope)/denominator)+constant)};}
    return true;
}
}
