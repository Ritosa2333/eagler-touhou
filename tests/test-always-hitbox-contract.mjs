import fs from "node:fs";
import { workspacePath } from "../lib/workspace-layout.mjs";
import assert from "node:assert/strict";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import {parse} from 'acorn';
import {runInNewContext} from 'node:vm';
import {runNativeCpp} from './support/native-cpp.mjs';

const th06Options = fs.readFileSync(workspacePath("th06", "src/EaglerOptions.hpp"), "utf8");
const th06Player = fs.readFileSync(workspacePath("th06", "src/Player.cpp"), "utf8");
const player = fs.readFileSync(workspacePath("th07", "src/Player.cpp"), "utf8");
const th08Shell = fs.readFileSync(workspacePath("th08", "th08_web/sdl-runtime/shell.mjs"), "utf8");
const th08Host = fs.readFileSync(workspacePath("th08", "th08_web/cpp/sdl/GameHost.cpp"), "utf8");
const th08Form = fs.readFileSync(workspacePath("th08", "th08_web/cpp/game/PlayerForm.cpp"), "utf8");
const th08Scene = fs.readFileSync(workspacePath("th08", "th08_web/cpp/game/PlayerScene.hpp"), "utf8");
const th10Exports = fs.readFileSync(workspacePath("th10", "th10_web/cpp/platform/ApplicationExports.cpp"), "utf8");
const th10World = fs.readFileSync(workspacePath("th10", "th10_web/cpp/platform/WorldPlayer.cpp"), "utf8");
const th10Movement = fs.readFileSync(workspacePath("th10", "th10_web/cpp/game/PlayerMovement.cpp"), "utf8");
const coveredGames = new Set();

if (!th06Options.includes("AlwaysShowHitbox()") ||
    !th06Player.includes("EaglerOptions::AlwaysShowHitbox()") ||
    !th06Player.includes("DrawEaglerHitbox(")) {
  throw new Error("th06: required always-hitbox presentation is missing");
}
coveredGames.add("th06");

