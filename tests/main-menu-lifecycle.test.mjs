import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import vm from 'node:vm';
import {OriginalMainMenuController,OriginalMainSettingsStore} from '../site/runtime/sound.mjs';

const playSource=fs.readFileSync(process.env.TH128_PLAY_SOURCE??new URL('../site/play.mjs',import.meta.url),'utf8');
function productionFunction(name){
  const match=new RegExp('^(?:async )?function '+name+'\\(','m').exec(playSource);
  assert(match,'Production function is available: '+name);
  const start=match.index,lineEnd=playSource.indexOf('\n',start);
  const end=playSource.slice(start,lineEnd).trimEnd().endsWith('}')?lineEnd:playSource.indexOf('\n}',start)+2;
  return playSource.slice(start,end);
}
function environment(functions,extra={}){
  const elements=new Map(),writes=[],messages=[],errors=[],nativeCalls=[];
  const saved={...OriginalMainSettingsStore.defaults(),bgm:85,se:70,debugClearMask:63,seenMask:5};
  const state={mainSavedSettings:structuredClone(saved),worldMode:false,active:true,paused:false,
    pressed:new Set(),module:{_th128_title_extra_save(mask){nativeCalls.push(mask);return 1;}},
    extraMenu:{refresh(){}},mainStorageTask:null};
  const menu={launching:false,musicRequestId:0};
  const context={state,menu,JSON,Uint8Array,Int32Array,structuredClone,
    find(selector){if(!elements.has(selector))elements.set(selector,{hidden:false,disabled:false,setAttribute(){}});return elements.get(selector);},
    status:message=>messages.push(message),error:problem=>errors.push(problem),
    nativeMenuSnapshot:()=>({available:true,page:0,ready:1,transitioning:0}),
    nativePageNames:['main','difficulty','route'],clock:{reset(){}},setDebugExtraSaveEligibility(){},
    suppressHeldConfirmKeys(){},window:{confirm:()=>true},
    updateOwnedMainSettings:async change=>{const changes=change(state.mainSavedSettings);writes.push(structuredClone(changes));return {...state.mainSavedSettings,...changes,revision:state.mainSavedSettings.revision+1};},
    originalMainController:()=>state.mainController,
    ...extra};
  vm.createContext(context);
  vm.runInContext(functions.map(productionFunction).join('\n'),context);
  return {context,state,menu,saved,elements,writes,messages,errors,nativeCalls};
}
function nativeTitle(initial,onTick=()=>{}){
  const state={page:0,selection:0,ready:0,transitioning:1,keyPrepared:0,keyCommitted:[0,0,0,0,0],
    bgmVolume:100,seVolume:80,rapid:0,mainAction:0,...initial};
  return {state,module:{UTF8ToString:value=>value,_th128_title_menu_state:()=>JSON.stringify(state),
    _th128_title_main_input:()=>1,_th128_tick(){onTick(state);return 1;},_th128_error:()=>''}};
}
for(const route of ['normal-game return','replay return']){
  test(route+' does not overwrite saved settings during unprepared title frames',async()=>{
    const env=environment(['syncOriginalMainSettings']),native=nativeTitle();
    let prepared=0;
    const controller=new OriginalMainMenuController({module:native.module,prepare:async()=>{
      prepared++;Object.assign(native.state,{keyPrepared:1,keyCommitted:[0,1,3,2,4],
        bgmVolume:85,seVolume:70,extraDebugClearMask:63});
    },consumeSound:async()=>{},consumeMusic:async()=>{},syncText:async()=>{},
      onState:env.context.syncOriginalMainSettings});
    env.state.mainController=controller;
    await controller.step();
    assert.equal(env.writes.length,0,'fresh title must not persist zeroed native key mapping or default volumes');
    assert.equal(env.state.mainSavedSettings.debugClearMask,63);
    Object.assign(native.state,{ready:1,transitioning:0});
    await controller.step();
    assert.equal(prepared,1);
    assert.equal(env.writes.length,0,'restoration is not a user settings edit');
    native.state.bgmVolume=80;
    await controller.step();
    assert.equal(env.writes.length,1,'a real prepared-menu setting edit is still persisted');
    assert.equal(env.state.mainSavedSettings.bgm,80);
    assert.equal(env.state.mainSavedSettings.debugClearMask,63);
  });
}
test('first ready title frame still waits for native settings preparation',async()=>{
  const env=environment(['syncOriginalMainSettings']);
  const native=nativeTitle({},state=>{state.ready=1;state.transitioning=0;});
  const controller=new OriginalMainMenuController({module:native.module,prepare:async()=>{},
    consumeSound:async()=>{},consumeMusic:async()=>{},syncText:async()=>{},
    onState:env.context.syncOriginalMainSettings});
  env.state.mainController=controller;
  await controller.step();
  assert.equal(env.writes.length,0,'ready alone does not mean key configuration was restored');
});
for(const controller of [null,{disposed:true,prepared:true}]){
  test('failed stage load does not reuse a disposed original route owner '+String(controller===null?'missing':'disposed'),()=>{
    const env=environment(['restoreNativePageAfterLaunchFailure'],{
      nativeMenuSnapshot:()=>({available:true,page:2,ready:1,transitioning:0}),syncNativeMenuState(){}});
    env.state.mainController=controller;
    assert.equal(env.context.restoreNativePageAfterLaunchFailure('stage initialization failed'),false,
      'must boot a new title and restore debug EX flags instead of preparing from route page2');
  });
}
test('preload failure keeps a still prepared live route menu',()=>{
  const env=environment(['restoreNativePageAfterLaunchFailure'],{
    nativeMenuSnapshot:()=>({available:true,page:2,ready:1,transitioning:0}),syncNativeMenuState(){}});
  env.state.mainController={prepared:true,disposed:false};
  assert.equal(env.context.restoreNativePageAfterLaunchFailure('preload unavailable'),true);
});
test('debug unlock waits for a ready nontransitioning prepared root menu',async()=>{
  const env=environment(['runOwnedSaveDebug'],{
    nativeMenuSnapshot:()=>({available:true,page:0,ready:0,transitioning:1})});
  env.state.mainController={prepared:false};
  await env.context.runOwnedSaveDebug('unlock');
  assert.equal(env.writes.length,0,'the debug save must not become6/6 before native root can accept it');
  assert.equal(env.nativeCalls.length,0);
  assert.equal(env.errors.length,0,'an early click should report waiting without stopping the title');
});
test('debug unlock blocks menu navigation until durable write and native EX sync finish',async()=>{
  let resolveWrite;
  const pending=new Promise(resolve=>{resolveWrite=resolve;});
  const env=environment(['runOwnedSaveDebug','handleNativeTitleKey'],{
    updateOwnedMainSettings:async()=>pending,exportSelectedOwnedReplay:async()=>{}});
  const inputs=[];
  env.state.mainController={prepared:true,disposed:false,release(){},keyDown:code=>inputs.push(code)};
  const running=env.context.runOwnedSaveDebug('unlock');
  await Promise.resolve();
  await Promise.resolve();
  env.context.handleNativeTitleKey('KeyZ');
  assert.equal(inputs.length,0,'Z cannot leave the root while flags are still being saved');
  resolveWrite({...env.saved,revision:1});
  await running;
  assert.equal(env.nativeCalls[0],63,'the saved mask reaches the original EX menu');
  assert.equal(env.errors.length,0);
  env.context.handleNativeTitleKey('ArrowDown');
  assert.deepEqual(inputs,['ArrowDown'],'normal menu input resumes after synchronization');
});
test('confirmed debug save pauses title ticks including gamepad polling',async()=>{
  let steps=0,syncs=0;
  const env=environment(['advance'],{syncNativeMenuState(){syncs++;}});
  env.state.mainController={step:async()=>{steps++;}};
  env.state.mainSaveDebugBusy=true;
  await env.context.advance();
  assert.equal(steps,0);
  assert.equal(syncs,0);
  env.state.mainSaveDebugBusy=false;
  await env.context.advance();
  assert.equal(steps,1);
  assert.equal(syncs,1);
});
test('canceling either confirmed debug action leaves storage and native flags unchanged',async()=>{
  const env=environment(['runOwnedSaveDebug'],{window:{confirm:()=>false}});
  env.state.mainController={prepared:true,disposed:false,busy:false};
  await env.context.runOwnedSaveDebug('clear');
  await env.context.runOwnedSaveDebug('unlock');
  assert.equal(env.writes.length,0);
  assert.equal(env.nativeCalls.length,0);
  assert.equal(env.state.mainSaveDebugBusy,undefined);
});
