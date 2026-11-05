import test from 'node:test';
import assert from 'node:assert/strict';

for(const game of ['th02','th03','th04'])test(`${game} only encodes a finished recording on save`,async()=>{
  const {createBrowserSoloRecorder}=await import(`../../ports/${game}/web/replay/external-recorder.js`);
  let files=0,failures=0;
  const recorder=createBrowserSoloRecorder({game:game.slice(2),language:'jp',identity:'0'.repeat(64),disk:null,
    onFile:()=>files++,onFailure:()=>failures++});
  recorder.config.onEnd({boundary:1});
  assert.equal(recorder.state.ready,true);
  assert.equal(recorder.file,null);
  assert.equal(files,0);
  assert.equal(failures,0);
  await assert.rejects(recorder.save(),/启动磁盘不可用/);
  assert.equal(failures,1);
});
