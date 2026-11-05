#pragma once
#include "ZunGraphics.hpp"
#include "Camera.hpp"
#include <array>
#include <vector>
namespace th09 {
class AnmRenderer {
    ZunGraphics& backend;std::vector<SpriteVertex> batch;PipelineState batch_state;u32 batch_texture=0;
    void unrotated(const AnmVm&,bool write_z);
    void rotated(const AnmVm&);
    bool drawable(const AnmVm&)const;
    i32 finish(AnmVm&,bool round,bool mirror=false,bool keep_color=false);
    void append(const SpriteVertex*);
public:
    explicit AnmRenderer(ZunGraphics&);
    // Historical ticks still execute title-owned Draw mutations. These 2D
    // sprite entry points only produce renderer scratch/vertices, so callers
    // suppressing GPU submission may omit their geometry as well.
    bool omit_sprite_geometry=false;
    PipelineState state;Viewport view;Vec2 shake;u32 tint=0x80808080;bool tint_enabled=false;
    std::array<SpriteVertex,4> quad;
    u32 texture=0;u8 blend_mode=3,depth_disabled=0xff;
    Camera camera;Matrix4 last_world,texture_matrix;const AnmLoadedSprite* current_sprite=nullptr;
    BackgroundCamera background_camera_data;u8 camera_mode=0xff;
    std::array<Vec3,4> world_vertices{{{-128,-128,0},{128,-128,0},{-128,128,0},{128,128,0}}};
    void flush();
    void set_viewport(const Viewport& v){flush();view=v;backend.viewport(v);}
    void transform(MatrixKind kind,const Matrix4& value){flush();backend.transform(kind,value);}
    void clear(bool color,bool depth,u32 rgba,float z=1){flush();backend.clear(color,depth,rgba,z);}
    void prepare(const AnmVm&);
    u32 color(const AnmVm&)const;
    i32 draw_no_rotation(AnmVm&,bool round=true,bool mirror=false);
    i32 draw_2d(AnmVm&,bool no_round=false);
    i32 draw_quad(AnmVm&,const SpriteVertex*);
    i32 draw_strip(AnmVm&,const SpriteVertex*,u32);
    Matrix4 world_matrix(AnmVm&);
    void transform_world(AnmVm&);
    i32 draw_world(AnmVm&);
    i32 draw_facing_camera(AnmVm&);
    i32 draw_3d(AnmVm&);
    void screen_camera(){flush();camera.screen(view);backend.transform(MatrixKind::View,camera.view);backend.transform(MatrixKind::Projection,camera.projection);camera_mode=0;}
    void scene_camera(const BackgroundCamera& c){flush();background_camera_data=c;camera.scene(view,c);backend.transform(MatrixKind::View,camera.view);backend.transform(MatrixKind::Projection,camera.projection);camera_mode=1;}
    void draw_colors(AnmVm&,Topology,const ColorVertex*,u32);
    void draw_lines(const ColorVertex*,u32);
    void rectangle(float left,float top,float right,float bottom,u32 top_color,u32 bottom_color);
    const std::vector<SpriteVertex>& pending()const{return batch;}
};
}
