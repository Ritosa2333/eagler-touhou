#include "MenuDrawing.hpp"
namespace th075 {
void MenuDrawing::region(ImageRenderer& images,i16 image,const CollisionRectangle& bounds,float l,float t,float r,float b,u32 color){images.quad(image,rectangle_quad(l,t,r,b),bounds,color);}
void MenuDrawing::name(ImageRenderer& images,float x,float y,u32 color,const std::array<u8,8>& name){auto q=rectangle_quad(x,y,float(double(x)+19),float(double(y)+20));for(auto byte:name){const i32 n=i8(byte),l=(n%26)*19,t=(n/26)*20;images.quad(2,q,{l,t,l+19,t+20},color);translate_quad(q,14,0,0);}}
void MenuDrawing::digits(ImageRenderer& images,float x,float y,u32 color,i32 value,i32 count){auto q=rectangle_quad(float(double(x)-19),y,x,float(double(y)+20));while(count>0||value>=0){const i32 l=(value%10)*19;images.quad(2,q,{l,40,l+19,60},color);if(count>0){value/=10;if(--count==0)break;}else value=value<10?-1:value/10;translate_quad(q,-11,0,0);}}
void MenuDrawing::slot(ImageRenderer& images,float x,float y,u32 color,u8 index){name(images,x,y,color,{13,40,71,u8(i8(index)/10+52),u8(i8(index)%10+52),100,100,100});}
void MenuDrawing::replay_file(ImageRenderer& images,float x,float y,u32 color,const std::string& path){if(path.size()>=16&&path.compare(0,13,"replay/replay")==0)name(images,x,y,color,{13,40,71,u8(path[13]+4),u8(path[14]+4),u8(path[15]+4),100,100});else name(images,x,y,color,{20,44,30,43,100,100,100,100});}
void MenuDrawing::score(ImageRenderer& images,float x,float y,u32 color,i32 value,i8 style,i32 count){auto q=rectangle_quad(float(double(x)-16),y,x,float(double(y)+20));const bool fixed=count>0;while(count>0||value>=0){const i32 l=(value%10)*16,t=i32(style)*20+128;images.quad(2,q,{l,t,l+16,t+20},color);if(fixed){value/=10;if(--count==0)break;}else value=value<10?-1:value/10;translate_quad(q,fixed?-16:-15,0,0);}}
}
