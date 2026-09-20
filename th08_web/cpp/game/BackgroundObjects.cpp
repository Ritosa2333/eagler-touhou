#include "BackgroundObjects.hpp"
#include "GraphicsMath.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
#include <cmath>
namespace th08 {
namespace {
Vec3 add(const Vec3& a,const Vec3& b){return {Scalar::add(a.x,b.x),Scalar::add(a.y,b.y),Scalar::add(a.z,b.z)};}
Vec3 sub(const Vec3& a,const Vec3& b){return {Scalar::sub(a.x,b.x),Scalar::sub(a.y,b.y),Scalar::sub(a.z,b.z)};}
Extended dot(const Vec3& a,const Vec3& b){return number(a.x)*number(b.x)+number(a.y)*number(b.y)+number(a.z)*number(b.z);}
Extended length(const Vec3& v){return number(dot(v,v).to_float()).square_root();}
Vec3 translated(const Vec3& local,const Vec3& instance,const Vec3& offset){return {(number(local.x)+number(instance.x)-number(offset.x)).to_float(),(number(local.y)+number(instance.y)-number(offset.y)).to_float(),(number(local.z)+number(instance.z)-number(offset.z)).to_float()};}
Vec3 scaled_add(const Vec3& origin,const Vec3& direction,float distance){return {(number(direction.x)*number(distance)+number(origin.x)).to_float(),(number(direction.y)*number(distance)+number(origin.y)).to_float(),(number(direction.z)*number(distance)+number(origin.z)).to_float()};}
}
Vec3 BackgroundObjects::project(const Vec3& position)const{
    Matrix4 world;world.identity();world.m[3][0]=position.x;world.m[3][1]=position.y;world.m[3][2]=position.z;Vec3 result;
    GraphicsMath::project(result,projection_input,&renderer.viewport,&renderer.projection_matrix,&renderer.view_matrix,&world);return result;
}
float BackgroundObjects::fog_amount(float distance)const{return ((number(state.fog.near_plane)-number(distance))/(number(state.fog.near_plane)-number(state.fog.far_plane))).to_float();}
u32 BackgroundObjects::fog_color(u32 original,float amount)const{
    ZunColor result{signed_bits(original)};const auto& target=state.fog.color;
    const auto channel=[&](u8 from,u8 to){return u8(from-u8((Extended::from_int(i32(from)-to)*number(amount)).truncate_int()));};
    result.b=channel(result.b,target.b);result.g=channel(result.g,target.g);result.r=channel(result.r,target.r);result.a=u8((Extended::from_int(result.a)*(number(1)-number(amount))).truncate_int());return u32(result.d3dColor);
}
void BackgroundObjects::snapshot(){
    previous_visuals.resize(presentation_vms.size());
    for(size_t i=0;i<presentation_vms.size();++i)previous_visuals[i].capture(presentation_vms[i]);
    presentation_vms.clear();
    if(!state.quad_vms||state.quad_count<=0)return;
    presentation_vms.assign(state.quad_vms,state.quad_vms+state.quad_count);
}
void BackgroundObjects::draw(i32 layer){
    if(!state.stage_data||!state.instances)return;projection_input={};renderer.background_camera(state.camera);i32 fog_mode=255;
    Vec3 right{renderer.view_matrix.m[0][0],renderer.view_matrix.m[0][1],renderer.view_matrix.m[0][2]};GraphicsMath::normalize(right,right);
    const auto eye=add(state.camera.position,state.camera.eye_offset);
    for(auto* instance=state.instances;instance->object>=0;++instance){
        auto& object=*state.objects[instance->object];if(i8(object.layer)!=layer)continue;
        const auto center_component=[](float origin,float instance,float offset,float size){return (number(origin)+number(instance)-number(offset)+number(size)/number(2)).to_float();};
        const Vec3 center{center_component(object.position.x,instance->position.x,state.position.x,object.dimensions.x),center_component(object.position.y,instance->position.y,state.position.y,object.dimensions.y),center_component(object.position.z,instance->position.z,state.position.z,object.dimensions.z)};
        const auto relative=sub(center,eye);if(number(state.distance_limit)<dot(relative,relative))continue;
        const float distance=dot(relative,state.camera.unused24).to_float(),limit=(length(object.dimensions)/number(2)+number(960)).to_float();
        if(!(distance<=limit&&distance>=80))continue;if(!presentation::render_only)object.flags|=2;
        for(auto* q=StageProgram::first(object);q->type>=0;q=StageProgram::next(*q)){auto& source=state.quad_vms[q->vm];if(!source.loadedSprite)continue;
            TH08_AUDIT_SCOPE(Background,instance,source.currentTimeInScript.current,uint32_t(reinterpret_cast<uintptr_t>(q)));
            AnmVm copy;AnmVm* vm=&source;if(presentation::render_only&&q->vm>=0&&q->vm<i32(presentation_vms.size())){const auto& raw=presentation_vms[q->vm];copy=raw;if(q->vm<i32(previous_visuals.size())){
                const auto& before=previous_visuals[q->vm];
                // Stage backgrounds commonly scroll texture coordinates through
                // authored AddU/AddV loops. Those instructions accumulate the UV
                // directly, so uvScrollVel is zero and the generic ANM sidecar
                // cannot infer continuity. Smooth only a small shortest-path UV
                // delta while the same sprite remains alive; larger authored
                // jumps and sprite changes stay discrete.
                const u32 owner_fields=before.authored_uv_fields(raw);
                before.apply(raw,copy,presentation::world_alpha,presentation::VisualSample::Attributes|presentation::VisualSample::Offset,owner_fields);
            }vm=&copy;}
            if(q->type==0)sprite(*vm,*static_cast<StageSpriteQuad*>(q),*instance,right,fog_mode);
            else if(q->type==1)beam(*vm,*static_cast<StageBeamQuad*>(q),*instance,right,fog_mode);
        }
    }
}
void BackgroundObjects::sprite(AnmVm& vm,const StageSpriteQuad& quad,const StageInstance& instance,const Vec3& right,i32& fog_mode){
    const auto coordinate=[](float animated,float origin,float instance,float offset){return (number(animated)+number(origin)+number(instance)-number(offset)).to_float();};
    vm.pos={coordinate(vm.pos2.x,quad.position.x,instance.position.x,state.position.x),coordinate(vm.pos2.y,quad.position.y,instance.position.y,state.position.y),coordinate(vm.pos2.z,quad.position.z,instance.position.z,state.position.z)};
    if(quad.width!=0)vm.scale.x=Scalar::div(quad.width,vm.loadedSprite->widthPx);
    if(quad.height!=0)vm.scale.y=Scalar::div(quad.height,vm.loadedSprite->heightPx);
    if((vm.type&15)!=2){if(!renderer.fog_disabled&&fog_mode!=1){renderer.set_fog(true);fog_mode=1;}renderer.draw_3d(vm);return;}
    const auto projected=project(vm.pos);const float width=quad.width!=0?quad.width:vm.loadedSprite->widthPx;
    const Vec3 shifted{(number(right.x)*number(width)*number(vm.scale.x)+number(vm.pos.x)).to_float(),(number(right.y)*number(width)*number(vm.scale.x)+number(vm.pos.y)).to_float(),(number(right.z)*number(width)*number(vm.scale.x)+number(vm.pos.z)).to_float()};
    vm.scale.x=vm.scale.y=(length(sub(project(shifted),projected))/number(width)).to_float();if(width<0)vm.scale.y=-vm.scale.y;
    float distance=length(sub(vm.pos,add(state.camera.position,state.camera.eye_offset))).to_float();const u32 color=vm.color1.d3dColor;
    if(distance>state.fog.near_plane){distance=fog_amount(distance);if(distance>=1)return;vm.color1.d3dColor=signed_bits(fog_color(color,distance));}
    vm.pos=projected;if(projected.z>=0&&projected.z<=1){if(fog_mode!=0){if(!renderer.fog_disabled)renderer.set_fog(false);fog_mode=0;}renderer.draw_no_rotation(vm,false);
        if((vm.type&0xf0)==0x10&&distance<state.fog.near_plane&&state.effect_visible){const i32 index=state.effect_flags;if(index>=0&&index<32)state.effect_positions[index]={projected.x,projected.y,0};state.effect_flags=wrapping_add(index,1);}
    }
    vm.color1.d3dColor=signed_bits(color);
}
void BackgroundObjects::beam(AnmVm& vm,const StageBeamQuad& quad,const StageInstance& instance,const Vec3& right,i32& fog_mode){
    const float width=quad.width!=0?quad.width:vm.loadedSprite->widthPx;const auto eye=add(state.camera.position,state.camera.eye_offset);
    const auto start=translated(quad.start,instance.position,state.position),end=translated(quad.end,instance.position,state.position),a=project(start),b=project(end);
    const float aw=(length(sub(project(scaled_add(start,right,width)),a))/number(2)).to_float(),bw=(length(sub(project(scaled_add(end,right,width)),b))/number(2)).to_float();
    const auto color=[&](const Vec3& point){const float distance=length(sub(point,eye)).to_float();const u32 original=vm.color1.d3dColor;if(distance<=state.fog.near_plane)return original;const float amount=fog_amount(distance);return amount>=1?0x00ffffffu:fog_color(original,amount);};
    const u32 ac=color(start),bc=color(end);auto direction=sub(b,a);projection_input=direction;const float distance=number((number(direction.x)*number(direction.x)+number(direction.y)*number(direction.y)).to_float()).square_root().to_float();
    if(!(distance>=1e-5f))return;
    const float inverse=Scalar::div(1,distance);direction={Scalar::mul(inverse,direction.x),Scalar::mul(inverse,direction.y),Scalar::mul(inverse,direction.z)};
    // Original reuses this local as the next quad's projection input.
    projection_input=direction;if(!(a.z>=0&&a.z<=1&&b.z>=0&&b.z<=1))return;
    SpriteVertex vertices[4];
    vertices[0].pos={(number(direction.y)*number(aw)+number(a.x)).to_float(),(number(a.y)-number(direction.x)*number(aw)).to_float(),a.z};
    vertices[1].pos={(number(a.x)-number(direction.y)*number(aw)).to_float(),(number(direction.x)*number(aw)+number(a.y)).to_float(),a.z};
    vertices[2].pos={(number(direction.y)*number(bw)+number(b.x)).to_float(),(number(b.y)-number(direction.x)*number(bw)).to_float(),b.z};
    vertices[3].pos={(number(b.x)-number(direction.y)*number(bw)).to_float(),(number(direction.x)*number(bw)+number(b.y)).to_float(),b.z};
    const float u0=Scalar::add(vm.loadedSprite->uvStart.x,vm.uvScrollPos.x),u1=Scalar::add(vm.loadedSprite->uvEnd.x,vm.uvScrollPos.x),v0=Scalar::add(vm.loadedSprite->uvStart.y,vm.uvScrollPos.y),v1=Scalar::add(vm.loadedSprite->uvEnd.y,vm.uvScrollPos.y);
    vertices[0].uv={u0,v0};vertices[1].uv={u1,v0};vertices[2].uv={u0,v1};vertices[3].uv={u1,v1};vertices[0].color=vertices[1].color=ac;vertices[2].color=vertices[3].color=bc;
    if(fog_mode!=0){if(!renderer.fog_disabled)renderer.set_fog(false);fog_mode=0;}renderer.draw_quad(vm,vertices);
}
}
