import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createRequire} from 'node:module';
import {runInNewContext} from 'node:vm';

const root=resolve(import.meta.dirname,'../..');
const {build}=createRequire(new URL('../../launcher/package.json',import.meta.url))('esbuild');

// Execute the authored shell and input adapter. Only DATA/IDB, DOM and the
// heavyweight native engine are boundaries; protocol/lifecycle/input code is real.
async function compileShell(game) {
  const owner=resolve(root,'ports',game,'web');
  const built=await build({entryPoints:[resolve(owner,'managed-shell.mjs')],bundle:true,write:false,
    format:'esm',platform:'browser',logLevel:'silent',plugins:[{name:'pc98-test-boundaries',setup(ctx){
      ctx.onResolve({filter:/^\.\/(?:managed-data|managed-storage|managed-preview)\.mjs$/},args=>
        args.importer===resolve(owner,'managed-shell.mjs') ? {path:args.path,namespace:'boundary'} : undefined);
      ctx.onResolve({filter:/./},args=>{
        if(args.kind==='dynamic-import' && args.importer===resolve(owner,'managed-shell.mjs'))
          return {path:args.path==='./managed-room-layout.mjs'?'room-layout':'engine',namespace:'boundary'};
        if(['./directory-keyboard.mjs','./direct-input-bridge.mjs'].includes(args.path))
          return {path:resolve(root,'common/browser',args.path.slice(2))};
      });
      ctx.onLoad({filter:/./,namespace:'boundary'},args=>({contents:args.path==='room-layout'
        ? 'export function mountManagedRoom(){}'
        : args.path==='engine'
        ? 'export const managedAPI=globalThis.boundary.api;'
        : args.path.includes('managed-data')
          ? 'export const ManagedContent={open:async()=>({install(){}})};export function installManagedWorkers(){}'
          : args.path.includes('managed-storage')
            ? 'export const openUserFiles=async()=>globalThis.boundary.store;export const userPath=p=>p;'
            : 'export const titlePreview=()=>globalThis.boundary.preview;'}));
    }}]});
  return new TextDecoder().decode(built.outputFiles[0].contents);
}

async function fixture(source,game,multiplayer) {
  const messages=[],calls=[],listeners=new Map(),timers=[];
  let clock=0;
  const boundary={
    store:{memory:new Map(),close(){calls.push('close');}},
    preview:{stop(){calls.push('preview-stop');},async prepare(){calls.push('prepare');return {}; }},
    api:{async start(){calls.push('start');},stop(){calls.push('stop');},
      async files(message){calls.push(message.command);return {};},async playback(){throw Error('unsupported playback');},
      health:()=>({frames:12,audio:{}})},
  };
  const parent={postMessage:message=>messages.push(message),__eaglerPrepareManagedRuntimeDataV1:async()=>({buffer:new ArrayBuffer(0)})};
  const context={boundary,parent,URL,Uint8Array,ArrayBuffer,TextEncoder,TextDecoder,console,
    location:{href:`https://test.invalid/${game}?runtimeEpoch=7&runtimeVariant=${multiplayer?'multiplayer':'normal'}`,origin:'https://test.invalid'},
    addEventListener(type,fn){const rows=listeners.get(type)??[];rows.push(fn);listeners.set(type,rows);},
    setInterval(fn){timers.push(fn);},performance:{now:()=>clock},
    fetch:async()=>({text:async()=>''}),DOMParser:class {parseFromString(){return {querySelectorAll:()=>[],body:{childNodes:[]}};}},
    document:{head:{append(){}},body:{replaceChildren(){}},querySelector:()=>({}),
      createElement:()=>({}),getElementById:()=>({})},
  };
  context.window=context;
  await runInNewContext(`(async()=>{${source}\n})()`,context);
  const flush=async()=>{for(let i=0;i<40;i++)await Promise.resolve();};
  function send(command,extra={},edge={}) {
    const message={protocol:'eagler-touhou/1',game,epoch:7,request:String(messages.length),command,...extra};
    for(const callback of listeners.get('message')??[])callback({source:parent,origin:context.location.origin,data:message,...edge});
  }
  async function rpc(command,extra={}) {send(command,extra);await flush();return messages.at(-1);}
  const configure=options=>rpc('configure',{music:'none',language:'ja',options});
  return {context,messages,calls,send,rpc,flush,configure,timers,advance(value){clock=value;}};
}

