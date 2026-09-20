// Recovered against TH08 1.00d; algorithm names and layout also informed by
// the MIT reference credited in cpp/licenses/th08-reference-MIT.txt.
#include "AnmRenderer.hpp"
#include "PresentationAudit.hpp"
#include "GameMath.hpp"
#include "GraphicsMath.hpp"
#include <cmath>
#include "../../../portable/numeric/SpriteNumber.hpp"
#include <algorithm>

namespace th08 {
AnmRenderer::AnmRenderer(SpriteBackend& output) : backend(output) {
    pending.reserve(6 * 4096);
    view_matrix.identity(); projection_matrix.identity(); last_world_matrix.identity();
    for (u32 i=0;i<4;++i) {
        const Vec2 uv{float(i&1),float((i>>1)&1)};
        world_quad[i]={world_vertices[i],uv};
        colored_world_quad[i]={world_vertices[i],0,uv};
    }
}

bool AnmRenderer::drawable(const AnmVm& vm) const noexcept {
    return vm.visible && vm.flag1 && vm.color1.a != 0;
}

u32 AnmRenderer::vm_color(const AnmVm& vm) const noexcept {
    u32 color=vm.flag17?vm.color2.d3dColor:vm.color1.d3dColor;
    if (mix_enabled) {
        u32 result=0;
        for(u32 shift=0;shift<32;shift+=8) {
            const u32 channel=((color>>shift)&255)*((mix_color>>shift)&255)/128;
            result|=std::min(channel,255u)<<shift;
        }
        color=result;
    }
    return color;
}

void AnmRenderer::unrotated_vertices(AnmVm& vm, bool write_z) {
    auto evaluate=[&](auto number){
        using N=decltype(number(0));
        auto sum_values=[&](float a,float b,float c){return number(a)+number(b)+number(c);};
        auto write_depth=[](auto* q,N z){for(unsigned i=0;i<4;++i)q[i].position.z=z.to_float();};

    const float half_width = (number(vm.spriteSize.x) * number(vm.scale.x) / number(2)).to_float();
    const float half_height = (number(vm.spriteSize.y) * number(vm.scale.y) / number(2)).to_float();
    const auto x = number(vm.pos.x), y = number(vm.pos.y);
    const auto w = number(half_width), h = number(half_height);
    quad[0].pos.x = quad[2].pos.x = (vm.anchor & 1) ? vm.pos.x : (x - w).to_float();
    quad[1].pos.x = quad[3].pos.x = (vm.anchor & 1) ? (w + x + w).to_float() : (w + x).to_float();
    quad[0].pos.y = quad[1].pos.y = (vm.anchor & 2) ? vm.pos.y : (y - h).to_float();
    quad[2].pos.y = quad[3].pos.y = (vm.anchor & 2) ? (h + y + h).to_float() : (h + y).to_float();
    if (write_z) for (auto& vertex : quad) vertex.pos.z = vm.pos.z;

    };
    if(arithmetic_precision()==Precision::Single&&arithmetic_rounding()==Rounding::NearestEven&&touhou::numeric::sprite_range({vm.spriteSize.x,vm.spriteSize.y,vm.scale.x,vm.scale.y,vm.pos.x,vm.pos.y}))
        return evaluate([](float v){return touhou::numeric::SpriteNumber(v);});
    return evaluate([](float v){return th08::number(v);});
}

i32 AnmRenderer::draw_no_rotation(AnmVm& vm, bool round) {
    if (!drawable(vm)) return -1;
    unrotated_vertices(vm, true);
    return draw_inner(vm, round ? 1 : 0);
}

void AnmRenderer::translate_rotation(SpriteVertex& vertex, float x, float y,
                                     float sin, float cos, float offset_x, float offset_y) {
    auto evaluate=[&](auto number){
        using N=decltype(number(0));
        auto sum_values=[&](float a,float b,float c){return number(a)+number(b)+number(c);};
        auto write_depth=[](auto* q,N z){for(unsigned i=0;i<4;++i)q[i].position.z=z.to_float();};

    vertex.pos.x = (number(x) * number(cos) - number(y) * number(sin) + number(offset_x)).to_float();
    vertex.pos.y = (number(x) * number(sin) + number(y) * number(cos) + number(offset_y)).to_float();

    };
    if(arithmetic_precision()==Precision::Single&&arithmetic_rounding()==Rounding::NearestEven&&touhou::numeric::sprite_range({x,y,sin,cos,offset_x,offset_y}))
        return evaluate([](float v){return touhou::numeric::SpriteNumber(v);});
    return evaluate([](float v){return th08::number(v);});
}

i32 AnmRenderer::draw_2d(AnmVm& vm, bool no_round) {
    if (!no_round && vm.rotation.z == 0) return draw_no_rotation(vm);
    if (!drawable(vm)) return -1;
    if (vm.rotation.z == 0) {
        // 0x463470 deliberately retains the preceding quad's Z in this branch.
        unrotated_vertices(vm, false);
    } else {
        const float sin = sine(vm.rotation.z).to_float(), cos = cosine(vm.rotation.z).to_float();
        const float w = (number(vm.spriteSize.x) * number(vm.scale.x) / number(2)).to_float();
        const float h = (number(vm.spriteSize.y) * number(vm.scale.y) / number(2)).to_float();
        translate_rotation(quad[0], -w, -h, sin, cos, vm.pos.x, vm.pos.y);
        translate_rotation(quad[1], w, -h, sin, cos, vm.pos.x, vm.pos.y);
        translate_rotation(quad[2], -w, h, sin, cos, vm.pos.x, vm.pos.y);
        translate_rotation(quad[3], w, h, sin, cos, vm.pos.x, vm.pos.y);
        for (auto& vertex : quad) {
            vertex.pos.z = vm.pos.z;
            if (vm.anchor & 1) vertex.pos.x = Scalar::add(vertex.pos.x,w);
            if (vm.anchor & 2) vertex.pos.y = Scalar::add(vertex.pos.y,h);
        }
    }
    return draw_inner(vm, 0);
}

Matrix4 AnmRenderer::world_matrix(AnmVm& vm) {
    if (!vm.flag16 && (vm.updateScale || vm.updateRotation)) {
        vm.matrix2=vm.matrix1;
        vm.matrix2.m[0][0]=Scalar::mul(vm.matrix2.m[0][0],vm.scale.x);
        vm.matrix2.m[1][1]=Scalar::mul(vm.matrix2.m[1][1],vm.scale.y);
        vm.updateScale=false;
        const float angles[]{vm.rotation.x,vm.rotation.y,vm.rotation.z};
        for (u32 axis=0; axis<3; ++axis) if (angles[axis]!=0) {
            Matrix4 rotation; GraphicsMath::rotation(rotation,axis,angles[axis]);
            GraphicsMath::multiply(vm.matrix2,vm.matrix2,rotation);
        }
        vm.updateRotation=false;
    }
    Matrix4 world=vm.matrix2;
    world.m[3][0]=vm.pos.x; world.m[3][1]=vm.pos.y; world.m[3][2]=vm.pos.z;
    if (vm.anchor&1) {
        const float half=(number(vm.spriteSize.x)*number(vm.scale.x)/number(2)).to_float();
        world.m[3][0]=(number(std::fabs(half))+number(vm.pos.x)).to_float();
    }
    if (vm.anchor&2) {
        const float half=(number(vm.spriteSize.y)*number(vm.scale.y)/number(2)).to_float();
        world.m[3][1]=(number(std::fabs(half))+number(vm.pos.y)).to_float();
    }
    return world;
}

void AnmRenderer::transform_world(AnmVm& vm) {
    const auto world=world_matrix(vm);
    for (u32 i=0; i<4; ++i) GraphicsMath::project(quad[i].pos,world_vertices[i],&viewport,&projection_matrix,&view_matrix,&world);
    last_world_matrix=world;
}

i32 AnmRenderer::draw_world(AnmVm& vm) {
    if (!drawable(vm)) return -1;
    transform_world(vm);
    return draw_inner(vm,0);
}

i32 AnmRenderer::transform_facing_camera(AnmVm& vm, ProjectionCallback callback, void* context) {
    const float sin=sine(vm.rotation.z).to_float(),cos=cosine(vm.rotation.z).to_float();
    Matrix4 world; world.identity();
    world.m[3][0]=vm.pos.x; world.m[3][1]=vm.pos.y; world.m[3][2]=vm.pos.z;
    Vec3 projected,reference;
    GraphicsMath::project(projected,{},&viewport,&projection_matrix,&view_matrix,&world);
    if (projected.z<0 || projected.z>1) return -1;
    GraphicsMath::project(reference,scene_camera.right,&viewport,&projection_matrix,&view_matrix,&world);
    const float dx=Scalar::sub(reference.x,projected.x);
    const float dy=Scalar::sub(reference.y,projected.y);
    const float dz=Scalar::sub(reference.z,projected.z);
    const float square=(number(dx)*number(dx)+number(dy)*number(dy)+number(dz)*number(dz)).to_float();
    const float half=(number(square).square_root()*number(.5f)).to_float();
    const float w=(number(half)*number(vm.spriteSize.x)*number(vm.scale.x)).to_float();
    const float h=(number(half)*number(vm.spriteSize.y)*number(vm.scale.y)).to_float();
    if (callback) callback(vm,projected,context);
    translate_rotation(quad[0],-w,-h,sin,cos,projected.x,projected.y);
    translate_rotation(quad[1],w,-h,sin,cos,projected.x,projected.y);
    translate_rotation(quad[2],-w,h,sin,cos,projected.x,projected.y);
    translate_rotation(quad[3],w,h,sin,cos,projected.x,projected.y);
    for (auto& vertex:quad) {
        vertex.pos.z=projected.z;
        if (vm.anchor&1) vertex.pos.x=Scalar::add(vertex.pos.x,w);
        if (vm.anchor&2) vertex.pos.y=Scalar::add(vertex.pos.y,h);
    }
    return 0;
}

i32 AnmRenderer::draw_facing_camera(AnmVm& vm, ProjectionCallback callback, void* context) {
    if (!drawable(vm) || transform_facing_camera(vm,callback,context)!=0) return -1;
    return draw_inner(vm,0);
}

void AnmRenderer::draw_player_bullet(AnmVm& vm) {
    switch (vm.playerBulletHitAnimationType) {
    case 0: draw_no_rotation(vm); break;
    case 1: draw_no_rotation(vm,false); break;
    case 2: draw_2d(vm); break;
    case 3: draw_2d(vm,true); break;
    case 4: draw_facing_camera(vm); break;
    case 5: draw_world(vm); break;
    }
}

i32 AnmRenderer::draw_inner(AnmVm& vm, u32 flags) {
    if (!vm.loadedSprite) return -1;
    for (auto& vertex : quad) {
        vertex.pos.x = Scalar::add(vertex.pos.x,shake.x);
        vertex.pos.y = Scalar::add(vertex.pos.y,shake.y);
    }
    if (flags & 1) {
        // Preserve subpixel motion for all sprites while retaining the original
        // half-pixel raster convention and the current quad's depth.
        const auto raster_position = [](float value) { return (number(value) - number(.5f)).to_float(); };
        quad[0].pos.x = quad[2].pos.x = raster_position(quad[0].pos.x);
        quad[1].pos.x = quad[3].pos.x = raster_position(quad[1].pos.x);
        quad[0].pos.y = quad[1].pos.y = raster_position(quad[0].pos.y);
        quad[2].pos.y = quad[3].pos.y = raster_position(quad[2].pos.y);
    }
    const auto& sprite = *vm.loadedSprite;
    quad[0].uv.x = quad[2].uv.x = Scalar::add(sprite.uvStart.x,vm.uvScrollPos.x);
    quad[1].uv.x = quad[3].uv.x = Scalar::add(sprite.uvEnd.x,vm.uvScrollPos.x);
    quad[0].uv.y = quad[1].uv.y = Scalar::add(sprite.uvStart.y,vm.uvScrollPos.y);
    quad[2].uv.y = quad[3].uv.y = Scalar::add(sprite.uvEnd.y,vm.uvScrollPos.y);
    float max_x = std::max(quad[0].pos.x, quad[1].pos.x), min_x = std::min(quad[0].pos.x, quad[1].pos.x);
    float max_y = std::max(quad[0].pos.y, quad[1].pos.y), min_y = std::min(quad[0].pos.y, quad[1].pos.y);
    for (u32 i = 2; i < 4; ++i) {
        max_x = std::max(quad[i].pos.x, max_x); min_x = std::min(quad[i].pos.x, min_x);
        max_y = std::max(quad[i].pos.y, max_y); min_y = std::min(quad[i].pos.y, min_y);
    }
    if (number(max_x) < Extended::from_int64(viewport.x) || number(max_y) < Extended::from_int64(viewport.y) ||
        Extended::from_int64(viewport.x + viewport.width) < number(min_x) ||
        Extended::from_int64(viewport.y + viewport.height) < number(min_y)) return 0;
    if (current_texture != sprite.texture) {
        current_texture = sprite.texture;
        flush();
        backend.bind_texture(current_texture);
    }
    if (current_shader != 1) { flush(); current_shader = 1; }
    if (!(flags & 2)) {
        const u32 color=vm_color(vm);
        for (auto& vertex : quad) vertex.color = color;
    }
    set_state(vm);
    TH08_AUDIT_SPRITE(vm,quad,4,shake);
    add_quad(quad);
    return 0;
}

void AnmRenderer::set_state(AnmVm& vm) {
    if (current_blend != vm.blendMode) {
        flush();
        current_blend = vm.blendMode;
        if (current_blend == 0) backend.destination_blend(Blends::InverseSourceAlpha);
        else if (current_blend == 1) backend.destination_blend(Blends::One);
    }
    if (!depth_test_disabled && disable_z_write != vm.zWriteDisabled) {
        disable_z_write = vm.zWriteDisabled;
        flush(); // Supervisor::SetRenderState flushes before changing depth writes.
        backend.write_depth(!disable_z_write);
    }
    ++state_changes;
}

void AnmRenderer::add_quad(const SpriteVertex* vertices) {
    for (const u32 index : {0u, 1u, 2u, 1u, 2u, 3u}) pending.push_back(vertices[index]);
}

void AnmRenderer::flush() {
    if (pending.empty()) return;
    backend.triangles(pending.data(), pending.size());
    pending.clear();
    ++flushes;
}

void AnmRenderer::clear() { pending.clear(); }

void AnmRenderer::background_camera(SceneCamera& value) {
    Camera::scene(view_matrix,projection_matrix,viewport,value);scene_camera=value;
    backend.transform(Matrices::View,view_matrix);backend.transform(Matrices::Projection,projection_matrix);camera_mode=1;
}
void AnmRenderer::screen_camera(){Camera::screen(view_matrix,projection_matrix,viewport);backend.transform(Matrices::View,view_matrix);backend.transform(Matrices::Projection,projection_matrix);camera_mode=0;}
void AnmRenderer::begin_background(){clear();current_blend=3;current_shader=disable_z_write=current_color_op=camera_mode=255;current_sprite=nullptr;current_texture=0;state_changes=flushes=0;}

void AnmRenderer::draw_dialogue_background(float left,float top,float right,float bottom) {
    const UntexturedVertex vertices[4]={{{left,top,0},1,0xd0000000},{{right,top,0},1,0xd0000000},{{left,bottom,0},1,0x90000000},{{right,bottom,0},1,0x90000000}};
    draw_gui_strip(vertices);
}
void AnmRenderer::draw_gui_strip(const UntexturedVertex* vertices) {
    flush();
    if(!color_compositing_disabled){RenderCommands(backend).SetColorOp(ColorOp::SelectFirst);}RenderCommands(backend).SetTextureArg(TextureArg::Diffuse);
    if(!depth_test_disabled)backend.write_depth(false);backend.vertex_format(VertexFormat::Untextured);backend.draw(Primitive::Strip,VertexFormat::Untextured,vertices,4);
    current_shader=current_color_op=disable_z_write=0xff;current_blend=3;
    if(!color_compositing_disabled){RenderCommands(backend).SetColorOp(ColorOp::Modulate);}RenderCommands(backend).SetTextureArg(TextureArg::Texture);
}
void AnmRenderer::draw_depth_mask(const UntexturedVertex* vertices,u32 count){
    backend.vertex_format(VertexFormat::Untextured);flush();backend.write_depth(true);
    RenderCommands(backend).SetColorOp(ColorOp::SelectFirst);RenderCommands(backend).SetTextureArg(TextureArg::Diffuse);
    backend.draw(Primitive::Strip,VertexFormat::Untextured,vertices,count);
    current_shader=current_color_op=disable_z_write=255;current_blend=3;
    RenderCommands(backend).SetColorOp(ColorOp::Modulate);RenderCommands(backend).SetTextureArg(TextureArg::Texture);
}

void AnmRenderer::draw_rectangle(float left,float top,float right,float bottom,const u32* colors,bool shaded) {
    flush();
    const UntexturedVertex vertices[4]{{{left,top,0},1,colors[0]},{{right,top,0},1,colors[1]},{{left,bottom,0},1,colors[2]},{{right,bottom,0},1,colors[3]}};
    if(!shaded||!color_compositing_disabled){RenderCommands(backend).SetColorOp(ColorOp::SelectFirst);}
    RenderCommands(backend).SetTextureArg(TextureArg::Diffuse);
    if(!depth_test_disabled)backend.write_depth(false);
    backend.destination_blend(Blends::InverseSourceAlpha);backend.vertex_format(VertexFormat::Untextured);
    backend.draw(Primitive::Strip,VertexFormat::Untextured,vertices,4);
    current_shader=current_color_op=disable_z_write=0xff;current_blend=3;current_texture=0;current_sprite=nullptr;
    if(!shaded||!color_compositing_disabled){RenderCommands(backend).SetColorOp(ColorOp::Modulate);}
    RenderCommands(backend).SetTextureArg(TextureArg::Texture);
}

void AnmRenderer::set_3d_state(AnmVm& vm) {
    if(current_blend!=vm.blendMode) {
        flush(); current_blend=vm.blendMode;
        if(current_blend==0)backend.destination_blend(Blends::InverseSourceAlpha);
        else if(current_blend==1)backend.destination_blend(Blends::One);
    }
    const u32 color=vm_color(vm);
    if(diffuse_source) {
        diffuse_source=0;
        if(!vertex_buffer_disabled) { flush(); RenderCommands(backend).SetDiffuseArg(TextureArg::Factor); }
    }
    if(!vertex_buffer_disabled) {
        if(current_factor!=color) { flush(); current_factor=color; backend.texture_factor(color); }
    } else {
        for(u32 i=0;i<4;++i) { quad[i].color=color; colored_world_quad[i].color=color; }
    }
    if(!depth_test_disabled && disable_z_write!=vm.zWriteDisabled) {
        flush(); disable_z_write=vm.zWriteDisabled; backend.write_depth(!disable_z_write);
    }
    if(camera_mode!=vm.flag15) {
        flush(); camera_mode=vm.flag15;
        if(camera_mode) {
            Camera::scene(view_matrix,projection_matrix,viewport,scene_camera);
        } else Camera::screen(view_matrix,projection_matrix,viewport);
        backend.transform(Matrices::View,view_matrix); backend.transform(Matrices::Projection,projection_matrix);
        backend.set_viewport(viewport);
    }
    ++state_changes;
}

i32 AnmRenderer::draw_3d(AnmVm& vm) {
    if(!drawable(vm) || !vm.loadedSprite)return -1;
    flush();
    auto world=world_matrix(vm);
    world.m[3][0]=Scalar::add(world.m[3][0],shake.x);
    world.m[3][1]=Scalar::add(world.m[3][1],shake.y);
    set_3d_state(vm);
    backend.transform(Matrices::World,world);
    // Original checks U twice here. A V-only scroll with the same sprite
    // does not refresh the cached texture transform.
    if(current_sprite!=vm.loadedSprite || vm.uvScrollPos.x!=0) {
        current_sprite=vm.loadedSprite;
        auto uv=vm.matrix3;
        uv.m[2][0]=Scalar::add(vm.loadedSprite->uvStart.x,vm.uvScrollPos.x);
        uv.m[2][1]=Scalar::add(vm.loadedSprite->uvStart.y,vm.uvScrollPos.y);
        backend.transform(Matrices::Texture,uv);
        if(current_texture!=vm.loadedSprite->texture) {
            current_texture=vm.loadedSprite->texture; backend.bind_texture(current_texture);
        }
    }
    const auto format=vertex_buffer_disabled?VertexFormat::ColoredWorld:VertexFormat::World;
    if(current_shader!=2) {
        backend.vertex_format(format);
        RenderCommands(backend).SetDiffuseArg(TextureArg::Factor);
        current_shader=2;
    }
    if(vertex_buffer_disabled)backend.draw(Primitive::Strip,format,colored_world_quad,4);
    else backend.draw(Primitive::Strip,format,world_quad,4);
    TH08_AUDIT_WORLD(vm,*this,world);
    return 0;
}

i32 AnmRenderer::draw_vertices(AnmVm& vm,const SpriteVertex* vertices,i32 count) {
    if(!drawable(vm) || !vm.loadedSprite || count<3)return -1;
    flush();
    if(current_texture!=vm.loadedSprite->texture) {
        current_texture=vm.loadedSprite->texture; backend.bind_texture(current_texture);
    }
    if(current_shader!=3) { backend.vertex_format(VertexFormat::Screen); current_shader=3; }
    set_state(vm);
    if(!diffuse_source) {
        diffuse_source=1;
        if(!vertex_buffer_disabled) { RenderCommands(backend).SetDiffuseArg(TextureArg::Diffuse); }
    }
    backend.draw(Primitive::Strip,VertexFormat::Screen,vertices,u32(count));
    TH08_AUDIT_CAPTURE(vm,vertices,u32(count),1);
    return 0;
}

i32 AnmRenderer::draw_quad(AnmVm& vm,const SpriteVertex* vertices) {
    if(!drawable(vm) || !vm.loadedSprite)return -1;
    if(current_texture!=vm.loadedSprite->texture) {
        current_texture=vm.loadedSprite->texture; flush(); backend.bind_texture(current_texture);
    }
    if(current_shader!=1) { flush(); current_shader=1; }
    set_state(vm); TH08_AUDIT_CAPTURE(vm,vertices,4,2); add_quad(vertices);
    return 0;
}

i32 AnmRenderer::texture_strip(AnmVm& vm,SpriteVertex* vertices,i32 count,bool vertical) {
    if(count<3 || !vm.loadedSprite)return -1;
    const auto& sprite=*vm.loadedSprite;
    const float start=vertical?sprite.uvStart.y:sprite.uvStart.x;
    const float end=vertical?sprite.uvEnd.y:sprite.uvEnd.x;
    const float scroll=vertical?vm.uvScrollPos.y:vm.uvScrollPos.x;
    const float step=((number(end)-number(start))/Extended::from_int((count+1)/2-1)).to_float();
    const float initial=Scalar::add(end,scroll);
    for(i32 side=0;side<2;++side) {
        float coordinate=initial;
        const float edge=vertical?(side?sprite.uvEnd.x:sprite.uvStart.x):(side?sprite.uvEnd.y:sprite.uvStart.y);
        const float other=Scalar::add(edge,vertical?vm.uvScrollPos.x:vm.uvScrollPos.y);
        for(i32 i=side;i<count;i+=2) {
            auto& vertex=vertices[i];
            vertex.uv=vertical?Vec2{other,coordinate}:Vec2{coordinate,other};
            vertex.color=vm.color1.d3dColor; vertex.reciprocal_w=1;
            coordinate=Scalar::sub(coordinate,step);
        }
    }
    return 0;
}

i32 AnmRenderer::draw_fan(AnmVm& vm,const UntexturedVertex* vertices,i32 count) {
    if(count<3)return -1;
    flush();
    if(current_shader!=4) { backend.vertex_format(VertexFormat::Untextured); current_shader=4; }
    set_state(vm);
    if(!color_compositing_disabled) { RenderCommands(backend).SetColorOp(ColorOp::SelectFirst); }
    RenderCommands(backend).SetTextureArg(TextureArg::Diffuse);
    flush(); backend.write_depth(false);
    backend.draw(Primitive::Fan,VertexFormat::Untextured,vertices,u32(count));
    TH08_AUDIT_FAN(vm,vertices,u32(count));
    current_shader=current_color_op=disable_z_write=0xff; current_blend=3;
    if(!color_compositing_disabled) { RenderCommands(backend).SetColorOp(ColorOp::Modulate); }
    RenderCommands(backend).SetTextureArg(TextureArg::Texture);
    return 0;
}
}
