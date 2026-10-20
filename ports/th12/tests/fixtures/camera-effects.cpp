#include "../../src/game/CameraEffects.hpp"
namespace th12 { Random random; }
namespace {
th12::CameraEffects manager;
float result[37];
}
extern "C" {
void camera_prepare() { manager.reset();th12::random = {42,0}; }
void camera_birth(int amplitude,int rise,int hold,int fall) { manager.shake(amplitude,rise,hold,fall); }
void camera_update(float rate,unsigned flags,int stopping) {
  // Previous-frame original Draw42 resets these fields between updates.
  manager.offsets = {};
  manager.tick(th12::random,rate,flags,stopping);
}
const float* camera_snapshot() {
  result[0] = float(th12::random.seed);
  result[1] = float(th12::random.calls);
  result[2] = manager.offsets[0];
  result[3] = manager.offsets[1];
  result[4] = float(manager.shakes.size());
  for(size_t i=0;i<manager.shakes.size();++i) {
    result[5+2*i] = float(manager.shakes[i].age);
    result[6+2*i] = manager.shakes[i].elapsed;
  }
  return result;
}
}
