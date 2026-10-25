import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {PresentationCadence,PresentationLimiter,needsPresentationCapture} from '../src/presentation-cadence.mjs';
import {installManagedShell,runtimeOptions} from '../src/managed-shell.mjs';
import {ManagedInput} from '../src/managed-input.mjs';
import {TitleScenes} from '../src/title-scenes.mjs';

test('room preview animates for minutes without entering or loading a demo',()=>{
  const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
  const start=source.indexOf('function tick(){'),end=source.indexOf('\nfunction frame(now){',start);
  const model=new TitleScenes();let presentations=0;
  const context={roomPreview:true,model,presentation:{tick(){presentations++;}}};
  runInNewContext(source.slice(start,end)+';globalThis.roomTick=tick;',context);
  model.idleFrames=1799;
  for(let frame=0;frame<60*180;++frame){
    context.roomTick();assert.equal(model.state.scene,'title');
    assert(!model.actions.some(a=>a.type==='demo'),'waiting never schedules Replay loading');
  }
  assert.equal(presentations,60*180);assert.equal(model.state.age,60*180,'title animation clock keeps advancing');
  model.enter('demo',0,{readyAt:0});context.roomTick();
  assert.equal(model.state.scene,'title','an already queued demo returns to the room title');
});

test('managed protocol: authenticates navigation, serializes resources and configures before launch',async()=>{
  const messages=[],calls=[];let listener,finish;
  const environment={location:{href:'https://example.test/th12?runtimeEpoch=7',origin:'https://example.test'},
    parent:{postMessage:message=>messages.push(message)},addEventListener:(_,callback)=>{listener=callback;}};
  const api={running:()=>false,resources:async items=>{calls.push(['resources',items]);if(items[0]?.wait)await new Promise(resolve=>{finish=resolve;});},
    configure:async value=>calls.push(['configure',value]),launch:async()=>calls.push(['launch']),key:()=>calls.push(['key'])};
  const emit=installManagedShell(api,environment,{announceReady:false});
  const flush=async()=>{for(let i=0;i<30;i++)await Promise.resolve();};
  const send=(command,extra={},envelope={})=>listener({source:environment.parent,origin:environment.location.origin,
    data:{protocol:'eagler-touhou/1',game:'th12',epoch:7,request:command,command,...extra},...envelope});
  send('launch');await flush();assert.equal(messages.at(-1).ok,false);assert.deepEqual(calls,[]);
  emit.ready();assert.equal(messages.at(-1).event,'ready');
  assert.equal(messages.at(-1).roomPreview,true);assert.equal(messages.at(-1).roomReturn,true);
  send('launch');await flush();assert.equal(messages.at(-1).ok,false);
  send('keyboard',{epoch:6});send('keyboard',{}, {origin:'https://other.test'});send('keyboard',{}, {source:{}});
  await flush();assert.deepEqual(calls,[]);
  send('resources',{resources:[{wait:true}]});send('configure',{music:'none',language:'ja',options:{}});send('launch');
  await flush();assert.deepEqual(calls.map(call=>call[0]),['resources']);
  finish();await flush();assert.deepEqual(calls.map(call=>call[0]),['resources','resources','configure','launch']);
  assert.equal(messages.at(-1).ok,true);assert.equal(messages.at(-1).epoch,7);
});

test('managed touch: accepts shared sensitivity bounds and rejects invalid values',()=>{
  for(const touchSensitivity of [100,180,300])assert.equal(runtimeOptions({touchSensitivity}).touchSensitivity,touchSensitivity);
  for(const touchSensitivity of [99,301,NaN,Infinity,'100'])assert.throws(()=>runtimeOptions({touchSensitivity}));
});

test('room Start cannot announce gameplay while its native launch is still pending',async()=>{
  const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
  const start=source.indexOf('async function launchRuntime('),end=source.indexOf('\nasync function bootstrap()',start);
  let finish;const pending=new Promise(resolve=>{finish=resolve;}),scenes=[];
  const context={terminated:false,multiplayer:{active:false,launch:()=>pending},initializeMultiplayer:async()=>{},
    runtimeOptions:{},multiplayerResult:false,roomPreview:true,launched:false,firstFrame:true,launchSerial:0,clearKeys(){},
    model:{enter:scene=>scenes.push(scene),actions:[]},audio:{},core:{_th12_music_serial:()=>1},stageMusic:{1:'stage1'},session:{stage:1},
    playMusic:async()=>{},captureGamePresentation(){},emit(){},localization:{language:'ja'},renderer:{spellWallClock:{reset(){}}},console};
  runInNewContext(source.slice(start,end)+';globalThis.launchRuntime=launchRuntime;',context);
  const launch=context.launchRuntime({launchSerial:4});await Promise.resolve();await Promise.resolve();
  assert.equal(context.launched,false,'a loading/title frame cannot acknowledge the new launch');assert.equal(context.launchSerial,4);
  finish();await launch;assert.equal(context.launched,true);assert.deepEqual(scenes,['playing']);
});

