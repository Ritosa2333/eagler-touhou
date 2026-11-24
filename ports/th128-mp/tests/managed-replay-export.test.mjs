import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {createConfirmedHistory} from '../runtime/confirmed-history.mjs';
import {makeTh128ReplayFile,readTh128ReplayFile} from '../runtime/replay-file.mjs';
import {createManagedReplayExport} from '../runtime/managed-replay-export.mjs';

const build={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64)};
async function record(epoch,identity=build){
  const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',mode:'parallel-stage',matchId:'managed-export',epoch,
    route:'A1-3',seed:34772,inputDelay:0,rollbackWindow:2,...identity});
  const history=await createConfirmedHistory({rules,allowedMask:763});
  history.append({frame:0,inputs:[0,1].map(player=>({player,held:0,pressed:0,released:0,predicted:false}))});
  history.seal('restart');return history.replayBytes();
}

test('a document retains one validated previous match and refuses to replace it with a different build',async()=>{
  const updates=[],signal=new AbortController(),owner=createManagedReplayExport({build,signal:signal.signal,onFile:file=>updates.push(file?.name??null)});
  assert.equal(owner.file(),null);const first=await record(1);
  await owner.remember(first);assert.equal(owner.file().name,'th128mp-managed-export-1.th128mp');
  const bad=await record(2,{...build,coreSHA256:'c'.repeat(64)});
  await assert.rejects(owner.remember(bad),/build/);assert.equal(updates.length,1);
  const second=await record(2);await owner.remember(second);
  assert.equal(owner.file().name,'th128mp-managed-export-2.th128mp');
  const read=await readTh128ReplayFile(owner.file(),build);assert.deepEqual(read.bytes,second);
  signal.abort();assert.equal(owner.file(),null);assert.equal(updates.at(-1),null);
});

test('a slower older validation or retired document cannot restore a file slot',async()=>{
  const life=new AbortController(),waiting=[],updates=[];
  const owner=createManagedReplayExport({build,signal:life.signal,onFile:file=>updates.push(file?.name??null),
    validate:bytes=>new Promise(resolve=>waiting.push(()=>resolve(makeTh128ReplayFile(bytes,build))))});
  const first=owner.remember(await record(1)),second=owner.remember(await record(2));
  waiting[1]();assert.equal(await second,true);waiting[0]();assert.equal(await first,false);
  assert.deepEqual(updates,['th128mp-managed-export-2.th128mp']);
  const late=owner.remember(await record(3));life.abort();waiting[2]();assert.equal(await late,false);assert.equal(owner.file(),null);
});
