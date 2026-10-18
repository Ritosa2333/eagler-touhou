#include "BulletArea.hpp"
#include <cmath>
namespace th15 {
namespace {
bool rounded_rectangle(float x,float y,float hx,float hy,float radius)noexcept{
    if(float(hx+radius)>=std::fabs(x)&&hy>=std::fabs(y))return true;
    if(hx>=std::fabs(x)&&float(hy+radius)>=std::fabs(y))return true;
    const float square=float(radius*radius),a=float(x-hx),b=float(y-hy),c=float(x+hx),d=float(y+hy);
    return square>float(float(a*a)+float(b*b))||square>float(float(c*c)+float(b*b))||square>float(float(a*a)+float(d*d))||square>float(float(c*c)+float(d*d));
}
}
bool bullet_circle_area(const BulletState& bullet,const Vec3& center,float radius,bool honor)noexcept{
    if((bullet.phase!=1&&bullet.phase!=2)||(honor&&bullet.collision_delay))return false;
    const float extent=float(float(bullet.hitbox.x*.5f)+radius),x=float(bullet.motion.position.x-center.x),y=float(bullet.motion.position.y-center.y);
    return float(extent*extent)>=float(float(x*x)+float(y*y));
}
bool bullet_rectangle_area(const BulletState& bullet,const Vec3& center,const Vec2& size,float angle)noexcept{
    if((bullet.phase!=1&&bullet.phase!=2)||bullet.collision_delay)return false;const auto& p=bullet.motion.position;
    const float dx=float(p.x-center.x),dy=float(p.y-center.y),sine=float(std::sin(double(-angle))),cosine=float(std::cos(double(-angle)));
    const float x=float(float(cosine*dx)-float(sine*dy)),y=float(float(cosine*dy)+float(sine*dx)),radius=float(bullet.scale*bullet.hitbox.x);
    if(!rounded_rectangle(x,y,float(size.x*.5f),float(size.y*.5f),radius))return false;
    // The source also clips to the playfield, so offscreen bullets do not
    // disappear before their visible part reaches the cancellation area.
    return rounded_rectangle(p.x,float(p.y-224.f),192.f,224.f,radius);
}
}
