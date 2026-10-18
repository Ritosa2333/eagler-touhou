#include "enemy_mesh.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::gameplay {
namespace {struct Host final:EnemyMeshServices {
    float clock_scale()override{return state::clock_scale;}
    void initialize(sprite::RenderMesh& m,float x,float y,float w,float h)override{sprite::initialize_render_mesh(m,x,y,w,h);}
    int view_offset(int i,unsigned axis)override{return sprite::mesh_environment::view_offset(i,axis);}
    int dimension(unsigned axis)override{return sprite::mesh_environment::scaled_dimension(axis);}
    void uv(const sprite::Vec3& p,float& u,float& v)override{sprite::render_mesh_uv(p.x,p.y,u,v);}
    void update(sprite::RenderMesh& m)override{sprite::update_render_mesh_strips(m);}
};}
EnemyMeshServices& enemy_mesh_services(){static Host host;return host;}
namespace unrecovered {void update_enemy_mesh_004a4190(EnemyState& s){update_enemy_mesh(s);}}
}
