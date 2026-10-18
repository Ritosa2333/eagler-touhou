#include "GamePresentation.hpp"
namespace th09 {
namespace {Topology topology(BattleGeometry kind){return kind==BattleGeometry::fan?Topology::Fan:kind==BattleGeometry::lines?Topology::Lines:Topology::Strip;}}
void GamePresentation::begin_field(i32 side){
    if(side<0||side>2)return;active_field=side;renderer.set_viewport(viewport(side));renderer.shake=offsets[side];
    if(side<2&&world&&world->backgrounds[side])renderer.scene_camera(world->backgrounds[side]->camera);else renderer.screen_camera();
}
void GamePresentation::animation(AnmVm& a,BattleSprite mode){
    switch(mode){case BattleSprite::unrotated:renderer.draw_no_rotation(a);break;case BattleSprite::rotated:renderer.draw_2d(a);break;case BattleSprite::automatic:renderer.draw_3d(a);break;case BattleSprite::mirrored:renderer.draw_no_rotation(a,true,true);break;}
}
void GamePresentation::colored(const AnmVm* vm,const AttackColorVertex* vertices,u32 count,BattleGeometry mode,bool additive){
    renderer.flush();PipelineState state=renderer.state;state.depthWrite=false;state.fog=false;state.destinationBlend=additive?BlendFactor::One:BlendFactor::InverseSourceAlpha;
    if(vm&&vm->blendMode<2)state.destinationBlend=vm->blendMode?BlendFactor::One:BlendFactor::InverseSourceAlpha;
    state.color.operation=state.alpha.operation=ColorOperation::First;state.color.first=state.alpha.first={ArgumentSource::Diffuse};
    static_assert(sizeof(AttackColorVertex)==sizeof(ColorVertex));backend.draw(state,0,topology(mode),VertexLayout::ScreenColor,vertices,count);
}
void GamePresentation::textured(const AnmVm& vm,const AttackTextureVertex* vertices,u32 count,BattleGeometry mode){
    if(!vm.loadedSprite)return;renderer.prepare(vm);renderer.flush();static_assert(sizeof(AttackTextureVertex)==sizeof(SpriteVertex));backend.draw(renderer.state,vm.loadedSprite->texture,topology(mode),VertexLayout::ScreenColorUv,vertices,count);
}
void GamePresentation::background(Background& bg,const Background& first,i32 side,bool overlay){
    stages.side=side;stages.field_view=viewport(side);stages.geometry={u32(16+320*side),16,{-144,0},288};stages.culling_camera=first.camera;renderer.shake=offsets[side];
    if(overlay)stages.draw_overlay(bg,stage_state[side]);else {stages.clear_positions();stages.draw_base(bg,stage_state[side]);}
}
void GamePresentation::draw_score_popups(){if(world&&world->battle)for(i32 s=0;s<2;++s)ascii.draw_popups(s,world->battle->state.geometry[s],world->battle->fields[s].player->motion.position);}
void GamePresentation::rectangle(float l,float t,float r,float b,u32 color,bool full){if(full)begin_field(2);renderer.rectangle(l,t,r,b,color,color);}
}
