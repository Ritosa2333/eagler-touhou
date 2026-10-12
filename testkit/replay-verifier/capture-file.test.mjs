import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,readFileSync,writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {TraceCapture} from './capture-file.mjs';
import {compareTraces} from './compare.mjs';
import {readTraceFile} from './golden-files.mjs';
import {joinCompletedTraces} from './join-traces.mjs';
import {acceptFileSuite,compareFileSuite} from './file-suite.mjs';
import {verifyGoldenFiles} from './golden-files.mjs';

const identity={game:'fixture',profile:'original/v1',stateSchema:'state/v1',traceCodec:'jsonl/v1',digestAlgorithm:'sha256-truncated-128/canonical-json-v1',replaySha256:'1'.repeat(64),executableSha256:'2'.repeat(64),resourceSha256:'3'.repeat(64)};
function fixture(path,complete=true,value=7,stage=1){const c=new TraceCapture(path,{identity,provider:'retail-fixture',categories:['rng']});c.tick({stage,replaySampleIndex:0,appliedInput:1,clocks:{stageFrame:1},scalars:{stage},categories:{rng:[value]}});c.finish({complete,reason:'fixture-end',evidence:{stage}});return c;}
test('Stream capture reports changed RNG at its first tick',async()=>{
  const root=mkdtempSync(join(tmpdir(),'oracle-capture-')),expected=join(root,'original.jsonl'),actual=join(root,'candidate.jsonl');fixture(expected);fixture(actual,true,8);
  const result=await compareTraces(readTraceFile(expected),readTraceFile(actual));assert.equal(result.status,'DIVERGED');assert.equal(result.comparedTicks,0);assert.equal(result.firstDifference.category,'rng');
});
test('Short provider and missing tick cannot acquire a completion pass',async()=>{
  const root=mkdtempSync(join(tmpdir(),'oracle-incomplete-')),path=join(root,'a.jsonl'),other=join(root,'b.jsonl');fixture(path,false);assert.equal((await compareTraces(readTraceFile(path),readTraceFile(path))).status,'INCOMPLETE');
  fixture(other);const rows=readFileSync(other,'utf8').trim().split('\n');rows.splice(2,1);writeFileSync(other,rows.join('\n')+'\n');assert.equal((await compareTraces(readTraceFile(other),readTraceFile(other))).status,'INCOMPLETE');
});
test('Stage join retains exact ticks and refuses incomplete stage input',async()=>{
  const root=mkdtempSync(join(tmpdir(),'oracle-join-')),a=join(root,'a.jsonl'),b=join(root,'b.jsonl'),output=join(root,'all.jsonl');fixture(a);fixture(b,true,7,2);
  await joinCompletedTraces([a,b],output);const result=await compareTraces(readTraceFile(output),readTraceFile(output));assert.equal(result.status,'PASS');assert.equal(result.comparedTicks,2);
  fixture(b,false);await assert.rejects(joinCompletedTraces([a,b],output),/incomplete/);
});
test('Golden acceptance binds original identity and rejects replacement/tampering',async()=>{
  const root=mkdtempSync(join(tmpdir(),'oracle-golden-')),golden=join(root,'golden');
  for(const id of ['demo','clear']){fixture(join(root,id+'.original.jsonl'));fixture(join(root,id+'.candidate.jsonl'));}
  const corpus={game:'fixture',suites:{quick:['demo'],daily:['clear']},cases:[{id:'demo',replaySha256:identity.replaySha256},{id:'clear',replaySha256:identity.replaySha256}]};
  await acceptFileSuite({corpus,captureRoot:root,goldenRoot:golden});
  await assert.rejects(acceptFileSuite({corpus,captureRoot:root,goldenRoot:golden}),/replace/);
  const path=join(golden,'manifest.json'),manifest=JSON.parse(readFileSync(path));manifest.identity.executableSha256='4'.repeat(64);writeFileSync(path,JSON.stringify(manifest));
  await assert.rejects(verifyGoldenFiles(golden,{game:'fixture',suites:corpus.suites}),/identity mismatch/);
});
test('Two matching traces cannot pass under the wrong corpus Replay identity',async()=>{
  const root=mkdtempSync(join(tmpdir(),'oracle-corpus-'));fixture(join(root,'demo.original.jsonl'));fixture(join(root,'demo.candidate.jsonl'));
  const corpus={game:'fixture',suites:{quick:['demo']},cases:[{id:'demo',replaySha256:'9'.repeat(64)}]};
  const result=await compareFileSuite({corpus,captureRoot:root,lane:'quick'});assert.equal(result.status,'FAIL');
});
