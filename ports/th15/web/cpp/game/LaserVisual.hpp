#pragma once
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "BulletAppearance.hpp"
#include "LaserMotion.hpp"
#include "CurveMotion.hpp"
#include "CurveGeometry.hpp"
namespace th15 {
class LaserVisual final:public AnmSpriteSource {
    AnmManager& animations;Rng& random;i32 resource_id,type=0,color=0;bool curve_source=false;std::vector<AnmGeometryVertex> curve_vertices;
    bool body_initialize();bool endpoint_initialize(AnmVm&,i32 script,bool interrupt,bool blend);bool tick(AnmVm&);
public:
    AnmVm body,head,tip;std::string error;
    LaserVisual(AnmManager& a,Rng& rng,i32 id):animations(a),random(rng),resource_id(id){}
    bool retain_children_on_retire=false;
    ~LaserVisual(){if(retain_children_on_retire){animations.registry.preserve_embedded_children(body);animations.registry.preserve_embedded_children(head);animations.registry.preserve_embedded_children(tip);}else{animations.registry.destroy_tree(body);animations.registry.destroy_tree(head);animations.registry.destroy_tree(tip);}}
    i32 sprite_index(i32 parameter)const noexcept override{return curve_source?wrapping_add(color,523):bullet_animation_parameter(type,color,parameter);}
    bool initialize_moving(i32 sprite,i32 colour);
    bool initialize_stationary(i32 sprite,i32 colour);
    bool initialize_curved(i32 sprite,i32 colour);
    bool update_curved();
    bool draw_curved(const CurveLaserMotion&,AnmRenderer&);
    bool rebind_body(i32 sprite,i32 colour);
    bool update(float width,float length,float traveled,bool moving);
    bool draw_moving(const MovingLaserMotion&,AnmRenderer&);
    bool draw_stationary(const StationaryLaserMotion&,AnmRenderer&);
};
}
