import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';

function boundary(){
  const calls={paths:[],fetches:[],writes:[],refreshes:0,samples:[]},files=new Set(),downloads=[];
  let snapshot={scene:'playing',kind:'game',instance:'1:1',track:'th12_02'},pad=0;
  const canvas={addEventListener(){},focus(){}},notice={textContent:''};
  const core={FS:{analyzePath:path=>{calls.paths.push(path);return {exists:files.has(path)};},writeFile(path,bytes){files.add(path);calls.writes.push([path,Array.from(bytes)]);}},
    UTF8ToString:()=>JSON.stringify(snapshot),_th12_host_state:()=>1,_th12_host_refresh_music(){calls.refreshes++;},
    _th12_host_key(){},_th12_host_controls(){},_th12_host_pressed(){}};
  const context={URL,TextEncoder,Uint8Array,console,performance:{now:()=>0},
    document:{querySelector:id=>id==='#notice'?notice:canvas,addEventListener(){}},addEventListener(){},
    RuntimeAssets:class{fetch(path){calls.fetches.push(path);return new Promise(yes=>downloads.push(yes));}},
    ManagedInput:class{pad(){return pad;}},TouchInput:class{sample(_state,hints){calls.samples.push({...hints});return {held:0,pressed:0};}}};
  context.window=context;
  const source=readFileSync(new URL('../site/game-sdl.mjs',import.meta.url),'utf8')
    .replace(/^import .*;\r?$/gm,'').replaceAll('import.meta.url',JSON.stringify('https://runtime.invalid/game.mjs')).replace('bootstrap().catch(fail);','');
  const entry=runInNewContext(source+'\n({key,beforeFrame,frame,attach(value){core=value;launched=true;},musicMode(value){music=value;}})',context);
  entry.attach(core);
  return {entry,calls,files,setTrack(value){snapshot={...snapshot,track:value};},setPad(value){pad=value;},
    async complete(){downloads.shift()({ok:true,arrayBuffer:async()=>Uint8Array.of(1,2,3).buffer});for(let n=0;n<8;n++)await Promise.resolve();}};
}

test('SDL frames inspect each existing music file once across frame and track changes',()=>{
  const f=boundary();f.files.add('/game/music/th12_02.ogg');f.files.add('/game/music/th12_03.ogg');
  for(let n=0;n<600;n++)f.entry.frame();
  assert.equal(f.calls.paths.length,1);assert.deepEqual(f.calls.fetches,[]);
  f.setTrack('th12_03');f.entry.frame();f.setTrack('th12_02');f.entry.frame();
  assert.equal(f.calls.paths.length,2);assert.deepEqual(f.calls.fetches,[]);
});

test('SDL pending music is requested once, installed and refreshed without per-frame filesystem walks',async()=>{
  const f=boundary();f.entry.musicMode('none');for(let n=0;n<10;n++)f.entry.frame();assert.equal(f.calls.paths.length,0);
  f.entry.musicMode('ogg');for(let n=0;n<600;n++)f.entry.frame();
  assert.equal(f.calls.paths.length,1);assert.deepEqual(f.calls.fetches,['/assets/music/th12_02.ogg']);
  await f.complete();for(let n=0;n<600;n++)f.entry.frame();
  assert.equal(f.calls.paths.length,1);assert.equal(f.calls.refreshes,1);
  assert.deepEqual(f.calls.writes,[['/game/music/th12_02.ogg',[1,2,3]]]);
});

test('SDL frame input retains independent Shift owners and keyboard/gamepad direction state',()=>{
  const f=boundary();f.entry.key('ShiftLeft',true,'physical');f.entry.key('ShiftRight',true,'host');
  f.entry.beforeFrame();f.entry.key('ShiftLeft',false,'physical');f.entry.beforeFrame();
  f.entry.key('ArrowUp',true,'host');f.entry.beforeFrame();f.entry.key('ShiftRight',false,'host');f.entry.beforeFrame();
  f.entry.key('ArrowUp',false,'host');f.setPad(32);f.entry.beforeFrame();f.setPad(0);f.entry.beforeFrame();
  assert.deepEqual(f.calls.samples,[{arrows:false,keyFocused:true},{arrows:false,keyFocused:true},
    {arrows:true,keyFocused:true},{arrows:true,keyFocused:false},{arrows:true,keyFocused:false},{arrows:false,keyFocused:false}]);
});
