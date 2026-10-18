#pragma once
#include "sprite.hpp"
#include "../game_session/session.hpp"
namespace th20::source::sprite {
struct RenderMesh {
    std::int32_t columns,rows;
    std::uint32_t root_handle;
    std::uint32_t* strip_handles;
    Animation** strips;
    Vertex28* vertices;
    Vec3* positions;
    std::int32_t view_index;
    game_session::Context* context;
};
static_assert(sizeof(RenderMesh)==0x24 && offsetof(RenderMesh,positions)==0x18);
namespace mesh_environment {
bool enabled(); // graphics+19c != null
Controller& controller();
AnimationFile& surface_animation(); // graphics+b40
game_session::Context& context(std::int32_t);
std::int32_t view_offset(std::int32_t index,unsigned axis); // window+38/+40
std::int32_t display_offset(unsigned axis); // graphics.viewports[2]+fc/+100
std::int32_t scaled_dimension(unsigned axis); // window+20a0/+20a4
}
void select_render_mesh_context(RenderMesh&,std::int32_t); //49e050
void construct_render_mesh(RenderMesh&,std::int32_t columns,std::int32_t rows,std::int32_t surface_mode,std::int32_t view_index); //49cf80
RenderMesh* create_render_mesh(std::int32_t columns,std::int32_t rows,std::int32_t surface_mode,std::int32_t view_index); //4712d0
void destroy_render_mesh_contents(RenderMesh&); //471ca0, reached via4713b0→4720d0
void destroy_render_mesh(RenderMesh*); //471210, preserves caller pointer
void update_render_mesh_strips(RenderMesh&); //49ddc0
void initialize_render_mesh(RenderMesh&,float x,float y,float width,float height); //49d850
void initialize_display_render_mesh(RenderMesh&,float x,float y,float width,float height); //49d5b0
void render_mesh_uv(float x,float y,float& u,float& v); //49df40, z argument unused
std::uint32_t create_surface_strip(Controller&,AnimationFile&,std::int32_t rows,std::int32_t script,bool alternate); //4de430/4de360
void hide_animation_tree(Animation&); //450160
void request_animation_deletion(Animation*); //44fb80
void request_animation_deletion(Controller&,std::uint32_t& handle); //44fca0/44fcd0
}
