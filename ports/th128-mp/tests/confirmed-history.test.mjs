import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {createConfirmedHistory,openConfirmedTimeline,readMultiplayerReplay} from '../runtime/confirmed-history.mjs';

const coreSHA256='a'.repeat(64),resourceSHA256='b'.repeat(64),build={coreSHA256,resourceSHA256},allowedMask=763;
const rules=epoch=>createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:'history-test',epoch,mode:'parallel-stage',
  route:'A1-3',seed:34772,...build,inputDelay:0,rollbackWindow:2});
const frame=(n,a,b,beforeA=0,beforeB=0)=>({frame:n,inputs:[[a,beforeA],[b,beforeB]].map(([held,prev],player)=>({
  player,held,pressed:held&~prev,released:prev&~held,predicted:false,
}))});

test('a multiplayer replay stores both confirmed seats, build and epoch',async()=>{
  const source=await createConfirmedHistory({rules:rules(1),allowedMask});
  const predicted=frame(0,0,128);predicted.inputs[1].predicted=true;
  assert.throws(()=>source.append(predicted),/confirmed/);
  assert.throws(()=>source.append(frame(1,0,128)),/contiguous/);
  const first=frame(0,1,128),second=frame(1,0,128,1,128);
  source.append(first);source.append(second);
  assert.throws(()=>source.replayBytes(),/sealed/);
  assert.throws(()=>source.append(frame(2,0,1,0,0)),/edge/);
  source.seal('restart');
  assert.throws(()=>source.append(frame(2,0,0,0,128)),/sealed/);
  const bytes=source.replayBytes();
  const replay=await readMultiplayerReplay(bytes,build);
  assert.deepEqual(replay.header,source.header);
  assert.deepEqual(replay.status(),{nextFrame:2,end:'restart'});
  assert.deepEqual(replay.read(0),first);assert.deepEqual(replay.read(1),second);assert.equal(replay.read(2),null);
  assert.ok(Object.isFrozen(replay.read(1).inputs[0]));
  await assert.rejects(readMultiplayerReplay(bytes,{...build,coreSHA256:'c'.repeat(64)}),/build/);
});

test('join mid-round from the beginning, reject gaps and stale epochs atomically, retain the tail',async()=>{
  const source=await createConfirmedHistory({rules:rules(1),allowedMask});
  const next=await createConfirmedHistory({rules:rules(2),allowedMask});
  source.append(frame(0,1,128));source.append(frame(1,1,0,1,128));source.append(frame(2,0,0,1,0));
  next.append(frame(0,128,0));
  const viewer=await openConfirmedTimeline(source.header,build),other=await openConfirmedTimeline(source.header,build);
  assert.equal(viewer.read(0),null);
  assert.throws(()=>viewer.accept(source.chunk(1,2)),/contiguous/);
  assert.throws(()=>viewer.accept(next.chunk(0,1)),/identity/);
  const batch=source.chunk(0,2),bad={...batch,frames:[batch.frames[0],[1,4]]};
  assert.throws(()=>viewer.accept(bad),/mask/);
  assert.equal(viewer.status().nextFrame,0);
  viewer.accept(batch);
  assert.equal(other.status().nextFrame,0,'each viewer advances independently');
  assert.deepEqual(viewer.read(1),frame(1,1,0,1,128));
  const tail=source.seal('completed');
  assert.throws(()=>viewer.finish(tail),/tail/);
  viewer.accept(source.chunk(2,1));viewer.finish(tail);
  assert.deepEqual(viewer.read(2),frame(2,0,0,1,0));
  assert.equal(viewer.status().end,'completed');
  assert.throws(()=>viewer.accept(source.chunk(0,1)),/ended/);
  assert.equal(source.chunk(3,1),null);
});
