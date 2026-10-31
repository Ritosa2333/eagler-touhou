import test from 'node:test';
import assert from 'node:assert/strict';
import {existsSync,readFileSync,writeFileSync,mkdtempSync,rmSync} from 'node:fs';
import {resolve,join} from 'node:path';
import {tmpdir} from 'node:os';
import {execFileSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {targets,root} from './inputs.mjs';
const directories=[...new Set(targets().map(target=>target.directory))];
function native(directory, fixture, includes, sources=[], prepare=()=>{}) {
 const temp=mkdtempSync(join(tmpdir(),'eagler-held-touch-'));
 try{
  prepare(temp);
  const program=join(temp,process.platform==='win32'?'verify.exe':'verify');
  execFileSync(process.env.CXX||'g++',['-std=c++17','-O2','-UNDEBUG',...includes.flatMap(path=>['-I',path]),'-I',temp,
   '-I',resolve(root,'common/include'),resolve(root,'tools/testing',fixture),...sources,'-o',program],{windowsHide:true,timeout:60000});
  execFileSync(program,[],{windowsHide:true,timeout:10000});
 }finally{rmSync(temp,{recursive:true,force:true});}
}
// Compile the production delta-consumption functions independently of SDL and game assets.
function functionSource(source, signature) {
  const start=source.search(signature);assert(start>=0,String(signature));
  const open=source.indexOf('{',start);let depth=1,end=open+1;
  for(;depth&&end<source.length;end++){if(source[end]==='{')depth++;if(source[end]==='}')depth--;}
  assert.equal(depth,0,String(signature));return source.slice(start,end);
}
function deltaFunctions(source) {
 return ['GetPlayerDelta','SetPlayerDelta','TakePlayerDelta','ConsumePlayerDelta']
  .map(name=>functionSource(source,new RegExp('(?:bool|void) Touch::'+name+'\\('))).join('\n');
}
for(const directory of directories){
 const header=resolve(root,directory,'portable/input/TouchController.hpp');
 const module=resolve(root,directory,'src/touch-input.mjs');
 const legacy=resolve(root,directory,'src/Touch.cpp');
 const fighter=resolve(root,directory,'web/cpp/game/FighterControls.cpp');
 const families=[header,module,legacy,fighter].filter(existsSync);
 assert.equal(families.length,1,`${directory}: register exactly one production input architecture`);
 const variants=targets().filter(target=>target.directory===directory).map(target=>target.variant).join('|');
 if(existsSync(header))test(`${directory} (${variants}) held drag and joystick survive temporary movement blocks`,()=>{
  native(directory,'held-touch.cpp',[resolve(header,'..')]);
 });
 if(existsSync(module))test(directory+' same pointer resumes after native player identity changes',async()=>{
  const {TouchInput}=await import(pathToFileURL(module));
  assert(readFileSync(resolve(root,directory,'portable/sdl/GameHost.cpp'),'utf8').includes('gestureScope'),
   'SDL state producer must supply the stable gesture scope');
  const producer=functionSource(readFileSync(resolve(root,directory,'site/game.mjs'),'utf8'),/function touchState\(/);
  const state=(instance,ready=true)=>new Function('ready','busy','session','model','runtimeOptions','core','hud','readJSON','profile','keyDown',producer+';return touchState();')
   (true,false,{stage:1},{state:{scene:'playing'}},{},{_th12_player_instance:()=>instance,_th12_player_state:()=>ready?1:2,_th12_player_speed:()=>512},()=>[0,0,200],()=>null,{settings:{keyMap:{}}},()=>false);
  for(const mode of ['touch','touch-unlimited'])for(const moveWhileBlocked of [false,true]){
   const input=new TouchInput();input.configure({touchEnabled:true,touchMovementMode:mode});
   const live=state(1);
   input.pointer({id:7,type:'down',x:100,y:100},live,0);input.pointer({id:7,type:'move',x:120,y:100},live,1);
   const blocked=state(1,false);input.sample(blocked);
   if(moveWhileBlocked)input.pointer({id:7,type:'move',x:130,y:100},blocked,2);
   const respawn={...state(2),x:1000,y:30000};input.sample(respawn);
   input.pointer({id:7,type:'move',x:140,y:100},respawn,3);input.pointer({id:7,type:'move',x:150,y:100},respawn,4);
   assert(input.sample(respawn).motion?.x>respawn.x,'same held pointer must move the respawned player');
   input.cancel();input.pointer({id:7,type:'move',x:160,y:100},respawn,5);assert.equal(input.sample(respawn).motion,null);
  }
 });
 if(existsSync(legacy))test(`${directory} (${variants}) consuming blocked-frame deltas preserves the held contact`,()=>{
  const source=readFileSync(legacy,'utf8');
  native(directory,'legacy-held-touch.cpp',[],[],temp=>writeFileSync(join(temp,'touch-delta.inc'),deltaFunctions(source)));
 });
 if(existsSync(fighter))test(`${directory} (${variants}) held direction survives fighter action transitions`,()=>{
  native(directory,'fighter-held-touch.cpp',[resolve(fighter,'..')],[fighter]);
 });
}
