#pragma once
#include "enemy_entity.hpp"
#include "../sprite_renderer/render_mesh.hpp"
namespace th20::source::gameplay {
class EnemyMeshServices {
public:
    virtual ~EnemyMeshServices()=default;
    virtual float clock_scale()=0;
    virtual void initialize(sprite::RenderMesh&,float,float,float,float)=0;
    virtual int view_offset(int,unsigned)=0;
    virtual int dimension(unsigned)=0;
    virtual void uv(const sprite::Vec3&,float&,float&)=0;
    virtual void update(sprite::RenderMesh&)=0;
};
void update_enemy_mesh(EnemyState&,EnemyMeshServices&); //4a4190
EnemyMeshServices& enemy_mesh_services();
inline void update_enemy_mesh(EnemyState& s){update_enemy_mesh(s,enemy_mesh_services());}
}
