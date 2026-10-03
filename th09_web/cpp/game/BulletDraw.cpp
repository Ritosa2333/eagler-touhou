#include "BulletDraw.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th09 {
void BulletDraw::draw(BulletManager& bullets,BulletVisuals& visuals,LaserManager& lasers,BulletDrawServices& s){
    s.begin_field();
    for(u32 n=0;n<LaserManager::capacity;++n){auto& laser=lasers.pool[n];if(!laser.active)continue;
        const float sine=float(std::sin(double(laser.direction))),cosine=float(std::cos(double(laser.direction)));
        const float center=(laser.end_offset-laser.start_offset)*.5f+laser.start_offset;
        laser.body.pos=s.geometry.to_screen({center*cosine+laser.position.x,center*sine+laser.position.y,0});laser.body.pos.z=.07f;laser.color=-1;s.draw_animation(laser.body);
        if((laser.start_offset<16.f||laser.speed==0)&&(!laser.reserved||laser.phase)){
            laser.glow.pos=s.geometry.to_screen({cosine*laser.start_offset+laser.position.x,sine*laser.start_offset+laser.position.y,0});laser.glow.pos.z=.05f;
            laser.glow.color1=laser.body.color1;laser.glow.flag6=1;laser.glow.color1.a=255;
            const float radius=laser.width*.1f;float size=(16.f-laser.start_offset)*.0625f*radius;if(size<=0)size=radius;laser.glow.scale={size,size};s.draw_animation(laser.glow);
        }
    }
    for(auto head:bullets.draw_heads){u32 visited=0;for(i32 index=head;index>=0&&u32(index)<bullets.pool.size()&&visited++<bullets.pool.size();index=bullets.pool[index].draw_next){
        auto& bullet=bullets.pool[index];const u32 kind=bullet.state>=2&&bullet.state<=5?u32(bullet.state)-1:0;auto& vm=visuals.instances[index][kind];
        if(visuals.checkpoint)visuals.checkpoint->BeforeBulletAnimation(visuals,u32(index),kind);
        vm.pos=s.geometry.to_screen(bullet.position);vm.pos.z=.05f;vm.color1.d3dColor=i32(u32(vm.color1.d3dColor)|0xffffff);
        if(vm.type){vm.rotation.z=float(add_angle(bullet.direction+1.5707963705062866f,0));vm.updateRotation=1;}s.draw_animation(vm);
    }}
    s.upper_effects();
}
}
