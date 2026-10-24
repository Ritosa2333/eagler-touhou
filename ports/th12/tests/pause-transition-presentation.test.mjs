import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {TitlePresentation} from '../src/title-presentation.mjs';
import {stageTransitionPose} from '../src/stage-clear-presentation.mjs';
import {orderPresentation} from '../src/draw-scheduler.mjs';

const file=process.env.TH12_TEST_ATLASES??new URL('../site/assets/atlases.json',import.meta.url);
const atlases=fs.existsSync(file)?JSON.parse(fs.readFileSync(file)):null;
for(const mode of ['start','replay'])test(`retail ${mode} pause selection highlights immediately in both directions`,{skip:!atlases&&'Set TH12_TEST_ATLASES to prepared retail atlases.json'},()=>{
  const presentation=new TitlePresentation({atlases},{},{data:{clears:{}}});
  const state={serial:1,scene:'pause',context:{mode},age:0,selection:0,phase:'in',choices:Array(mode==='replay'?3:4).fill({}),events:[]};
  const tick=()=>{presentation.tick(state);state.age++;};
  for(let i=0;i<50;i++)tick();
  state.phase='ready';
  for(const row of [1,2,1,0,1,2,0]){
    state.selection=row;state.events=[{type:'selection'}];tick();
    const script=(mode==='replay'?94:90)+row;
    for(let frame=0;frame<6;frame++){
      const selected=presentation.bindings.get('popup').poses().find(p=>p.script===script);
      assert.equal(selected.r,1,`row${row}, frame${frame} must keep the authored pink highlight`);
      assert.ok(selected.g>=128/255&&selected.alpha>.9);tick();
    }
  }
});
test('stage field transition retains thirty byte-alpha steps and composes before the surface copy',()=>{
  assert.equal(stageTransitionPose(null),null);
  const outgoing=[],incoming=[];
  for(let age=0;age<=30;age++){
    const out=stageTransitionPose({out:true,age}),into=stageTransitionPose({out:false,age});
    outgoing.push(out.rectangle.argb>>>24);incoming.push(into.rectangle.argb>>>24);
    assert.deepEqual(out.rectangle.bounds,[32,16,416,464]);assert.equal(out.clip,1);
    assert.deepEqual(orderPresentation([{drawPriority:12},{drawPriority:14},out,{drawPriority:24}]).map(p=>p.drawPriority),[11,12,14,24]);
  }
  assert.deepEqual([outgoing[0],outgoing[15],outgoing[30]],[0,127,255]);
  assert.deepEqual([incoming[0],incoming[15],incoming[30]],[255,127,0]);
  assert.ok(outgoing.every((value,index)=>index===0||value>outgoing[index-1]));
  assert.ok(incoming.every((value,index)=>index===0||value<incoming[index-1]));
});
