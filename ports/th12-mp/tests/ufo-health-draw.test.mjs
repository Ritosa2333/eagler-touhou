import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {Renderer} from '../src/renderer.mjs';

const native=JSON.parse(fs.readFileSync(new URL('fixtures/ufo-health-draw-native.json',import.meta.url)));
const rgba=argb=>[((argb>>>16)&255)/255,((argb>>>8)&255)/255,(argb&255)/255,(argb>>>24)/255];

test('UFO136 red health bar and137 parented shadow match original45ce60 full-width rectangle corners',()=>{
  assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  assert.equal(native.records.length,4);
  for(const record of native.records){
    const s=record.state,color=rgba(s.colorARGB),parent=s.parent;
    const x=s.position[0]+s.engine[0]+s.offset[0]+(parent?parent.position[0]+parent.engine[0]:0),
      y=s.position[1]+s.engine[1]+s.offset[1]+(parent?parent.position[1]+parent.engine[1]:0),
      sx=s.scale[0]*(parent?.scale[0]??1),sy=s.scale[1]*(parent?.scale[1]??1);
    const renderer=Object.create(Renderer.prototype),draws=[];
    renderer.solidTriangles=(vertices,indices)=>draws.push({vertices,indices});
    renderer.state({mode:14,visible:true,x:0,y:0,r:color[0],g:color[1],b:color[2],alpha:color[3],
      primaryColor:color,geometryRadius:s.size[0],geometryWidth:s.size[1],sx,sy,
      rotation:s.rotation+(parent?.rotation??0),anchorX:2,anchorY:2},x,y);
    assert.equal(draws.length,1);assert.deepEqual(draws[0].indices,[0,1,2,0,2,3]);
    // Native uses a four-point triangle strip; WebGL uses two triangles.
    const nativeByQuad=[record.vertices[3],record.vertices[2],record.vertices[0],record.vertices[1]];
    for(let i=0;i<4;i++){
      const actual=draws[0].vertices[i],expected=nativeByQuad[i];
      assert.equal(actual.x,expected[0],`script${s.script} health${s.health} corner${i} X`);
      assert.equal(actual.y,expected[1],`script${s.script} health${s.health} corner${i} Y`);
      assert.deepEqual(actual.color,rgba(expected[4]));
    }
    assert.equal(s.size[0],32);assert.equal(s.size[1],4);
    assert.equal(s.layer,s.script===136?10:9);assert.equal(record.rngUnchanged,true);
  }
});
