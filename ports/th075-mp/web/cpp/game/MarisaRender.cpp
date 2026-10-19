#include "MarisaState.hpp"
#include "QuadTransform.hpp"
#include "ZunGraphics.hpp"
#include <cmath>
namespace th075 {
void render_marisa_laser(FighterActor& actor,MarisaLaserState& laser,const CameraMotion& camera,ZunGraphics& graphics,u32 texture){
    if(!laser.visible)return;
    const float cameraX=float(double(camera.x)-160.0);
    const float cameraY=float(double(camera.y)+420.0);
    graphics.blend(SpriteBlend::AlphaAdd);
    for(i32 ring=1;ring<=8;ring++){
        const i16 radius=i16(i32(double(laser.width)*(ring*10+50)));
        const float elapsed=float(u16(actor.combat.elapsed));
        const float zAngle=float(double(elapsed)*5.0+ring*5);
        const float yAngle=float(std::cos(double(float(double(elapsed)/41.0+ring*7)))*30.0+(ring*260%60-30)+actor.motion.facing*90);
        const float xAngle=float(std::sin(double(float(double(elapsed)/50.0+ring*6)))*30.0+(ring*336%40-20));
        const float x=float(double(laser.x)+i16(ring*100)*laser.facing);
        for(u32 angle=0;angle<36;angle++){
            const float radians=float(double(angle)*double(0.017453289f)*10.0);
            const float px=float(std::cos(double(radians))*radius),py=float(std::sin(double(radians))*radius);
            QuadPoints points{};
            points[0]={px,py,0,1};points[1]={px,py,float(double(laser.width)*64.0/2.0),1};
            rotate_quad(points,0,0,0,xAngle,yAngle,zAngle);
            translate_quad(points,x,-laser.y,0);
            translate_quad(points,cameraX,cameraY,0);
            for(u32 side=0;side<2;side++){
                auto& vertex=laser.vertices[angle*2+side];vertex.x=points[side][0];vertex.y=points[side][1];vertex.z=points[side][2];
            }
        }
        for(u32 side=0;side<2;side++){
            auto& end=laser.vertices[72+side];const auto& start=laser.vertices[side];end.x=start.x;end.y=start.y;end.z=start.z;
        }
        graphics.projected_strip(laser.vertices.data(),laser.vertices.size(),texture);
    }
}
}
