#include "BattlePortraits.hpp"
#include <cstdlib>
namespace th075 {
void render_story_portrait(ImageRenderer& images,const FighterActor& actor){
    const i32 level=actor.notice.level,duration=actor.notice.duration;
    const i32 l=level*level,d=duration*duration;
    const float x=actor.side?float(d+704+l):float(-l-d-96);
    images.image(actor.noticePortrait,x,float(l),u32(actor.notice.brightness)<<24|0xffffff,0,0,1,0,actor.side?1:0);
}
void render_fade_portrait(ImageRenderer& images,const FighterActor& actor){
    const u32 fade=actor.combat.fade;const u8 alpha=u8(160-std::abs(128-i32(fade))*5/4);
    images.image(actor.fadeKind,actor.side?float(fade/2+640):float(-i32(fade/2)),0,u32(alpha)<<24|0xffffff,0,0,1,0,actor.side?1:0);
}
void render_victory_portrait(ImageRenderer& images,const FighterActor& actor,i16 image,float slide,float y,u32 color){
    const float x=actor.side?float(((640.0-double(slide))-256.0)+160.0+160.0):float(((double(slide)+0.0)-256.0)+160.0);
    images.image(image,x,y,color,0,0,1,0,actor.side?1:0);
}
void render_recovery_markers(ImageRenderer& images,const BattleHudView& view,const CameraMotion& camera){
    images.blend(SpriteBlend::Alpha);const float cameraX=float(double(camera.x)-160.0);
    for(const auto* fighter:view.fighters){
        const auto& a=*fighter;const u32 flags=a.collision.frame?a.collision.frame->flags[0]:0;
        const i16 action=a.animation.cursor.action,recovery=a.movement.recoveryDelay;
        if((flags&0x100000)||a.combat.health<=0||action<50||action>149||a.motion.y<=0||recovery>=75||(action>=95&&action<=99))continue;
        u32 color=0xffff0000;
        if(recovery>=41){const u8 green=u8(i32((double(recovery)-40.0)/35.0*255.0));color=u32(u8(255-green))<<16|0xff00ff00;}
        else if(recovery>=21){const u8 green=u8(i32((double(recovery)-20.0)/20.0*255.0));color=u32(green)<<8|0xffff0000;}
        const double x=double(cameraX)+double(a.motion.x),y=(double(camera.y)+420.0)-double(a.motion.y);
        CollisionRectangle rect{i32(x),i32(y),i32(x+i32(a.motion.facing)*i32(recovery)*4),i32(y+3.0)};
        images.graphics.filled_rectangle(rect,color);rect.top+=3;rect.bottom+=1;images.graphics.filled_rectangle(rect,0xff000000);
    }
}
}
