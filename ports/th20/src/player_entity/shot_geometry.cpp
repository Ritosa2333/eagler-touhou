#include "shot_geometry.hpp"
#include "../damage_regions/geometry.hpp"
#include "../ecl_vm/math.hpp"
#include <cmath>
namespace th20::source::player_entity::shot_geometry {
namespace {
float add(float a,float b){return recovered::add32(a,b);}float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}float mul(float a,float b){return recovered::mul32(a,b);}float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float squared(float x,float y){return add(mul(x,x),mul(y,y));}
}
bool line_equation(float& slope,float& intercept,float ax,float ay,float bx,float by){
    const float distance=std::fabs(sub(bx,ax));
    if(!(distance<0.01f)){slope=div(sub(by,ay),sub(bx,ax));intercept=sub(ay,div(mul(sub(by,ay),ax),sub(bx,ax)));}
    else{slope=0;intercept=ax;}return distance<0.01f;
}
bool segment_intersection(float& x,float& y,float ax,float ay,float bx,float by,float cx,float cy,float dx,float dy){
    if(!geometry::segment_segment(ax,ay,bx,by,cx,cy,dx,dy))return false;
    float slope0,intercept0,slope1,intercept1;const bool vertical0=line_equation(slope0,intercept0,ax,ay,bx,by),vertical1=line_equation(slope1,intercept1,cx,cy,dx,dy);
    if(!vertical0&&!vertical1){x=div(sub(intercept1,intercept0),sub(slope0,slope1));y=add(div(mul(sub(intercept1,intercept0),slope0),sub(slope0,slope1)),intercept0);}
    else if(vertical0&&vertical1){if(!(std::fabs(sub(ax,cx))<0.001f))return false;x=ax;y=ay;}
    else if(vertical0){x=ax;y=add(mul(slope1,ax),intercept1);}else{x=cx;y=add(mul(slope0,cx),intercept0);}return true;
}
void ray_circle(float& first,float& second,const sprite::Vec3& origin,float angle,const sprite::Vec3& center,float radius){
    float x=sub(center.x,origin.x),y=sub(center.y,origin.y);ecl::math::rotate(x,y,-angle);
    if(radius<std::fabs(y)||x< -radius||(x<0&&mul(radius,radius)<squared(x,y))){first=second=0;return;}
    const float root=ecl::math::square_root(sub(1.0f,mul(div(y,radius),div(y,radius))));first=sub(x,mul(radius,root));second=add(x,mul(radius,root));
}
bool ray_rectangle(sprite::Vec3& first,sprite::Vec3& second,const sprite::Vec3& origin,float ray_angle,float x,float y,float width,float height,float rectangle_angle){
    sprite::Vec2 corners[4]{{div(-width,2),div(-height,2)},{div(-width,2),div(height,2)},{div(width,2),div(height,2)},{div(width,2),div(-height,2)}};
    // The original tests ray_angle, rather than rectangle_angle, before rotating.
    if(ray_angle!=0){const float sine=ecl::math::sine(rectangle_angle),cosine=ecl::math::cosine(rectangle_angle);for(auto& corner:corners){const float old_x=corner.x,old_y=corner.y;corner.y=add(mul(old_y,cosine),mul(old_x,sine));corner.x=sub(mul(old_x,cosine),mul(old_y,sine));}}
    for(auto& corner:corners){corner.x=add(corner.x,x);corner.y=add(corner.y,y);}
    float vx=1000.0f,vy=0;ecl::math::rotate(vx,vy,ray_angle);const sprite::Vec2 start{sub(origin.x,vx),sub(origin.y,vy)},end{add(origin.x,vx),add(origin.y,vy)};
    constexpr unsigned edges[4][2]{{0,1},{1,2},{2,3},{3,0}};sprite::Vec3 intersections[2]{};unsigned count=0;
    for(unsigned i=0;i<4&&count<2;++i){const auto& a=corners[edges[i][0]];const auto& b=corners[edges[i][1]];if(segment_intersection(intersections[count].x,intersections[count].y,start.x,start.y,end.x,end.y,a.x,a.y,b.x,b.y))++count;}
    if(!count)return false;unsigned near=0,far=0;if(count==2){if(!(squared(sub(start.x,intersections[1].x),sub(start.y,intersections[1].y))>squared(sub(start.x,intersections[0].x),sub(start.y,intersections[0].y))))near=1;far=1-near;} //COMISS/JBE also selects the second intersection for unordered distances
    first.x=intersections[near].x;first.y=intersections[near].y;second.x=intersections[far].x;second.y=intersections[far].y;return true;
}
}
