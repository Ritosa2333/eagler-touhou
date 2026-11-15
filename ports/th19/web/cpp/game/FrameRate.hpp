#pragma once
namespace th19 {
// Count presented frames, independently of the fixed 60 Hz simulation clock.
struct FrameRate {
 double start=-1;int count=0;float value=0;
 void present(double now){
  if(start<0||now<start||now-start>2000){start=now;count=0;value=0;return;}
  ++count;const double elapsed=now-start;
  if(elapsed>=500){value=float(count*1000.0/elapsed);start=now;count=0;}
 }
};
}
