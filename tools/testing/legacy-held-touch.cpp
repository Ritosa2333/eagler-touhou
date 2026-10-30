#include <cassert>
using f32=float;
struct {bool active=false;} g_MoveFinger;
float g_AccumDx=0,g_AccumDy=0;
bool g_MoveGestureUncaptured=false,g_UsedCheatMovementThisRun=false;
struct EaglerOptions {
 static bool TouchEnabled(){return true;}
 static bool TouchMovementUsesJoystick(){return false;}
 static bool UnlimitedTouch(){return false;}
};
struct Touch {
 static bool GetPlayerDelta(f32*,f32*);
 static void SetPlayerDelta(f32,f32);
 static bool TakePlayerDelta(f32*,f32*,bool*);
 static void ConsumePlayerDelta(f32,f32);
};
#include "touch-delta.inc"
int main(){
 // SDL owns this contact. Only deltas are consumed while the player cannot move.
 g_MoveFinger.active=true;g_MoveGestureUncaptured=true;
 float x=0,y=0;bool begin=false;
 Touch::SetPlayerDelta(20,0);assert(Touch::GetPlayerDelta(&x,&y)&&x==20);
 Touch::ConsumePlayerDelta(x,y);assert(g_MoveFinger.active);
 for(int frame=0;frame<30;frame++){
  Touch::SetPlayerDelta(2,0);assert(Touch::TakePlayerDelta(&x,&y,&begin));
  assert(x==2&&g_MoveFinger.active);assert(begin==(frame==0));
 }
 Touch::SetPlayerDelta(3,0);assert(Touch::GetPlayerDelta(&x,&y)&&x==3);
 g_MoveFinger.active=false;assert(!Touch::GetPlayerDelta(&x,&y)&&x==0&&y==0);
}
