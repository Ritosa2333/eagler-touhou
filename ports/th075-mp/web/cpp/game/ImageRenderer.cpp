#include "ImageRenderer.hpp"
#include <cmath>
namespace th075 {
void ImageRenderer::image(i16 index,float xValue,float yValue,u32 color,float anchorXValue,float anchorYValue,float scaleValue,float rotation,u8 flags,bool centeredScale){
    if(!graphics.backend.drawing_enabled())return;
    const auto resource=resources.image(index);if(!resource.texture||!resource.width||!resource.height)return;
    const double x=xValue,y=yValue,anchorX=anchorXValue,anchorY=anchorYValue,scale=scaleValue,width=resource.width,height=resource.height;
    QuadPoints points;
    if(scaleValue==1.0f&&rotation==0.0f){
        if(!centeredScale&&(flags&1))points=rectangle_quad(float(x+anchorX-width),float(y-anchorY),float(x+anchorX),float(y-anchorY+height));
        else points=rectangle_quad(float(x-anchorX),float(y-anchorY),float(x-anchorX+width),float(y-anchorY+height));
    }else if(scaleValue==1.0f||rotation!=0.0f){
        const float radians=float(double(rotation)*double(0.017453289f));
        const double c=float(std::cos(double(radians))*scale),s=float(std::sin(double(radians))*scale);
        points[0]={float(anchorY*s+(x-anchorX*c)),float((y-anchorX*s)-anchorY*c),0,0};
        points[3]={float((x-anchorX*c)-(height-anchorY)*s),float((height-anchorY)*c+(y-anchorX*s)),0,0};
        points[1]={float(anchorY*s+(width-anchorX)*c+x),float(((width-anchorX)*s+y)-anchorY*c),0,0};
        points[2]={float(((width-anchorX)*c+x)-(height-anchorY)*s),float((height-anchorY)*c+(width-anchorX)*s+y),0,0};
    }else if(centeredScale){
        points=rectangle_quad(float(x-anchorX*scale),float(y-anchorY*scale),float((width-anchorX)*scale+x),float((height-anchorY)*scale+y));
    }else if(flags&1){
        points=rectangle_quad(float((x+anchorX)-width*scale),float(y-anchorY),float(x+anchorX),float((height*scale+y)-anchorY));
    }else{
        points=rectangle_quad(float(x-anchorX),float(y-anchorY),float(width*scale+(x-anchorX)),float((height*scale+y)-anchorY));
    }
    if(flags&1){std::swap(points[0],points[1]);std::swap(points[2],points[3]);}
    graphics.textured_quad(points,{0,0,i32(resource.width),i32(resource.height)},color,resource.texture);
}
void ImageRenderer::quad(i16 index,const QuadPoints& points,const CollisionRectangle& rect,u32 color){const auto resource=resources.image(index);if(resource.texture)graphics.textured_quad(points,rect,color,resource.texture);}
void ImageRenderer::projected(i16 index,const QuadPoints& points,const CollisionRectangle& rect,float x,float y,float distance,u32 color){const auto resource=resources.image(index);if(resource.texture)graphics.projected_quad(points,rect,x,y,distance,color,resource.texture);}
void ImageRenderer::strip(i16 index,ScreenVertex* points,u32 count){const auto resource=resources.image(index);if(resource.texture)graphics.projected_strip(points,count,resource.texture);}
}
