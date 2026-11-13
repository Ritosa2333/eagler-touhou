#include "AnmRenderer.hpp"

#include "PortTitle.hpp"

#include <algorithm>

#include <cmath>

namespace th11 {

using namespace touhou::graphics;

#if PORT_TITLE == 14

void AnmRenderer::set_stage_depth_write(bool value){

    if(stage_depth_cache&&*stage_depth_cache==value)return;

    flush();graphics.pipeline().depthWrite=value;if(stage_depth_cache)*stage_depth_cache=value;

}

#endif

void AnmRenderer::flush(){if(vertices.empty())return;auto& p=graphics.pipeline();p.textureTransform=false;p.color.second=p.alpha.second={ArgumentSource::Diffuse};graphics.set_layout(VertexLayout::ScreenColorUv);graphics.triangles(vertices.size()/3,vertices.data(),sizeof(AnmVertex));vertices.clear();}

void AnmRenderer::invalidate(){flush();texture_handle=blend_mode=filter=~0u;uv_sprite=nullptr;}

void AnmRenderer::solid_rectangle(float left,float top,float right,float bottom,u32 color){

    // 447f70 draws directly, then invalidates the ANM material cache.

    flush();auto& p=graphics.pipeline();p.alpha.operation=p.color.operation=ColorOperation::First;

    p.alpha.first=p.color.first={ArgumentSource::Diffuse};p.destinationBlend=BlendFactor::InverseSourceAlpha;

    struct Vertex{Vec3 position;float rhw;u32 color;};

    const Vertex quad[]={{{left,top,0},1,color},{{right,top,0},1,color},{{left,bottom,0},1,color},{{right,bottom,0},1,color}};

    graphics.set_layout(VertexLayout::ScreenColor);graphics.primitives(Topology::Strip,2,quad,sizeof(Vertex));

    texture_handle=blend_mode=filter=~0u;uv_sprite=nullptr;

    p.alpha.operation=p.color.operation=ColorOperation::Multiply;p.alpha.first=p.color.first={ArgumentSource::Texture};

}

void AnmRenderer::set_camera(SceneCamera& value,bool screen){

    flush();if(screen)value.screen();else value.perspective();

#if PORT_TITLE == 14

    render_width=value.render_width;render_height=value.render_height;

#endif

    camera.position=value.position;camera.right=value.right;camera.view=value.view;camera.projection=value.projection;camera.viewport=value.viewport;

    camera.fog_near=value.fog.near_distance;camera.fog_far=value.fog.far_distance;camera.fog_color=value.fog.color;

    offset=value.offset;graphics.set_matrix(MatrixKind::View,value.view);graphics.set_matrix(MatrixKind::Projection,value.projection);set_viewport(value.viewport);

}

#if PORT_TITLE == 14

void AnmRenderer::upload_camera(const SceneCamera& value){

    render_width=value.render_width;render_height=value.render_height;

    // Retail4461e0 uploads cached transforms; it does not run446250.

    flush();

    camera.position=value.position;camera.right=value.right;camera.view=value.view;camera.projection=value.projection;camera.viewport=value.viewport;

    camera.fog_near=value.fog.near_distance;camera.fog_far=value.fog.far_distance;camera.fog_color=value.fog.color;

    offset=value.offset;graphics.set_matrix(MatrixKind::View,value.view);graphics.set_matrix(MatrixKind::Projection,value.projection);set_viewport(value.viewport);

}

#endif

u32 AnmRenderer::color(const AnmVm& vm)const noexcept {const bool secondary=(kPortTitle==13||kPortTitle==14)?((vm.th13_native_flags>>17)&3)==1:(vm.flags&0x8000)!=0;return tinted_color(secondary?vm.secondary_color:vm.color);}

u32 AnmRenderer::tinted_color(u32 original)const noexcept {if(!tint_enabled)return original;u32 out=0;for(u32 shift=0;shift<32;shift+=8)out|=std::min(255u,(((original>>shift)&255)*((tint>>shift)&255))>>7)<<shift;return out;}

void AnmRenderer::material(const AnmVm& vm){

    if(vm.sprite){const auto handle=graphics.texture(*vm.bound_sprite_resource(),vm.sprite->texture);if(handle!=texture_handle){flush();texture_handle=handle;graphics.bind_texture(handle);}}

    const u32 blend=vm.blend_mode();if(blend!=blend_mode){flush();blend_mode=blend;auto& p=graphics.pipeline();

        if constexpr(kPortTitle==14){

            // 4753a0: modes 10..15 retain the prior device blend state.

            constexpr BlendFactor source[]={BlendFactor::SourceAlpha,BlendFactor::SourceAlpha,BlendFactor::SourceAlpha,BlendFactor::One,BlendFactor::InverseDestinationColor,BlendFactor::DestinationColor,BlendFactor::InverseSourceColor,BlendFactor::DestinationAlpha,BlendFactor::SourceAlpha,BlendFactor::SourceAlpha};

            constexpr BlendFactor destination[]={BlendFactor::InverseSourceAlpha,BlendFactor::One,BlendFactor::One,BlendFactor::Zero,BlendFactor::InverseSourceColor,BlendFactor::Zero,BlendFactor::InverseSourceAlpha,BlendFactor::InverseDestinationAlpha,BlendFactor::One,BlendFactor::One};

            if(blend<10){p.sourceBlend=source[blend];p.destinationBlend=destination[blend];p.blendEquation=blend==2?BlendEquation::ReverseSubtract:blend==8?BlendEquation::Minimum:blend==9?BlendEquation::Maximum:BlendEquation::Add;}

        }else switch(blend){

        case 0:p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::InverseSourceAlpha;break;

        case 1:p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::One;break;

        case 2:p.sourceBlend=BlendFactor::Zero;p.destinationBlend=BlendFactor::InverseSourceColor;break;

        case 3:p.sourceBlend=BlendFactor::One;p.destinationBlend=BlendFactor::Zero;break;

        case 4:p.sourceBlend=BlendFactor::InverseDestinationColor;p.destinationBlend=BlendFactor::InverseSourceAlpha;break;

        case 5:p.sourceBlend=BlendFactor::DestinationColor;p.destinationBlend=BlendFactor::Zero;break;

        case 6:p.sourceBlend=BlendFactor::InverseSourceColor;p.destinationBlend=BlendFactor::InverseSourceAlpha;break;

        }

    }

    const u32 point=vm.flags>>31;if(point!=filter){flush();filter=point;graphics.pipeline().minFilter=graphics.pipeline().magFilter=point?Filter::Nearest:Filter::Linear;}

    if constexpr(kPortTitle==13||kPortTitle==14){

        // 467281 / 4672cb: 0=wrap, 1=clamp, 2=mirror; 3 retains the device state.

        auto address=[](u32 mode,Address previous){return mode==0?Address::Repeat:mode==1?Address::Clamp:mode==2?Address::Mirror:previous;};

        auto& p=graphics.pipeline();const auto u=address(vm.flags2&3,p.addressU),v=address(vm.th13_native_flags>>30,p.addressV);

        if(u!=p.addressU||v!=p.addressV){flush();p.addressU=u;p.addressV=v;}

    }

}

float AnmRenderer::fog_amount(Vec3 position,Vec3 origin)const noexcept{

    const float x=float(double(position.x)-origin.x),y=float(double(position.y)-origin.y),z=float(double(position.z)-origin.z);

#if PORT_TITLE == 14

    const float squared=float(float(float(y*y)+float(x*x))+float(z*z)),distance=float(std::sqrt(double(squared)));

    return !(distance>fog_start)?0:float(float(fog_start-distance)/float(camera.fog_near-camera.fog_far));

#else

    const float squared=float((double(x)*x+double(y)*y)+double(z)*z),distance=float(std::sqrt(double(squared)));

    return distance<=fog_start?0:float((double(distance)-fog_start)/float(double(camera.fog_far)-camera.fog_near));

#endif

}

u32 AnmRenderer::fog_color(u32 value,float amount,bool fade_alpha)const noexcept{

    if(amount==0)return value;if(amount>=1)return (value&0xff000000)|(camera.fog_color&0xffffff);

#if PORT_TITLE == 14

    const float channels[]={fog_channels.x,fog_channels.y,fog_channels.z};u32 out=0;

    for(u32 i=0;i<3;++i){const i32 original=(value>>(i*8))&255;const float target=fade_alpha?float(truncate_int(channels[i])):channels[i];

        const i32 delta=truncate_int(float(float(float(original)-target)*amount));out|=u32(u8(u32(original)-u32(delta)))<<(i*8);}

    const float squared=float(amount*amount),cubed=float(squared*amount);

    return out|(fade_alpha?(u32(u8(truncate_int(float(float(1-cubed)*float(value>>24)))))<<24):(value&0xff000000));

#else

    const float channels[]={fog_channels.x,fog_channels.y,fog_channels.z};u32 out=0;

    for(u32 i=0;i<3;++i){const i32 original=(value>>(i*8))&255;const double target=fade_alpha?double(i32(channels[i])):double(channels[i]);out|=u32(u8(original-i32((double(original)-target)*amount)))<<(i*8);}

    return out|(fade_alpha?u32(u8(i32(double(value>>24)*(1.-amount))))<<24:value&0xff000000);

#endif

}

int AnmRenderer::submit_quad(AnmVm& vm,Vec3 (&positions)[4],bool pixel,const u32* colors){

    for(auto& p:positions){p.x=float(double(p.x)+offset.x);p.y=float(double(p.y)+offset.y);}

    if(pixel){
#if PORT_TITLE == 14
        // Retail4757b4..475816 rounds four coordinates, then broadcasts.
        const float left=float(std::nearbyint(double(positions[0].x))-.5),right=float(std::nearbyint(double(positions[1].x))-.5);
        const float top=float(std::nearbyint(double(positions[0].y))-.5),bottom=float(std::nearbyint(double(positions[2].y))-.5);
        for(u32 i=0;i<4;++i){positions[i].x=(i&1)?right:left;positions[i].y=(i&2)?bottom:top;}
#else
        for(auto& p:positions){p.x=float(std::nearbyint(double(p.x))-.5);p.y=float(std::nearbyint(double(p.y))-.5);}
#endif
    }

#if PORT_TITLE == 14

    // 47581c..4759a7 precedes the visibility test, including clipped calls.

    for(u32 i=0;i<4;++i){th14_quad[i].position=positions[i];vm.screen_quad_positions[i]=positions[i];th14_quad[i].uv=anm_vertex_uv(vm,i);}

#endif

    float min_x=positions[0].x,max_x=min_x,min_y=positions[0].y,max_y=min_y;for(u32 i=1;i<4;++i){min_x=std::min(min_x,positions[i].x);max_x=std::max(max_x,positions[i].x);min_y=std::min(min_y,positions[i].y);max_y=std::max(max_y,positions[i].y);}

    if(max_x<double(viewport.x)||max_y<double(viewport.y)||min_x>double(viewport.x+viewport.width)||min_y>double(viewport.y+viewport.height))return 0;

    material(vm);u32 diffuse=color(vm);u32 gradient[4];

    if constexpr(kPortTitle==13||kPortTitle==14){const u32 mode=(vm.th13_native_flags>>17)&3;if(!colors&&mode>=2){const auto a=tinted_color(vm.color),b=tinted_color(vm.secondary_color);gradient[0]=a;gradient[1]=mode==2?b:a;gradient[2]=mode==2?a:b;gradient[3]=b;colors=gradient;}}

#if PORT_TITLE == 14

    if(!colors){

        // Solid color modes update +53c before the global tint; gradients

        // leave that cache intact. The independent-position flag is irrelevant.

        u32 value=((vm.th13_native_flags>>17)&3)==1?vm.secondary_color:vm.color;

        if((vm.flags2&0x1000000)&&vm.parent){u32 mixed=0;for(u32 shift=0;shift<32;shift+=8)mixed|=std::min(255u,(((value>>shift)&255)*((vm.parent->inherited_color>>shift)&255))>>7)<<shift;value=mixed;}

        vm.inherited_color=value;diffuse=tinted_color(value);

    }

#endif

#if PORT_TITLE == 14

    auto& quad=th14_quad;for(u32 i=0;i<4;++i)quad[i].color=colors?colors[i]:diffuse;

#else

    AnmVertex quad[4];for(u32 i=0;i<4;++i)quad[i]={positions[i],1,colors?colors[i]:diffuse,anm_vertex_uv(vm,i)};

#endif

    for(u32 i:{0,1,2,1,2,3})vertices.push_back(quad[i]);if(vertices.size()>=6144)flush();return 0;

}

int AnmRenderer::draw_ascii_sprite(AnmVm& vm){

    // 401670 submits 44fe30/44f880 directly, including zero-alpha glyphs.

    if(!vm.sprite||!vm.resource)return -2;

    #if PORT_TITLE == 14
    const float display_scale=float(render_width)/640.f;const AnmRenderSpace space{display_scale,{float(i32(render_width)/2),float((i32(render_height)-448)/2)},{float(i32(render_width)/2),float(i32(float(display_scale*16.f)))}};
    Vec3 positions[4];if(!anm_quad_positions(vm,positions,space))return -2;
#else
    Vec3 positions[4];if(!anm_quad_positions(vm,positions))return -2;
#endif

    return submit_quad(vm,positions,true);

}

#if PORT_TITLE == 14

int AnmRenderer::draw_transition_type0(AnmVm& vm){

    if(!vm.geometry)return -2;

    auto& transition=*static_cast<TransitionType0Th14*>(vm.geometry);

    const bool alpha_pass=transition.state==2||(transition.state==0&&graphics.backbuffer_has_alpha());

    auto& p=graphics.pipeline();

    if(alpha_pass){

        flush();p.alphaTest=false;p.separateAlphaBlend=true;

        p.sourceBlend=BlendFactor::Zero;p.destinationBlend=BlendFactor::One;p.blendEquation=BlendEquation::Add;

        p.sourceAlphaBlend=BlendFactor::One;p.destinationAlphaBlend=BlendFactor::Zero;p.alphaBlendEquation=BlendEquation::Add;

        struct Vertex{Vec3 position;float reciprocal_w;u32 color;};

        float left=0,top=0,right=float(render_width),bottom=float(render_height);

        if(transition.state==2){left=128.f;top=16.f;right=512.f;bottom=464.f;}

        const Vertex quad[]={{{left,top,0},1,0},{{right,top,0},1,0},{{left,bottom,0},1,0},{{right,bottom,0},1,0}};

        p.alpha.operation=p.color.operation=ColorOperation::First;p.alpha.first=p.color.first={ArgumentSource::Diffuse};

        graphics.set_layout(VertexLayout::ScreenColor);graphics.primitives(Topology::Strip,2,quad,sizeof(Vertex));

        p.alphaTest=true;p.alpha.operation=p.color.operation=ColorOperation::Multiply;p.alpha.first=p.color.first={ArgumentSource::Texture};

        p.sourceAlphaBlend=BlendFactor::SourceAlpha;p.destinationAlphaBlend=BlendFactor::One;p.alphaBlendEquation=BlendEquation::Add;
        blend_mode=10; // Native40ac01 force the next material selection.

    }

    for(u32 i=0;i<4;++i)if(draw(transition.embedded[i])==-2)return -2;

    if(alpha_pass){
        // Retail40ac7f disables separate alpha before material's pending flush.
        p.separateAlphaBlend=false;if(draw(transition.embedded[4])==-2)return -2;flush();
    }

    return 0;

}

#endif

int AnmRenderer::draw(AnmVm& vm){

#if PORT_TITLE == 14

    if(vm.th13_effect==1&&draw_transition_type0(vm)==-2)return -2;

#endif

#if PORT_TITLE == 13

    if(vm.th13_effect==5&&draw_ribbon(vm)==-2)return -2;

#elif PORT_TITLE == 14

    if(vm.th13_effect==8){

        // 40b3a0 writes D3DRS_ZWRITEENABLE directly without updating the

        // Supervisor cache. Preserve the resulting first-strip behavior.

        flush();auto& p=graphics.pipeline();const bool previous_depth=stage_depth_cache?*stage_depth_cache:p.depthWrite;

        p.fog=true;p.depthWrite=false;

        const bool visible=(vm.flags&3)==3;

        const int result=draw_world_strip(vm,vm.geometry?static_cast<AnmTh14Ribbon*>(vm.geometry)->vertices:nullptr,80,!(vm.flags&0x800)&&!previous_depth);

        p.fog=false;

        if(stage_depth_cache)set_stage_depth_write(true);

        else p.depthWrite=visible?((vm.flags&0x800)||!previous_depth):!previous_depth;

        if(result==-2)return -2;

    }

#endif

    const u32 mode=vm.drawing_mode();

    const bool rectangle=(kPortTitle==13||kPortTitle==14)&&(mode==15||mode==16||mode==17||mode==19||mode==20||mode==21);

    if((vm.flags&3)!=3||((kPortTitle==13||kPortTitle==14)&&(vm.flags2&0x20)))return -1;

#if PORT_TITLE == 14

    if(stage_depth_cache)set_stage_depth_write(false);

    else if(graphics.pipeline().depthWrite){flush();graphics.pipeline().depthWrite=false;}

#endif

    if(kPortTitle!=14||mode<=8||mode==18){if(!(vm.color>>24)&&!((kPortTitle==13||kPortTitle==14)&&(vm.secondary_color>>24))&&!rectangle)return -1;}

    // A script sets its sprite from an instruction, so an animation that has not reached that

    // instruction yet has none. material() would dereference it, so it cannot be drawn - but that is a

    // "nothing to draw" state, not a fault: the layer walk already skips retired animations, and a

    // missing sprite must not abort the session. Only an animation with no resource at all is a fault,

    // because nothing is bound to execute.

    if(!vm.resource)return -2;

    if(!vm.sprite&&!rectangle)return 0;

#if PORT_TITLE == 13 || PORT_TITLE == 14

    if(rectangle){

        // 46a94a/46ad00/46afa0/46b050/46b3c0: untextured shapes.

        // Extra scale and pivot do not participate in this native path.

        flush();const auto center=vm.render_position();

        float width=float(double(vm.sprite_size.x)*vm.scale.x),height=float(double(vm.sprite_size.y)*vm.scale.y),angle=vm.rotation.z;

        if(vm.parent&&!(vm.flags2&(kPortTitle==14?0x8000:0x10000))){width=float(double(width)*vm.parent->scale.x);height=float(double(height)*vm.parent->scale.y);angle=float(double(angle)+vm.parent->rotation.z);}

        if constexpr(kPortTitle==14)if(((vm.flags2>>19)&7)==2){width=float(width*.5f);height=float(height*.5f);}

        const u32 ax=(vm.flags>>18)&3,ay=(vm.flags>>20)&3;if(ax==3||ay==3)return -2;

        material(vm);

        auto& p=graphics.pipeline();p.depthWrite=false;p.textureTransform=false;

        p.color.operation=p.alpha.operation=ColorOperation::First;p.color.first=p.alpha.first={ArgumentSource::Diffuse};

        struct Vertex{Vec3 position;float rhw;u32 color;};

        if(mode==16||mode==17){

            const i32 segments=vm.integers[0];if(segments<=0||segments>65535)return -2;

            constexpr double pi=3.1415927410125732421875,tau=6.283185482025146484375;

            const float step=float(tau/segments);std::vector<Vertex> circle;circle.reserve(segments+2);

            if(mode==16)circle.push_back({{center.x,center.y,0},1,vm.color});

            for(i32 i=0;i<=segments;++i){

                const float x=float(std::cos(double(angle))*width),y=float(std::sin(double(angle))*width);

                circle.push_back({{float(double(x)+center.x),float(double(y)+center.y),0},1,mode==16?vm.secondary_color:vm.color});

                angle=float(double(angle)+step);u32 loops=0;

                while(angle>pi){angle=float(double(angle)-tau);if(loops++>32)break;}

                while(angle<-pi){angle=float(double(angle)+tau);if(loops++>32)break;}

            }

            graphics.set_layout(VertexLayout::ScreenColor);graphics.primitives(mode==16?Topology::Fan:Topology::LineStrip,segments,circle.data(),sizeof(Vertex));

            texture_handle=blend_mode=filter=~0u;uv_sprite=nullptr;

            p.color.operation=p.alpha.operation=ColorOperation::Multiply;p.color.first=p.alpha.first={ArgumentSource::Texture};return 0;

        }

        const float cs=float(std::cos(double(angle))),sn=float(std::sin(double(angle)));

        auto emit=[&](float w,float h,u32 left_color,u32 right_color){

            const float left=ax==0?float(-double(w)*.5):ax==1?0:-w,right=ax==0?float(double(w)*.5):ax==1?w:0;

            const float top=ay==0?float(-double(h)*.5):ay==1?0:-h,bottom=ay==0?float(double(h)*.5):ay==1?h:0;

            const float xs[]={left,right,left,right},ys[]={top,top,bottom,bottom};Vertex vertices[4];

            for(u32 i=0;i<4;++i){

                const float x=kPortTitle==14?float(float(float(cs*xs[i])-float(sn*ys[i]))+center.x):float(double(cs)*xs[i]-double(sn)*ys[i]+center.x);

                const float y=kPortTitle==14?float(float(float(cs*ys[i])+float(sn*xs[i]))+center.y):float(double(cs)*ys[i]+double(sn)*xs[i]+center.y);

                vertices[i]={{x,y,0},1,(i&1)?right_color:left_color};

            }

            graphics.set_layout(VertexLayout::ScreenColor);graphics.primitives(Topology::Strip,2,vertices,sizeof(Vertex));

        };

        const u32 a=vm.color,b=mode==19||mode==21?vm.secondary_color:vm.color;

        if(mode==20||mode==21)emit(float(double(width)+1),float(double(height)+1),(a&0xffffff)|((a>>1)&0x7f000000),(b&0xffffff)|((b>>1)&0x7f000000));

        emit(width,height,a,b);

        texture_handle=blend_mode=filter=~0u;uv_sprite=nullptr;

        p.color.operation=p.alpha.operation=ColorOperation::Multiply;p.color.first=p.alpha.first={ArgumentSource::Texture};

        return 0;

    }

#endif

    if(mode<=3){

#if PORT_TITLE == 14

        const float display_scale=float(render_width)/640.f;const AnmRenderSpace space{display_scale,{float(i32(render_width)/2),float((i32(render_height)-448)/2)},{float(i32(render_width)/2),float(i32(float(display_scale*16.f)))}};
        Vec3 positions[4];if(!anm_th14_draw_quad_positions(vm,positions,space))return -2;

        return submit_quad(vm,positions,mode==0);

#elif PORT_TITLE == 13

        // Draw mode 3 calls the rotated helper (468170); the screen-UV

        // query 46aca0 deliberately uses the unrotated helper for that mode.

        Vec3 positions[4];const u32 saved_flags=vm.flags;

        if(mode==3)vm.flags=(vm.flags&~0x3c00000u)|(1u<<22);

        const bool ok=anm_quad_positions(vm,positions);vm.flags=saved_flags;if(!ok)return -2;

        return submit_quad(vm,positions,mode==0);

#else

        Vec3 positions[4];const u32 saved_flags=vm.flags;

        const bool pixel=mode==0||(mode==1&&vm.rotation.z==0);

        const u32 geometry_mode=pixel?0:mode==3?(vm.rotation.z==0?2:1):mode;

        vm.flags=(vm.flags&~0x3c00000u)|(geometry_mode<<22);const bool ok=anm_quad_positions(vm,positions);vm.flags=saved_flags;if(!ok)return -2;

        return submit_quad(vm,positions,pixel);

#endif

    }

#if PORT_TITLE == 14

    if(mode==5){
        th14_world=anm_legacy_world_matrix_th14(vm,render_width,render_height);
        Vec3 positions[4];for(u32 i=0;i<4;++i)positions[i]=th14_quad[i].position;

        const int result=submit_quad(vm,positions,false);

        for(auto& vertex:th14_quad)vertex.reciprocal_w=1; // 4779ba, also after clipping.

        return result;

    }

#endif

    if(mode==4||mode==5){Vec3 positions[4];if(mode==4?!anm_billboard_quad(vm,camera,positions):!anm_projected_quad(vm,camera,positions))return -1;return submit_quad(vm,positions,false);}

    if(mode==6||mode==7){

        Vec3 positions[4];u32 colors[4];

        if(mode==6){

            if(!anm_billboard_quad(vm,camera,positions))return -1;

#if PORT_TITLE == 14

            for(u32 i=0;i<4;++i)th14_quad[i].position=positions[i];

#endif

            const Vec3 center{float(double(float(double(vm.position.x)+vm.script_position.x))+vm.child_position.x),float(double(float(double(vm.position.y)+vm.script_position.y))+vm.child_position.y),float(double(float(double(vm.position.z)+vm.script_position.z))+vm.child_position.z)};

            const float amount=fog_amount(center,camera.position);if(amount>=1)return -1;

            const u32 value=fog_color(color(vm),amount,true);for(auto& v:colors)v=value;

        }else{

#if PORT_TITLE == 14

            th14_world=anm_legacy_world_matrix_th14(vm,render_width,render_height);const auto& world=th14_world;for(u32 i=0;i<4;++i)positions[i]=th14_quad[i].position;

#else

            Matrix4 world;if(!anm_projected_quad(vm,camera,positions,&world))return -1;

#endif

            
#if PORT_TITLE == 14
            const Vec3 corners[]={{-256,-256,0},{0,-256,0},{-256,0,0},{0,0,0}};
#else
            const Vec3 corners[]={{-128,-128,0},{128,-128,0},{-128,128,0},{128,128,0}};
#endif
            const u32 value=vm.flags&0x8000?vm.secondary_color:vm.color;

            for(u32 i=0;i<4;++i){float p[4];GraphicsMath::transform(corners[i],world,p);colors[i]=fog_color(value,fog_amount({p[0],p[1],p[2]},fog_origin),false);}

        }

#if PORT_TITLE == 14

        for(u32 i=0;i<4;++i)th14_quad[i].color=colors[i];

        const int result=submit_quad(vm,positions,false,colors);

        if(mode==7)for(auto& vertex:th14_quad)vertex.reciprocal_w=1;

        return result;

#else

        return submit_quad(vm,positions,false,colors);

#endif

    }

    if(mode==8){

        // TH13 46c950 supplies nine vertex quads, indexed by anchor in 469aca.

        // Anchoring occurs before scale/rotation, not as a world translation.

        if constexpr(kPortTitle==14)if(!(vm.color>>24))return -1;

        flush();
#if PORT_TITLE == 14
        const float display_scale=float(render_width)/640.f;
        // Native446905/446910 and446c8f/446cb1 use distinct origins.
        const AnmRenderSpace space{display_scale,
            {float(i32(render_width)/2),float((i32(render_height)-448)/2)},
            {float(i32(render_width)/2),float(i32(float(display_scale*16.f)))}};
        const auto world=anm_world_matrix_th14(vm,false,space);
#else
        const auto world=anm_world_matrix(vm,kPortTitle!=13&&kPortTitle!=14);
#endif
        material(vm);auto& p=graphics.pipeline();p.textureTransform=true;p.textureFactor=kPortTitle==14?tinted_color((vm.th13_native_flags&0x60000)?vm.secondary_color:vm.color):color(vm);p.color.second=p.alpha.second={ArgumentSource::Factor};

#if PORT_TITLE == 13 || PORT_TITLE == 14

        // 46932d disables writes for bit 13. The custom strip at 469b73

        // deliberately uses the opposite test; do not share that predicate.

#if PORT_TITLE == 14

        if(stage_depth_cache)set_stage_depth_write((vm.flags&0x800)==0);else

#endif

        p.depthWrite=(vm.flags&0x800)==0;

#endif

        graphics.set_matrix(MatrixKind::World,world);

        // The original tests horizontal scroll twice. A vertical-only change

        // on the same sprite retains the previous texture matrix.

        bool uv_changed=uv_sprite!=vm.sprite||vm.uv_offset.x!=0;

#if PORT_TITLE == 13 || PORT_TITLE == 14

        uv_changed=uv_changed||vm.texture_scale.x!=1||vm.texture_scale.y!=1;

#endif

        if(uv_changed){uv_sprite=vm.sprite;cached_uv=vm.uv_matrix;cached_uv.m[8]=float(double(vm.uv[0].x)+vm.uv_offset.x);cached_uv.m[9]=float(double(vm.uv[0].y)+vm.uv_offset.y);

#if PORT_TITLE == 13 || PORT_TITLE == 14

            cached_uv.m[0]=float(double(cached_uv.m[0])*vm.texture_scale.x);cached_uv.m[5]=float(double(cached_uv.m[5])*vm.texture_scale.y);

#endif

        }

        graphics.set_matrix(MatrixKind::Texture,cached_uv);

        const u32 ax=(kPortTitle==13||kPortTitle==14)?(vm.flags>>18)&3:0,ay=(kPortTitle==13||kPortTitle==14)?(vm.flags>>20)&3:0;

        if(ax==3||ay==3)return -2;

        const float left=ax==0?-128:ax==1?0:-256,top=ay==0?-128:ay==1?0:-256;

        struct Vertex{Vec3 position;Vec2 uv;};const Vertex quad[]={{{left,top,0},{0,0}},{{left+256,top,0},{1,0}},{{left,top+256,0},{0,1}},{{left+256,top+256,0},{1,1}}};

        graphics.set_layout(VertexLayout::WorldUv);graphics.primitives(Topology::Strip,2,quad,sizeof(Vertex));return 0;

    }

    if(mode==10)return 0; // Its registered draw callback submits the ripple.

    if(mode==9||mode==11||mode==12||mode==13||((kPortTitle==13||kPortTitle==14)&&mode==14)){

        if constexpr(kPortTitle==14)if(!(vm.color>>24))return -1;

        if(!vm.geometry)return -2;flush();material(vm);auto& p=graphics.pipeline();p.textureTransform=false;p.color.second=p.alpha.second={ArgumentSource::Diffuse};graphics.set_layout(VertexLayout::ScreenColorUv);

        const bool fan=mode==11;if(fan)p.depthWrite=false;const u32 count=u32(vm.integers[0])*2-2;

        graphics.primitives(fan?Topology::Fan:Topology::Strip,count,vm.geometry,sizeof(AnmVertex));return 0;

    }

#if PORT_TITLE == 14

    if(mode==23||mode==24)return draw_world_strip(vm,static_cast<const AnmWorldVertex*>(vm.geometry),u32(vm.integers[0])*2);

#endif

    return 0; // Reserved modes have no draw operation in the original dispatcher.

}

#if PORT_TITLE == 14

int AnmRenderer::draw_th14_screen_strip(AnmVm& vm,const AnmVertex* data,u32 count){

    // 478a10 submits pretransformed white vertices with the VM's material.

    if((vm.flags&3)!=3||!(vm.color>>24))return -1;

    if(!vm.resource||!vm.sprite||!data||count<2)return -2;

    flush();material(vm);auto& p=graphics.pipeline();p.textureTransform=false;

    p.color.second=p.alpha.second={ArgumentSource::Diffuse};

    graphics.set_layout(VertexLayout::ScreenColorUv);graphics.primitives(Topology::Strip,count-2,data,sizeof(AnmVertex));return 0;

}

int AnmRenderer::draw_world_strip(AnmVm& vm,const AnmWorldVertex* data,u32 count,int depth_override){

    if((vm.flags&3)!=3)return -1;

    if(!vm.resource||!vm.sprite||!data||count<3||count>16*1024*1024/sizeof(AnmWorldVertex))return -2;

    flush();material(vm);auto& p=graphics.pipeline();

    if(stage_depth_cache){

        if(depth_override<0)set_stage_depth_write((vm.flags&0x800)==0);

        else{p.depthWrite=depth_override!=0;*stage_depth_cache=(vm.flags&0x800)==0;}

    }else p.depthWrite=depth_override<0?(vm.flags&0x800)==0:depth_override!=0;

    Matrix4 world;world.identity();world.m[0]=float(vm.extra_scale.x*vm.scale.x);world.m[5]=float(vm.extra_scale.y*vm.scale.y);

    const float angles[]={vm.rotation.x,vm.rotation.y,vm.rotation.z};

    for(u32 axis=0;axis<3;++axis)if(angles[axis]!=0)world=GraphicsMath::multiply(world,GraphicsMath::rotation(axis,angles[axis]));

    vm.sprite_matrix.identity();vm.transform_matrix=world;vm.flags&=~12u;

    world.m[12]=float(float(vm.position.x+vm.script_position.x)+vm.child_position.x);

    if((vm.flags2&0x60000)&&!vm.parent)world.m[12]=float(world.m[12]+320.f);

    world.m[13]=float(float(vm.position.y+vm.script_position.y)+vm.child_position.y);world.m[14]=float(float(vm.position.z+vm.script_position.z)+vm.child_position.z);

    graphics.set_matrix(MatrixKind::World,world);

    if(uv_sprite!=vm.sprite||vm.uv_offset.x!=0||vm.texture_scale.x!=1||vm.texture_scale.y!=1){

        uv_sprite=vm.sprite;cached_uv=vm.uv_matrix;cached_uv.m[8]=float(vm.uv[0].x+vm.uv_offset.x);cached_uv.m[9]=float(vm.uv[0].y+vm.uv_offset.y);

        cached_uv.m[0]=float(cached_uv.m[0]*vm.texture_scale.x);cached_uv.m[5]=float(cached_uv.m[5]*vm.texture_scale.y);

    }

    graphics.set_matrix(MatrixKind::Texture,cached_uv);p.textureTransform=true;p.color.operation=p.alpha.operation=ColorOperation::Multiply;p.color.first=p.alpha.first={ArgumentSource::Texture};p.color.second=p.alpha.second={ArgumentSource::Diffuse};

    graphics.set_layout(VertexLayout::WorldColorUv);graphics.primitives(Topology::Strip,count-2,data,sizeof(AnmWorldVertex));return 0;

}

#endif

#if PORT_TITLE == 13

int AnmRenderer::draw_ribbon(AnmVm& vm){

    if((vm.flags&3)!=3)return -1;

    if(!vm.sprite||!vm.resource||!vm.geometry)return -2;

    flush();material(vm);auto& p=graphics.pipeline();p.fog=true;p.depthWrite=(vm.flags&0x800)!=0;

    Matrix4 world;world.identity();world.m[0]=float(double(vm.extra_scale.x)*vm.scale.x);world.m[5]=float(double(vm.extra_scale.y)*vm.scale.y);

    const float angles[]={vm.rotation.x,vm.rotation.y,vm.rotation.z};

    for(u32 axis=0;axis<3;++axis)if(angles[axis]!=0)world=GraphicsMath::multiply(world,GraphicsMath::rotation(axis,angles[axis]));

    vm.sprite_matrix.identity();vm.transform_matrix=world;vm.flags&=~12u;

    world.m[12]=float(double(vm.position.x)+vm.script_position.x+vm.child_position.x);

    if((vm.flags2&0x40000)&&!vm.parent)world.m[12]=float(double(world.m[12])+320);

    world.m[13]=float(double(vm.position.y)+vm.script_position.y+vm.child_position.y);world.m[14]=float(double(vm.position.z)+vm.script_position.z+vm.child_position.z);

    graphics.set_matrix(MatrixKind::World,world);

    if(uv_sprite!=vm.sprite||vm.uv_offset.x!=0||vm.texture_scale.x!=1||vm.texture_scale.y!=1){

        uv_sprite=vm.sprite;cached_uv=vm.uv_matrix;cached_uv.m[8]=float(double(vm.uv[0].x)+vm.uv_offset.x);cached_uv.m[9]=float(double(vm.uv[0].y)+vm.uv_offset.y);

        cached_uv.m[0]=float(double(cached_uv.m[0])*vm.texture_scale.x);cached_uv.m[5]=float(double(cached_uv.m[5])*vm.texture_scale.y);

    }

    graphics.set_matrix(MatrixKind::Texture,cached_uv);p.textureTransform=true;p.color.operation=p.alpha.operation=ColorOperation::Multiply;p.color.first=p.alpha.first={ArgumentSource::Texture};p.color.second=p.alpha.second={ArgumentSource::Diffuse};

    graphics.set_layout(VertexLayout::WorldColorUv);graphics.primitives(Topology::Strip,30,static_cast<AnmRibbon*>(vm.geometry)->vertices,sizeof(AnmWorldVertex));

    p.fog=false;return 0;

}

#endif

int AnmRenderer::draw_ripple(AnmVm& vm){

    if(!vm.geometry||!vm.resource||!vm.sprite)return -2;flush();material(vm);auto& p=graphics.pipeline();p.depthWrite=false;p.textureTransform=false;p.color.second=p.alpha.second={ArgumentSource::Diffuse};graphics.set_layout(VertexLayout::ScreenColorUv);graphics.primitives(Topology::Fan,31,vm.geometry,sizeof(AnmVertex));return 0;

}

int AnmRenderer::draw_layer(AnmVm* first){for(auto* vm=first;vm;vm=vm->draw_next){if((vm->flags&0x4000000)||(kPortTitle==14&&(vm->flags2&0x20)))continue;if(vm->before_update==anm_ripple_update&&draw_ripple(*vm)==-2)return -2;if(vm->draw_callback)vm->draw_callback(*vm);if(draw(*vm)==-2)return -2;}return 1;}

}

