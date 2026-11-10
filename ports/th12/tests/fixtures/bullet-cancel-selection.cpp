#include "../../src/bullet.hpp"
#include "../../src/game/BulletPool.hpp"
using namespace th12;
struct SelectionWorld : eb::World {
  void emit(const eb::Emission&) override {}
  void unsupported(uint32_t) override {}
} world;
extern "C" int cancel_selection(int kind, int color, int previous, int target) {
  eb::State state;
  state.cancelEffect=previous;
  eb::Program program;
  if(target>=0) eb::set(program,0,false,0x1000000,target,0,0,0);
  eb::initialize(state,1,program,kind,color,0,0,0,0,world);
  return state.cancelEffect;
}