export function registerPC98AdapterTests(game) {
  const compiled=compileShell(game);
  for(const multiplayer of [false,true]) {
    const variant=multiplayer?'multiplayer':'normal';
    const setup=async()=>fixture(await compiled,game,multiplayer);
    test(`pc98 protocol: ${variant} authenticates, configures, reports frames and retires input`,async()=>{
      const f=await setup();
      assert.equal(f.messages.at(-1).event,'ready');assert.equal(f.messages.at(-1).roomPreview,multiplayer);
      assert.equal((await f.rpc('launch')).ok,false);
      const count=f.messages.length;
      f.send('configure',{epoch:6});f.send('configure',{game:'th99'});f.send('configure',{protocol:'other'});
      f.send('configure',{}, {source:{}});f.send('configure',{}, {origin:'https://other.invalid'});
      await f.flush();assert.equal(f.messages.length,count);
      assert.equal((await f.configure({touchEnabled:true})).ok,true);
      if(multiplayer)assert.equal((await f.rpc('prepare-room')).ok,true);
      assert.equal((await f.rpc('launch',{launchSerial:42})).ok,true);
      assert.ok(f.calls.includes('start'));
      f.advance(1100);f.timers[0]();
      assert.equal(f.messages.find(m=>m.event==='first-frame').launchSerial,42);
      assert.ok(f.messages.some(m=>m.event==='frame-health'));
      assert.ok(f.messages.every(m=>m.epoch===7&&m.game===game));
      assert.equal((await f.rpc('unsupported')).ok,false);
      assert.match(f.messages.at(-1).error,/运行命令/);
      assert.equal((await f.rpc('network-cancel')).ok,true);
      assert.equal(f.messages.findLast(m=>m.event==='exit').status,'success');
      assert.ok(f.calls.indexOf('sync')<f.calls.indexOf('stop'));
      assert.equal(f.context.__eaglerDirectInputBridge.submit({protocol:'eagler-touhou/1',game,epoch:7,command:'keyboard-clear'}),false);
    });
    test(`pc98 touch: ${variant} validates sensitivity and applies scaled motion`,async()=>{
      const f=await setup();
      for(const value of [99,301,NaN,Infinity,'150'])assert.equal((await f.configure({touchSensitivity:value})).ok,false);
      let previous=0;
      for(const value of [100,150,300]){
        assert.equal((await f.configure({touchSensitivity:value,touchEnabled:true})).ok,true);
        const input=f.context.__eaglerPC98Input;
        input.pointer({type:'down',id:1,x:.25,y:.25});input.pointer({type:'move',id:1,x:.26,y:.25});
        const motion=input.motion();assert.ok(motion.x>previous);previous=motion.x;
        assert.equal(input.motion().x,0);
        input.clear();assert.equal(input.motion().active,false);
      }
    });
    test(`pc98 gamepad: ${variant} maps all actions through the authenticated Launcher owner`,async()=>{
      const f=await setup();let actions=0,seat;
      const owner=f.context.__eaglerGamepadMapping={schema:'eagler-touhou/gamepad-input/1',protocol:'eagler-touhou/1',game,epoch:7,
        origin:f.context.location.origin,sample(value){seat=value;return actions;},enabled:()=>true,clear(){}};
      await f.configure({netplayPlayer:2});await f.rpc('launch');
      const input=f.context.__eaglerPC98Input;
      const expected=game==='th03'?[512,16,32,128,1,2,4,8,256]:[32,16,64,128,1,2,4,8,256];
      for(const [index,mask]of [1,2,4,8,16,32,64,128,512].entries()){
        actions=mask;assert.equal(input.sample(),expected[index],`semantic action ${mask}`);assert.equal(seat,2);
      }
      actions=32;owner.epoch=6;assert.equal(input.sample(),0);
      owner.epoch=7;owner.origin='https://other.invalid';assert.equal(input.sample(),0);
      owner.origin=f.context.location.origin;await f.rpc('network-cancel');assert.equal(input.sample(),0);
    });
  }
  test('pc98 presentation: exercises the actual frame gate without a display-rate simulation claim',async()=>{
    if(game==='th01'){
      const source=readFileSync(resolve(root,'ports/th01/web/app.js'),'utf8');
      const start=source.indexOf('function loop(now){'),end=source.indexOf('\nasync function startMusic',start);
      assert.ok(start>=0&&end>start);
      for(const hz of [60,120,144]){
        let ticks=0;
        const context={configuring:false,last:0,accumulator:0,paused:false,document:{hidden:false},tick:1000/60,
          buttons:()=>[],performance:{now:()=>0},core:{th_step(){ticks++;}},horizontalTouch:{consume(){}},pressed:new Set(),
          logicMs:0,steps:0,playEffects(){},lastDraw:0,present(){},musicWanted:false,statsAt:1e12,requestAnimationFrame(){}};
        runInNewContext(source.slice(start,end)+';globalThis.frame=loop;',context);
        for(let n=1;n<=hz*10;n++)context.frame(n*1000/hz);
        assert.ok(Math.abs(ticks-600)<=2,`${hz} Hz: ${ticks} logic steps`);
      }
    }else{
      const {createFrameGate}=await import(new URL(`../../ports/${game}/web/frame-limit.js`,import.meta.url));
      for(const hz of [60,120,144]){
        const due=createFrameGate();let frames=0;
        for(let n=0;n<hz*10;n++)if(due(n*1000/hz))frames++;
        assert.equal(frames,600);
        assert.equal(due(NaN),false);assert.equal(due(-1),true);
      }
    }
  });
}
