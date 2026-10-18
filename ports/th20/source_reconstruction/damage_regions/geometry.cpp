#include "geometry.hpp"
#include "../ecl_vm/math.hpp"
#include <emmintrin.h>
#include <algorithm>
#include <array>
#include <cmath>
namespace th20::source::geometry {
namespace {
namespace math=ecl::math;
constexpr float pi=3.1415927410125732421875f;
float a(float x,float y){return _mm_cvtss_f32(_mm_add_ss(_mm_set_ss(x),_mm_set_ss(y)));}
float s(float x,float y){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(x),_mm_set_ss(y)));}
float m(float x,float y){return _mm_cvtss_f32(_mm_mul_ss(_mm_set_ss(x),_mm_set_ss(y)));}
float d(float x,float y){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(x),_mm_set_ss(y)));}
int trunc(float x){return _mm_cvtt_ss2si(_mm_set_ss(x));}
struct Point{float x,y;};
using Corners=std::array<Point,4>;
float square(float x,float y){return a(m(x,x),m(y,y));}
Corners corners(float x,float y,float w,float h,float angle){
    Corners result{{{d(-w,2),d(-h,2)},{d(-w,2),d(h,2)},{d(w,2),d(h,2)},{d(w,2),d(-h,2)}}};
    for(auto& p:result){if(angle!=0)math::rotate(p.x,p.y,angle);p.x=a(p.x,x);p.y=a(p.y,y);}return result;
}
bool contains_corner(float x,float y,float w,float h,float angle,const Corners& points){ //458b60
    for(auto p:points){p.x=s(p.x,x);p.y=s(p.y,y);if(angle!=0)math::rotate(p.x,p.y,-angle);if(d(w,2)>=std::fabs(p.x)&&d(h,2)>=std::fabs(p.y))return true;}return false;
}
Point ellipse(float angle,float rx,float ry){return {m(math::cosine(angle),rx),m(math::sine(angle),ry)};} //459190
Point nearest(float x,float y,float cx,float cy,float radius){ //4532c0, z=0
    Point p{s(cx,x),s(cy,y)};const auto length=math::square_root(a(square(p.x,p.y),0));
    if(std::fabs(length)>=.01f){p.x=d(p.x,length);p.y=d(p.y,length);}p.x=a(m(p.x,radius),x);p.y=a(m(p.y,radius),y);return p;
}
}
bool circle_point(float x,float y,float cx,float cy,float r){return square(s(cx,x),s(cy,y))<=m(r,r);}
bool rectangle_point(float x,float y,float cx,float cy,float w,float h){return d(w,2)>std::fabs(s(x,cx))&&d(h,2)>std::fabs(s(y,cy));}
bool ellipse_point(float x,float y,float cx,float cy,float rx,float ry,float angle){x=s(x,cx);y=s(y,cy);math::rotate(x,y,-angle);return 1>=a(d(m(x,x),m(rx,rx)),d(m(y,y),m(ry,ry)));}
bool segment_segment(float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4){
    const auto first=a(m(s(x1,x2),s(y3,y1)),m(s(y1,y2),s(x1,x3)));
    const auto second=a(m(s(x1,x2),s(y4,y1)),m(s(y1,y2),s(x1,x4)));
    if(!(0>=m(first,second)))return false;
    if(first==0&&second==0){
        if(x2<x1){std::swap(x1,x2);std::swap(y1,y2);}if(x4<x3){std::swap(x3,x4);std::swap(y3,y4);}
        // The original orders by x only, including its collinear vertical quirk.
        return !(x4<x1||y4<y1||x2<x3||y2<y3);
    }
    return 0>=m(a(m(s(x3,x4),s(y1,y3)),m(s(y3,y4),s(x3,x1))),a(m(s(x3,x4),s(y2,y3)),m(s(y3,y4),s(x3,x2))));
}
bool rectangle_circle(float x,float y,float w,float h,float angle,float cx,float cy,float r){
    x=s(cx,x);y=s(cy,y);math::rotate(x,y,-angle);const auto hw=d(w,2),hh=d(h,2),ax=std::fabs(x),ay=std::fabs(y);
    if(a(hw,r)>=ax&&hh>=ay)return true;if(hw>=ax&&a(hh,r)>=ay)return true;
    const auto rr=m(r,r);return rr>square(s(x,hw),s(y,hh))||rr>square(a(x,hw),s(y,hh))||rr>square(s(x,hw),a(y,hh))||rr>square(a(x,hw),a(y,hh));
}
bool rectangle_rectangle(float x,float y,float w,float h,float angle,float ox,float oy,float ow,float oh,float oa){
    const auto reach=a(math::square_root(square(d(w,2),d(h,2))),math::square_root(square(d(ow,2),d(oh,2))));
    if(!(reach>math::square_root(square(s(x,ox),s(y,oy)))))return false;
    const auto first=corners(x,y,w,h,angle),second=corners(ox,oy,ow,oh,oa);
    if(contains_corner(x,y,w,h,angle,second)||contains_corner(ox,oy,ow,oh,oa,first))return true;
    for(int i=0;i<4;++i)for(int j=0;j<4;++j){const auto p=first[i],q=first[(i+1)%4],r=second[j],t=second[(j+1)%4];if(segment_segment(p.x,p.y,q.x,q.y,r.x,r.y,t.x,t.y))return true;}return false;
}
bool ellipse_circle(float x,float y,float r,float cx,float cy,float rx,float ry,float angle){
    const auto maximum=rx>ry?rx:ry,minimum=ry>rx?rx:ry;
    if(minimum>r&&ellipse_point(x,y,cx,cy,s(rx,r),s(ry,r),angle))return true;
    x=s(x,cx);y=s(y,cy);math::rotate(x,y,-angle);
    if(r>maximum&&m(s(r,maximum),s(r,maximum))>=square(x,y))return true;
    if(1>r||1>maximum)return false;
    auto current=-pi;
    if(r>maximum){const auto samples=std::max(8,trunc(a(rx,ry))/4);for(int i=0;i<samples;++i){auto p=ellipse(current,rx,ry);current=a(d(m(pi,2),float(samples)),current);if(circle_point(p.x,p.y,x,y,r))return true;}}
    else {const auto samples=std::max(8,trunc(d(r,4)));for(int i=0;i<samples;++i){Point p;math::polar(p.x,p.y,current,r);current=a(d(m(pi,2),float(samples)),current);if(ellipse_point(a(p.x,x),a(p.y,y),0,0,rx,ry,0))return true;}}
    return false;
}
bool rectangle_ellipse(float x,float y,float w,float h,float angle,float cx,float cy,float rx,float ry,float ea){
    const auto er=rx>ry?rx:ry,rr=w>h?w:h;x=s(x,cx);y=s(y,cy);math::rotate(x,y,-ea);math::rotate(w,h,math::wrap_angle(s(angle,ea)));
    if(rr>er){const auto samples=std::max(8,trunc(a(rx,ry))/4);for(int i=0;i<samples;++i){auto p=ellipse(angle,rx,ry);angle=a(d(m(pi,2),float(samples)),angle);if(rectangle_point(p.x,p.y,x,y,w,h))return true;}}
    else {const auto samples=std::max(3,trunc(d(rr,8)));auto py=d(-h,2);for(int j=0;j<samples;++j){auto px=d(-w,2);for(int i=0;i<samples;++i){if(ellipse_point(a(px,x),a(py,y),0,0,rx,ry,0))return true;px=a(d(w,float(samples-1)),px);}py=a(d(h,float(samples-1)),py);}}
    return false;
}
bool polygon_segment(float ax,float ay,float bx,float by,float cx,float cy,float radius,float angle,int sides){
    for(int i=0;i<sides;++i){Point p{radius,0},q{radius,0};math::rotate(p.x,p.y,angle);angle=math::wrap_angle(a(d(m(pi,2),float(sides)),angle));math::rotate(q.x,q.y,angle);if(segment_segment(a(p.x,cx),a(p.y,cy),a(q.x,cx),a(q.y,cy),ax,ay,bx,by))return true;}return false;
}
bool star_segment(float ax,float ay,float bx,float by,float cx,float cy,float outer,float inner,float angle,int sides){
    const auto count=static_cast<int>(static_cast<unsigned>(sides)*2);for(int i=0;i<count;++i){Point p{i%2?inner:outer,0},q{i%2?outer:inner,0};math::rotate(p.x,p.y,angle);angle=math::wrap_angle(a(d(d(m(pi,2),float(sides)),2),angle));math::rotate(q.x,q.y,angle);if(segment_segment(a(p.x,cx),a(p.y,cy),a(q.x,cx),a(q.y,cy),ax,ay,bx,by))return true;}return false;
}
bool polygon_point(float x,float y,float cx,float cy,float radius,float angle,int sides){return !polygon_segment(x,y,cx,cy,cx,cy,radius,angle,sides);}
bool star_point(float x,float y,float cx,float cy,float outer,float inner,float angle,int sides){return !star_segment(x,y,cx,cy,cx,cy,outer,inner,angle,sides);}
bool polygon_circle(float x,float y,float r,float cx,float cy,float pr,float angle,int sides){if(circle_point(cx,cy,x,y,r))return true;const auto p=nearest(x,y,cx,cy,r);return polygon_point(p.x,p.y,cx,cy,pr,angle,sides);}
bool star_circle(float x,float y,float r,float cx,float cy,float outer,float inner,float angle,int sides){if(circle_point(cx,cy,x,y,r))return true;const auto p=nearest(x,y,cx,cy,r);return star_point(p.x,p.y,cx,cy,outer,inner,angle,sides);}
bool rectangle_polygon(float x,float y,float w,float h,float angle,float cx,float cy,float r,float pa,int sides){
    if(rectangle_point(cx,cy,x,y,w,h)||polygon_point(x,y,cx,cy,r,pa,sides))return true;const auto points=corners(x,y,w,h,angle);
    for(int i=0;i<4;++i){const auto p=points[i],q=points[(i+1)%4];if(polygon_segment(p.x,p.y,q.x,q.y,cx,cy,r,pa,sides))return true;}return false;
}
bool rectangle_star(float x,float y,float w,float h,float angle,float cx,float cy,float outer,float inner,float pa,int sides){
    if(rectangle_point(cx,cy,x,y,w,h)||star_point(x,y,cx,cy,outer,inner,pa,sides))return true;const auto points=corners(x,y,w,h,angle);
    for(int i=0;i<4;++i){const auto p=points[i],q=points[(i+1)%4];if(star_segment(p.x,p.y,q.x,q.y,cx,cy,outer,inner,pa,sides))return true;}return false;
}
}
