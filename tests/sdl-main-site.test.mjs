// Run the real SDL browser entry against native/IO doubles, then read its
// window through the production Launcher's gameplay-path model. No game runs.
import test,{before} from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createRequire} from 'node:module';
import {resolve} from 'node:path';
import {runInNewContext} from 'node:vm';
import {authoredSourcesPlugin} from '../../../launcher/lib/authored-source-resolution.mjs';
import {sdlStopError} from '../src/sdl-lifecycle.mjs';
import {multiplayerHeldInput} from '../src/multiplayer.mjs';

const launcher=resolve(import.meta.dirname,'../../../launcher');
const {build}=createRequire(resolve(launcher,'package.json'))('esbuild');
let createGameplayNetwork;
before(async()=>{
  const result=await build({absWorkingDir:launcher,entryPoints:['app/models/gameplay-network.ts'],
    bundle:true,write:false,platform:'node',format:'esm',logLevel:'silent',plugins:[authoredSourcesPlugin(launcher)]});
  ({createGameplayNetwork}=await import('data:text/javascript;base64,'+Buffer.from(result.outputFiles[0].contents).toString('base64')));
});

async function fixture(){
  const events=[],fatals=[],audio=[],calls={ticks:0,saves:0,stops:0};let native,shouldFail=false;
  const options={netplayMode:'lan',netplayPlayer:0,netplayPlayerCount:2,netplayDifficulty:1,netplayLoadouts:[{character:0,shot:0},{character:1,shot:0}]};
  const canvas={focus(){},addEventListener(){},requestFullscreen:async()=>{}};
  const notice={textContent:''},localization={language:'ja',activate(){return this;}};
  const store={list:async()=>[],sync:async()=>{}};
  const multiplayer={active:false,launch:async()=>{multiplayer.active=true;},
    save:async()=>{calls.saves++;},stop(){calls.stops++;multiplayer.active=false;},state:()=>({stage:1}),
    tick(){calls.ticks++;if(shouldFail)throw Error('RTC peer P2 control channel error');return [];}};
  const core={FS:{readdir:()=>['.','..']},UTF8ToString:()=>'{"scene":"playing","kind":"game","stage":1}',
    _th12_host_options(){},_th12_host_music_mode(){},_th12_host_mp_ready:()=>1,
    _th12_host_key(){},_th12_host_clear(){},_th12_host_controls(){},_th12_host_pressed(){},
    _th12_host_audio:paused=>audio.push(paused),_th12_host_mp_sound(){},_th12_host_mp_start(){},
    _th12_host_mp_room(){},_th12_host_flush(){},_th12_host_state:()=>1,
    callMain(){native.th12Started();return 0;}};
  const context={URL,Map,Set,Uint8Array,Number,ArrayBuffer,console,performance:{now:()=>0},innerWidth:640,innerHeight:480,
    document:{hidden:false,querySelector:id=>id==='#notice'?notice:canvas,addEventListener(){}},addEventListener(){},
    RuntimeAssets:class{managed=true;fonts=new Map();},ManagedInput:class{clear(){}pad(){return 0;}},
    TouchInput:class{configure(){}cancel(){}sample(){return {held:0,pressed:0};}},
    createTH12SDL:async callbacks=>{native=callbacks;return core;},mountSdlContent:async()=>{},
    prepareLocalization:async()=>localization,fetch:async()=>({ok:true,json:async()=>({})}),
    createMultiplayerRuntime:()=>multiplayer,multiplayerHeldInput,sdlStopError,testEmit:e=>events.push(e),
    __eaglerCrashReporterV1:{fatal:e=>fatals.push(e),bindCore(){}},
    requestAnimationFrame:callback=>callback()};
  context.window=context;
  const source=readFileSync(new URL('../site/game-sdl.mjs',import.meta.url),'utf8')
    .replace(/^import .*;\r?$/gm,'').replaceAll('import.meta.url',JSON.stringify('https://game.test/runtime/game.mjs'))
    .replace('bootstrap().catch(fail);','');
  const entry=runInNewContext(source+'\nemit=testEmit;({configure,bootNative,launch,returnRoom,fail,frame,setStore(value){store=value;}})',context);
  entry.setStore(store);
  await entry.configure({options,language:'ja',music:'none'});await entry.bootNative();
  const peers=new Map([[1,{inputOpen:false,controlOpen:false,pc:{connectionState:'connected'}}]]);
  context.__th12PeerTransport=context.__eaglerPeerTransport={peers,failed:false,disconnected:false};
  context.__eaglerNetplayTransport='rtc';context.__eaglerNetplayPath='direct';
  const timers=new Map();let id=0;
  const model=createGameplayNetwork({runtime:{getSnapshot:()=>({epoch:1,game:'th12',launched:true,runtimeInfo:{}}),
    getMidiEventContext:()=>({epoch:1,target:context,document:notice}),subscribe:()=>()=>{}},
    context:()=>({product:'th12mp',runtimeVariant:'multiplayer',spectator:false,replayViewer:false,player:0,playerCount:2}),
    english:()=>true,translate:key=>key,now:()=>0,
    timers:{setTimeout(fn){const key=++id;timers.set(key,fn);return key;},clearTimeout:key=>timers.delete(key),setInterval:()=>++id,clearInterval(){}}});
  return {entry,model,context,core,options,peers,events,fatals,audio,calls,native:()=>native,
    breakChannel(){shouldFail=true;},restoreChannel(){shouldFail=false;},
    poll(){for(const [key,fn]of [...timers]){timers.delete(key);fn();}}};
}

