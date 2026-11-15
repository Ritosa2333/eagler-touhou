import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {parseStd,StageCamera,StageBackground} from '../site/runtime/renderer.mjs';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const expected=[
  {frame:0,offset:-1.8369700935892946e-15,up:-1.224646790546622e-18},
  {frame:1,offset:-0.1840730756521225,up:-0.00012271538435015827},
  {frame:20,offset:-3.644702672958374,up:-0.0024298017378896475},
  {frame:60,offset:-10.073384284973145,up:-0.006715589668601751},
];

function stageData(){
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
  const manifest=path.resolve(root,'local',catalog.variants.ja.manifest);
  return parseStd(fs.readFileSync(path.join(path.dirname(manifest),'raw/stage_b2.std')));
}

test('stage_b2 STD mode6 advances native phase and is consumed by cameraView',()=>{
  const data=stageData();
  const command=data.commands.find(item=>item.opcode===12);
  assert.deepEqual({offset:command.offset,time:command.time,integers:command.integers},
    {offset:80,time:0,integers:[6]});
  const camera=new StageCamera(data);
  for(const expectedFrame of expected){
    while(camera.frame<expectedFrame.frame)camera.tick();
    const snapshot=camera.snapshot();
    assert.equal(snapshot.effect.mode,6);
    assert.equal(snapshot.effect.offset[0],expectedFrame.offset);
    assert.equal(snapshot.effect.upX,expectedFrame.up);
    assert.equal(snapshot.view.position[0],Math.fround(snapshot.position[0]+expectedFrame.offset));
    assert.equal(snapshot.view.up[0],expectedFrame.up);
    assert.ok(snapshot.effect.offset.every(Number.isFinite));
    assert.ok([...snapshot.view.position,...snapshot.view.direction,...snapshot.view.up].every(Number.isFinite));
  }
  const module={_th128_bind:()=>true,_th128_tick:()=>true,UTF8ToString:()=>''};
  const background=new StageBackground(data,module),view=background.snapshot().camera.view;
  assert.deepEqual(view,background.camera.cameraView());

  const reset=new StageCamera({commands:[
    {offset:0,time:0,opcode:12,integers:[0],floats:[]},
  ]}).snapshot();
  assert.deepEqual(reset.effect,{mode:0,current:-1,frame:0,time:0,offset:[0,0,0],upX:0});

  const noop=new StageCamera({commands:[
    {offset:0,time:0,opcode:12,integers:[2],floats:[]},
  ]}).snapshot();
  assert.equal(noop.effect.mode,2);

  for(const mode of [1,4,5,7])
    assert.throws(()=>new StageCamera({commands:[
      {offset:0,time:0,opcode:12,integers:[mode],floats:[]},
    ]}),new RegExp(`STD12 mode${mode} consumer pending`));
  assert.throws(()=>new StageCamera({commands:[
    {offset:0,time:0,opcode:12,integers:[256],floats:[]},
  ]}),/Invalid original STD effect mode/);
});
