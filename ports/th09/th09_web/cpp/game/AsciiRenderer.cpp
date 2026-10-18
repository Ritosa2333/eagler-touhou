#include "AsciiRenderer.hpp"
#include <cstdio>
#include <algorithm>
namespace th09 {
bool AsciiRenderer::initialize(){
    std::memset(&glyph,0,sizeof(glyph));std::memset(&digit,0,sizeof(digit));
    std::memset(queue.data(),0,sizeof(queue));std::memset(popups.data(),0,sizeof(popups));
    count=0;color=0xffffffff;scale={1,1};field_view=0;spacing=9;popup_cursor=0;
    auto* file=resources.animation(AnimationFile::ascii);if(!file)return false;
    glyph.Initialize();digit.Initialize();glyph.anmFile=digit.anmFile=file;
    if(file->SetSprite(&glyph,32)||file->SetSprite(&digit,97))return false;
    digit.pos.z=.1f;return true;
}
void AsciiRenderer::text(const Vec3& p,const char* s){
    if(!s||count>=queue.size())return;auto& q=queue[count++];
    const auto length=std::min(std::strlen(s),sizeof(q.text)-1);std::memcpy(q.text,s,length);q.text[length]=0;
    q.position=p;q.color=color;q.scale=scale;q.field_view=field_view;q.reserved=0;
}
void AsciiRenderer::number(const Vec3& p,i32 width,i32 value){char s[64];std::snprintf(s,sizeof(s),"%*d",std::min(width,32),value);text(p,s);}
void AsciiRenderer::clock(const Vec3& p,i32 stage,i32 minutes,i32 seconds){char s[64];if(stage>=0)std::snprintf(s,sizeof(s),"STAGE %d %.2d:%.2d",stage,minutes,seconds);else std::snprintf(s,sizeof(s),"%.2d:%.2d",minutes,seconds);text(p,s);}
void AsciiRenderer::popup(i32 side,const Vec3& p,i32 value,u32 rgba){
    if(side<0||side>1)return;if(popup_cursor>99)popup_cursor=0;auto& q=popups[side][popup_cursor++];
    q.active=1;q.count=0;
    if(value<=0){q.digits[0]=value<0?10:0;q.count=1;}
    else do{q.digits[q.count++]=u8(value%10);value/=10;}while(value);
    q.position=p;q.color=rgba;q.time.reset();
}
void AsciiRenderer::update(const PopupFrame& f){
    if(f.paused||f.game_over||(f.game_flags&0x1800))return;
    for(u32 side=0;side<2;++side)if(!(f.field_flags[side]&1))for(auto& q:popups[side])if(q.active){
        q.position.y-=f.timing.rate*.2f;q.time.tick(f.timing);if(q.time.current>60)q.active=0;
    }
}
void AsciiRenderer::draw_popups(i32 side,const PlayfieldGeometry& geometry,const Vec3& player){
    if(side<0||side>1)return;auto* file=resources.animation(AnimationFile::ascii);if(!file)return;output.ascii_view(side);
    for(const auto& q:popups[side])if(q.active){
        const auto p=geometry.to_screen({q.position.x-float(q.count*4),q.position.y,0});digit.pos.x=p.x;digit.pos.y=p.y;
        const float dx=player.x-q.position.x,dy=player.y-q.position.y;const i32 distance=i32(dy*dy+dx*dx);
        const u32 alpha=distance>4096?208:distance>1024?80+u32((distance-1024)*128/3072):80;
        digit.color1.d3dColor=i32((q.color&0xffffff)|(alpha<<24));
        for(i32 n=q.count-1;n>=0;--n){const auto id=u32(q.digits[n])+97;if(id>=file->spriteCount)continue;
            digit.loadedSprite=&file->sprites[id];digit.spriteSize.x=digit.loadedSprite->widthPx;
            output.ascii_sprite(digit);digit.pos.x+=8;
        }
    }
}
void AsciiRenderer::draw_text(){
    auto* file=resources.animation(AnimationFile::ascii);if(!file)return;glyph.flags|=0x1801;i32 previous_view=1;
    for(u32 n=0;n<count;++n){const auto& q=queue[n];glyph.pos=q.position;glyph.scale=q.scale;
        if(previous_view!=q.field_view){output.ascii_view(q.field_view?0:2);previous_view=q.field_view;}
        for(const auto* p=reinterpret_cast<const u8*>(q.text);*p;++p){
            if(*p==10){glyph.pos.y+=q.scale.y*14;glyph.pos.x=q.position.x;}
            else {const u32 index=u32(*p)-32;if(*p!=32&&index<file->spriteCount){glyph.loadedSprite=&file->sprites[index];glyph.color1.d3dColor=i32(q.color);output.ascii_sprite(glyph);}glyph.pos.x+=float(spacing)*q.scale.x;}
        }
    }
    if(previous_view)output.ascii_view(2);
}
}
