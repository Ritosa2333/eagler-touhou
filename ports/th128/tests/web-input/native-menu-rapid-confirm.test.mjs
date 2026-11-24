import assert from 'node:assert/strict';
import test from 'node:test';
import {OriginalMainMenuController,OriginalPauseController} from '../../site/runtime/sound.mjs';
import {OriginalEndingInput} from '../../site/runtime/native-ending-bridge-v6.mjs';
import {bootPlay,functionSource} from './play-harness.mjs';

const aliases=['KeyC','ControlLeft'],rapid=0x200;
const no=async()=>{};
function title(rows){
 const module={UTF8ToString:x=>x,_th128_title_menu_state:()=>'{"ready":1,"transitioning":0,"page":0}',_th128_tick:()=>1,
  _th128_title_main_input:(held,pressed)=>{rows.push([held,pressed]);return 1;}};
 return new OriginalMainMenuController({module,prepare:no,consumeSound:no,consumeMusic:no,syncText:no});
}
function pause(rows){
 const module={UTF8ToString:x=>x,_th128_world_pause_state:()=>'{"active":true,"ready":true}',_th128_world_pause_open:()=>1,
  _th128_world_pause_tick:(held,pressed)=>{rows.push([held,pressed]);return 1;},_th128_world_pause_commit:()=>1};
 return new OriginalPauseController({module,releaseInput:no,consumeSound:no,audio:{enter:no,leave:no,suspend:no},onResume:no,onTitle:no,onRestart:no});
}
for(const [name,make] of [['title / options',title],['pause / Replay save',pause]]){
 test(`${name} passes the native confirm bit for C and left Ctrl`,async()=>{
  const rows=[],controller=make(rows);await controller.open?.();
  for(const code of aliases){controller.keyDown(code);controller.keyUp(code);await controller.step();}
  assert.deepEqual(rows,aliases.map(()=>[0,rapid]));
 });
 test(`${name} does not release or confirm again while another rapid key is held`,async()=>{
  const rows=[],controller=make(rows);await controller.open?.();
  controller.keyDown('ControlLeft');await controller.step();
  controller.keyDown('KeyC');controller.keyUp('ControlLeft');await controller.step();
  controller.keyUp('KeyC');await controller.step();
  assert.deepEqual(rows,[[rapid,rapid],[rapid,0],[0,0]]);
 });
}
test('ending and postrun name/Replay owners receive one confirm edge and the final release',()=>{
 for(const code of aliases){
  const input=new OriginalEndingInput();assert.equal(input.down(code),true);assert.equal(input.up(code),true);
  assert.deepEqual(input.sample(),{holding:0,pressed:rapid,released:rapid});
 }
 const input=new OriginalEndingInput();input.adoptHeld(new Set(['ControlLeft','KeyC']));
 input.up('ControlLeft');assert.deepEqual(input.sample(),{holding:rapid,pressed:0,released:0});
 input.up('KeyC');assert.deepEqual(input.sample(),{holding:0,pressed:0,released:rapid});
});
test('browser fallback menus treat each rapid-fire alias as Z, without sending a World shot',()=>{
 const e=bootPlay({worldMode:false});e.menu.page='info';
 for(const code of aliases){e.key('keydown',code);e.key('keyup',code);}
 assert.deepEqual(e.menus,aliases.map(()=>['menu','KeyZ']));assert.deepEqual(e.receipts,[]);e.input.dispose();
});

test('a rapid confirm held while changing scenes is suppressed until release',()=>{
 const state={pressed:new Set([...aliases,'ControlRight','KeyZ','Enter','ArrowLeft'])},suppressedKeys=new Set();
 new Function('state','suppressedKeys',functionSource('suppressHeldConfirmKeys')+';suppressHeldConfirmKeys();')(state,suppressedKeys);
 assert.deepEqual(suppressedKeys,new Set(['KeyZ','Enter',...aliases]));
});

for(const [name,make] of [['title / options',title],['pause / Replay save',pause]]){
 test(name+' ignores right Ctrl',async()=>{
  const rows=[],controller=make(rows);await controller.open?.();
  controller.keyDown('ControlRight');controller.keyUp('ControlRight');await controller.step();assert.deepEqual(rows,[[0,0]]);
 });
}
test('ending and browser fallback menus ignore right Ctrl',()=>{
 const input=new OriginalEndingInput();assert.equal(input.down('ControlRight'),false);assert.equal(input.up('ControlRight'),false);
 assert.deepEqual(input.sample(),{holding:0,pressed:0,released:0});
 const e=bootPlay({worldMode:false});e.menu.page='info';e.key('keydown','ControlRight');e.key('keyup','ControlRight');
 assert.deepEqual(e.menus,[]);e.input.dispose();
});
test('Replay fast-forward responds to left Ctrl only',()=>{
 const state={replayPlayback:{},thpracTools:{snapshot:()=>({frame:10}),speedFor:()=>({fast:4,slow:30,fps:60})},pressed:new Set()};
 const rate=new Function('state',functionSource('practiceLogicRate')+';return practiceLogicRate;')(state);
 state.pressed.add('ControlRight');assert.equal(rate(),60);
 state.pressed.add('ControlLeft');assert.equal(rate(),240);
});