test('managed gamepad: maps every semantic action and rejects retired owners',()=>{
  let actions=0,samples=0;const environment={location:{href:'https://example.test/th12?runtimeEpoch=7',origin:'https://example.test'}};
  const owner=environment.__eaglerGamepadMapping={schema:'eagler-touhou/gamepad-input/1',protocol:'eagler-touhou/1',
    game:'th12',epoch:7,origin:environment.location.origin,enabled:()=>true,sample:()=>{samples++;return actions;}};
  const input=new ManagedInput(environment),bits=[1,2,8,256,16,32,64,128,512,0x80000,4,0x200000];
  for(let index=0;index<bits.length;index++){actions=1<<index;assert.equal(input.pad(),bits[index]);}
  assert.equal(samples,12);actions=4096;assert.equal(input.pad(),0);
  owner.epoch=6;assert.equal(input.pad(),null);assert.equal(samples,13);
});

test('managed presentation: actual browser loop keeps 60 Hz logic at 60, 120 and 144 Hz',()=>{
  const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
  const start=source.indexOf('function frame(now){'),end=source.indexOf('\nfunction eventKey',start);
  assert.ok(start>=0&&end>start);
  for(const hz of [60,120,144]){
    let ticks=0,draws=0;
    const context={terminated:false,backgrounded:false,fatalError:false,last:0,lastPresentation:0,accumulator:0,presentationInterval:1000/60,presentationCadence:new PresentationCadence(),
      runtimeOptions:{limitPresentationTo60:false},presentationLimiter:new PresentationLimiter(),TICK:1000/60,meter:{frame(){}},loadingScreen:{advance(){},renderOverlay(){}},
      ready:true,busy:false,session:null,model:{actions:[],state:{scene:'playing'}},profile:{settings:{keyMap:{}}},
      keyDown:()=>false,previousPadUI:0,logicalStep:1000/60,captureThisTick:false,launched:false,multiplayer:null,
      needsPresentationCapture,interpolateGamePresentation:()=>false,
      tick:()=>{ticks++;},present:()=>{draws++;},requestAnimationFrame(){},fail:error=>{throw error;}};
    runInNewContext(source.slice(start,end)+';globalThis.frame=frame;',context);
    for(let index=1;index<=hz*10;index++)context.frame(index*1000/hz);
    assert.ok(Math.abs(ticks-600)<=1,`${hz} Hz: ${ticks} logic steps`);assert.equal(draws,hz*10);
    assert.equal(context.presentationCadence.fastFrames,hz===60?0:4);
    context.backgrounded=true;context.frame(20_000);assert.equal(context.accumulator,0);
    context.backgrounded=false;context.frame(20_001);assert.ok(Math.abs(ticks-600)<=1);
  }
});

test('a saturated catch-up budget still publishes a fresh game picture on every browser frame',()=>{
  const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
  const start=source.indexOf('function frame(now){'),end=source.indexOf('\nfunction eventKey',start);
  for(const interval of [150,200,250]){
    let ticks=0,captured=0;const pictures=[];
    const context={terminated:false,backgrounded:false,fatalError:false,last:0,accumulator:0,presentationInterval:0,presentationCadence:new PresentationCadence(),
      runtimeOptions:{limitPresentationTo60:false},presentationLimiter:new PresentationLimiter(),TICK:1000/60,meter:{frame(){}},loadingScreen:{advance(){},renderOverlay(){}},
      ready:true,busy:false,session:null,model:{actions:[],state:{scene:'playing'}},profile:{settings:{keyMap:{}}},
      keyDown:()=>false,previousPadUI:0,logicalStep:1000/60,captureThisTick:false,launched:false,multiplayer:null,
      needsPresentationCapture,interpolateGamePresentation:()=>false,
      tick(){ticks++;if(context.captureThisTick)captured=ticks;},present(){pictures.push(captured);},
      requestAnimationFrame(){},fail(error){throw error;}};
    runInNewContext(source.slice(start,end)+';globalThis.frame=frame;',context);
    for(let n=1;n<=60;n++)context.frame(n*interval);
    assert.equal(ticks,60*8,'catch-up work remains bounded without skipping logical frames');
    for(let n=0;n<pictures.length;n++)assert.equal(pictures[n],(n+1)*8,`${interval} ms frame ${n} must render the latest completed tick`);
  }
});
