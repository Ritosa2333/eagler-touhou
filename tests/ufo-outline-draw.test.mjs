import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {Renderer,outlineVertices} from '../src/renderer.mjs';
const native=JSON.parse(fs.readFileSync(new URL('fixtures/ufo-outline-draw-native.json',import.meta.url)));
const rgba=argb=>[((argb>>>16)&255)/255,((argb>>>8)&255)/255,(argb&255)/255,(argb>>>24)/255];

test('retail204/205 circles and211 completion hint are original45d710 perimeter lines, with no filled center',()=>{
  assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  for(const record of native.records){
    const s=record.state,color=rgba(s.primary),radius=Math.fround(s.radius*s.scale[0]);
    const vertices=outlineVertices(...s.position,radius,s.rotation,s.count,color);
    assert.equal(vertices.length,record.vertices.length);assert.equal(record.primitive,3);assert.equal(vertices.length,s.count+1);
    for(let i=0;i<vertices.length;i++){
      const [x,y]=record.vertices[i];assert.ok(Math.abs(vertices[i].x-x)<=0.00001,`script${s.script} vertex${i} X`);
      assert.ok(Math.abs(vertices[i].y-y)<=0.00001,`script${s.script} vertex${i} Y`);
      assert.deepEqual(vertices[i].color,rgba(record.vertices[i][4]));
    }
    assert.equal(record.rngUnchanged,true);
    const renderer=Object.create(Renderer.prototype),calls=[];
    renderer.solidTriangles=()=>assert.fail('mode16 must never cover the UFO with a filled fan');
    renderer.solidLineStrip=(points,blend)=>calls.push({points,blend});
    renderer.state({visible:true,mode:16,geometryCount:s.count,geometryRadius:s.radius,sx:s.scale[0],sy:s.scale[1],
      rotation:s.rotation,x:0,y:0,primaryColor:color,r:0,g:0,b:0,alpha:1,blend:(s.flags>>>5)&7},...s.position);
    assert.equal(calls.length,1);assert.deepEqual(calls[0].points,vertices);
  }
});

test('each UFO outline is a separate WebGL line strip without connecting two UFOs',()=>{
  const renderer=Object.create(Renderer.prototype),draws=[];
  Object.assign(renderer,{gl:{LINE_STRIP:3},solid:{},batch:[],material(){},texture(){},primitive(mode){this.mode=mode;},
    flush(){if(this.batch.length){draws.push({mode:this.mode,vertices:this.batch.slice()});this.batch=[];}}});
  for(const x of [100,300])renderer.solidLineStrip(outlineVertices(x,100,20,0,5,[1,0,0,1]),1);
  assert.equal(draws.length,2);for(const draw of draws){assert.equal(draw.mode,3);assert.equal(draw.vertices.length,60);}
});
