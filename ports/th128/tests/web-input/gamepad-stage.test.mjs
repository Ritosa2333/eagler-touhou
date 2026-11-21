import test from 'node:test';
import assert from 'node:assert/strict';
import {bootPlay,functionSource} from './play-harness.mjs';
import {mountThprac128Menu} from '../../site/runtime/thprac128-menu.mjs';
import {installLauncherGamepadInput,mountStageGamepad} from '../../site/runtime/gamepad-input.mjs';
import {Element,flush} from '../practice-menu/surface.mjs';

function stageTick(e){
 const idle={advance:()=>({steps:0,stalled:false}),reset(){}};
 const globals={...e,localConfig:null,mountStageGamepad,practiceMenu:null,thpracToolsPanel:null,pollLocalBindings:null,requestAnimationFrame(){},clock:idle,
  originalMenuClock:idle,highFramerateCatchup:false,highFramerateEnabled:true,renderInterpolationPhase:()=>1,draw:()=>false,resetHighFrameratePresentation(){},
  originalPresentationFPS:{reset(){}},syncGameplayFrameUI(){},error:problem=>{throw problem;}};
 const keys=Object.keys(globals);
 return new Function(...keys,['stageGamepadOwner','pollStageGamepad'].map(functionSource).join('\n')+'\nasync '+functionSource('present')+';return present;')(...keys.map(k=>globals[k]));
}

test('the game polls its own handoff-safe controller path even with practice switched off',async()=>{
 const e=bootPlay(),tick=stageTick(e);
 const pad={connected:true,id:'controller fixture',axes:[.9,0],buttons:Array.from({length:16},()=>({pressed:false,value:0}))};
 e.window.navigator={getGamepads:()=>[pad]};
 try{
  pad.buttons[0].pressed=true;
  await tick(0);
  assert.deepEqual(e.receipts.at(-1),[129,1,0], 'the emulated controller travels through real keyboard ownership and World bit conversion');
  pad.axes=[0,-.9];pad.buttons[0].pressed=false;pad.buttons[2].pressed=true;pad.buttons[4].pressed=true;
  await tick(16.67);assert.deepEqual(e.receipts.at(-1),[536,8,0]);
  pad.axes=[0,0];pad.buttons[2].pressed=pad.buttons[4].pressed=false;pad.buttons[1].value=1;
  await tick(33.34);assert.deepEqual(e.receipts.at(-1),[2,2,0]);
  pad.connected=false;await tick(50);assert.deepEqual(e.receipts.at(-1),[0,0,2]);
  pad.connected=true;pad.buttons[1].value=0;pad.buttons[9].pressed=true;
  await tick(66.67);assert.deepEqual(e.pause,['pause']);
 }finally{e.input.dispose();}
});

test('a Launcher remap is authoritative and a held confirm from the old scene cannot fire in the new stage',async()=>{
 const e=bootPlay(),tick=stageTick(e);
 const identity={target:e.window,protocol:'eagler-touhou/1',game:'th128',epoch:9,origin:'https://preview.invalid',isReady:()=>true};
 const uninstall=installLauncherGamepadInput(identity);let mask=1;
 e.window.navigator={getGamepads:()=>[{connected:true,axes:[-.9,0],buttons:[]}]};
 e.window.__eaglerGamepadMapping={schema:'eagler-touhou/gamepad-input/1',protocol:identity.protocol,game:identity.game,epoch:identity.epoch,origin:identity.origin,enabled:()=>true,clear(){},sample:()=>mask};
 try{
  await tick(0);e.state.stageGamepad.suppressHeldActions();e.receipts.length=0;
  await tick(16);assert.deepEqual(e.receipts,[],'confirmed button remains ignored while held');
  mask=128;await tick(32);assert.deepEqual(e.receipts.at(-1),[128,128,0],'the selected authored gamepad overrides the raw left axis');
  mask=129;await tick(48);assert.deepEqual(e.receipts.at(-1),[129,1,0]);
  const before=e.receipts.length;mask=0;await tick(64);
  assert.equal(e.receipts.slice(before).reduce((released,[,,bit])=>released|bit,0),129);
  assert.equal(e.receipts.at(-1)[0],0);
 }finally{uninstall();e.input.dispose();}
});

function practiceFixture(mode){
 const root=new Element('section'),nodes=new Map();
 root.querySelectorAll=()=>[];
 root.querySelector=selector=>{
  const key=selector.match(/^\[data-thprac="([A-Za-z0-9_]+)"\]$/)?.[1];
  if(!key)return null;
  if(!nodes.has(key))nodes.set(key,new Element(['mode','startPoint','path','stage','difficulty','section','phase','warp','locale'].includes(key)?'select':'div'));
  return nodes.get(key);
 };
 const selected=[];
 const menu=mountThprac128Menu({root,getAvailability:()=>({ready:true,busy:false,inRun:false}),launch:async session=>{
  selected.push(session);return{session,stageId:session.params.stage+1,difficulty:session.difficulty,nativeTainted:true,normalModeClear:false};
 }});
 return {root,nodes,menu,selected,async open(){await menu.open({route:true});nodes.get('mode').value=String(mode);menu.refresh();}};
}
for(const mode of [0,1])test(`native controller confirm leaves the route panel in a valid state (practice mode ${mode})`,async()=>{
 const fixture=practiceFixture(mode);await fixture.open();
 assert.equal(fixture.menu.isOpen,true);
 const launched=[];
 const state={module:{_th128_title_sound_count:()=>0,_th128_title_main_action_ack:()=>1}};
 const run=new Function('state','practiceMenu','launchNativeRoute','async '+functionSource('handleOriginalMainAction')+';return handleOriginalMainAction;')(state,fixture.menu,route=>launched.push({route,open:fixture.menu.isOpen}));
 try{
  await run({mainAction:1,selection:0,mainActionSerial:1});await flush();
  assert.equal(fixture.menu.isOpen,false);
  assert.deepEqual(launched,mode?[]:[{route:0,open:false}]);
  assert.equal(fixture.selected.length,mode);
 }finally{fixture.menu.dispose();}
});
