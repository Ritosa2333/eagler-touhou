import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import {pathToFileURL,fileURLToPath} from 'node:url';
const R=fileURLToPath(new URL('../../',import.meta.url));
const target=R+'/site/runtime/world-presentation.mjs',product=fs.existsSync(target)?await import(pathToFileURL(target)):{};
const read=module=>{assert.equal(typeof product.readWorldDrawRecords,'function','typed World presentation reader missing');return product.readWorldDrawRecords(module);};
const fields=['id','parent','bank','sprite','entry','layer','mode','anchorX','anchorY','blend','flags','flags2','color','secondary','script','clock','visualTicks','width','height','x','y','z','rotation','sx','sy','u','v','uvScaleX','uvScaleY','u0','v0','u1','v1','rx','ry'];
function encoded(records,{mixed=false}={}){
 const heap=new Uint8Array(64*1024*1024),header=new Float64Array(heap.buffer,64,8),rows=new Float64Array(heap.buffer,128,records.length*39);
 const refs=records.map((record,index)=>{
  if(mixed&&record.geometry)return record;
  for(let k=0;k<fields.length;k++)rows[index*39+k]=record[fields[k]]??0;
  rows[index*39+35]=record.effectSystem==='unified-primary'?1:0;rows[index*39+36]=record.stageSurface==='main'?2:record.stageSurface==='stage'?1:0;
  const passes=[null,'hostile-bullets','hostile-lasers','player','items','frozen-bullets','original-percent-hud','original-freeze-reward-panel','original-fps','original-menu-font0','original-pause-shade','original-pause-backdrop','original-pause-front','playerShotGroup','original-spell-bonus'];
  rows[index*39+37]=Math.max(0,passes.indexOf(record.drawPass??null));rows[index*39+38]=Object.hasOwn(record,'effectSystem')?0:1;return index;
 });
 // Test packet includes slots for all records; use only non-geometry fixture in invariance case.
 const text=Buffer.from(JSON.stringify(refs)),side=128+records.length*39*8;heap.set(text,side);header.set([0x54483131,1,records.length,39,128,side,text.length,0]);
 return{heap,header,rows,module:{HEAPU8:heap,_th128_world_draw_packet:()=>64,_th128_world_bullet_draw_error:()=>'',UTF8ToString:p=>typeof p==='string'?p:Buffer.from(heap.subarray(p,heap.indexOf(0,p))).toString()}};
}
const row={id:8,parent:0,bank:0,sprite:33,effectSystem:'unified-primary',entry:0,layer:13,mode:0,anchorX:0,anchorY:0,blend:1,flags:6,flags2:0,color:4294967295,secondary:0,script:3,clock:2,visualTicks:2,width:16,height:16,x:120.12345886230469,y:200,z:0,rotation:0,sx:1,sy:1,u:0,v:0,uvScaleX:1,uvScaleY:1,u0:.25,v0:.25,u1:.5,v1:.5,rx:0,ry:0};
function json9(value){return Number.isInteger(value)?value:Number(value.toPrecision(9));}
const expected=Object.fromEntries(Object.entries(row).map(([k,v])=>[k,typeof v==='number'?json9(v):v]));
test('typed World record preserves original general9 floats and every sprite field',()=>{assert.deepEqual(read(encoded([row]).module),[expected]);});
test('malformed header bounds version stride and nonfinite values are rejected',()=>{
 for(const [index,value]of [[0,0],[1,2],[2,65537],[3,8],[4,67108864],[5,-1],[6,67108864]]){
  const h=encoded([row]);h.header[index]=value;assert.throws(()=>read(h.module),/packet|bounds|schema|stride|budget/i);
 }
 const h=encoded([row]);h.rows[20]=NaN;assert.throws(()=>read(h.module),/finite|packet/i);
});
test('invalid sidecar references and enum values stop presentation',()=>{
 const h=encoded([row]);h.rows[37]=99;assert.throws(()=>read(h.module),/enum|packet/i);
 const q=encoded([row]);q.heap[q.header[5]+1]=57;assert.throws(()=>read(q.module),/reference|packet/i);
});
test('legacy runtime uses unchanged JSON path and propagates its native draw error',()=>{
 const m={_th128_world_bullet_draws:()=>JSON.stringify([expected]),UTF8ToString:x=>x,_th128_world_bullet_draw_error:()=>''};assert.deepEqual(read(m),[expected]);
 m._th128_world_bullet_draw_error=()=>'native error';assert.throws(()=>read(m),/native error/);
});
test('returned records detach from mutable native memory and preserve unsigned identity',()=>{
 const h=encoded([{...row,id:0x80000007}]),records=read(h.module);h.rows.fill(0);assert.equal(records[0].id,0x80000007);assert.equal(records[0].x,expected.x);
});

test('native general9 decimal half-way values round to even instead of Javascript precision ties',()=>{
 const h=encoded([{...row,x:345.5078125,rotation:.4814453125}]),r=read(h.module)[0];assert.equal(r.x,345.507812);assert.equal(r.rotation,.481445312);
});

test('trusted draw mode keeps exact native floats without per-scalar decimal formatting',()=>{
 const h=encoded([row]);const output=product.readWorldDrawRecords(h.module,{rawNumbers:true});
 assert.equal(output[0].x,row.x);assert.notEqual(output[0].x,expected.x);
 assert.equal(Math.fround(output[0].x),Math.fround(expected.x));
});

test('cached sprite mode preserves canonical general9 tie-even and unsigned fields',()=>{
 const h=encoded([{...row,x:345.5078125,rotation:.4814453125,id:0x80000007}]);const fast=product.readWorldDrawRecords(h.module,{fastNumbers:true}),legacy=read(h.module);
 assert.deepEqual(fast,legacy);assert.equal(fast[0].x,345.507812);assert.equal(fast[0].id,0x80000007);
});

const source=fs.readFileSync(R+'/site/play.mjs','utf8'),lo=source.indexOf('function maybeAutoHandoff(){'),hi=source.indexOf('\nfunction enterTitleMenu',lo);assert(lo>=0&&hi>lo);
test('natural stage handoff never displays capture-token debugging UI',async()=>{
 const elements=new Map(),find=s=>{if(!elements.has(s))elements.set(s,{hidden:true,textContent:''});return elements.get(s);};let loads=0;
 const state={autoFlow:true,worldMode:true,handoff:false,handoffFailed:false,pendingCarryToken:0,worldSnapshot:{stageTransition:{requested:true}},module:{_th128_world_capture_stage_carry:()=>7,UTF8ToString:x=>x},handoffGeneration:0,pressed:new Set(),shotMode:true};
 const selection={stage:{id:2}};const run=new Function('state','flowCapabilitiesReady','transitionSelection','handoffFailure','clock','resetHighFrameratePresentation','worldMovementInput','find','status','checkWorld','lastCheckWorldError',source.slice(lo,hi)+';return maybeAutoHandoff;')(state,()=>true,()=>selection,e=>{throw Error(e);},{reset(){}},()=>{},()=>{},find,()=>{},async()=>{loads++;state.stageSelection=selection;},'');
 run();await new Promise(resolve=>setImmediate(resolve));assert.equal(loads,1);assert.equal(find('#runtime-overlay').hidden,true,'no debug overlay during a natural carry');assert.equal(state.pendingCarryToken,0);assert.equal(state.handoff,false);
});
