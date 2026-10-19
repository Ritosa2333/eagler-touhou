#include "BackgroundStage.hpp"
#include <cmath>
namespace th075 {
void BackgroundStage::draw_back_BG09a(ImageRenderer& images,const CameraMotion& camera){
    auto& g=images.graphics;
    const i32 cameraX=i32(double(camera.x)+160.0),cameraY=i32(camera.y);
    constexpr double radians=double(0.017453289f);
    const u32 red=u32(i32(std::sin(double(timer)/radians*0.0003)*128.0+127.0));
    const u32 green=u32(i32(std::sin(double(timer)/radians*0.0002)*64.0+191.0));
    const u32 blue=u32(i32(std::sin(double(timer)/radians*0.0005)*64.0+191.0));
    g.filter(Filter::Nearest);
    images.image(1,480,float(double(cameraY)*0.8+240.0),((red&255)<<16)|0xff000000|((green&255)<<8)|(blue&255),512,450,2);
    images.image(2,480,float(double(cameraY)*0.8+160.0),0xffffffff,256,256,2);
    images.image(0,float(double(cameraX)*0.1+464.0),float(cameraY+240),0xffffffff,512,232);
    images.blend(SpriteBlend::AlphaAdd);
    for(i32 i=0;i<129;i+=2){
        auto& left=waterVertices[i];auto& right=waterVertices[i+1];
        left.x=float((double(i)*double(camera.x)/128.0-60.0)-double(i*160)/128.0);
        right.x=float(double(i*160)/128.0+double(i)*double(camera.x)/128.0+1020.0);
        left.y=right.y=float(cameraY+264+i*2);left.z=right.z=0;
        left.u=0;right.u=0.68359375f;left.v=right.v=float(double(i+1)/128.0);
        left.color=right.color=0xffffffff;
    }
    images.strip(3,waterVertices.data(),130);
    const float upperY=float(double(cameraY)*0.8-4.0),lowerY=float(double(cameraY+384)-4.0);
    QuadPoints reflection{{{-150,upperY,0,0},{874,upperY,0,0},{float(double(cameraX)*0.5+1024.0-150.0),lowerY,0,0},{float(double(cameraX)*0.5-150.0),lowerY,0,0}}};
    images.quad(i16((timer/2)%21+5),reflection,{0,0,256,256},0xc0ffffff);
    images.blend(SpriteBlend::Alpha);
}
}
