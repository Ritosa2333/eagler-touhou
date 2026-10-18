#include "ScreenGrid.hpp"
#include "VectorMath.hpp"
#include "AnmVisualState.hpp"
namespace th15 {
namespace {
Vec3 normalized(const Vec3& p)noexcept{const float squared=float(float(float(p.x*p.x)+float(p.y*p.y))+float(p.z*p.z));if(!(squared>=0x1p-46f))return {};const float inverse=float(1.f/float(std::sqrt(double(squared)))),correction=float(3.f-float(float(squared*inverse)*inverse)),refined=float(float(.5f*inverse)*correction);return {float(p.x*refined),float(p.y*refined),float(p.z*refined)};}
void uv(AnmGeometryVertex& v,const Vec3& p,const ScreenGridViewport& view){v.uv={float(p.x/float(view.width)),float(p.y/float(view.height))};if(v.uv.x<0)v.uv.x=0;if(v.uv.y<0)v.uv.y=0;}
}
bool ScreenGrid::initialize(u32 x,u32 y){if(x<2||y<2||x>256||y>256)return false;columns=x;rows=y;vertices.resize(size_t(x)*y);sampling.resize(size_t(x)*y);return true;}
bool ScreenGrid::place(const ScreenGridViewport& view,const Vec2& top_left,float width,float height){if(!columns||!rows||view.width<=0||view.height<=0)return false;const float dx=float(width/float(columns-1)),dy=float(height/float(rows-1));float x=float(float(view.origin_x)+top_left.x);for(u32 column=0;column<columns;column++){float y=float(float(view.origin_y)+top_left.y);for(u32 row=0;row<rows;row++){const u32 index=column*rows+row;auto& v=vertices[index];sampling[index]=v.position={x,y,0};v.reciprocal_w=1;v.color=0xffffffff;uv(v,v.position,view);y=float(y+dy);}x=float(x+dx);}return true;}
bool ScreenGrid::radial(EnemyDistortionState& state,const Vec3& enemy,float rate,const ScreenGridViewport& view){
    if(!state.active)return true;const float radius=state.radius;if(state.target>radius)state.radius=float(radius+float(rate*2.f));
    const Vec2 origin{float(float(enemy.x-radius)-20.f),float(float(enemy.y-radius)-20.f)};const float extent=float(float(radius*2.f)+40.f);if(!place(view,origin,extent,extent))return false;
    const Vec3 center{float(float(float(view.width)*.5f)+enemy.x),float(float(view.origin_y)+enemy.y),enemy.z};float px=state.phase_x,py=state.phase_y;const float squared=float(radius*radius);
    for(u32 index=0;index<vertices.size();index++){
        auto& v=vertices[index];auto& sample=sampling[index];Vec3 delta{float(sample.x-center.x),float(sample.y-center.y),float(sample.z-center.z)};
        const float distance=float(float(delta.y*delta.y)+float(delta.x*delta.x)),difference=float(squared-distance);
        if(difference>=0){const float weight=float(difference/squared);u32 color=0xff000000;for(u32 channel=0;channel<3;channel++){const u32 original=(state.color>>(channel*8))&255;const float faded=float(255.f-float(float(255-original)*weight));color|=u32(u8(truncate_int(faded)))<<(channel*8);}v.color=color;const float strength=float(weight*32.f);delta=normalized(delta);const float x=float(float(strength*delta.x)+float(float(float(std::sin(double(px)))*weight)*8.f)),y=float(float(strength*delta.y)+float(float(float(std::sin(double(py)))*weight)*8.f));v.position.x=float(v.position.x+x);v.position.y=float(v.position.y+y);v.position.z=sample.z=0;}
        else v.color&=0x00ffffffu;
        px=normalize_angle(float(px+.098174773156642913818359375f));py=normalize_angle(float(py-.0490873865783214569091796875f));
        if(float(view.left)>=sample.x)v.position.x=sample.x=float(float(view.left)+1.f);else{const float right=float(float(view.left)+384.f);if(sample.x>=right)v.position.x=sample.x=float(right-1.f);}
        if(float(view.origin_y)>=sample.y)v.position.y=sample.y=float(float(view.origin_y)+1.f);else{const float bottom=float(float(view.origin_y)+448.f);if(sample.y>=bottom)v.position.y=sample.y=float(bottom-1.f);}
        uv(v,sample,view);
    }
    state.phase_x=normalize_angle(float(state.phase_x+float(rate*.19634954631328582763671875f)));state.phase_y=normalize_angle(float(state.phase_y+float(rate*.098174773156642913818359375f)));return true;
}
bool ScreenGrid::strip(u32 column,std::vector<AnmGeometryVertex>& out)const{if(column+1>=columns)return false;out.resize(size_t(rows)*2);for(u32 row=0;row<rows;row++){out[2*row]=vertices[column*rows+row];out[2*row+1]=vertices[(column+1)*rows+row];}return true;}
}
