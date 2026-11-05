#include "render_mesh.hpp"
#include "pool.hpp"
#include "named_spawn.hpp"
#include "binding.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "../core_scheduler/scheduler.hpp"
#include <cstring>
#include <new>
namespace th20::source::sprite {
namespace n=th20::recovered;namespace e=mesh_environment;namespace q=scheduler;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
void update_child_flags(Animation& a,bool remove){if(remove)a.fields_550[8]=1;else a.base.flags[1]&=~1u;
    auto* first=a.links[3].next;if(first){q::Iterator iterator(reinterpret_cast<q::Link*>(first));while(iterator.current){auto* child=reinterpret_cast<Animation*>(iterator.current->value);update_child_flags(*child,remove);iterator.advance();}}
}
}
void hide_animation_tree(Animation& a){update_child_flags(a,false);}
void request_animation_deletion(Animation* a){if(a)update_child_flags(*a,true);}
void request_animation_deletion(Controller& c,std::uint32_t& handle){request_animation_deletion(find_animation(c,handle));handle=0;}
void select_render_mesh_context(RenderMesh& mesh,std::int32_t index){mesh.view_index=index;mesh.context=&e::context(index);}
std::uint32_t create_surface_strip(Controller& c,AnimationFile& file,std::int32_t rows,std::int32_t script,bool alternate){
    auto handle=spawn_named_animation(c,file,nullptr,script,alternate?42:41);auto* a=resolve_animation_handle(c,handle);
    const auto bytes=static_cast<std::uint32_t>(rows)*56;a->fields_550[7]=bytes;a->fields_550[5]=reinterpret_cast<std::uint32_t>(runtime::allocate_bytes(bytes));
    if(rows<3)a->base.flags[0]&=0xffffff00u;else {
        a->base.flags[0]=(a->base.flags[0]&0xffffff00u)|12;if(!alternate)a->base.flags[2]|=0x30;
        a->base.fields_444[0]=rows;auto* vertices=reinterpret_cast<Vertex28*>(a->fields_550[5]);
        for(std::int32_t i=0;i<static_cast<std::int32_t>(static_cast<std::uint32_t>(rows)*2);++i){vertices[i].z=0;vertices[i].rhw=1;vertices[i].color=0xffffffffu;}
    }return handle;
}
void construct_render_mesh(RenderMesh& mesh,std::int32_t columns,std::int32_t rows,std::int32_t surface_mode,std::int32_t view_index){
    mesh={};if(!e::enabled())return;select_render_mesh_context(mesh,view_index);mesh.columns=columns;mesh.rows=rows;
    // Preserve the original allocation sizes, including columns*4-1.
    mesh.strip_handles=static_cast<std::uint32_t*>(runtime::allocate_bytes(static_cast<std::uint32_t>(columns)*4-1));mesh.strips=static_cast<Animation**>(runtime::allocate_bytes(static_cast<std::uint32_t>(columns)*4-1));
    mesh.vertices=static_cast<Vertex28*>(runtime::allocate_bytes(static_cast<std::uint32_t>(columns)*28*static_cast<std::uint32_t>(rows)));mesh.positions=static_cast<Vec3*>(runtime::allocate_bytes(static_cast<std::uint32_t>(columns)*12*static_cast<std::uint32_t>(rows)));
    auto& c=e::controller();auto& file=e::surface_animation();const auto script=surface_mode?(e::scaled_dimension(0)==640?13:e::scaled_dimension(0)==960?14:15):0;
    mesh.root_handle=create_surface_strip(c,file,2,script,surface_mode!=0);auto* root=resolve_animation_handle(c,mesh.root_handle);root->field_5c8=reinterpret_cast<std::uint32_t>(&mesh);hide_animation_tree(*root);if(surface_mode)set_animation_layer(*root,27);
    for(std::int32_t i=0;i<static_cast<std::int32_t>(static_cast<std::uint32_t>(columns)-1);++i){mesh.strip_handles[i]=create_surface_strip(c,file,rows,script,surface_mode!=0);mesh.strips[i]=resolve_animation_handle(c,mesh.strip_handles[i]);auto& a=*mesh.strips[i];a.base.flags[0]&=0xffff00ffu;a.base.flags[2]&=~0x3000000u;if(surface_mode)set_animation_layer(a,27);}
}
RenderMesh* create_render_mesh(std::int32_t columns,std::int32_t rows,std::int32_t surface_mode,std::int32_t view_index){auto* mesh=static_cast<RenderMesh*>(::operator new(sizeof(RenderMesh)));std::memset(mesh,0,sizeof(*mesh));try{construct_render_mesh(*mesh,columns,rows,surface_mode,view_index);}catch(...){::operator delete(mesh);throw;}return mesh;}
void destroy_render_mesh_contents(RenderMesh& mesh){
    request_animation_deletion(e::controller(),mesh.root_handle);
    if(mesh.vertices){runtime::release_bytes(mesh.vertices);mesh.vertices=nullptr;}if(mesh.positions){runtime::release_bytes(mesh.positions);mesh.positions=nullptr;}
    for(std::int32_t i=0;i<static_cast<std::int32_t>(static_cast<std::uint32_t>(mesh.columns)-1);++i)request_animation_deletion(e::controller(),mesh.strip_handles[i]);
    if(mesh.strip_handles){runtime::release_bytes(mesh.strip_handles);mesh.strip_handles=nullptr;}if(mesh.strips){runtime::release_bytes(mesh.strips);mesh.strips=nullptr;}
}
void destroy_render_mesh(RenderMesh* mesh){if(mesh){destroy_render_mesh_contents(*mesh);std::lock_guard guard(runtime::shared_locks().slot(1));::operator delete(mesh);}}
void render_mesh_uv(float x,float y,float& u,float& v){u=div(x,n::int_float(e::scaled_dimension(0)));v=div(y,n::int_float(e::scaled_dimension(1)));if(u<0.f)u=0;if(v<0.f)v=0;}
void update_render_mesh_strips(RenderMesh& mesh){if(mesh.columns){auto* vertex=mesh.vertices;for(std::int32_t col=0;col<static_cast<std::int32_t>(static_cast<std::uint32_t>(mesh.columns)-1);++col){auto* out=reinterpret_cast<Vertex28*>(mesh.strips[col]->fields_550[5]);for(std::int32_t row=0;row<mesh.rows;++row){std::memcpy(out,vertex,sizeof(Vertex28));std::memcpy(out+1,vertex+mesh.rows,sizeof(Vertex28));out+=2;++vertex;}}}}
void initialize_render_mesh(RenderMesh& mesh,float x,float y,float width,float height){
    if(!mesh.columns)return;float px=n::add32(n::int_float(e::view_offset(mesh.view_index,0)),x),py=n::add32(n::int_float(e::view_offset(mesh.view_index,1)),y);
    const float count_x=sub(n::int_float(mesh.columns),1.f),count_y=sub(n::int_float(mesh.rows),1.f);auto* vertex=mesh.vertices;auto* position=mesh.positions;
    for(std::int32_t col=0;col<mesh.columns;++col){for(std::int32_t row=0;row<mesh.rows;++row){*position={px,py,0};std::memcpy(vertex,position,sizeof(Vec3));render_mesh_uv(position->x,position->y,vertex->u,vertex->v);vertex->rhw=1;vertex->color=0xffffffffu;py=n::add32(py,div(height,count_y));++position;++vertex;}py=n::add32(n::int_float(e::view_offset(mesh.view_index,1)),y);px=n::add32(px,div(width,count_x));}update_render_mesh_strips(mesh);
}
void initialize_display_render_mesh(RenderMesh& mesh,float x,float y,float width,float height){
    if(!mesh.columns)return;float px=x;const float count_x=sub(n::int_float(mesh.columns),1.f),count_y=sub(n::int_float(mesh.rows),1.f);auto* vertex=mesh.vertices;auto* position=mesh.positions;
    for(std::int32_t col=0;col<mesh.columns;++col){float py=y;for(std::int32_t row=0;row<mesh.rows;++row){*position={px,py,0};std::memcpy(vertex,position,sizeof(Vec3));render_mesh_uv(position->x,position->y,vertex->u,vertex->v);vertex->x=n::add32(n::int_float(e::display_offset(0)),vertex->x);vertex->y=n::add32(n::int_float(e::display_offset(1)),vertex->y);vertex->rhw=1;vertex->color=0xffffffffu;py=n::add32(py,div(height,count_y));++position;++vertex;}px=n::add32(px,div(width,count_x));}update_render_mesh_strips(mesh);
}
}
