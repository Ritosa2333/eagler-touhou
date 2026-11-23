import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {createMultiplayerSessionOwner} from '../../th128/site/runtime/multiplayer-session.mjs';
import {createMultiplayerRound} from '../runtime/round.mjs';

const allowedMask=763;
const setup=async ({rollbackWindow=2,inputDelay=0}={})=>{
  const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:'round-test',epoch:1,
    mode:'parallel-stage',route:'A1-3',seed:34772,coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64),
    inputDelay,rollbackWindow});
  const session=await createMultiplayerSessionOwner({rules,allowedMask,localPlayer:0});
  const sent=[],confirmed=[],released=[],restored=[];
  let state=[[],[]],captures=0;
  const engine={
    checkpoint(){captures++;return structuredClone(state);},
    restore(snapshot){restored.push(snapshot);state=structuredClone(snapshot);},
    release(snapshot){released.push(snapshot);},
    step({inputs}){
      for(const {player,held,pressed,released} of inputs)state[player].push([held,pressed,released]);
      return state.map(track=>track.at(-1));
    },
  };
  const round=createMultiplayerRound({session,localPlayer:0,engine,sendInput:packet=>sent.push(packet),
    onConfirmed:(row,effects)=>confirmed.push({row,effects})});
  const other=(frame,held,epoch=1)=>({matchId:rules.matchId,epoch,player:1,frame,held});
  return {round,session,other,sent,confirmed,released,restored,state:()=>state,captures:()=>captures};
};

test('late input rewinds the engine; only the corrected, continuous rows commit', async()=>{
  const late=await setup(),onTime=await setup();
  assert.equal(late.round.tick(2).advanced,true);
  assert.equal(late.round.tick(0).advanced,true);
  assert.deepEqual(late.state()[1],[[0,0,0],[0,0,0]]);
  assert.equal(late.round.tick(0).reason,'waiting-input');
  assert.deepEqual(late.confirmed,[]);
  late.round.receiveInput(late.other(1,0),1);
  late.round.flush();
  assert.deepEqual(late.confirmed,[],'frame 1 cannot publish before frame 0');
  late.round.receiveInput(late.other(0,8),1);
  late.round.flush();
  onTime.round.receiveInput(onTime.other(0,8),1);
  onTime.round.receiveInput(onTime.other(1,0),1);
  onTime.round.tick(2);onTime.round.tick(0);
  assert.deepEqual(late.state(),onTime.state());
  assert.deepEqual(late.state()[1],[[8,8,0],[0,0,8]]);
  assert.equal(late.restored.length,1);
  assert.equal(late.round.status().replayedFrames,2);
  assert.deepEqual(late.confirmed,onTime.confirmed);
  assert.deepEqual(late.confirmed.map(item=>item.row.frame),[0,1]);
  assert.ok(late.confirmed.every(item=>item.row.inputs.every(input=>!input.predicted)));
  late.round.flush();
  assert.equal(late.confirmed.length,2);
  assert.equal(late.released.length,late.captures());
});

test('pure delay samples local input once during a stall and emits once after confirmation',async()=>{
  const {round,other,sent,state,confirmed,captures}=await setup({rollbackWindow:0});
  assert.equal(round.tick(2).reason,'waiting-input');
  assert.equal(round.tick(8).reason,'waiting-input');
  assert.deepEqual(sent.map(({frame,held})=>[frame,held]),[[0,2]]);
  assert.throws(()=>round.receiveInput({...other(0,1),player:0},1),/source player/);
  assert.throws(()=>round.receiveInput(other(0,1),0),/remote seat/i);
  assert.throws(()=>round.receiveInput(other(0,1,2),1),/epoch/);
  round.receiveInput(other(0,1),1);
  assert.equal(round.tick(8).advanced,true);
  assert.deepEqual(state(),[[[2,2,0]],[[1,1,0]]]);
  assert.deepEqual(confirmed.map(item=>item.row.frame),[0]);
  assert.equal(captures(),0);
});

test('clock may wait for one ready frame without making a checkpoint or resampling the already sent input',async()=>{
  const {round,other,sent,state,captures}=await setup();
  let samples=0;const read=()=>{samples++;return 64;};
  assert.equal(round.tick(read,{allowPrediction:false}).reason,'waiting-input');
  assert.deepEqual(state(),[[],[]]);assert.equal(captures(),0);
  assert.equal(round.tick(read,{allowPrediction:false}).reason,'waiting-input');
  round.receiveInput(other(0,128),1);
  assert.equal(round.tick(read,{allowPrediction:false}).advanced,true);assert.equal(samples,1);
  assert.deepEqual(sent.map(p=>[p.frame,p.held]),[[0,64]]);
  assert.deepEqual(state(),[[[64,64,0]],[[128,128,0]]]);assert.equal(captures(),0);
  assert.equal(round.tick(0,{allowPrediction:true}).advanced,true);assert.equal(captures(),1);
});