test('SDL MP publishes the native Module consumed by the main-site readiness gate',async()=>{
  const f=await fixture();try{
    assert.equal(f.context.Module,f.core);assert.equal(f.core.eaglerOptions,f.options);
    assert.equal(f.model.readNativeSnapshot()?.mode,'lan');
    let ready=false;const pending=f.model.waitForGameplayPath({isCurrent:()=>true}).then(()=>{ready=true;});
    await Promise.resolve();assert.equal(ready,false,'an initialized core is not proof of open gameplay channels');
    f.peers.get(1).inputOpen=true;f.poll();await Promise.resolve();assert.equal(ready,false);
    f.peers.get(1).controlOpen=true;f.poll();await pending;assert.equal(ready,true);
  }finally{f.model.dispose();}
});

test('a failed SDL MP session stops network ticks and reports its first error once',async()=>{
  const f=await fixture();try{
    await f.entry.launch();f.breakChannel();
    for(let n=0;n<8;n++)f.core.th12MultiplayerTick(1,0,0,0,n*16);
    f.entry.frame();
    assert.equal(f.calls.ticks,1);assert.equal(f.fatals.length,1);
    assert.equal(f.events.filter(e=>e.event==='error').length,1);
    assert.equal(f.context.__eaglerNetplayFailed,true);
    assert.equal(f.context.__eaglerNetplayError,'RTC peer P2 control channel error');
    assert.deepEqual(f.audio,[1]);
    await assert.rejects(f.model.waitForGameplayPath({isCurrent:()=>true}),/RTC peer P2 control channel error/);
  }finally{f.model.dispose();}
});

test('returning from a failed SDL run preserves saving and permits a fresh launch',async()=>{
  const f=await fixture();try{
    await f.entry.launch();f.breakChannel();f.core.th12MultiplayerTick(0,0,0,0,0);
    await f.entry.returnRoom();
    assert.equal(f.calls.saves,1);assert.equal(f.calls.stops,1);
    assert.equal(f.context.__eaglerNetplayFailed,false);assert.equal(f.context.__eaglerNetplayError,'');
    assert.equal(f.audio.at(-1),0);
    f.restoreChannel();await f.entry.launch();f.core.th12MultiplayerTick(1,0,0,0,16);
    assert.equal(f.calls.ticks,2);
  }finally{f.model.dispose();}
});
