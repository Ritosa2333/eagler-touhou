#include "regions.hpp"
#include "geometry.hpp"
namespace th20::source::damage {
bool intersects(const Region& r,const sprite::Vec3& position,const sprite::Vec2* size,float angle,float radius){ //4c1030
    const auto x=r.motion.position.x,y=r.motion.position.y;
    if(!size){switch((r.flags>>1)&7){
    case 0:return geometry::rectangle_circle(x,y,r.size.x,r.size.y,r.angle,position.x,position.y,radius);
    case 1:return geometry::circle_point(x,y,position.x,position.y,recovered::add32(r.radius,radius));
    case 2:return geometry::ellipse_circle(position.x,position.y,radius,x,y,r.size.x,r.size.y,r.angle);
    case 3:return geometry::polygon_circle(position.x,position.y,radius,x,y,r.radius,r.angle,r.polygon_sides);
    case 4:return geometry::star_circle(position.x,position.y,radius,x,y,r.radius,r.inner_radius,r.angle,r.polygon_sides);
    default:return false;
    }}
    switch((r.flags>>1)&7){
    case 0:return geometry::rectangle_rectangle(x,y,r.size.x,r.size.y,r.angle,position.x,position.y,size->x,size->y,angle);
    case 1:return geometry::rectangle_circle(position.x,position.y,size->x,size->y,angle,x,y,r.radius);
    case 2:return geometry::rectangle_ellipse(position.x,position.y,size->x,size->y,angle,x,y,r.size.x,r.size.y,r.angle);
    case 3:return geometry::rectangle_polygon(position.x,position.y,size->x,size->y,angle,x,y,r.radius,r.angle,r.polygon_sides);
    case 4:return geometry::rectangle_star(position.x,position.y,size->x,size->y,angle,x,y,r.radius,r.inner_radius,r.angle,r.polygon_sides);
    default:return false;
    }
}
}
