import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import {pathToFileURL} from 'node:url';import crypto from 'node:crypto';
import path from 'node:path';
const root=path.resolve(import.meta.dirname,'../..');
const c=JSON.parse(fs.readFileSync(process.env.TH128_FROZEN_BENCH_CONFIG??path.join(root,'local/frozen-display-benchmark.json'),'utf8'));
const {orderDrawLayers}=await import(pathToFileURL(root+'/site/runtime/draw.mjs'));
const current=await import(pathToFileURL(root+'/site/runtime/renderer-owned-ending-v2.mjs'));
const timing=await import(pathToFileURL(root+'/site/runtime/timing.mjs'));
let baselineSource=fs.readFileSync(c.baselineRenderer,'utf8');
for(const[p,file]of [["../resource-fetch.mjs","site/resource-fetch.mjs"],["./draw.mjs","site/runtime/draw.mjs"],["../inspector.mjs","site/inspector.mjs"]])baselineSource=baselineSource.replace("'"+p+"'","'"+pathToFileURL(root+'/'+file).href+"'");
const baseline=await import('data:text/javascript;base64,'+Buffer.from(baselineSource).toString('base64'));
const oldTiming=await import(pathToFileURL(c.baselineTiming));
const raw=JSON.parse(fs.readFileSync(c.drawInput,'utf8')),records=orderDrawLayers(raw,{world:true}),digest=b=>crypto.createHash('sha256').update(b).digest('hex');
function harness(Module,observe=false){
 const events=[],uploads=[],constants=new Map();let count=0;
 const gl=new Proxy({NO_ERROR:0,getError:()=>0,getParameter:()=>false,
 bufferData(_t,data){if(observe)uploads.push(digest(Buffer.from(data.buffer,data.byteOffset,data.byteLength)));count++;},
 drawArrays(...a){if(observe)events.push(['draw',...a]);count++;},
 bindTexture(...a){if(observe)events.push(['texture',...a]);},
 blendFunc(...a){if(observe)events.push(['blend',...a]);},blendEquation(...a){if(observe)events.push(['equation',...a]);}
 },{get(o,k){if(k in o)return o[k];if(String(k)===String(k).toUpperCase()){if(!constants.has(k))constants.set(k,constants.size+100);return constants.get(k);}return()=>{};}});
 const renderer=Object.create(Module.WebGlRenderer.prototype),textures=new Map();
 for(const row of records)if(row.bank!==undefined&&row.entry!==undefined)textures.set(row.bank+':'+row.entry,{texture:'bank'+row.bank+'entry'+row.entry,width:512,height:512,initialized:true});
 textures.set('6:2',{texture:'stage',width:1280,height:960,dynamic:true,initialized:true});
 Object.assign(renderer,{gl,canvas:{width:1280,height:960},textures,vertices:[],current:null,blend:-1,point:null,program:'sprite',array:'array',buffer:'buffer',textureBytes:0,solidTexture:'solid'});
 return {renderer,events,uploads,count:()=>count};
}
test('all 8236 real frozen draw records retain exact Float32 vertices texture/blend/draw ordering',()=>{
 assert.equal(records.length,8236);const a=harness(baseline,true),b=harness(current,true);
 a.renderer.render(records,{world:true,shake:[0,0]});b.renderer.render(records,{world:true,shake:[0,0]});
 assert.deepEqual(b.events,a.events);assert.deepEqual(b.uploads,a.uploads);assert.equal(b.count(),a.count());
});
test('interpolation output stays detached and equivalent on the real freeze record set',()=>{
 const first=records.map(r=>({...r,x:r.x-1})),before={generation:1,stage:1,frame:1,records:first,background:[]},after={generation:1,stage:1,frame:2,records,background:[]};
 const a=new oldTiming.WorldPresentationInterpolator(),b=new timing.WorldPresentationInterpolator();
 a.capture(before);a.capture(after);b.capture(before);b.capture(after);
 const x=a.sample(.5),y=b.sample(.5);assert.deepEqual(y,x);y.records[0].x=99999;assert.deepEqual(b.sample(.5),a.sample(.5));
});
function median(times){return times.slice().sort((a,b)=>a-b)[Math.floor(times.length/2)];}
function measure(fn){for(let n=0;n<4;n++)fn();const out=[];for(let n=0;n<9;n++){const t=performance.now();fn();out.push(performance.now()-t);}return {median:median(out),samples:out};}
test('render CPU for the same frozen records falls by at least 25 percent without dropping draws',()=>{
 const a=harness(baseline),b=harness(current);
 const old=measure(()=>a.renderer.render(records,{world:true,shake:[0,0]})),now=measure(()=>b.renderer.render(records,{world:true,shake:[0,0]}));
 console.log(JSON.stringify({label:'renderCPUwithGLSeam',records:records.length,old,now,ratio:now.median/old.median,deviceFPS:false}));
 assert(now.median<old.median*.75,'same-frame render CPU regression: missing target reduction');
});
test('capture CPU for the same frozen records falls by at least 25 percent while preserving deep snapshots',()=>{
 let frame=0;const a=new oldTiming.WorldPresentationInterpolator(),b=new timing.WorldPresentationInterpolator();
 const before={generation:1,stage:1,records,background:[]};
 const old=measure(()=>a.capture({...before,frame:++frame})),now=measure(()=>b.capture({...before,frame:++frame}));
 console.log(JSON.stringify({label:'captureCPU',records:records.length,old,now,ratio:now.median/old.median,deviceFPS:false}));
 assert(now.median<old.median*.75,'same-frame capture CPU regression: missing target reduction');
});

test('native mode0 half-pixel ties retain the original even rounding for all anchors',()=>{
 const list=[];for(const x of [-3.5,-2.5,-1.5,-.5,0,.5,1.5,2.5])for(const anchorX of [0,1,2])for(const anchorY of [0,1,2]){
  list.push({id:list.length,bank:0,entry:0,sprite:1,layer:0,mode:0,blend:0,flags:2,flags2:0,color:0xffffffff,secondary:0,
   width:5,height:7,x,y:x,sx:1,sy:1,anchorX,anchorY,rotation:0,rx:0,ry:0,u:0,v:0,uvScaleX:1,uvScaleY:1,u0:0,v0:0,u1:1,v1:1});
 }
 const a=harness(baseline,true),b=harness(current,true);
 a.renderer.render(list);b.renderer.render(list);assert.deepEqual(b.uploads,a.uploads);
});
