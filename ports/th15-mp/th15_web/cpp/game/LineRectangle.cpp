#include "DamageSources.hpp"
#include <cmath>
namespace th15 {
bool line_rectangle(const Vec2& origin,float angle,const Vec2& center,const Vec2& size,float rotation,Vec2& near,Vec2& far){
    const float hx=float(size.x*.5f),hy=float(size.y*.5f);std::array<Vec2,4> edges={Vec2{float(size.x*-.5f),float(size.y*-.5f)},Vec2{float(size.x*-.5f),hy},Vec2{hx,hy},Vec2{hx,float(size.y*-.5f)}};
    const float sine=float(std::sin(double(rotation))),cosine=float(std::cos(double(rotation)));for(auto& p:edges){if(angle!=0){const float y=p.y;p.y=float(float(y*cosine)+float(p.x*sine));p.x=float(float(p.x*cosine)-float(y*sine));}p.x=float(p.x+center.x);p.y=float(p.y+center.y);}
    const float a=float(std::sin(double(angle))),b=float(std::cos(double(angle))),rx=float(float(b*1000.f)-float(a*0.f)),ry=float(float(b*0.f)+float(a*1000.f));const Vec2 begin={float(origin.x-rx),float(origin.y-ry)},end={float(origin.x+rx),float(origin.y+ry)};const float line_dx=float(begin.x-end.x),line_dy=float(begin.y-end.y);std::array<Vec2,2> hits{};u32 count=0;
    for(u32 i=0;i<4;i++){const Vec2 p=edges[i],q=edges[(i+1)%4];const float delta_x=float(begin.x-p.x),u=float(float(float(p.y-begin.y)*line_dx)+float(delta_x*line_dy)),v=float(float(float(begin.x-q.x)*line_dy)+float(float(q.y-begin.y)*line_dx));if(float(u*v)>0)continue;bool intersect=false;
        if(u==0&&v==0){Vec2 lo=begin,hi=end,e0=p,e1=q;if(end.x<begin.x){lo=end;hi=begin;}if(q.x<p.x){e0=q;e1=p;}intersect=lo.x<=e1.x&&lo.y<=e1.y&&e0.x<=hi.x&&e0.y<=hi.y;}
        else{const float ex=float(p.x-q.x),ey=float(p.y-q.y),f=float(float(float(p.x-end.x)*ey)+float(float(end.y-p.y)*ex)),g=float(float(float(p.x-begin.x)*ey)+float(float(begin.y-p.y)*ex));intersect=float(f*g)<=0;}if(!intersect)continue;
        const float dx=float(end.x-begin.x),dy=float(end.y-begin.y),ex=float(q.x-p.x),ey=float(q.y-p.y);float slope=0,constant=begin.x,edge_slope=0,edge_constant=p.x;if(std::abs(double(dx))>=.01){slope=float(dy/dx);constant=float(begin.y-float(float(dy*begin.x)/dx));}if(std::abs(double(ex))>=.01){edge_slope=float(ey/ex);edge_constant=float(p.y-float(float(ey*p.x)/ex));}Vec2 hit;
        if(std::abs(double(dx))<.01){if(std::abs(double(ex))<.01){if(std::abs(double(delta_x))>=.001)continue;hit=begin;}else hit={begin.x,float(float(begin.x*edge_slope)+edge_constant)};}
        else if(std::abs(double(ex))<.01)hit={p.x,float(float(slope*p.x)+constant)};
        else{const float difference=float(edge_constant-constant),denominator=float(slope-edge_slope);hit={float(difference/denominator),float(float(float(difference*slope)/denominator)+constant)};}hits[count++]=hit;if(count==2)break;
    }if(count==0)return false;if(count==1){near=far=hits[0];return true;}const float x0=float(begin.x-hits[0].x),y0=float(begin.y-hits[0].y),x1=float(begin.x-hits[1].x),y1=float(begin.y-hits[1].y),d0=float(float(x0*x0)+float(y0*y0)),d1=float(float(x1*x1)+float(y1*y1));if(d0<d1){near=hits[0];far=hits[1];}else{near=hits[1];far=hits[0];}return true;
}
}
