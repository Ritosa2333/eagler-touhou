#include "AnmRenderer.hpp"
#include <cmath>
namespace th09 {
Matrix4 AnmRenderer::world_matrix(AnmVm& vm){
    if(!vm.flag16&&(vm.updateScale||vm.updateRotation)){
        vm.matrix2=vm.matrix1;vm.matrix2.m[0][0]*=vm.scale.x;vm.matrix2.m[1][1]*=vm.scale.y;vm.updateScale=false;
        const float angles[]={vm.rotation.x,vm.rotation.y,vm.rotation.z};for(u32 n=0;n<3;++n)if(angles[n])vm.matrix2=GraphicsMath::multiply(vm.matrix2,GraphicsMath::rotation(n,angles[n]));vm.updateRotation=false;
    }
    auto m=vm.matrix2;m.m[3][0]=vm.pos.x;m.m[3][1]=vm.pos.y;m.m[3][2]=vm.pos.z;
    if(vm.anchor&1)m.m[3][0]=std::fabs(vm.spriteSize.x*vm.scale.x*.5f)+vm.pos.x;
    if(vm.anchor&2)m.m[3][1]=std::fabs(vm.spriteSize.y*vm.scale.y*.5f)+vm.pos.y;return m;
}
void AnmRenderer::transform_world(AnmVm& vm){last_world=world_matrix(vm);const auto combined=GraphicsMath::multiply(GraphicsMath::multiply(last_world,camera.view),camera.projection);for(u32 n=0;n<4;++n)quad[n].position=GraphicsMath::project(world_vertices[n],view,combined);}
i32 AnmRenderer::draw_world(AnmVm& vm){if(!drawable(vm))return -1;transform_world(vm);return finish(vm,false);}
i32 AnmRenderer::draw_facing_camera(AnmVm& vm){
    if(!drawable(vm))return -1;Matrix4 world;world.identity();world.m[3][0]=vm.pos.x;world.m[3][1]=vm.pos.y;world.m[3][2]=vm.pos.z;
    const auto combined=GraphicsMath::multiply(GraphicsMath::multiply(world,camera.view),camera.projection);
    const Vec3 center=GraphicsMath::project({},view,combined);if(center.z<0||center.z>1)return -1;
    const Vec3 reference=GraphicsMath::project(camera.right,view,combined);
    const float half_ratio=GraphicsMath::length({reference.x-center.x,reference.y-center.y,reference.z-center.z})*.5f;
    const float width=vm.spriteSize.x*vm.scale.x*half_ratio,height=vm.spriteSize.y*vm.scale.y*half_ratio,sine=float(std::sin(double(vm.rotation.z))),cosine=float(std::cos(double(vm.rotation.z)));
    for(u32 n=0;n<4;++n){float x=n&1?width:-width,y=n&2?height:-height;auto& v=quad[n].position;v={x*cosine-y*sine+center.x,y*cosine+x*sine+center.y,center.z};if(vm.anchor&1)v.x+=width;if(vm.anchor&2)v.y+=height;}
    return finish(vm,false);
}
i32 AnmRenderer::draw_3d(AnmVm& vm){
    if(!drawable(vm))return -1;flush();auto world=world_matrix(vm);world.m[3][0]+=shake.x;world.m[3][1]+=shake.y;prepare(vm);
    if(camera_mode!=vm.flag15){if(vm.flag15)scene_camera(background_camera_data);else screen_camera();}backend.transform(MatrixKind::World,world);
    // The original cache invalidates on U scroll; a V-only change retains its
    // previous texture matrix until the active sprite changes.
    if(current_sprite!=vm.loadedSprite||vm.uvScrollPos.x){current_sprite=vm.loadedSprite;texture_matrix=vm.matrix3;texture_matrix.m[2][0]=vm.loadedSprite->uvStart.x+vm.uvScrollPos.x;texture_matrix.m[2][1]=vm.loadedSprite->uvStart.y+vm.uvScrollPos.y;backend.transform(MatrixKind::Texture,texture_matrix);}
    const WorldVertex vertices[4]={{{-128,-128,0},{0,0}},{{128,-128,0},{1,0}},{{-128,128,0},{0,1}},{{128,128,0},{1,1}}};
    auto s=state;s.textureTransform=true;s.textureFactor=color(vm);s.color.second=s.alpha.second={ArgumentSource::Factor};backend.draw(s,texture,Topology::Strip,VertexLayout::WorldUv,vertices,4);last_world=world;return 0;
}
}
