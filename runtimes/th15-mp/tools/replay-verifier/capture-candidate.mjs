import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';
import {core,memory,string,root} from '../../th15_web/tests/cpp/helpers.mjs';
import {preloadStage} from '../../th15_web/tests/cpp/stage-assets-fixture.mjs';
import {inspectReplay} from './inspect-replay.mjs';
import {capture,identity,row} from './observation.mjs';
const repo=fileURLToPath(new URL('../../',import.meta.url)),corpus=JSON.parse(readFileSync(new URL('./corpus.json',import.meta.url)));
const common=resolve(process.env.EAGLER_COMMON_ROOT||fileURLToPath(new URL('../../../eagler-common',import.meta.url)));
const {joinCompletedTraces}=await import(pathToFileURL(resolve(common,'testkit/replay-verifier/join-traces.mjs')));
const option=name=>{const i=process.argv.indexOf(name);return i<0?null:process.argv[i+1];};
const lane=option('--lane')||'all',ids=option('--case')?[option('--case')]:lane==='all'?Object.values(corpus.suites).flat():corpus.suites[lane];
const out=resolve(option('--output')||resolve(repo,'artifacts/replay-verifier/candidate'));
if(!ids)throw Error('Unknown suite');
for(const id of ids){
  const fixture=corpus.cases.find(f=>f.id===id);if(!fixture)throw Error('Unknown fixture');
  const path=resolve(repo,fixture.source),raw=readFileSync(path);assert.equal(createHash('sha256').update(raw).digest('hex'),fixture.replaySha256);
  const metadata=await inspectReplay(path),outputs=[];
  if(fixture.kind!=='demo'){assert.equal(metadata.clearStage,8);assert.equal(metadata.character,fixture.character);assert.equal(metadata.difficulty,fixture.difficulty);assert.deepEqual(metadata.stages.map(s=>s.stage),fixture.stages);}
  for(const {stage,frames} of metadata.stages){
    const c=await core(),asset=preloadStage(c,stage,metadata.character),run=c.run_session_create(asset.fixture),p=c.allocate(raw.length),state=c.allocate(64);
    memory(c,p,raw.length).set(raw);c.stage_assets_viewport(asset.fixture,128,16,320,16);assert.equal(c.run_session_load(run,stage,metadata.character,p,raw.length),1,string(c,c.run_session_error(run)));
    const completion=fixture.kind==='demo'?0:c.run_session_oracle_completion_create(run);if(fixture.kind!=='demo')assert.ok(completion);
    const name=fixture.kind==='demo'?id:id+'-stage'+stage,output=resolve(out,name+'.candidate.jsonl');outputs.push(output);
    const stream=capture(output,identity(root,raw),'th15/current-wasi-core');let complete=false;
    try{for(let frame=0;frame<frames;frame++){
      assert.equal(c.run_session_step_world(run),1,string(c,c.run_session_error(run)));
      c.run_session_world_state(run,state);const words=Array.from(new Uint32Array(c.memory.buffer,state,16));stream.tick(row(stage,frame,words));
      if(fixture.kind!=='demo'&&c.run_session_oracle_completed(completion)){complete=true;break;}
      if(fixture.kind==='demo'&&frame+1===frames)complete=true;
    }
    assert.ok(complete,'Candidate must complete stored Demo or clear the stage');
    stream.finish({complete:true,reason:fixture.kind==='demo'?'replay-fixture-consumed':'stage-clear',evidence:{stage,ticks:stream.ticks}});
    console.log(JSON.stringify({id,stage,ticks:stream.ticks}));
    }finally{if(completion)c.run_session_oracle_completion_delete(completion);c.run_session_delete(run);asset.close();c.release(p);c.release(state);}
  }
  if(fixture.kind!=='demo')await joinCompletedTraces(outputs,resolve(out,id+'.candidate.jsonl'));
}
