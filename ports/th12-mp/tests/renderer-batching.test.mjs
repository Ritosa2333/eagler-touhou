import test from 'node:test';
import assert from 'node:assert/strict';
import {Renderer} from '../src/renderer.mjs';
import {createRendererFixture,drawRendererWorkload,rendererWorkloads} from './helpers/renderer-workload.mjs';

test('growing vertex storage preserves all vertices and uploads only the live prefix after reuse',()=>{
  const {renderer:r,draws,stats}=createRendererFixture(Renderer,{record:true});
  const quad=()=>r.region('bullet',0,0,0,16,16,32.25,48.5,16,16,0,.3,[.2,.4,.6]);
  r.begin();for(let i=0;i<600;i++)quad();r.flush();
  assert.equal(draws.length,1,'capacity growth does not split a batch');
  assert.equal(draws[0].count,600*6);
  const corners=[[24.25,40.5,0,0],[40.25,40.5,16/256,0],[40.25,56.5,16/256,16/256],
    [24.25,40.5,0,0],[40.25,56.5,16/256,16/256],[24.25,56.5,0,16/256]];
  const expected=corners.flatMap(p=>[...p,.2,.4,.6,.3,1,0]).map(Math.fround);
  for(let i=0;i<600;i++)assert.deepEqual(draws[0].vertices.slice(i*60,(i+1)*60),expected);
  const storage=r.batch.data;
  r.begin();quad();r.flush();r.flush();
  assert.equal(r.batch.data,storage,'frame start and flush retain capacity');
  assert.equal(draws.length,2,'empty flush emits nothing');
  assert.equal(draws[1].count,6);assert.deepEqual(draws[1].vertices,expected);
  assert.equal(stats.uploadBytes,(600+1)*60*4,'unused capacity never reaches the GPU');
});

test('steady renderer workloads reuse upload storage and sampling state across frames',()=>{
  for(const workload of Object.values(rendererWorkloads)){
    const {renderer,stats,reset}=createRendererFixture(Renderer);
    drawRendererWorkload(renderer,workload);reset();
    for(let i=0;i<3;i++)drawRendererWorkload(renderer,workload);
    assert.equal(stats.uploadBuffers,0);
    assert.equal(stats.parameters,0);
    assert.equal(stats.draws,3*Math.ceil(workload.sprites/workload.group));
    assert.equal(stats.uploadBytes,3*workload.sprites*6*10*4);
  }
});

test('texture changes flush pending vertices with their original filter, address and blend',()=>{
  const {renderer:r,gl:g,draws}=createRendererFixture(Renderer,{record:true});
  const draw=(entry,point,blend)=>r.region('bullet',entry,0,0,16,16,50,50,16,16,0,1,[1,1,1],blend,{point});
  r.begin();draw(0,false,0);draw(1,true,1);draw(0,true,0);draw(0,false,0);r.flush();
  assert.deepEqual(draws.map(d=>d.texture),[1,2,1,1]);
  assert.deepEqual(draws.map(d=>d.parameters[g.TEXTURE_MIN_FILTER]),[g.LINEAR,g.NEAREST,g.NEAREST,g.LINEAR]);
  assert.deepEqual(draws.map(d=>d.parameters[g.TEXTURE_MAG_FILTER]),[g.LINEAR,g.NEAREST,g.NEAREST,g.LINEAR]);
  assert.deepEqual(draws.map(d=>d.blend),[[g.SRC_ALPHA,g.ONE_MINUS_SRC_ALPHA],
    [g.SRC_ALPHA,g.ONE],[g.SRC_ALPHA,g.ONE_MINUS_SRC_ALPHA],[g.SRC_ALPHA,g.ONE_MINUS_SRC_ALPHA]]);
  const vertices=[{x:1,y:2,u:.1,v:.2,rhw:.5,color:0x80112233},
    {x:3,y:4,u:.3,v:.4,rhw:2,color:0x40224466},{x:5,y:6,u:.5,v:.6,rhw:1,color:0xff336699}];
  for(const addressFlags of [0,1,2,3])r.mesh({a:{name:'bullet',entry:0,blend:0},vertices,primitive:4,addressFlags,filter:1});
  r.flush();
  assert.deepEqual(draws.slice(4).map(d=>[d.parameters[g.TEXTURE_WRAP_S],d.parameters[g.TEXTURE_WRAP_T]]),
    [[g.CLAMP_TO_EDGE,g.CLAMP_TO_EDGE],[g.REPEAT,g.CLAMP_TO_EDGE],[g.CLAMP_TO_EDGE,g.REPEAT],[g.REPEAT,g.REPEAT]]);
  assert.deepEqual(draws.at(-1).vertices,vertices.flatMap(v=>[v.x,v.y,v.u,v.v,
    ((v.color>>>16)&255)/255,((v.color>>>8)&255)/255,(v.color&255)/255,(v.color>>>24)/255,1/v.rhw,0]).map(Math.fround));
});

test('line strips stay separate and two-ended trails keep both vertices',()=>{
  const {renderer:r,gl:g,draws}=createRendererFixture(Renderer,{record:true});
  const vertices=[{x:1,y:2,color:[1,0,0,.5]},{x:3,y:4,color:[0,1,0,1]}];
  r.begin();r.solidLineStrip(vertices,0);r.solidLineStrip(vertices,1);
  r.trail(8,12,0,4,0xff112233,0x80445566,.5);r.flush();
  assert.deepEqual(draws.map(d=>[d.mode,d.count]),[[g.LINE_STRIP,2],[g.LINE_STRIP,2],[g.LINES,2]]);
  assert.deepEqual(draws[2].vertices,[6,12,.5,.5,17/255,34/255,51/255,.5,1,0,
    10,12,.5,.5,68/255,85/255,102/255,128/255*.5,1,0].map(Math.fround));
});

test('background capture updates texture-local sampling state before later mesh reuse',()=>{
  const {renderer:r,gl:g,draws,stats}=createRendererFixture(Renderer,{record:true});
  const texture={texture:g.createTexture(),w:640,h:480,defaultWrap:g.REPEAT,flipY:true};
  r.textures.set('text:2',texture);
  const vertices=[{x:0,y:0,u:0,v:0,color:0xffffffff,rhw:1},
    {x:10,y:0,u:1,v:0,color:0xffffffff,rhw:1},{x:10,y:10,u:1,v:1,color:0xffffffff,rhw:1}];
  const mesh={a:{name:'text',entry:2,blend:0},vertices,primitive:4,filter:1,addressFlags:0};
  r.begin();r.mesh(mesh);r.stageSurface(14);r.mesh(mesh);r.flush();
  assert.deepEqual(draws.map(d=>d.parameters[g.TEXTURE_MIN_FILTER]),[g.NEAREST,g.LINEAR,g.NEAREST]);
  assert.deepEqual(draws.map(d=>d.parameters[g.TEXTURE_WRAP_S]),[g.CLAMP_TO_EDGE,g.REPEAT,g.CLAMP_TO_EDGE]);
  const before=stats.parameters;r.mesh(mesh);r.flush();
  assert.equal(stats.parameters,before,'unchanged sampling is retained after the capture transition');
});
