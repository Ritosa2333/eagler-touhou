import assert from 'node:assert/strict';
import {test} from 'node:test';
import {managedSceneStep,confirmedGameOver} from '../runtime/managed-view.mjs';

test('confirmed Replay/spectator batches advance background in each logical step; live prediction does not',()=>{
  const rows=[],pair={step:row=>rows.push(row.frame)},background={frame:0,tick(){this.frame++;}};
  const row=(frame,predicted=false)=>({frame,inputs:[{predicted},{predicted:false}]});
  const confirmed=managedSceneStep(pair,background,{confirmedOnly:true});
  for(let i=0;i<4;i++)confirmed(row(i));
  assert.equal(background.frame,4);assert.deepEqual(rows,[0,1,2,3]);
  assert.throws(()=>confirmed(row(4,true)),/confirmed/);
  assert.throws(()=>confirmed(row(5)),/confirmed/);
  const live=managedSceneStep(pair,background);live(row(4,true));
  assert.equal(background.frame,4);assert.deepEqual(rows,[0,1,2,3,4]);
});

test('terminal presentation cannot announce speculative GameOver or reuse it for the other Replay seat',()=>{
  const terminal={reason:'game-over',frame:12};
  const pair={terminal:seat=>seat===0?terminal:null};
  assert.equal(confirmedGameOver(pair,0,12),null);
  assert.deepEqual(confirmedGameOver(pair,0,13),terminal);
  assert.equal(confirmedGameOver(pair,1,13),null);
  pair.terminal=()=>null;
  assert.equal(confirmedGameOver(pair,0,13),null);
});

test('a cooperative watcher consumes all three confirmed seats and advances a single background',()=>{
 const n=[],stage={step:r=>n.push(r.inputs.length)},bg={frame:0,tick(){this.frame++;}};const step=managedSceneStep(stage,bg,{confirmedOnly:true,players:3});
 const row=f=>({frame:f,inputs:[0,1,2].map(player=>({player,predicted:false}))});step(row(0));assert.deepEqual([bg.frame,n],[1,[3]]);assert.throws(()=>step({frame:1,inputs:row(1).inputs.slice(0,2)}));
});
test('after a native handoff the renderer never draws old textures/STD or advances the new background by the global match age',async()=>{
 const {createCoopStageScreen}=await import('../runtime/managed-view.mjs');const enter=(f,id,base)=>({frame:f,stageStart:base,stageFrame:f-base,stageId:id});
 const bg=stage=>({stage,frame:0,tick(){this.frame++;}}),loads=[],ctl=new AbortController();let done;
 const screen=createCoopStageScreen({id:1,background:bg(1),signal:ctl.signal,open:async id=>{loads.push(id);await new Promise(r=>{done=r});return bg(id);}});
 const old=screen.at(enter(90,1,0),87);assert.equal(old.frame,87);assert.equal(screen.at(enter(101,4,100),100),null);assert.deepEqual(loads,[]);
 assert.equal(screen.at(enter(101,4,100),101),null);assert.equal(screen.at(enter(103,4,100),102),null);assert.deepEqual(loads,[4]);done();await Promise.resolve();await Promise.resolve();
 const fresh=screen.at(enter(104,4,100),103);assert.equal(fresh.stage,4);assert.equal(fresh.frame,3);
 screen.close();assert.throws(()=>screen.at(enter(104,4,100),104));
});