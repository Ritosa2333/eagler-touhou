import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {createConfirmedHistory} from '../runtime/confirmed-history.mjs';
import {makeTh128ReplayFile,readTh128ReplayFile} from '../runtime/replay-file.mjs';

const build={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64)};
async function recording(){
  const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:'local-file-round',epoch:2,mode:'parallel-stage',
    route:'A1-3',seed:34772,inputDelay:0,rollbackWindow:2,...build});
  const history=await createConfirmedHistory({rules,allowedMask:763});
  history.append({frame:0,inputs:[64,128].map((held,player)=>({player,held,pressed:held,released:0,predicted:false}))});
  history.seal('restart');return history.replayBytes();
}
test('multiplayer file export preserves exact bytes and adopts its native format again',async()=>{
  const input=await recording(),file=await makeTh128ReplayFile(input,build);
  assert.equal(file.name,'th128mp-local-file-round-2.th128mp');
  assert.equal(file.type,'application/octet-stream');assert.deepEqual(new Uint8Array(await file.arrayBuffer()),input);
  const reopened=await readTh128ReplayFile(file,build);
  assert.deepEqual(reopened.bytes,input);assert.equal(reopened.name,file.name);
  assert.deepEqual(reopened.tape.status(),{nextFrame:1,end:'restart'});
  assert.deepEqual(reopened.tape.read(0).inputs.map(row=>row.held),[64,128]);
  assert.match(reopened.sha256,/^[a-f0-9]{64}$/);
});
test('a single-player file or a different loaded build cannot enter multiplayer Replay',async()=>{
  const data=await recording();
  await assert.rejects(readTh128ReplayFile(new File([data],'th128_00.rpy'),build),/TH128 multiplayer Replay file/);
  await assert.rejects(readTh128ReplayFile(new File([data],'network.th128mp'),{...build,coreSHA256:'c'.repeat(64)}),/build/);
  await assert.rejects(readTh128ReplayFile(new File([new Uint8Array(8*1024*1024+1)],'huge.th128mp'),build),/size/);
});
