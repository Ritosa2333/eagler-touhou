#pragma once
#include <array>
#include <algorithm>
namespace th19 {
struct SpiritRequest {
    float x=0,y=0; // source field position
    int kind=0,inherited=0;
};
// 0x51d200 initializes 40; 0x51afe0 raises the limit at these frame counts.
inline int spiritCapacity(int frame){return frame<3600?40:frame<7200?60:frame<10800?80:999;}
struct SpiritTransit {
    SpiritRequest request;
    float targetX=0,targetY=0;
    int age=-1,duration=60,script=19;
    static int flightDuration(unsigned draw){return 60+int((draw&65535u)%120u);}
    // 0x51b480 advances the current clock before checking the end.
    bool tick(){return ++age>duration;}
};
struct SpiritLaunch {
    SpiritTransit transit;
    float startTangentX=0,endTangentX=0;
};
// 0x51b6c0 selects an animation and adjusts the initial target by species.
inline SpiritTransit prepareSpiritTransit(SpiritRequest request,unsigned delayDraw,
    float targetX,float targetY,float signedDraw1,float signedDraw2,
    unsigned signDraw,float opponentX){
    SpiritTransit t;t.request=request;t.duration=SpiritTransit::flightDuration(delayDraw);
    t.targetX=targetX;t.targetY=targetY;
    if(request.kind==2){t.script=22;t.targetX=std::clamp(opponentX+signedDraw1*48.f,-140.f,140.f);}
    else if(request.kind==3){t.script=25;t.targetY=signedDraw1*128.f+144.f;
        t.targetX=signedDraw2*64.f-148.f;if(!(signDraw&1))t.targetX=-t.targetX;
        t.targetX=std::clamp(t.targetX,-140.f,140.f);}
    else if(request.kind==4)t.script=28;
    return t;
}
// EXE 0x51d4e0, only the kind 1..4 death branch. Distances are field units.
inline int deathSpiritKind(float x,float y,float playerX,float playerY){
    const float dx=playerX-x,dy=playerY-y,d2=dx*dx+dy*dy;
    return d2<=4624.f?2:d2<=16384.f?3:4;
}
struct SpiritArrival {
    const char* sub=nullptr;int targetSide=0,life=200;
    std::array<int,4> variables{};
};
// 0x51b480: consume an already-arrived request, not an immediate pickup.
// Enhancement belongs to the sender; suppression belongs to the receiver.
inline SpiritArrival spiritArrival(int sender,int kind,int inherited,float x,bool enhanced,bool suppressed){
    SpiritArrival a;a.targetSide=1-sender;
    switch(kind){case 1:a.sub="GhostPure";break;case 2:a.sub="GhostWolf";break;
      case 3:a.sub=x>=0?"GhostOtterL":"GhostOtterR";break;case 4:a.sub="GhostEagle";break;default:return a;}
    a.variables[0]=inherited+1;a.variables[2]=inherited+1;
    if(kind!=1){a.variables[0]+=enhanced?1:0;a.variables[1]=enhanced?1:0;
      if(suppressed){a.variables[0]=std::max(0,a.variables[0]-1);a.variables[1]=2;}}
    return a;
}
}
