#include "ZunGraphics.hpp"
namespace th075 {
void ZunGraphics::monochrome(bool enabled){
    if(enabled==monochromeEnabled)return;auto& p=backend.pipeline();
    if(enabled){previousColor=p.color;previousAlpha=p.alpha;
        p.color={ColorOperation::LuminanceMultiply,{ArgumentSource::Texture},{ArgumentSource::Diffuse}};
        p.alpha={ColorOperation::Multiply,{ArgumentSource::Texture},{ArgumentSource::Diffuse}};
    }else{p.color=previousColor;p.alpha=previousAlpha;}
    monochromeEnabled=enabled;
}
void ZunGraphics::blend(SpriteBlend mode){
    static constexpr BlendFactor source[]={BlendFactor::SourceAlpha,BlendFactor::One,BlendFactor::SourceAlpha,BlendFactor::Zero,BlendFactor::Zero,BlendFactor::One,BlendFactor::DestinationColor,BlendFactor::InverseDestinationColor};
    static constexpr BlendFactor destination[]={BlendFactor::InverseSourceAlpha,BlendFactor::Zero,BlendFactor::One,BlendFactor::InverseSourceColor,BlendFactor::SourceColor,BlendFactor::One,BlendFactor::One,BlendFactor::Zero};
    const auto i=u32(mode);if(i>=8)return;backend.set_source_blend(source[i]);backend.set_destination_blend(destination[i]);
}
void ZunGraphics::alpha(SpriteAlpha mode){
    auto& p=backend.pipeline();p.blend=p.alphaTest=mode!=SpriteAlpha::Disabled;
    p.alpha.first.source=mode==SpriteAlpha::Vertex?ArgumentSource::Diffuse:ArgumentSource::Texture;
}
void ZunGraphics::depth(DepthMode mode){
    auto& p=backend.pipeline();switch(mode){
        case DepthMode::Disabled:p.depthTest=false;break;
        case DepthMode::Enabled:p.depthTest=true;p.depthCompare=Compare::LessEqual;break;
        case DepthMode::WriteEnabled:p.depthWrite=true;break;
        case DepthMode::WriteDisabled:p.depthWrite=false;break;
        case DepthMode::LessEqual:p.depthCompare=Compare::LessEqual;break;
        case DepthMode::Always:p.depthCompare=Compare::Always;break;
        case DepthMode::Less:p.depthCompare=Compare::Less;break;
        case DepthMode::GreaterEqual:p.depthCompare=Compare::GreaterEqual;break;
        case DepthMode::Clear:backend.clear_depth();break;
    }
}
void ZunGraphics::filter(Filter value){backend.set_texture_filter(value,value);}
u32 ZunGraphics::modulate(u32 color)const{
    if(colorMultiplier==1.0f)return color;
    u32 value=color&0xff000000;
    for(u32 shift:{0u,8u,16u}){const i32 scaled=i32(double((color>>shift)&255)*double(colorMultiplier));value|=(u32(scaled)&255)<<shift;}return value;
}
void ZunGraphics::transform(ScreenVertex& v)const{
    v.x=float(double(drawScale)*double(v.x)+double(originX));v.y=float(double(drawScale)*double(v.y)+double(originY));
}
void ZunGraphics::filled_rectangle(const CollisionRectangle& rect,u32 color){
    std::array<ScreenVertex,4> vertices;
    const float x[]={float(rect.left),float(double(rect.right)+1.0)},y[]={float(rect.top),float(double(rect.bottom)+1.0)};
    color=modulate(color);constexpr u32 order[]={0,1,3,2};
    for(u32 i=0;i<4;i++){auto& v=vertices[i];const u32 j=order[i];v.x=x[j&1];v.y=y[j>>1];v.z=0;v.reciprocalW=1;v.color=color;v.u=float(j&1);v.v=float(j>>1);transform(v);}
    backend.draw(0,Topology::Fan,vertices.data(),4);
}
void ZunGraphics::textured_quad(const std::array<std::array<float,4>,4>& points,const CollisionRectangle& rect,u32 color,u32 texture){
    const auto size=backend.texture_size(texture);if(!size.width||!size.height)return;
    std::array<ScreenVertex,4> vertices;constexpr u32 order[]={0,1,3,2};
    const float u[]={float(rect.left)/float(size.width),float(rect.right)/float(size.width)};
    const float v[]={float(rect.top)/float(size.height),float(rect.bottom)/float(size.height)};
    color=modulate(color);
    for(u32 i=0;i<4;i++){auto& out=vertices[i];out.x=points[order[i]][0]-0.5f;out.y=points[order[i]][1]-0.5f;out.u=u[i&1];out.v=v[i>>1];out.color=color;transform(out);}
    backend.draw(texture,Topology::Strip,vertices.data(),4);
}
bool ZunGraphics::projected_quad(const std::array<std::array<float,4>,4>& points,const CollisionRectangle& rect,float centerX,float centerY,float distance,u32 color,u32 texture){
    const auto size=backend.texture_size(texture);if(!size.width||!size.height)return false;
    std::array<ScreenVertex,4> vertices;color=modulate(color);
    for(u32 i=0;i<4;i++){
        auto& out=vertices[i];out.reciprocalW=float(double(distance)/(double(distance)-double(points[i][2])));if(!(out.reciprocalW<2.0f))return false;
        out.x=float((double(points[i][0])-double(centerX))*double(out.reciprocalW)+double(centerX)-0.5);
        out.y=float((double(points[i][1])-double(centerY))*double(out.reciprocalW)+double(centerY)-0.5);
        out.z=1.0f-out.reciprocalW/2.0f;
        out.u=float(i==0||i==3?rect.left:rect.right)/float(size.width);out.v=float(i<2?rect.top:rect.bottom)/float(size.height);out.color=color;transform(out);
    }
    backend.draw(texture,Topology::Fan,vertices.data(),4);return true;
}
void ZunGraphics::projected_strip(ScreenVertex* vertices,u32 count,u32 texture){
    for(u32 i=0;i<count;i++){auto& out=vertices[i];transform(out);out.reciprocalW=float(3000.0/(3000.0-double(out.z)));out.z=1.0f-out.reciprocalW/2.0f;out.color=modulate(out.color);}
    if(count>=3)backend.draw(texture,Topology::Strip,vertices,count);
}
}
