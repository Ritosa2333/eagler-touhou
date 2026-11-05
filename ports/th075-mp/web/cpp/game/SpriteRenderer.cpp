#include "SpriteRenderer.hpp"
namespace th075 {
void SpriteTrails::remove_front(){if(!entries.empty())entries.pop_front();}
void SpriteTrails::append(const SpriteTrail& value){entries.push_back(value);if(i32(entries.size())>maximum)remove_front();}
void SpriteRenderer::set_blend(u8 mode){
    if(mode>2)return;const SpriteBlend blend=mode==0?SpriteBlend::Alpha:mode==1?SpriteBlend::AlphaAdd:SpriteBlend::InverseColor;
    graphics.depth(mode==0?DepthMode::WriteEnabled:DepthMode::WriteDisabled);graphics.blend(blend);
}
void SpriteRenderer::draw(const SpriteTrail& sprite,const SpriteImage& image){
    const CollisionRectangle rect{0,0,i32(image.width),i32(image.height)};
    const auto& p=sprite.projection;
    if(p[0]==0&&p[1]==0&&p[2]==0)graphics.textured_quad(sprite.points,rect,sprite.color,image.texture);
    else graphics.projected_quad(sprite.points,rect,p[0],p[1],p[2],sprite.color,image.texture);
}
void SpriteRenderer::fighter(const ImageFrame& frame,const FighterMotion& motion,const SpritePose& pose,i16 action,i16 freeze,i8 flashTimer,i32 noticeTimer,SpriteTrails& history,TrailPolicy policy,float cameraX,float cameraY){
    const auto image=resources.image(i16(frame.sprite));if(!image.texture)return;
    SpriteTrail sprite;sprite.sprite=i16(frame.sprite);sprite.points=rectangle_quad(0,0,float(image.width),float(image.height));
    const float facing=float(motion.facing),cx=float(frame.originX),cy=float(frame.originY);
    if(frame.hasVisual){
        const auto& v=frame.visual;set_blend(v.mode);
        scale_quad(sprite.points,cx,cy,0,v.scale[0],v.scale[1],v.scale[2]);
        rotate_quad(sprite.points,cx,cy,0,v.rotation[0],v.rotation[1],v.rotation[2]);
        scale_quad(sprite.points,0,0,0,facing,1,1);
        sprite.color=(u32(v.alpha)<<24)|(u32(v.red)<<16)|(u32(v.green)<<8)|v.blue;
        for(u32 i=0;i<3;i++)sprite.projection[i]=float(v.projection[i]);
    }else{set_blend(0);if(frame.basicVisual)scale_quad(sprite.points,0,0,0,facing*2,2,2);}
    if(pose.active){
        if(pose.flags&0x80)graphics.monochrome(true);
        const float px=float((double(cx)+double(pose.offset[0]))*double(facing)),py=float(double(cy)-double(pose.offset[1])),pz=pose.offset[2];
        scale_quad(sprite.points,px,py,pz,pose.scale[0],pose.scale[1],pose.scale[2]);
        rotate_quad(sprite.points,px,py,pz,pose.rotation[0],pose.rotation[1],float(double(facing)*double(pose.rotation[2])));
        sprite.color=pose.color;for(u32 i=0;i<3;i++)sprite.projection[i]=float(pose.projection[i]);
    }
    translate_quad(sprite.points,float(double(motion.x)-i32(motion.facing)*i32(frame.originX)),float(-double(motion.y)-i32(frame.originY)),0);
    sprite.worldX=motion.x;sprite.worldY=motion.y;
    if(!policy.disabled){history.append(sprite);if(policy.trimIdle&&noticeTimer<1){history.remove_front();history.remove_front();}}
    float screenX=float(double(cameraX)-160.0);if(freeze>0&&action>=50&&action<150)screenX+=float((freeze%2)*-4+2);
    translate_quad(sprite.points,screenX,float(double(cameraY)+420.0),0);draw(sprite,image);
    if(flashTimer>0){set_blend(1);draw(sprite,image);}
    graphics.monochrome(false);
}
void SpriteRenderer::object(const ImageFrame& frame,const FighterMotion& motion,const SpritePose& pose,float cameraX,float cameraY){
    const auto image=resources.image(i16(frame.sprite));if(!image.texture)return;
    SpriteTrail sprite;sprite.sprite=i16(frame.sprite);sprite.points=rectangle_quad(0,0,float(image.width),float(image.height));
    const float facing=float(motion.facing),cx=float(frame.originX),cy=float(frame.originY);
    if(frame.hasVisual){
        const auto& v=frame.visual;set_blend(v.mode);scale_quad(sprite.points,cx,cy,0,v.scale[0],v.scale[1],v.scale[2]);
        rotate_quad(sprite.points,cx,cy,0,v.rotation[0],v.rotation[1],v.rotation[2]);scale_quad(sprite.points,0,0,0,facing,1,1);
        sprite.color=(u32(v.alpha)<<24)|(u32(v.red)<<16)|(u32(v.green)<<8)|v.blue;
        for(u32 i=0;i<3;i++)sprite.projection[i]=float(v.projection[i]);
    }else{set_blend(0);if(frame.basicVisual)scale_quad(sprite.points,0,0,0,facing*2,2,2);}
    if(pose.active){
        if(pose.flags&0x80)graphics.monochrome(true);
        const float px=float((double(cx)+double(pose.offset[0]))*double(facing)),py=float(double(cy)-double(pose.offset[1])),pz=pose.offset[2];
        scale_quad(sprite.points,px,py,pz,pose.scale[0],pose.scale[1],pose.scale[2]);
        rotate_quad(sprite.points,px,py,pz,pose.rotation[0],pose.rotation[1],float(double(facing)*double(pose.rotation[2])));
        sprite.color=pose.color;for(u32 i=0;i<3;i++)sprite.projection[i]=float(pose.projection[i]);
    }
    // Objects combine camera/world translation before its float store; fighter
    // trails intentionally store world positions before applying the camera.
    const float screenX=float(double(cameraX)-160.0);
    translate_quad(sprite.points,float((double(screenX)+double(motion.x))-i32(motion.facing)*i32(frame.originX)),float(((double(cameraY)+420.0)-double(motion.y))-i32(frame.originY)),0);
    if(sprite.projection[0]!=0||sprite.projection[1]!=0||sprite.projection[2]!=0){
        sprite.projection[0]=float((double(screenX)+double(motion.x))+double(sprite.projection[0]));
        sprite.projection[1]=float(((double(cameraY)+420.0)-double(motion.y))+double(sprite.projection[1]));
    }
    draw(sprite,image);graphics.monochrome(false);
}
void SpriteRenderer::screen(const ImageFrame& frame,const FighterMotion& motion,const SpritePose& pose){
    const auto image=resources.image(i16(frame.sprite));if(!image.texture)return;
    SpriteTrail sprite;sprite.sprite=i16(frame.sprite);sprite.points=rectangle_quad(0,0,float(image.width),float(image.height));
    const float facing=float(motion.facing),cx=float(frame.originX),cy=float(frame.originY);
    if(frame.hasVisual){
        const auto& v=frame.visual;set_blend(v.mode);scale_quad(sprite.points,cx,cy,0,v.scale[0],v.scale[1],v.scale[2]);
        rotate_quad(sprite.points,cx,cy,0,v.rotation[0],v.rotation[1],v.rotation[2]);scale_quad(sprite.points,0,0,0,facing,1,1);
        sprite.color=(u32(v.alpha)<<24)|(u32(v.red)<<16)|(u32(v.green)<<8)|v.blue;
        for(u32 i=0;i<3;i++)sprite.projection[i]=float(v.projection[i]);
    }else{set_blend(0);if(frame.basicVisual)scale_quad(sprite.points,0,0,0,facing*2,2,2);}
    if(pose.active){
        if(pose.flags&0x80)graphics.monochrome(true);
        const float px=float((double(cx)+double(pose.offset[0]))*double(facing)),py=float(double(cy)-double(pose.offset[1])),pz=pose.offset[2];
        scale_quad(sprite.points,px,py,pz,pose.scale[0],pose.scale[1],pose.scale[2]);
        rotate_quad(sprite.points,px,py,pz,pose.rotation[0],pose.rotation[1],float(double(facing)*double(pose.rotation[2])));
        sprite.color=pose.color;for(u32 i=0;i<3;i++)sprite.projection[i]=float(pose.projection[i]);
    }
    translate_quad(sprite.points,float(double(motion.x)-i32(motion.facing)*i32(frame.originX)),float(double(motion.y)-i32(frame.originY)),0);
    if(sprite.projection[0]!=0||sprite.projection[1]!=0||sprite.projection[2]!=0){
        sprite.projection[0]=float(double(motion.x)+double(sprite.projection[0]));
        sprite.projection[1]=float(double(motion.y)+double(sprite.projection[1]));
    }
    draw(sprite,image);graphics.monochrome(false);
}
void SpriteRenderer::shadow(const ImageFrame& frame,const FighterMotion& motion,const SpritePose& pose,const ShadowStyle& style,float cameraX,float cameraY,float groundOffset){
    const auto image=resources.image(i16(frame.sprite));if(!image.texture)return;
    SpriteTrail sprite;sprite.sprite=i16(frame.sprite);sprite.points=rectangle_quad(0,0,float(image.width),float(image.height));
    const float facing=float(motion.facing),cx=float(frame.originX),cy=float(frame.originY);
    if(frame.hasVisual){
        const auto& v=frame.visual;scale_quad(sprite.points,cx,cy,0,v.scale[0],v.scale[1],v.scale[2]);
        rotate_quad(sprite.points,cx,cy,0,v.rotation[0],v.rotation[1],v.rotation[2]);scale_quad(sprite.points,0,0,0,facing,1,1);
    }else if(frame.basicVisual)scale_quad(sprite.points,0,0,0,facing*2,2,2);
    if(pose.active){
        const float px=float((double(cx)+double(pose.offset[0]))*double(facing)),py=float(double(cy)-double(pose.offset[1])),pz=pose.offset[2];
        scale_quad(sprite.points,px,py,pz,pose.scale[0],pose.scale[1],pose.scale[2]);
        rotate_quad(sprite.points,px,py,pz,pose.rotation[0],pose.rotation[1],float(double(facing)*double(pose.rotation[2])));
    }
    translate_quad(sprite.points,float(double(motion.x)-i32(motion.facing)*i32(frame.originX)),float(-double(motion.y)-i32(frame.originY)),0);
    scale_quad(sprite.points,0,0,0,style.scale[0],style.scale[1],style.scale[2]);
    rotate_quad(sprite.points,0,0,0,style.rotation[0],style.rotation[1],style.rotation[2]);
    sprite.color=style.color;sprite.projection=style.projection;
    const float screenX=float(double(cameraX)-160.0);
    translate_quad(sprite.points,screenX,float(double(cameraY)+420.0+double(groundOffset)),0);
    if(sprite.projection[0]!=0||sprite.projection[1]!=0||sprite.projection[2]!=0){sprite.projection[0]=float(double(sprite.projection[0])+double(screenX));sprite.projection[1]=float(double(sprite.projection[1])+double(cameraY));}
    graphics.filter(Filter::Linear);draw(sprite,image);graphics.filter(Filter::Nearest);
}
void SpriteRenderer::trails(const SpriteTrails& history,float cameraX,float cameraY){
    if(history.entries.empty()||history.interval<=0||history.maximum==0)return;
    u32 index=0;for(const auto& entry:history.entries){
        if(index%u32(history.interval)==u32(history.interval-1)){
            const auto image=resources.image(entry.sprite);if(image.texture){
                set_blend(entry.blend);SpriteTrail sprite=entry;
                sprite.color=((((sprite.color>>24)*index)/u32(i32(history.maximum)))<<24)|(history.colorMask&sprite.color);
                translate_quad(sprite.points,float(double(cameraX)-160.0),float(double(cameraY)+420.0),0);draw(sprite,image);
            }
        }index++;
    }
}
}
