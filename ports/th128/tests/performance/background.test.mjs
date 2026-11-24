import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'../..');
const {WorldPresentationInterpolator}=await import(pathToFileURL(root+'/site/runtime/timing.mjs'));
const {WebGlRenderer}=await import(pathToFileURL(root+'/site/runtime/renderer-owned-ending-v2.mjs'));
const fog=[1,2,3,255,100,1000];
function vertex(x,y,w){return[x,y,1830/1770*w-108000/1770,w,0,0,1,1,1,1,0.25];}
function quad(id,points,{entry=0,blend=0,layer=0}={}){
 const vertices=[0,1,2,0,2,3].flatMap(index=>vertex(...points[index]));
 return{presentationId:id,bank:8,entry,blend,layer,fog:fog.slice(),vertices};
}
function tiles(depthDelta=0){
 return[
 quad('near',[[-20,20,20+depthDelta],[20,20,20+depthDelta],[20,-20,100+depthDelta],[-20,-20,100+depthDelta]]),
 quad('far',[[-20,-20,100+depthDelta],[20,-20,100+depthDelta],[20,-60,180+depthDelta],[-20,-60,180+depthDelta]])
 ];
}
function history(before,after){
 const h=new WorldPresentationInterpolator();
 h.capture({generation:1,stage:1,frame:1,records:[],background:before});
 h.capture({generation:1,stage:1,frame:2,records:[],background:after});return h;
}
test('adjacent STD tiles retain the exact shared edge when one crosses the near plane',()=>{
 const before=tiles(5),current=tiles(),h=history(before,current),s=h.sample(.5);
 const a=s.background[0].vertices.slice(22,26),b=s.background[1].vertices.slice(11,15);
 assert.deepEqual(a,b,'one tile must not snap to now while its neighbour interpolates');
 assert.deepEqual([a[0],a[1],a[3]],[20,-20,102.5]);assert(Math.abs(a[2]-44.95762711864407)<1e-10);
 assert.deepEqual(current,tiles(),'interpolation must keep native geometry detached');
});
test('background interpolation retains UVs tint fog and all triangle ordering',()=>{
 const current=tiles(),h=history(tiles(5),current),s=h.sample(.25);
 for(let i=0;i<2;i++)for(let base=0;base<66;base+=11)assert.deepEqual(s.background[i].vertices.slice(base+4,base+11),current[i].vertices.slice(base+4,base+11));
 assert.deepEqual(s.background.map(m=>[m.presentationId,m.entry,m.layer,m.fog]),current.map(m=>[m.presentationId,m.entry,m.layer,m.fog]));
});
test('a newly visible tile falls back the complete background to a single camera phase',()=>{
 const current=tiles(),s=history([tiles(5)[1]],current).sample(.5);
 assert.deepEqual(s.background,current,'partial tile history cannot tear a new shared edge');
});
test('ambiguous duplicate tile identities cannot interpolate only part of a background',()=>{
 const before=tiles(5),current=tiles();before.push(structuredClone(before[0]));
 assert.deepEqual(history(before,current).sample(.5).background,current);
});
test('camera fog changes fall back coherently and frame gaps never interpolate',()=>{
 const before=tiles(5),current=tiles();before[0].fog[0]=9;
 assert.deepEqual(history(before,current).sample(.5).background,current);
 const h=new WorldPresentationInterpolator();h.capture({generation:1,stage:1,frame:1,records:[],background:before});
 h.capture({generation:1,stage:1,frame:9,records:[],background:current});assert.deepEqual(h.sample(.5).background,current);
});
function glHarness(){
 const calls={uploads:[],draws:[],depth:[],fog:[],bindings:[]};let bound;
 const gl=new Proxy({
  bufferData(_target,values){calls.uploads.push(Array.from(values));},
  drawArrays(_mode,start,count){calls.draws.push({start,count,texture:bound});},
  bindTexture(_target,texture){bound=texture;calls.bindings.push(texture);},
  depthMask(value){calls.depth.push(value);},
  uniform3fv(_uniform,value){calls.fog.push(Array.from(value));}
 },{get(target,key){if(key in target)return target[key];if(String(key)===String(key).toUpperCase())return key;return()=>{};}});
 const renderer=Object.create(WebGlRenderer.prototype);
 Object.assign(renderer,{gl,canvas:{width:1280,height:960},textures:new Map([['8:0',{texture:'A'}],['8:1',{texture:'B'}]]),
 stageProgram:'stage',stageArray:'stageArray',stageBuffer:'stageBuffer',stageFog:'fog',program:'sprite',array:'array',buffer:'buffer',
 vertices:[],blend:-1,point:null,current:null});
 return {renderer,calls};
}
function drawRows(){
 const rows=tiles();rows.push({...structuredClone(rows[0]),presentationId:'third',entry:1,blend:1});
 rows.push({...structuredClone(rows[1]),presentationId:'fourth'});return rows;
}
test('STD background uses one vertex upload per display',()=>{
 const {renderer,calls}=glHarness(),rows=drawRows();renderer.drawBackground({geometry:()=>rows});
 assert.equal(calls.uploads.length,1);
 assert.deepEqual(calls.uploads[0],Array.from(new Float32Array(rows.flatMap(m=>m.vertices))));
});
test('only adjacent equal texture blend and fog draws merge in stable triangle order',()=>{
 const {renderer,calls}=glHarness();renderer.drawBackground({geometry:drawRows});
 assert.deepEqual(calls.draws,[{start:0,count:12,texture:'A'},{start:12,count:6,texture:'B'},{start:18,count:6,texture:'A'}]);
 // The initial depth clear explicitly enables writes, before the first group.
 assert.deepEqual(calls.depth,[true,true,false,true,true]);
});
test('different fog colours are never merged into one draw',()=>{
 const {renderer,calls}=glHarness(),rows=tiles();rows[1].fog[0]=7;renderer.drawBackground({geometry:()=>rows});
 assert.deepEqual(calls.draws,[{start:0,count:6,texture:'A'},{start:6,count:6,texture:'A'}]);
 assert.deepEqual(calls.fog,[[3/255,2/255,1/255],[3/255,2/255,7/255]]);
});
test('growing and shrinking background submissions keep the exact active vertex extent',()=>{
 const {renderer,calls}=glHarness();renderer.drawBackground({geometry:drawRows});renderer.drawBackground({geometry:()=>[tiles()[0]]});
 assert.equal(calls.uploads.length,2);assert.equal(calls.uploads[0].length,264);assert.equal(calls.uploads[1].length,66);
 assert.deepEqual(calls.draws.at(-1),{start:0,count:6,texture:'A'});
});
test('invalid or missing background data keeps explicit failures',()=>{
 const {renderer}=glHarness(),bad=tiles();bad[0].vertices[3]=NaN;assert.throws(()=>renderer.drawBackground({geometry:()=>bad}),/geometry/);
 const missing=tiles();missing[0].entry=999;assert.throws(()=>renderer.drawBackground({geometry:()=>missing}),/texture unavailable/);
});
