import assert from 'node:assert/strict';
import test from 'node:test';
import * as draws from '../site/runtime/draw.mjs';
import {WebGlRenderer} from '../site/runtime/renderer-owned-ending-v2.mjs';

function mesh(){
  return {mode:12,geometry:{kind:'triangle-strip',vertices:Array.from({length:34},(_,index)=>({
    x:index%2?200:100,y:16+Math.floor(index/2)*20,z:0,rhw:1,color:index%2?0xff123456:0x80402010,u:index%2?.8:.2,v:Math.floor(index/2)/16}))}};
}
function harness(width=1280,height=960){
  const events=[],uploads=[];
  let texture=null;
  const graphics={TEXTURE_2D:1,ARRAY_BUFFER:2,STREAM_DRAW:3,TRIANGLES:4,COLOR_BUFFER_BIT:5,SCISSOR_TEST:6,
    RGB:7,REPEAT:8,LINEAR:9,NEAREST:10,TEXTURE_WRAP_S:11,TEXTURE_WRAP_T:12,TEXTURE_MIN_FILTER:13,TEXTURE_MAG_FILTER:14,
    SRC_ALPHA:15,ONE_MINUS_SRC_ALPHA:16,ONE:17,ZERO:18,ONE_MINUS_SRC_COLOR:19,ONE_MINUS_DST_COLOR:20,DST_COLOR:21,NO_ERROR:0,
    createTexture:()=>({id:'surface'+events.length}),deleteTexture:value=>events.push(['delete',value?.id]),
    bindTexture:(_,value)=>{texture=value;},texParameteri:()=>{},bindVertexArray:()=>{},bindBuffer:()=>{},
    viewport:(...values)=>events.push(['viewport',...values]),clearColor:()=>{},clear:()=>events.push(['clear']),enable:()=>{},disable:()=>{},scissor:(...values)=>events.push(['scissor',...values]),
    FUNC_ADD:22,FUNC_REVERSE_SUBTRACT:23,blendEquation:value=>events.push(['equation',value]),
    blendFunc:()=>{},uniform2fv:()=>{},uniform4fv:()=>{},getError:()=>0,
    copyTexImage2D:(...values)=>events.push(['copy',texture?.id,...values]),
    copyTexSubImage2D:(...values)=>events.push(['copySub',texture?.id,...values]),
    bufferData:(_,values)=>{uploads.push([...values]);},drawArrays:(_,start,count)=>events.push(['draw',texture?.id,start,count]),
    texImage2D:(...values)=>events.push(['texImage',...values])};
  const renderer=Object.create(WebGlRenderer.prototype);
  Object.assign(renderer,{gl:graphics,canvas:{width,height},textures:new Map([['0:0',{texture:{id:'image'},width:2,height:2}]]),vertices:[],current:null,blend:-1,point:null,
    verticalClamp:[-1048576,1048576],spriteClamp:[-1048576,-1048576,1048576,1048576]});
  return {renderer,events,uploads};
}
function quad(layer){return {layer,bank:0,entry:0,color:0xffffffff,width:2,height:2,x:150,y:30,sx:1,sy:1,mode:0,
  anchorX:1,anchorY:1,blend:0,flags:2,u:0,v:0,uvScaleX:1,uvScaleY:1,u0:0,v0:0,u1:1,v1:1};}

test('typed526 strips retain every vertex color and flip only captured framebufferV',()=>{
  assert.equal(typeof draws.triangleStripVertices,'function','typed original526 triangle-strip geometry is missing');
  const record=mesh(),before=structuredClone(record);
  const output=draws.triangleStripVertices(record,{flipY:true});
  assert.equal(output.length,32*3*8);
  const vertex=index=>[record.geometry.vertices[index].x,record.geometry.vertices[index].y,
    record.geometry.vertices[index].u,1-record.geometry.vertices[index].v,...draws.argbColor(record.geometry.vertices[index].color)].map(Math.fround);
  assert.deepEqual([...output.slice(0,48)],[...vertex(0),...vertex(1),...vertex(2),...vertex(2),...vertex(1),...vertex(3)]);
  assert.deepEqual(record,before);
  for(const mutate of [value=>value.geometry.vertices.pop(),value=>value.geometry.vertices[0].x=NaN,
    value=>value.geometry.vertices[0].rhw=0,value=>value.geometry.vertices[0].z=1,value=>value.geometry.vertices[0].color=-1,
    value=>value.mode=0,value=>value.geometry.kind='unknown']){
    const invalid=structuredClone(record);mutate(invalid);assert.throws(()=>draws.triangleStripVertices(invalid));
  }
});

