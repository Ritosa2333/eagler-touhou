#include "ShotTh16.hpp"
#include <algorithm>
#include <cmath>
#if PORT_TITLE == 16
namespace th11 {
namespace {
Vec2 turn(Vec2 p,float a){if(a==0)return p;const float s=float(std::sin(double(a))),c=float(std::cos(double(a)));return {p.x*c-p.y*s,p.y*c+p.x*s};}
std::array<Vec2,4> corners(Vec3 p,Vec2 size,float angle){const float x=size.x*-.5f,y=size.y*-.5f;std::array<Vec2,4> out={Vec2{x,y},Vec2{x,-y},Vec2{-x,-y},Vec2{-x,y}};for(auto& v:out){v=turn(v,angle);v.x+=p.x;v.y+=p.y;}return out;}
bool inside(Vec2 q,Vec3 p,Vec2 size,float angle){const auto v=turn({q.x-p.x,q.y-p.y},-angle);return size.x*.5f>=std::abs(v.x)&&size.y*.5f>=std::abs(v.y);}
float side(Vec2 a,Vec2 b,Vec2 p){return (p.x-a.x)*(b.y-a.y)+(a.y-p.y)*(b.x-a.x);}
bool segments(Vec2 a,Vec2 b,Vec2 c,Vec2 d){
 const float s=side(a,b,c),t=side(a,b,d);if(s*t>0)return false;
 if(s!=0||t!=0)return side(c,d,a)*side(c,d,b)<=0;
 if(a.x>b.x)std::swap(a,b);if(c.x>d.x)std::swap(c,d);
 return d.x>=a.x&&d.y>=a.y&&b.x>=c.x&&b.y>=c.y;
}
float floating(u32 word){float value;std::memcpy(&value,&word,4);return value;}
}
bool shot_th16_rectangles(Vec3 p,Vec2 size,float angle,Vec3 q,Vec2 other,float other_angle)noexcept{
 const float dx=p.x-q.x,dy=p.y-q.y,hx=size.x*.5f,hy=size.y*.5f,ox=other.x*.5f,oy=other.y*.5f;
 if(float(std::sqrt(double(dx*dx+dy*dy)))>=float(std::sqrt(double(hx*hx+hy*hy)))+float(std::sqrt(double(ox*ox+oy*oy))))return false;
 const auto a=corners(p,size,angle),b=corners(q,other,other_angle);
 for(const auto v:b)if(inside(v,p,size,angle))return true;
 for(const auto v:a)if(inside(v,q,other,other_angle))return true;
 for(u32 j=0;j<4;++j)for(u32 k=0;k<4;++k)if(segments(a[j],a[(j+1)%4],b[k],b[(k+1)%4]))return true;
 return false;
}
bool shot_th16_circle_rectangle(Vec3 rectangle,Vec2 size,float angle,Vec3 circle,float radius)noexcept{
 const float dx=circle.x-rectangle.x,dy=circle.y-rectangle.y;
 const float sine=float(std::sin(-double(angle))),cosine=float(std::cos(-double(angle)));
 const float x=dx*cosine-dy*sine,y=dy*cosine+dx*sine,hx=size.x*.5f,hy=size.y*.5f;
 const float ax=std::abs(x),ay=std::abs(y);
 if(hx+radius>=ax&&hy>=ay)return true;
 if(hx>=ax&&hy+radius>=ay)return true;
 const float r2=radius*radius;
 float a=x-hx,b=y-hy;if(r2>a*a+b*b)return true;
 a=x+hx;if(r2>a*a+b*b)return true;
 b=y+hy;a=x-hx;if(r2>a*a+b*b)return true;
 a=x+hx;return r2>a*a+b*b;
}
bool shot_th16_overlaps(const Th16DamageSource& record,const Th16DamageQuery& query)noexcept{
 const auto& w=record.words;const Vec3 p{floating(w[7]),floating(w[8]),floating(w[9])};const Vec2 size{floating(w[5]),floating(w[6])};const float angle=floating(w[3]),radius=floating(w[1]);
 if(!(w[0]&2))return query.size?shot_th16_rectangles(p,size,angle,query.position,*query.size,query.angle):shot_th16_circle_rectangle(p,size,angle,query.position,query.radius);
 if(query.size)return shot_th16_circle_rectangle(query.position,*query.size,query.angle,p,radius);
 const float x=p.x-query.position.x,y=p.y-query.position.y,r=radius+query.radius;return x*x+y*y<=r*r;
}
bool shot_th16_beam_intersection(Vec3 origin,Vec3 center,Vec2 size,float angle,float beam_angle,Vec2& first)noexcept{
    const float hx=size.x*-.5f,hy=size.y*-.5f;
    std::array<Vec2,4> corners={Vec2{hx,hy},Vec2{hx,-hy},Vec2{-hx,-hy},Vec2{-hx,hy}};
    const float sn=float(std::sin(double(angle))),cs=float(std::cos(double(angle)));
    for(auto& p:corners){if(beam_angle!=0)p={p.x*cs-p.y*sn,p.y*cs+p.x*sn};p.x+=center.x;p.y+=center.y;}
    const float s=float(std::sin(double(beam_angle))),c=float(std::cos(double(beam_angle)));
    const float dx=s*0.f-c*1000.f,dy=c*0.f+s*1000.f;
    // The retail helper's sin/cos return order yields x=cos, y=sin.
    const Vec2 plus{origin.x-dx,origin.y+dy},minus{origin.x+dx,origin.y-dy};
    std::array<Vec2,2> points{};u32 count=0;
    const auto side=[](Vec2 a,Vec2 b,Vec2 p){return (b.x-p.x)*(b.y-a.y)+(p.y-b.y)*(b.x-a.x);};
    for(u32 i=0;i<4&&count<2;++i){const auto q=corners[i],r=corners[(i+1)%4];
        const float u=side(plus,minus,q),v=side(plus,minus,r);if(u*v>0)continue;
        if(u==0&&v==0){auto aa=plus,bb=minus,qq=q,rr=r;if(bb.x>aa.x)std::swap(aa,bb);if(qq.x>rr.x)std::swap(qq,rr);
            if(rr.x<bb.x||rr.y<bb.y||aa.x<qq.x||aa.y<qq.y)continue;
        }else if(side(q,r,plus)*side(q,r,minus)>0)continue;
        const float line_dx=plus.x-minus.x,line_dy=plus.y-minus.y,edge_dx=r.x-q.x,edge_dy=r.y-q.y;
        const bool lv=std::abs(line_dx)<.01f,ev=std::abs(edge_dx)<.01f;
        const float ls=lv?0:line_dy/line_dx,li=lv?minus.x:minus.y-(line_dy*minus.x)/line_dx;
        const float es=ev?0:edge_dy/edge_dx,ei=ev?q.x:q.y-(q.x*edge_dy)/edge_dx;
        Vec2 p;
        if(!lv&&!ev){const float d=ei-li,den=ls-es;p={d/den,(ls*d)/den+li};}
        else if(!lv)p={q.x,ls*q.x+li};
        else if(!ev)p={minus.x,minus.x*es+ei};
        else{if(std::abs(minus.x-q.x)>=.001f)continue;p=minus;}
        points[count++]=p;
    }
    if(!count)return false;first=points[0];
    if(count==2){const auto distance=[&](Vec2 p){const float x=minus.x-p.x,y=minus.y-p.y;return x*x+y*y;};if(distance(points[1])<=distance(points[0]))first=points[1];}
    return true;
}
}
#endif
