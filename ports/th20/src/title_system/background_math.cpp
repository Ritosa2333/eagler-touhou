#include "background.hpp"
#include "data.hpp"
#include "../ecl_vm/math.hpp"
#include <cmath>
#include <algorithm>
#include <xmmintrin.h>
namespace th20::source::title {
std::uint32_t shade_component(int component,float weight,float direction){
    const auto value=static_cast<float>(component);const auto scaled=value-((value*direction)/data::f_0056c8cc)*weight;
    return static_cast<unsigned>(std::clamp(_mm_cvtt_ss2si(_mm_set_ss(scaled)),0,255))&255;
}
void deform_background(TitleInf& o,state::Random& random){
    auto& mesh=*o.mesh;auto* v=mesh.vertices;
    for(int column=0;column<mesh.columns;++column)for(int row=0;row<mesh.rows;++row,++v){
        const float diagonal=v->x-v->y;const float distance=std::fabs(o.wave-diagonal);
        const float weight=distance>data::f_0056cdb0?0.f:(data::f_0056cdb0-distance)/data::f_0056cdb0;
        const float angle=ecl::math::wrap_angle(((data::f_0056e0f0*data::f_0056c8d0)*((o.wave-diagonal)/data::f_00570560))/data::f_0056e048);
        const float x=-ecl::math::sine(angle),y=-ecl::math::cosine(angle);
        if(column!=0&&column!=mesh.columns-1&&row!=0&&row!=mesh.rows-1){v->x+=x*data::f_0056f280*weight;v->y+=y*data::f_0056f280*weight;v->z+=0.f;}
        const float direction=-x+y;
        const auto red=shade_component(static_cast<int>(o.color[2]),weight,direction),green=shade_component(static_cast<int>(o.color[1]),weight,direction),blue=shade_component(static_cast<int>(o.color[0]),weight,direction);
        v->color=blue|(green<<8)|(red<<16)|((o.color[3]&255)<<24);
    }
    o.wave+=data::f_0056d7bc;
    if(o.wave>data::f_00575698)o.wave-=static_cast<float>(state::next(random)%1000u)+data::f_0057569c;
}
}