test('fixed1280x960 stage capture retains doubled pixels and rejects obsolete output sizes',()=>{
  const {renderer,events}=harness(1280,960);renderer.textures.set('6:2',{dynamic:true,texture:null,width:640,height:480,initialized:false});
  assert.doesNotThrow(()=>renderer.captureStage());
  assert.deepEqual(events.find(event=>event[0]==='copy').slice(7,9),[1280,960]);
  assert.ok(events.some(event=>event[0]==='scissor'&&event.slice(1).join(',')==='256,32,768,896'));
  assert.equal(renderer.textureBytes,1280*960*3);renderer.setResolution(1280,960);
  assert.equal(renderer.textures.get('6:2').initialized,true);
  assert.throws(()=>renderer.setResolution(640,480),/1280/);assert.throws(()=>renderer.setResolution(1000,700));
  assert.equal(renderer.textureBytes,1280*960*3);
  assert.deepEqual([renderer.canvas.width,renderer.canvas.height],[1280,960]);
});

test('actual text@R entries register as dynamic surfaces without fake PNG fetches',async()=>{
  const {renderer}=harness();
  await renderer.loadBank(6,{entries:[{index:0,name:'@',width:512,height:512,texture:null},
    {index:2,name:'@R',width:640,height:480,texture:null}]},'http://127.0.0.1/assets/');
  assert.equal(renderer.textures.has('6:0'),false);
  assert.deepEqual(renderer.textures.get('6:2'),{texture:null,width:640,height:480,dynamic:true,flipY:true,initialized:false});
  await assert.rejects(()=>renderer.loadBank(6,{entries:[{index:3,name:'@R',width:0,height:480,texture:null}]},'http://127.0.0.1/assets/'));
});

test('native stage capture happens once before layer5, Main82 and all526 rows sample that immutable RGB image',async()=>{
  const {renderer,events,uploads}=harness();
  await renderer.loadBank(6,{entries:[{index:2,name:'@R',width:640,height:480,texture:null}]},'http://127.0.0.1/assets/');
  const main={...quad(29),bank:6,entry:2,blend:3,stageSurface:'main',sprite:83,script:82,width:384,height:448,
    x:128,y:16,u0:.2,v0:16/480,u1:.8,v1:464/480};
  const row={...quad(29),bank:6,entry:2,blend:0,...mesh()};
  const records=draws.orderDrawLayers([quad(8),row,quad(0),quad(5),main],{world:true});
  const ordered=[records[0],records[1],main,row,records.at(-1)];
  renderer.render(ordered,{world:true});
  assert.ok(events.some(event=>event[0]==='equation'&&event[1]===renderer.gl.FUNC_ADD));
  assert.equal(events.filter(event=>event[0]==='copy').length,1);
  const capture=events.findIndex(event=>event[0]==='copy');
  assert.ok(events.slice(0,capture).some(event=>event[0]==='draw'&&event[1]==='image'));
  assert.equal(events[capture][4],renderer.gl.RGB);
  assert.equal(renderer.textures.get('6:2').initialized,true);
  assert.ok(uploads.some(values=>values.length>=32*3*8));
  const firstMain=uploads.find(values=>values.length===48&&values[0]===128&&values[1]===16);
  assert.ok(firstMain);
  assert.equal(firstMain[3],Math.fround(1-16/480));
  renderer.render(ordered,{world:true});
  assert.equal(events.filter(event=>event[0]==='copySub').length,1);
  const invalid=[main,{...row,geometry:{kind:'triangle-strip',vertices:[]}}];
  const eventCount=events.length;assert.throws(()=>renderer.render(invalid,{world:true}));assert.equal(events.length,eventCount);
  assert.throws(()=>renderer.render([row],{world:true}),/Main82|main/i);
});
