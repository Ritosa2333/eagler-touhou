import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {PresentationCadence,PresentationLimiter,needsPresentationInterpolation,needsPresentationCapture} from '../src/presentation-cadence.mjs';
import {installManagedShell,managedCommandHandler,runtimeOptions} from '../src/managed-shell.mjs';
import {ManagedInput} from '../src/managed-input.mjs';

test('reconfiguration preserves the local display limit when omitted',async()=>{
  const settings=[],handler=managedCommandHandler({running:()=>false,resources:async()=>{},configure:async value=>settings.push(value)});
  await handler({command:'configure',music:'none',language:'ja',options:{limitPresentationTo60:false}});
  await handler({command:'configure',music:'none',language:'ja',options:{}});
  assert.equal(settings.at(-1).options.limitPresentationTo60,false);
  await handler({command:'configure',music:'none',language:'ja',options:{limitPresentationTo60:true}});
  await handler({command:'configure',music:'none',language:'ja',options:{}});
  assert.equal(settings.at(-1).options.limitPresentationTo60,true);
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
  const interpolation=source.match(/^function interpolateGamePresentation\(\)\{.*\}$/m)?.[0];
  assert.ok(start>=0&&end>start);
  assert.ok(interpolation,'actual browser interpolation selector');
  for(const hz of [60,120,144])for(const limited of [false,true]){
    let ticks=0,draws=0;
    const context={terminated:false,backgrounded:false,fatalError:false,last:0,lastPresentation:0,accumulator:0,presentationInterval:1000/60,presentationCadence:new PresentationCadence(),
      presentationLimiter:new PresentationLimiter(),needsPresentationInterpolation,needsPresentationCapture,
      runtimeOptions:{limitPresentationTo60:limited},TICK:1000/60,meter:{frame(){}},loadingScreen:{advance(){},renderOverlay(){}},
      ready:true,busy:false,session:null,model:{actions:[],state:{scene:'playing'}},profile:{settings:{keyMap:{}}},
      keyDown:()=>false,previousPadUI:0,logicalStep:1000/60,captureThisTick:false,launched:false,
      tick:()=>{ticks++;},present:()=>{draws++;},requestAnimationFrame(){},fail:error=>{throw error;}};
    runInNewContext(interpolation+'\n'+source.slice(start,end)+';globalThis.frame=frame;',context);
    for(let index=1;index<=hz*10;index++)context.frame(index*1000/hz);
    assert.ok(Math.abs(ticks-600)<=1,`${hz} Hz, limit ${limited}: ${ticks} logic steps`);
    assert.ok(Math.abs(draws-(limited?600:hz*10))<=1,`${hz} Hz, limit ${limited}: ${draws} draws`);
    if(!limited)assert.equal(context.presentationCadence.fastFrames,hz===60?0:4);
    context.backgrounded=true;context.frame(20_000);assert.equal(context.accumulator,0);
    context.backgrounded=false;context.frame(20_001);assert.ok(Math.abs(ticks-600)<=1);
  }
});

test('a saturated catch-up budget still publishes a fresh game picture on every browser frame',()=>{
  const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
  const start=source.indexOf('function frame(now){'),end=source.indexOf('\nfunction eventKey',start);
  const interpolation=source.match(/^function interpolateGamePresentation\(\)\{.*\}$/m)?.[0];assert.ok(interpolation);
  for(const interval of [150,200,250]){
    let ticks=0,captured=0;const pictures=[];
    const context={terminated:false,backgrounded:false,fatalError:false,last:0,accumulator:0,presentationInterval:0,presentationCadence:new PresentationCadence(),
      runtimeOptions:{limitPresentationTo60:false},presentationLimiter:new PresentationLimiter(),TICK:1000/60,meter:{frame(){}},loadingScreen:{advance(){},renderOverlay(){}},
      ready:true,busy:false,session:null,model:{actions:[],state:{scene:'playing'}},profile:{settings:{keyMap:{}}},
      keyDown:()=>false,previousPadUI:0,logicalStep:1000/60,captureThisTick:false,launched:false,
      needsPresentationInterpolation,needsPresentationCapture,
      tick(){ticks++;if(context.captureThisTick)captured=ticks;},present(){pictures.push(captured);},
      requestAnimationFrame(){},fail(error){throw error;}};
    runInNewContext(interpolation+'\n'+source.slice(start,end)+';globalThis.frame=frame;',context);
    for(let n=1;n<=60;n++)context.frame(n*interval);
    assert.equal(ticks,60*8,'catch-up work remains bounded without skipping logical frames');
    for(let n=0;n<pictures.length;n++)assert.equal(pictures[n],(n+1)*8,`${interval} ms frame ${n} must render the latest completed tick`);
  }
});

test('managed input advertises existing mapping identities while keyboard delivery stays queued',()=>{
  const environment={location:{href:'https://example.test/th12?runtimeEpoch=7',origin:'https://example.test'}};
  const input=new ManagedInput(environment),keyboard=environment.__eaglerKeyboardInputBridge,pad=environment.__eaglerGamepadInputBridge;
  assert.equal(keyboard.schema,'eagler-touhou/keyboard-input/1');assert.equal(keyboard.epoch,7);
  assert.equal(keyboard.origin,environment.location.origin);assert.equal(keyboard.submit({command:'keyboard'}),false);
  assert.equal(Object.isFrozen(keyboard),true);assert.equal(pad.owned(),false);assert.equal(pad.sample(0),-1);
  environment.__eaglerGamepadMapping={schema:pad.schema,protocol:pad.protocol,game:pad.game,epoch:7,origin:pad.origin,enabled:()=>true,sample:()=>128};
  assert.equal(pad.owned(),true);assert.equal(pad.sample(0),128);assert.equal(input.pad(),128);
  environment.__eaglerGamepadMapping.epoch=8;assert.equal(pad.owned(),false);assert.equal(pad.sample(0),-1);
  const preview={location:{href:'https://example.test/preview',origin:'https://example.test'}};
  new ManagedInput(preview);assert.equal(preview.__eaglerKeyboardInputBridge,undefined);
});
