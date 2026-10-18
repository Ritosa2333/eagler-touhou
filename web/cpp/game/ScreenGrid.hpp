#pragma once
#include "AnmGeometry.hpp"
namespace th15 {
struct ScreenGridViewport {i32 width=640,height=480,origin_x=320,origin_y=16,left=128;};
struct EnemyDistortionState {float target=0,radius=16;u32 color=0;float phase_x=0,phase_y=0;bool active=false;};
// CPU geometry for captured-screen effects. Strips retain the original
// column/row order and can be submitted through ZunGraphics without a device ABI.
class ScreenGrid {
    u32 columns=0,rows=0;
public:
    std::vector<AnmGeometryVertex> vertices;std::vector<Vec3> sampling;
    bool initialize(u32 columns,u32 rows);
    bool place(const ScreenGridViewport&,const Vec2& top_left,float width,float height);
    bool radial(EnemyDistortionState&,const Vec3& enemy,float rate,const ScreenGridViewport&);
    bool strip(u32 column,std::vector<AnmGeometryVertex>& out)const;
    u32 column_count()const noexcept{return columns;}u32 row_count()const noexcept{return rows;}
};
}
