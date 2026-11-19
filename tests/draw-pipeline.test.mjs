import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import * as draws from '../site/runtime/draw.mjs';
import {circleFanVertices} from '../site/runtime/draw.mjs';

test('native mode16 circle fan triangulates original center/edge colors and wraps the final segment',()=>{
 const record={mode:16,geometry:{kind:'circle-fan',vertices:[{x:10,y:20,color:0x80ff00ff},{x:20,y:20,color:0x01ff50ff},
  {x:10,y:30,color:0x01ff50ff},{x:0,y:20,color:0x01ff50ff},{x:10,y:10,color:0x01ff50ff},{x:20,y:20,color:0x01ff50ff}]}};
 const vertices=circleFanVertices(record);assert.equal(vertices.length,4*3*8);
 const point=index=>Array.from(vertices.slice(index*8,index*8+8));
 const center=Array.from(new Float32Array([10,20,0,0,...draws.argbColor(0x80ff00ff)])),edge=Array.from(new Float32Array([20,20,0,0,...draws.argbColor(0x01ff50ff)]));
 assert.deepEqual(point(0),center);assert.deepEqual(point(1),edge);assert.deepEqual(point(2),Array.from(new Float32Array([10,30,0,0,...draws.argbColor(0x01ff50ff)])));
 assert.deepEqual(point(vertices.length/8-1),edge);
 for(const mutate of [value=>value.geometry.vertices.pop(),value=>value.geometry.vertices[1].x=Infinity,value=>value.geometry.kind='texture']){
  const invalid=structuredClone(record);mutate(invalid);assert.throws(()=>circleFanVertices(invalid));}
});
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('world shake projects game sprites and typed geometry without mutating simulation records or HUD',()=>{
    assert.equal(typeof draws.applyWorldShake,'function');
    const record={x:320,y:416,layer:0,drawPass:'player'};const shifted=draws.applyWorldShake(record,[-12,4]);
    assert.equal(shifted.x,308);assert.equal(shifted.y,420);assert.equal(record.x,320);
    const hud={x:0,y:0,layer:23};assert.equal(draws.applyWorldShake(hud,[-12,4]),hud);
    const fan={layer:16,geometry:{kind:'circle-fan',vertices:[{x:10,y:20}]}};
    assert.deepEqual(draws.applyWorldShake(fan,[1,2]).geometry.vertices,[{x:11,y:22}]);assert.deepEqual(fan.geometry.vertices,[{x:10,y:20}]);
    assert.throws(()=>draws.applyWorldShake(record,[Infinity,0]));
});
test('manual bullet/player/frozen-bullet callbacks retain native priorities independently of their ANM layer',()=>{
    const report=JSON.parse(fs.readFileSync(path.join(process.env.TH128_RESEARCH_FIXTURES??path.join(root,'local/research'),'player-render-fixtures.json')));
    assert.equal(typeof draws.nativeRecordPriority,'function');
    for(const pass of report.passes)assert.equal(draws.nativeRecordPriority({layer:0,drawPass:pass.name}),pass.priority);
    const records=[{id:1,layer:0,drawPass:'hostile-bullets'},{id:2,layer:0,drawPass:'player'},{id:3,layer:7},
        {id:4,layer:0,drawPass:'frozen-bullets'},{id:5,layer:0,drawPass:'player'},{id:6,layer:16}];
    assert.deepEqual(draws.orderDrawLayers(records,{world:true}).map(record=>record.id),[3,4,2,5,1,6]);
    assert.throws(()=>draws.nativeRecordPriority({layer:0,drawPass:'invented'}));
});
test('world layer ordering follows actual native callback priorities, not numeric layer29 after HUD23',()=>{
    assert.equal(typeof draws.nativeDrawPriority,'function','original TH128 draw priority mapping is missing');
    const report=JSON.parse(fs.readFileSync(path.join(process.env.TH128_RESEARCH_FIXTURES??path.join(root,'local/research'),'draw-pipeline-fixtures.json')));
    assert.equal(report.executable.sha256,report.executable.sha256After);
    assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/draw_pipeline_probe.py'))).digest('hex'),report.generatorSha256);
    for(const sample of report.layers)assert.equal(draws.nativeDrawPriority(sample.layer),sample.priority);
    const records=[{id:1,layer:23},{id:2,layer:8},{id:3,layer:29},{id:4,layer:0},{id:5,layer:5},{id:6,layer:29}];
    assert.deepEqual(draws.orderDrawLayers(records,{world:true}).map(record=>record.id),[4,5,3,6,2,1]);
    assert.deepEqual(records.map(record=>record.id),[1,2,3,4,5,6]);
    assert.deepEqual(draws.orderDrawLayers(records).map(record=>record.id),[4,5,2,1,3,6]);
    for(const value of [-1,27,28,33,NaN,1.5])assert.throws(()=>draws.nativeDrawPriority(value));
});
