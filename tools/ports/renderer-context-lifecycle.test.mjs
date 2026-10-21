import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {runNativeCpp} from '../../launcher/tests/support/native-cpp.mjs';

const root=resolve(import.meta.dirname,'../..');
const renderer=readFileSync(resolve(root,'ports/th08/portable/sdl/Renderer.cpp'),'utf8');
const lifecycle=readFileSync(resolve(root,'ports/th08/portable/sdl/RendererContextLifecycle.hpp'),'utf8');
const gameHost=readFileSync(resolve(root,'ports/th08/web/cpp/sdl/GameHost.cpp'),'utf8');

function blockAfter(source, anchor) {
  const start=source.indexOf(anchor);
  assert.notEqual(start,-1,`missing source anchor: ${anchor}`);
  const open=source.indexOf('{',start);
  let depth=0;
  for(let i=open;i<source.length;i++){
    if(source[i]==='{')depth++;
    if(source[i]==='}'&&--depth===0)return source.slice(open+1,i);
  }
  assert.fail(`unclosed source block: ${anchor}`);
}

test('renderer initializes only missing SDL subsystems and requires a live owner context for GL release',()=>{
  const init=blockAfter(renderer,'bool Renderer::initialize()');
  assert.match(init,/initialize_missing_subsystems\(rendererSubsystems/);
  const initHelper=blockAfter(lifecycle,'bool initialize_missing_subsystems(');
  assert.match(initHelper,/required\s*&\s*~is_initialized\(required\)/);
  assert.match(initHelper,/return\s+!missing\s*\|\|\s*initialize\(missing\)/);

  const releaseHelper=blockAfter(lifecycle,'bool release_with_current_context(');
  assert.match(releaseHelper,/if\s*\(!window\s*\|\|\s*!context\s*\|\|\s*!make_current\(window,\s*context\)\)\s*return false/);
  assert.ok(releaseHelper.indexOf('make_current(window, context)')<releaseHelper.indexOf('release_objects()'));

  const destructor=blockAfter(renderer,'Renderer::~Renderer()');
  const discard=blockAfter(renderer,'void Renderer::discard()');
  assert.doesNotMatch(discard,/\bgl[A-Z]\w*\s*\(/);
  assert.ok(destructor.indexOf('discard()')<destructor.indexOf('release_with_current_context'));
  assert.ok(destructor.indexOf('release_with_current_context(window,context')<destructor.indexOf('glDeleteVertexArrays'));
  assert.ok(destructor.indexOf('release_with_current_context(window,context')<destructor.search(/\bgl[A-Z]\w*\s*\(/));
  assert.ok(destructor.indexOf('glDeleteVertexArrays')<destructor.indexOf('SDL_GL_DestroyContext(context)'));
  assert.match(destructor,/if\(!released\)[\s\S]*std::abort\(\)/);
});

test('production SDL lifecycle helpers execute idempotent subsystem init and context-owned release order',async()=>{
  const result=await runNativeCpp(`#include "RendererContextLifecycle.hpp"
#include <cassert>
#include <vector>
int main(){
  using touhou::sdl::detail::initialize_missing_subsystems;
  using touhou::sdl::detail::release_with_current_context;
  using Flags=unsigned;
  Flags initialized=0x3;
  int initCalls=0;
  Flags initializedWith=0;
  auto isInitialized=[&](Flags){return initialized;};
  auto initialize=[&](Flags missing){++initCalls;initializedWith=missing;initialized|=missing;return true;};
  assert(initialize_missing_subsystems<Flags>(0x3,isInitialized,initialize));
  assert(initCalls==0);
  initialized=0x1;
  assert(initialize_missing_subsystems<Flags>(0x3,isInitialized,initialize));
  assert(initCalls==1&&initializedWith==0x2);
  assert(initialize_missing_subsystems<Flags>(0x3,isInitialized,initialize));
  assert(initCalls==1);

  struct Window{} window;
  int contextToken=1;
  int currentCalls=0,releaseCalls=0;
  auto makeCurrent=[&](Window*,int*){++currentCalls;return true;};
  auto release=[&]{++releaseCalls;};
  assert((!release_with_current_context<Window,int*>(nullptr,&contextToken,makeCurrent,release)));
  assert((!release_with_current_context<Window,int*>(&window,nullptr,makeCurrent,release)));
  assert(currentCalls==0&&releaseCalls==0);
  auto failCurrent=[&](Window*,int*){++currentCalls;return false;};
  assert((!release_with_current_context<Window,int*>(&window,&contextToken,failCurrent,release)));
  assert(currentCalls==1&&releaseCalls==0);
  std::vector<int> order;
  auto orderedCurrent=[&](Window*,int*){order.push_back(1);return true;};
  auto orderedRelease=[&]{order.push_back(2);};
  assert((release_with_current_context<Window,int*>(&window,&contextToken,orderedCurrent,orderedRelease)));
  assert((order==std::vector<int>{1,2}));
}` ,{includes:[resolve(import.meta.dirname,'../../ports/th08/portable/sdl')]});
  assert.equal(result,'');
});

test('the WebGL owner does not globally quit SDL at session teardown or issue GL deletes without its context',()=>{
  assert.doesNotMatch(renderer,/SDL_Quit\s*\(/);
  assert.match(gameHost,/SDL_WasInit\(SDL_INIT_GAMEPAD\)/);
  const close=blockAfter(gameHost,'EX("sdl_game_close")');
  assert.ok(close.indexOf('runtime.reset()')<close.indexOf('sdl_detach()'));
});
