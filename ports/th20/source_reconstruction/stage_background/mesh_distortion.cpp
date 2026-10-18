#include "update.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../ecl_vm/math.hpp"
#include <bit>
namespace th20::source::background {
namespace n=th20::recovered;namespace m=th20::source::ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
constexpr float pi=std::bit_cast<float>(0x40490fdbu);
}
void update_mesh_distortion(ScriptState& s,const float* rate){
    for(unsigned i=0;i<2;++i){
        auto* mesh=reinterpret_cast<sprite::RenderMesh*>(s.fields_3294[9+i]);if(!mesh)continue;
        float phase_x=s.mesh_phase_x[i],phase_y=s.mesh_phase_y[i];
        if(s.mesh_mode!=1&&s.mesh_mode!=2)continue;
        sprite::initialize_render_mesh(*mesh,-192.f,0.f,384.f,s.mesh_mode==1?128.f:448.f);
        auto* vertex=mesh->vertices;auto* position=mesh->positions;
        for(int column=0;column<mesh->columns;++column){
            for(int row=0;row<mesh->rows;++row){
                vertex->color=(vertex->color&0xffffffu)|0xc0000000u;
                const float amplitude=sub(24.f,div(n::mul32(n::int_float(row),24.f),n::int_float(mesh->rows-1)));
                const float x=n::mul32(m::sine(phase_x),amplitude),y=n::mul32(m::sine(phase_y),amplitude);
                if(column&&row&&column!=mesh->columns-1&&row!=mesh->rows-1){vertex->x=n::add32(vertex->x,x);vertex->y=n::add32(vertex->y,y);vertex->z=0.f;position->z=0.f;}
                phase_x=m::wrap_angle(n::add32(phase_x,div(pi,4.7f)));++vertex;++position;
            }
            phase_y=m::wrap_angle(sub(phase_y,div(pi,2.1f)));
        }
        s.mesh_phase_x[i]=m::wrap_angle(n::add32(s.mesh_phase_x[i],div(pi,64.f)));
        s.mesh_phase_y[i]=m::wrap_angle(n::add32(s.mesh_phase_y[i],div(pi,80.f)));
        if(s.mesh_mode==2)n::timer_tick(s.mesh_timers[i],rate);
    }
}
}
