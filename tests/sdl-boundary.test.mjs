// Execute each production browser entry with IO doubles and real input owners.
import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {ManagedInput,keyboardCode} from '../src/managed-input.mjs';
import {TouchInput} from '../src/touch-input.mjs';

function fixture(){
  const handlers=new Map(),events=[],audio=[],keys=[],controls=[],files=new Map();let flushes=0,refreshes=0;
  const snapshot={kind:'game',scene:'playing',instance:1,ready:true,x:0,y:8192,track:'th12_01'};
  const canvas={addEventListener(){},getBoundingClientRect:()=>({left:0,top:0,width:640,height:480})},notice={textContent:''};
  const core={FS:{analyzePath:path=>({exists:files.has(path)}),unlink:path=>files.delete(path),writeFile:(path,bytes)=>files.set(path,bytes),readdir:()=>[]},
    _th12_host_key:(...args)=>keys.push(args),_th12_host_clear(){},_th12_host_controls:(...args)=>controls.push(args),_th12_host_pressed(){},
    _th12_host_audio:value=>audio.push(value),_th12_host_flush:()=>flushes++,
    _th12_host_state:()=>1,UTF8ToString:()=>JSON.stringify(snapshot),_th12_host_refresh_music:()=>refreshes++};
  const context={URL,TextEncoder,Uint8Array,console,performance:{now:()=>100},innerWidth:640,innerHeight:480,keyboardCode,
    document:{hidden:false,querySelector:id=>id==='#notice'?notice:canvas,addEventListener(){}},
    location:new URL('https://game.test/runtime/th12.html?runtimeEpoch=1'),
    addEventListener:(type,callback)=>handlers.set(type,callback),
    RuntimeAssets:class{music=new Map();async install(items){for(const {path} of items)if(path.startsWith('/music/'))this.music.set(path,{arrayBuffer:async()=>Uint8Array.of(7,8).buffer});}fetch=async()=>({ok:true,arrayBuffer:async()=>Uint8Array.of(7,8).buffer});},
    ManagedInput:class extends ManagedInput{constructor(){super(context);}},TouchInput,
    testCore:core,testEmit:event=>events.push(event),testStore:{sync:async()=>{}}};
  context.window=context;
  const source=readFileSync(new URL('../site/game-sdl.mjs',import.meta.url),'utf8').replace(/^import .*;\r?$/gm,'')
    .replaceAll('import.meta.url',JSON.stringify('https://game.test/runtime/game.mjs')).replace('bootstrap().catch(fail);','');
  const entry=runInNewContext(source+'\ncore=testCore;store=testStore;emit=testEmit;nativeStarted=true;launched=true;({frame,beforeFrame,key,touch,nativeError,installResources,failure:()=>lastFailure})',context);
  files.set('/game/music/th12_01.ogg',Uint8Array.of(1));entry.frame();
  return {entry,context,handlers,events,audio,keys,controls,files,notice,flushes:()=>flushes,refreshes:()=>refreshes,snapshot};
}

test('pagehide releases keyboard and held touch before saving; pageshow permits fresh input',async()=>{
  const f=fixture();f.entry.key('ArrowRight',true);
  f.entry.touch.configure({touchEnabled:true});
  f.entry.touch.pointer({type:'down',id:1,x:100,y:100},{...f.snapshot,key:'game:1'});
  assert.equal(f.entry.touch.points.size,1);
  f.handlers.get('pagehide')();f.entry.beforeFrame();
  assert.deepEqual(f.keys.at(-1),[39,0]);assert.equal(f.entry.touch.points.size,0);
  assert.deepEqual(f.controls.at(-1),[0,0,0,0]);assert.equal(f.audio.at(-1),1);
  await new Promise(resolve=>setImmediate(resolve));assert.equal(f.flushes(),1);
  f.handlers.get('pageshow')();assert.equal(f.audio.at(-1),0);
  f.entry.key('ArrowRight',true);assert.deepEqual(f.keys.at(-1),[39,1]);
  f.context.document.hidden=true;f.handlers.get('pageshow')();assert.equal(f.audio.length,2);
});

test('native music errors are recoverable, deduplicated, and retry only after that resource is replaced',async()=>{
  const f=fixture(),path='/game/music/th12_01.ogg';f.files.set(path,Uint8Array.of(1));
  f.entry.frame();
  for(let n=0;n<2;n++)f.entry.nativeError('cannot decode music resource','audio-resource','th12_01');
  assert.equal(f.events.filter(e=>e.event==='error').length,1);
  assert.equal(f.events.at(-1).code,'audio-resource');assert.equal(f.entry.failure(),'');
  assert.match(f.notice.textContent,/th12_01/);
  await f.entry.installResources([{path:'/unifont.otf'}]);assert.equal(f.files.has(path),true);
  await f.entry.installResources([{path:'/music/th12_01.ogg'}]);assert.deepEqual(Array.from(f.files.get(path)),[7,8]);
  f.entry.frame();await new Promise(resolve=>setImmediate(resolve));
  assert.deepEqual(Array.from(f.files.get(path)),[7,8]);assert.equal(f.refreshes(),1);
  assert.equal(f.notice.textContent,'');
  f.entry.nativeError('another failed replacement','audio-resource','th12_01');
  assert.equal(f.events.filter(e=>e.event==='error').length,2);
  f.entry.nativeError('terminal failure');assert.equal(f.entry.failure(),'terminal failure');
});
