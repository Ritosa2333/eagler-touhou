#include "CameraMotion.hpp"
#include <algorithm>
#include <cmath>
namespace th075 {
void CameraMotion::reset_parameters(){biasX=0;biasY=20;amplitude=angleX=angleY=0;}
void CameraMotion::initialize(const FighterMotion& first,const FighterMotion& second){advance(first,second);smoothX=targetX;smoothY=targetY;x=targetX;y=float(double(targetY)+300.0);}
void CameraMotion::advance(const FighterMotion& first,const FighterMotion& second){
    targetX=float(640.0-((50.0-double(biasX))*double(first.x)+(50.0+double(biasX))*double(second.x))/100.0);
    if(targetX<-320)targetX=-320;if(targetX>320)targetX=320;if(fixed)targetX=fixedX;
    if(targetX<=smoothX){smoothX=float(double(smoothX)-((double(smoothX)-double(targetX))/30.0+5.0));if(smoothX<targetX)smoothX=targetX;}
    else{smoothX=float(double(smoothX)-((double(smoothX)-double(targetX))/30.0-5.0));if(smoothX>targetX)smoothX=targetX;}
    const double separation=std::fabs(double(float(double(first.x)-double(second.x))));const i32 verticalLimit=std::max(0,std::min(320,i32(880.0-separation)));
    const float verticalWeighted=float((50.0-double(biasY))*double(first.y)-(50.0+double(biasY))*double(second.y));targetY=float(std::fabs(double(verticalWeighted))/100.0);
    if(float(verticalLimit/3)<targetY)targetY=float(verticalLimit/3);if(fixed)targetY=fixedY;
    if(targetY<=smoothY){smoothY=float(double(smoothY)-((double(smoothY)-double(targetY))/20.0+1.0));if(smoothY<targetY)smoothY=targetY;}
    else{smoothY=float(double(smoothY)-((double(smoothY)-double(targetY))/20.0-1.0));if(smoothY>targetY)smoothY=targetY;}
    if(amplitude>=1){
        const float horizontalAngle=float(double(angleX)*double(0.017453289f)),verticalAngle=float(double(angleY)*double(0.017453289f));shakeX=float(std::cos(double(horizontalAngle))*double(amplitude));shakeY=float(std::sin(double(verticalAngle))*double(amplitude));
        angleX=float(double(angleX)+83.0);if(angleX>=360)angleX=float(double(angleX)-360.0);angleY=float(double(angleY)+107.0);if(angleY>=360)angleY=float(double(angleY)-360.0);
        amplitude=float(double(amplitude)*double(0.95f));if(amplitude<1)amplitude=shakeX=shakeY=0;
    }
    targetSpan=float(880.0-separation);if(fixed)targetSpan=fixedSpan;
    if(targetSpan>320)targetSpan=320;else{const double minimum=std::fabs(double(float(320.0*double(biasX)/50.0)));if(double(targetSpan)<minimum)targetSpan=float(minimum);}
    if(span<=targetSpan){span=float(double(span)-((double(span)-double(targetSpan))/40.0-11.0));if(span>targetSpan)span=targetSpan;}
    else{span=float(double(span)-((double(span)-double(targetSpan))/40.0+11.0));if(span<targetSpan)span=targetSpan;}
    if(biasX<0){biasX=float(double(biasX)+10.0);if(biasX>0)biasX=0;}else if(biasX>0){biasX=float(double(biasX)-10.0);if(biasX<0)biasX=0;}
    if(first.y>second.y)biasY=40;else if(first.y<second.y)biasY=-40;
    x=float(double(smoothX)+double(shakeX));y=float(double(smoothY)+double(shakeY)+300.0);
}
void CameraMotion::snap(){if(!fixed){x=targetX;span=targetSpan;smoothY=targetY;}else{x=fixedX;span=fixedSpan;smoothY=fixedY;}y=float(double(smoothY)+300.0);smoothX=x;}
}
