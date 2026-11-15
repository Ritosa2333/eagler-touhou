import assert from 'node:assert/strict';
import test from 'node:test';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {createRequire} from 'node:module';
const {parse}=createRequire(new URL('../../launcher/package.json',import.meta.url))('acorn');
for(const game of ['th15','th15-mp']){
 const source=readFileSync(new URL(`../../ports/${game}/web/sdl-runtime/shell.mjs`,import.meta.url),'utf8');
 const ast=parse(source,{ecmaVersion:'latest',sourceType:'module'});
 const declaration=name=>{const node=ast.body.find(n=>n.type==='FunctionDeclaration'&&n.id.name===name);assert.ok(node,name);return source.slice(node.start,node.end);};
 test(`${game}: desktop and touch use the same default resolution`,()=>{
  for(const coarse of [false,true]){
   const resolve=runInNewContext(declaration('renderScale')+';renderScale',{matchMedia:()=>({matches:coarse})});
   for(const value of [0,NaN,3,-1])assert.equal(resolve({},value),2);
   assert.equal(resolve({},1),1);assert.equal(resolve({},2),2);
   assert.equal(resolve({renderScale:1},2),1);assert.equal(resolve({renderScale:2},1),2);
   assert.equal(resolve({renderScale:0},0),2);
  }
  assert.equal(runInNewContext(declaration('renderScale')+';renderScale',{})({},0),2);
 });
 test(`${game}: native launch receives default and explicit resolutions`,async()=>{
  for(const mode of game==='th15'?['launch']:['launch','prepare-room'])for(const [options,query,expected] of [[{},'',2],[{},'?renderScale=1',1],[{renderScale:1},'?renderScale=2',1],[{renderScale:2},'?renderScale=1',2]]){
   const calls=[];const context={options,query:new URLSearchParams(query),parent:{location:{search:''}},URLSearchParams,
    game:'th15',protocol:'eagler-touhou/1',runtimeBuild:{multiplayer:game==='th15-mp'},launched:false,roomPreview:false,roomOwned:false,
    core:{_th15_render_scale:n=>calls.push(['scale',n]),_th15_prepare_loading:()=>1,_th15_initialize:()=>{calls.push(['initialize']);return 1;},
     _th15_room_preview:()=>1,_th15_loop_start:()=>{},_th15_loop_stop:()=>{}},
    fetch:async()=>({json:async()=>({})}),performance:{now:()=>0,mark(){}},requestAnimationFrame:cb=>cb(0),
    installStartupBranding:async()=>{},finishStartupAnimation:async()=>{},$:()=>({}),apply(){},emit(){},
    multiplayerBridge:{launch:async()=>true},spectatorPlayback:{start(){}},err:()=>''};
   const functions=['renderScale',...(game==='th15-mp'?['initializeTitle']:[]),'command'].map(declaration).join('\n');
   const command=runInNewContext(functions+';command',context);await command({command:mode});
   assert.deepEqual(calls,[['scale',expected],['initialize']],`${mode} configures the native renderer before initialization`);
   if(mode==='prepare-room'){await command({command:'launch'});assert.equal(calls.length,2,'room launch reuses the initialized renderer');}
  }
 });
}
