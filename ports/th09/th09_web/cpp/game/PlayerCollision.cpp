#include "PlayerCollision.hpp"
#include "GameMath.hpp"
namespace th09 {
bool intersects_box(const Bounds& b,Vec2 p,Vec2 e) noexcept {
    return b.minimum.x<=float(p.x)+e.x&&b.minimum.y<=float(p.y)+e.y&&float(p.x)-e.x<=b.maximum.x&&float(p.y)-e.y<=b.maximum.y;
}
bool intersects_circle(Vec2 player,float player_radius,Vec2 center,float radius) noexcept {
    const float dx=player.x-center.x,dy=player.y-center.y;
    const float r=float(radius)+player_radius;
    // The original rejects a point exactly on the combined-radius boundary.
    return r*r>float(dx)*dx+float(dy)*dy;
}
bool collects_item(i32 life,const Bounds& b,Vec2 p,Vec2 e) noexcept {
    if(life!=0&&life!=3)return false;
    // The original Vec3 division helper turns item width/height into half size.
    const Vec2 half{e.x*0.5f,e.y*0.5f};
    const Vec2 lo{p.x-half.x,p.y-half.y},hi{p.x+half.x,p.y+half.y};
    return b.minimum.x<=hi.x&&lo.x<=b.maximum.x&&b.minimum.y<=hi.y&&lo.y<=b.maximum.y;
}
u32 intersects_rotated_box(Vec2 player,Vec2 hit_extent,Vec2 center,Vec2 extent,Vec2 pivot,float angle,bool near) noexcept {
    const Vec2 relative{player.x-pivot.x,player.y-pivot.y};Vec2 transformed{};rotate(transformed,relative,-angle);
    transformed.x+=pivot.x;transformed.y+=pivot.y;
    const Vec2 hit_min{transformed.x-hit_extent.x,transformed.y-hit_extent.y},hit_max{transformed.x+hit_extent.x,transformed.y+hit_extent.y};
    const Vec2 box_min{center.x-extent.x,center.y-extent.y},box_max{center.x+extent.x,center.y+extent.y};
    if(hit_min.x<=box_max.x&&box_min.x<=hit_max.x&&hit_min.y<=box_max.y&&box_min.y<=hit_max.y)return 1;
    if(near&&hit_min.x<=float(box_max.x)+48&&float(box_min.x)-48<=hit_max.x&&hit_min.y<=float(box_max.y)+48&&float(box_min.y)-48<=hit_max.y)return 2;
    return 0;
}
}
