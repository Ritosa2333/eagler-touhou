#include "QuadTransform.hpp"
namespace th075 {
namespace {
constexpr u32 cosineBits[3600]={
#include "AngleTable.inc"
};
float lookup(double tenths){
    const i32 signedIndex=i32(tenths);
    const u32 magnitude=signedIndex<0?0u-u32(signedIndex):u32(signedIndex);
    float value;const u32 bits=cosineBits[magnitude%3600];std::memcpy(&value,&bits,4);return value;
}
}
float angle_cos(float degrees){return lookup(double(degrees)*10.0);}
float angle_sin(float degrees){return lookup(double(degrees)*10.0-900.0);}
double angle_tan(float degrees){const float cosine=angle_cos(degrees);return cosine==0?0:double(angle_sin(degrees))/double(cosine);}
QuadPoints rectangle_quad(float left,float top,float right,float bottom){return {{{left,top,0,0},{right,top,0,0},{right,bottom,0,0},{left,bottom,0,0}}};}
void translate_quad(QuadPoints& points,float x,float y,float z){for(auto& p:points){p[0]+=x;p[1]+=y;p[2]+=z;}}
void scale_quad(QuadPoints& points,float centerX,float centerY,float centerZ,float x,float y,float z){
    const float center[]={centerX,centerY,centerZ},scale[]={x,y,z};
    for(u32 axis=0;axis<3;axis++)if(scale[axis]!=1.0f)
        for(auto& p:points)p[axis]=float((double(p[axis])-double(center[axis]))*double(scale[axis])+double(center[axis]));
}
void rotate_quad(QuadPoints& points,float centerX,float centerY,float centerZ,float xDegrees,float yDegrees,float zDegrees){
    const double cx=centerX,cy=centerY,cz=centerZ;
    if(zDegrees!=0){
        const double c=angle_cos(zDegrees),s=angle_sin(zDegrees);
        for(auto& p:points){const double oldX=p[0],oldY=p[1];p[0]=float(((oldX-cx)*c+cx)-(oldY-cy)*s);p[1]=float((oldY-cy)*c+(oldX-cx)*s+cy);}
    }
    if(yDegrees!=0){
        const double c=angle_cos(yDegrees),s=angle_sin(yDegrees);
        for(auto& p:points){const double oldX=p[0],oldZ=p[2];p[0]=float((oldX-cx)*c+(oldZ-cz)*s+cx);p[2]=float(((oldZ-cz)*c+cz)-(oldX-cx)*s);}
    }
    if(xDegrees!=0){
        const double c=angle_cos(xDegrees),s=angle_sin(xDegrees);
        for(auto& p:points){const double oldY=p[1],oldZ=p[2];p[1]=float((oldY-cy)*c+(oldZ-cz)*s+cy);p[2]=float(((oldZ-cz)*c+cz)-(oldY-cy)*s);}
    }
}
}
