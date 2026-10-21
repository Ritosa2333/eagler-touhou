import assert from 'node:assert/strict';
import test from 'node:test';
import {resolve} from 'node:path';
import {runNativeCpp} from '../../launcher/tests/support/native-cpp.mjs';

const includes=[
  resolve(import.meta.dirname,'../../ports/th08/portable/input'),
  resolve(import.meta.dirname,'../../common/include'),
];

test('production TouchController rebases blocked movement before applying a fresh pointer delta',async()=>{
  const output=await runNativeCpp(`#include "TouchController.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
using touhou::input::TouchController;
using touhou::input::TouchState;

int main(){
  TouchState ready{};
  ready.context=1;ready.instance=7;ready.ready=true;ready.x=100;ready.y=200;
  ready.fast=4;ready.slow=2;ready.min_x=-1000;ready.max_x=1000;
  ready.min_y=-1000;ready.max_y=1000;

  TouchController active;
  active.pointer(0,41,.5f,.5f,100,ready,false);
  const auto initial=active.sample(ready,101,false,false);
  assert(initial.motion==1&&active.active());
  active.pointer(1,41,.5125f,.5f,102,ready,false);
  const auto moved=active.sample(ready,103,false,false);
  assert(moved.motion==1&&moved.x>ready.x&&active.active());

  // Both branches start from identical live-primary state. While unavailable,
  // the next physical MOVE refreshes the pointer baseline but emits no motion.
  auto unavailable=ready;unavailable.ready=false;
  TouchController eventFirst=active;
  eventFirst.pointer(1,41,.5175f,.5f,104,unavailable,false);
  const auto blocked=eventFirst.sample(unavailable,105,false,false);
  assert(blocked.motion==0&&eventFirst.active());

  // If the first ready event arrives before an owner sample, production treats
  // it as a rebase: the gesture stays owned and no new target delta is applied.
  TouchController eventBeforeReadySample=eventFirst;
  eventBeforeReadySample.pointer(1,41,.5225f,.5f,106,ready,false);
  const auto rebased=eventBeforeReadySample.sample(ready,107,false,false);
  assert(rebased.motion==1&&eventBeforeReadySample.active());
  assert(std::abs(rebased.x-moved.x)<.0001f&&std::abs(rebased.y-moved.y)<.0001f);

  // In the other real ordering, the input owner samples the ready state first.
  // The following fresh MOVE is relative to the last physical point observed
  // while blocked, so only the new 0.005 normalized delta becomes a target.
  TouchController ownerSampleFirst=eventFirst;
  const auto recoverySample=ownerSampleFirst.sample(ready,106,false,false);
  assert(recoverySample.motion==1&&ownerSampleFirst.active());
  assert(std::abs(recoverySample.x-moved.x)<.0001f);
  ownerSampleFirst.pointer(1,41,.5225f,.5f,107,ready,false);
  const auto resumed=ownerSampleFirst.sample(ready,108,false,false);
  assert(resumed.motion==1&&ownerSampleFirst.active());
  assert(std::abs((resumed.x-moved.x)-3.2f)<.0002f);
  assert(std::abs(resumed.y-moved.y)<.0001f);
  std::cout<<"blocked owner sampling preserves the gesture and rebases before fresh movement";
}
`,{includes});
  assert.match(output,/rebases before fresh movement/);
});
