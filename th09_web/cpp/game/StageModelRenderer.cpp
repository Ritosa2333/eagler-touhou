#include "StageModelRenderer.hpp"
#include <cmath>
namespace th09 {
namespace {
Vec3 add(const Vec3& a,const Vec3& b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
Vec3 sub(const Vec3& a,const Vec3& b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
float squared(const Vec3& p){return (p.x*p.x+p.y*p.y)+p.z*p.z;}
float dot(const Vec3& a,const Vec3& b){return (a.z*b.z+a.y*b.y)+a.x*b.x;}
u32 fog_color(u32 color,const BackgroundFog& fog,float distance){
    if(distance<=fog.near_plane)return color;const float amount=(fog.near_plane-distance)/(fog.near_plane-fog.far_plane);if(amount>=1)return color&0xffffff;
    u32 result=u32(u8(i32((1-amount)*float(u8(color>>24)))))<<24;
    for(u32 shift=0;shift<24;shift+=8){const i32 channel=u8(color>>shift),target=u8(fog.color>>shift);result|=u32(u8(channel-i32(float(channel-target)*amount)))<<shift;}return result;
}
}
void StageModelRenderer::draw(Background& bg,i32 layer,StageDrawServices& s){
    s.camera.scene(s.viewport,bg.camera);s.begin_models(s.camera);Matrix4 identity;identity.identity();
    const auto untranslated=GraphicsMath::multiply(GraphicsMath::multiply(identity,s.camera.view),s.camera.projection);
    const Vec3 basis=GraphicsMath::normalize({s.camera.view.m[0][0],s.camera.view.m[0][1],s.camera.view.m[0][2]});
    // The original projection input is shared with the ribbon direction. A
    // ribbon changes the input used by subsequent primitives in this pass.
    Vec3 projection_point{};
    const auto project=[&](const Vec3& p){return GraphicsMath::project(projection_point,s.viewport,GraphicsMath::translated_projection(p,s.camera.view,s.camera.projection,untranslated));};
    const Vec3 cull_eye=add(s.first_camera.position,s.first_camera.offset),eye=add(bg.camera.position,bg.camera.offset);i32 fog_mode=-1;
    const auto fog=[&](bool value){if(fog_mode!=i32(value)){s.fog_enabled(value);fog_mode=value;}};
    for(const auto& instance:bg.resource.instances){if(instance.object<0||u32(instance.object)>=bg.resource.objects.size())continue;auto& object=bg.resource.objects[instance.object];if(object.layer!=layer)continue;
        const Vec3 translated=sub(add(object.position,instance.position),bg.position),center{object.size.x*.5f+translated.x,object.size.y*.5f+translated.y,object.size.z*.5f+translated.z},delta=sub(center,cull_eye);
        if(squared(delta)>bg.distance_squared)continue;const float depth=dot(delta,s.first_camera.unit_direction);
        if(depth>GraphicsMath::length(object.size)*.5f+1280.f||depth<80.f)continue;object.flags|=2;
        for(const auto& p:object.primitives){auto& vm=bg.primitives[p.animation];if(!vm.loadedSprite)continue;const auto& sprite=*vm.loadedSprite;
            if(p.type==0){
                vm.pos=sub(add(add(vm.pos2,p.position),instance.position),bg.position);if(p.size.x)vm.scale.x=p.size.x/sprite.widthPx;if(p.size.y)vm.scale.y=p.size.y/sprite.heightPx;
                if((vm.type&15)!=2){fog(true);s.world_sprite(vm);continue;}
                const Vec3 location=vm.pos,screen=project(location);const float width=p.size.x?p.size.x:sprite.widthPx;
                const Vec3 reference=project({width*vm.scale.x*basis.x+location.x,width*vm.scale.x*basis.y+location.y,width*vm.scale.x*basis.z+location.z});
                const float scale=GraphicsMath::length(sub(reference,screen))/width;vm.scale={scale,width<0?-scale:scale};
                float distance=GraphicsMath::length(sub(location,eye));const u32 saved_color=u32(vm.color1.d3dColor);
                if(bg.fog.near_plane<distance){distance=(bg.fog.near_plane-distance)/(bg.fog.near_plane-bg.fog.far_plane);if(distance>=1)continue;vm.color1.d3dColor=i32(fog_color(saved_color,bg.fog,GraphicsMath::length(sub(location,eye))));}
                vm.pos=screen;if(screen.z>=0&&screen.z<=1){fog(false);s.screen_sprite(vm);if((vm.type&0xf0)==0x10&&distance<bg.fog.near_plane&&s.capture_positions&&s.captured.size()<32)s.captured.push_back(screen);}vm.color1.d3dColor=i32(saved_color);
            }else if(p.type==1){
                const float width=p.width?p.width:sprite.widthPx;const Vec3 start=sub(add(p.position,instance.position),bg.position),end=sub(add(p.end,instance.position),bg.position),a=project(start),b=project(end);
                const Vec3 ax=project({basis.x*width+start.x,basis.y*width+start.y,width*basis.z+start.z}),bx=project({basis.x*width+end.x,basis.y*width+end.y,width*basis.z+end.z});
                const float r0=GraphicsMath::length(sub(ax,a))*.5f,r1=GraphicsMath::length(sub(bx,b))*.5f;
                const u32 ca=fog_color(u32(vm.color1.d3dColor),bg.fog,GraphicsMath::length(sub(start,eye))),cb=fog_color(u32(vm.color1.d3dColor),bg.fog,GraphicsMath::length(sub(end,eye)));
                projection_point=sub(b,a);const float length=std::sqrt(projection_point.x*projection_point.x+projection_point.y*projection_point.y);if(length<.00001f)continue;const float inverse=1.f/length;projection_point.x*=inverse;projection_point.y*=inverse;projection_point.z*=inverse;const auto direction=projection_point;if(a.z<0||a.z>1||b.z<0||b.z>1)continue;
                const float u0=sprite.uvStart.x+vm.uvScrollPos.x,u1=sprite.uvEnd.x+vm.uvScrollPos.x,v0=sprite.uvStart.y+vm.uvScrollPos.y,v1=sprite.uvEnd.y+vm.uvScrollPos.y;
                SpriteVertex vertices[4]={{{a.x+direction.y*r0,a.y-direction.x*r0,a.z},1,ca,{u0,v0}},{{a.x-direction.y*r0,direction.x*r0+a.y,a.z},1,ca,{u1,v0}},{{b.x+direction.y*r1,b.y-direction.x*r1,b.z},1,cb,{u0,v1}},{{b.x-direction.y*r1,b.y+direction.x*r1,b.z},1,cb,{u1,v1}}};
                fog(false);s.screen_quad(vm,vertices);
            }
        }
    }
}
}