if (!player.includes("AnmVm g_EaglerHitboxVm")) {
  throw new Error("th07: always-hitbox visual must own a standalone render VM");
}
if (!player.includes("const Rng savedRng = g_Rng;") || !player.includes("g_Rng = savedRng;")) {
  throw new Error("th07: always-hitbox visual must preserve gameplay RNG around ANM execution");
}
if (/AlwaysShowHitbox\(\)[\s\S]{0,300}SpawnEffect\(24/.test(player)) {
  throw new Error("th07: always-show-hitbox must never allocate native EffectManager effect 24");
}
coveredGames.add("th07");

if (!th08Shell.includes("core.sdl_touch_display?.(options.alwaysHitbox?1:0)") ||
    !th08Host.includes('EX("sdl_touch_display")') ||
    !th08Form.includes("const bool show_hitbox=focused||always_hitbox") ||
    !th08Scene.includes("fixed_effect(22,p,2,0xffffffff)")) {
  throw new Error("th08: required always-hitbox must reuse the native focus marker without changing collision");
}
coveredGames.add("th08");

if (!th10Exports.includes('application_touch_display') ||
    !th10Exports.includes("always_hitbox=hitbox!=0") ||
    !th10World.includes("always_hitbox=&w.always_hitbox") ||
    !th10Movement.includes("focused||(env.always_hitbox&&*env.always_hitbox)") ||
    !th10Movement.includes("create(*env.effect_file,0x160,9,AnimationPlacement::WorldBack")) {
  throw new Error("th10: required always-hitbox must reuse the native focus marker without changing gameplay focus");
}
coveredGames.add("th10");

const th11Shell = fs.readFileSync(workspacePath('th11','th11_web/sdl-runtime/shell.mjs'),'utf8');
const th11Host = fs.readFileSync(workspacePath('th11','th11_web/cpp/sdl/Application.cpp'),'utf8');
const th11Battle = fs.readFileSync(workspacePath('th11','th11_web/cpp/game/GameBattle.cpp'),'utf8');
assert.match(th11Shell, /th11_always_hitbox\(/, 'th11: shell forwards the display option');
assert.match(th11Host, /session\.always_hitbox=on!=0/, 'th11: native display option owner');
assert.match(th11Battle, /!always_hitbox\|\|draw_hitbox/, 'th11: optional standalone hitbox drawing');
assert.match(th11Battle, /auto vm=\*n->value;vm\.position=/, 'th11: display changes a copy of the marker');
coveredGames.add('th11');

for (const game of ['th09','th15','th20']) {
  const hostFile = game==='th09'?'Application.cpp':game==='th15'?'BrowserRuntime.cpp':'HitboxDisplay.cpp';
  const host = fs.readFileSync(workspacePath(game,`${game}_web/cpp/sdl`,hostFile),'utf8');
  const symbol=game==='th20'?'sdl_always_hitbox':`${game}_always_hitbox`;
  assert.ok(host.includes(symbol), `${game}: native option endpoint`);
  const drawPath = game==='th09'?'th09_web/cpp/game/GameBattle.cpp':game==='th15'?'th15_web/cpp/game/PlayerDraw.cpp':'th20_web/cpp/sdl/HitboxDisplay.cpp';
  const draw=fs.readFileSync(workspacePath(game,drawPath),'utf8');
  assert.match(draw,/hitbox\(|hitbox_display\.sample|select_animation_template/,`${game}: standalone native marker`);
  if(game!=='th09')assert.doesNotMatch(draw,/execute_animation\(|\.tick\(|\.create\(/,`${game}: drawing must not execute or allocate game animation`);
  coveredGames.add(game);
}

// Run each actual directory-shell option adapter with native ABI spies. This
// catches a missing shell call even when the C++ display implementation exists.
for(const game of ['th08','th09','th10','th11','th15','th20']){
  const file=workspacePath(game,`${game}_web/sdl-runtime`,game==='th09'?'managed.mjs':'shell.mjs');
  const source=fs.readFileSync(file,'utf8'), ast=parse(source,{ecmaVersion:'latest',sourceType:'module'});
  const name=['th08','th10','th20'].includes(game)?'applyOptions':'apply';
  const declaration=ast.body.find(node=>node.type==='FunctionDeclaration'&&node.id?.name===name);
  assert.ok(declaration,`${game}: option application owner`);
  const symbol={th08:'sdl_touch_display',th09:'_th09_always_hitbox',th10:'application_touch_display',th11:'th11_always_hitbox',th15:'_th15_always_hitbox',th20:'sdl_always_hitbox'}[game];
  for(const enabled of [false,true,false]){
    const calls=[],core=new Proxy({}, {get:(_,key)=>(...args)=>{if(key===symbol)calls.push(args.at(-1));return 0;}});
    const options={alwaysHitbox:enabled};
    const context={core,Module:{},options,app:1,music:false,launched:false,multiplayerRuntime:false,
      runtimeBuild:{multiplayer:false},applyTouchOptions(){},practice:null,touchPolicy(){return 0;},fileExists(){return false;},root(){return '/save';}};
    runInNewContext(source.slice(declaration.start,declaration.end)+`;${name}()`,context);
    assert.deepEqual(calls,[enabled?1:0],`${game}: forward the exact setting through its native ABI`);
  }
}

// Execute TH10's real public setter and world creation method with lightweight
// dependency owners. Configure-before-title and subsequent scenes must inherit
// the option, even when there is no World at configuration time.
for(const owner of ['th10','th10mp']){
const exports=fs.readFileSync(workspacePath(owner,'th10_web/cpp/platform/ApplicationExports.cpp'),'utf8');
const th10App=fs.readFileSync(workspacePath(owner,'th10_web/cpp/platform/Application.cpp'),'utf8');
const setter=exports.match(/void application_touch_display\([^\n]+/)?.[0];
const ensure=th10App.slice(th10App.indexOf('bool Application::ensure_world()'),th10App.indexOf('void Application::advance_loading()'));
assert.ok(setter&&ensure);
await runNativeCpp(`#include <cassert>
using i32=int;
struct World{bool always_hitbox=false;};
template<class T,class... Args>T* create(Args&&...){return new T;}
namespace browser{struct Application{
 bool always_hitbox=false;World* world=nullptr;int error=0,state=0,engine=0,fonts=0,input=0,audio=0,effects=0;
 struct Startup{int* shared;int* scores;};Startup* startup=nullptr;
 bool ensure_world();
};}
using browser::Application;
${setter}
${ensure}
int main(){Application app;int value=0;Application::Startup startup{&value,&value};app.startup=&startup;
 application_touch_display(&app,1);assert(!app.world&&app.always_hitbox);
 assert(app.ensure_world()&&app.world->always_hitbox);
 application_touch_display(&app,0);assert(!app.world->always_hitbox);
 delete app.world;app.world=nullptr;assert(app.ensure_world()&&!app.world->always_hitbox);
 delete app.world;app.world=nullptr;application_touch_display(&app,1);assert(app.ensure_world()&&app.world->always_hitbox);
 delete app.world;application_touch_display(nullptr,1);
}`);
}

assert.deepEqual([...coveredGames].sort(), Object.keys(PRODUCT_GAMES).sort(),
  "required always-hitbox verification must be updated when a formal game adapter is registered");
console.log(`All-game always-hitbox presentation contract: PASS (${Object.keys(PRODUCT_GAMES).map(game => game.toUpperCase()).join("/")})`);
