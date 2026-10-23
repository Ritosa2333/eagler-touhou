import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import test from 'node:test';
import {runNativeCpp} from '../../launcher/tests/support/native-cpp.mjs';

test('one held touch swipe stays one navigation through the original input repeat threshold',async()=>{
  const root=resolve(import.meta.dirname,'../..');
  const source=await readFile(resolve(root,'ports/th08/web/cpp/game/InputController.cpp'),'utf8');
  const method=source.match(/void InputFrame::update\(u16 buttons,bool sticky\) noexcept \{[\s\S]*?\n\}/)?.[0];
  assert.ok(method,'The actual native input consumer must be available');
  const output=await runNativeCpp(`#include "TouchController.hpp"
#include <cstdint>
#include <cassert>
#include <iostream>
namespace th08 { using u16=std::uint16_t; struct InputFrame {
u16 current=0,previous=0,scrolling=0,held=0;
void update(u16 buttons,bool sticky=false) noexcept;
};
${method}
}
int main(){
  touhou::input::TouchController touch;
  touhou::input::TouchState menu;menu.context=0;
  touch.pointer(0,51,.5f,.3f,100,menu,false);
  touch.pointer(1,51,.5f,.5f,101,menu,false);
  th08::InputFrame native;
  int steps=0;
  for(int tick=0;tick<48;++tick){
    const auto sample=touch.sample(menu,102+tick,false,false);
    native.update(sample.keys[40]?32:0);
    if((native.current&32)&&((native.current&32)!=(native.previous&32)||native.scrolling))++steps;
  }
  assert(steps==1);
  touch.pointer(1,51,.5f,.3f,200,menu,false);
  touch.pointer(2,51,.5f,.3f,201,menu,false);
  assert(!touch.sample(menu,202,false,false).keys[90]);
  touch.pointer(0,52,.5f,.3f,300,menu,false);
  touch.pointer(1,52,.5f,.5f,301,menu,false);
  const auto next=touch.sample(menu,302,false,false);assert(next.keys[40]);
  std::cout<<"one gesture one step, no return-to-origin confirm, next gesture active";
}
`,{includes:[resolve(root,'ports/th08/portable/input'),resolve(root,'common/include')]});
  assert.match(output,/one gesture one step/);
});
