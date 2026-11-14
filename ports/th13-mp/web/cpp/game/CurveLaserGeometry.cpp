#include "CurveLaserGeometry.hpp"
#include "StageMath.hpp"

namespace th13 {
void curve_laser_strip(const std::vector<CurveLaserSample>& samples, float width,
                       float v0, float v1, std::vector<AnmGeometryVertex>& out) {
    out.resize(samples.size()*2);
    float u=0;
    for(size_t i=0;i<samples.size();++i) {
        for(size_t side=0;side<2;++side) {
            const float turn=side==0?1.5707964f:-1.5707964f;
            float angle=float(samples[i].angle+turn);
            if(i) {
                angle=wrap_angle(angle);
                const float previous=wrap_angle(float(samples[i-1].angle+turn));
                float difference=float(previous-angle);
                if(difference>3.1415927f)
                    difference=float(double(previous)-(double(angle)+6.2831855f));
                else if(float(angle-previous)>3.1415927f)
                    difference=float(double(previous)-(double(angle)-6.2831855f));
                angle=float(double(wrap_angle(float(double(wrap_angle(difference))*.5)))+angle);
            }
            float x,y;polar_xy(wrap_angle(angle),float(double(width)*.5),x,y);
            // The two additions have separate f32 stores in436370.
            auto& vertex=out[i*2+side];
            vertex.position={float(double(float(samples[i].position.x+x))+320.0),
                             float(double(float(samples[i].position.y+y))+16.0),0};
            vertex.reciprocal_w=1;vertex.color=0xffffffff;
            vertex.uv={u,side==0?v0:v1};
        }
        if(samples.size()>1)u=float(1.0/double(samples.size()-1)+double(u));
    }
}
}
