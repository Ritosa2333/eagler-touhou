#pragma once
#include <algorithm>
#include <array>
#include <cmath>
namespace th12 {
// One authoritative tick's resolved adapter input. No DOM event touches the
// player position, and ordinary keyboard/replay frames leave this disabled.
struct AdaptedMotion {
  int mode=0, x=0, y=0;
  std::array<int,4> result{}; // actual fixed dx/dy, used mode, unlimited used.
  void begin(int kind,int a,int b) { mode=kind;x=a;y=b;result={}; }
  void finish() { mode=0; }
  bool apply(int& px,int& py,int speed) {
    if(mode<1||mode>4)return false;
    const int ox=px,oy=py;
    if(mode==4) { px+=x;py+=y; }
    else {
      double dx=mode==3?double(x)*speed/32767.:double(x)-px;
      double dy=mode==3?double(y)*speed/32767.:double(y)-py;
      const double distance=std::hypot(dx,dy);
      if(mode!=2&&distance>speed&&distance>0) { dx*=speed/distance;dy*=speed/distance; }
      px+=int(dx);py+=int(dy);
    }
    px=std::clamp(px,-0x5c00,0x5c00);py=std::clamp(py,0x1000,0xd800);
    result={px-ox,py-oy,mode,mode==2&&(px!=ox||py!=oy)};return true;
  }
};
extern AdaptedMotion adaptedMotion;
}
