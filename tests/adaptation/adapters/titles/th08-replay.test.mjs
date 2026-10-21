import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join,resolve} from 'node:path';
import {assessTh08ReplayCapture,createTh08ReplaySuite,resolveTh08CaptureRowsPath} from './th08-replay.mjs';
import {runReplaySuite} from '../../../../common/testkit/replay-verifier/suite.mjs';

test('TH08 capture eligibility requires hash-bound diagnostic identity and natural title return',()=>{
  const hash='a'.repeat(64),build={schema:'th08/presentation-lab-build/1',commit:'b'.repeat(40),instrumented:true,
    wasm:'c'.repeat(64),loader:'d'.repeat(64),sourceDigest:'e'.repeat(64),toolDigest:'f'.repeat(64),toolSources:{'adapter.mjs':'1'.repeat(64)}};
  const valid={schema:'th08/current-replay-capture/v2',complete:true,stopReason:'returned-to-title',terminalScene:1,
    replay:{sha256:hash,bytes:123},errors:[],build};
  assert.equal(assessTh08ReplayCapture(valid,{lane:'quick',id:'demo0',expectedSha256:hash,expectedBytes:123}),null);
  for(const changed of [
    {...valid,schema:'th08/current-replay-capture/v1'},
    {...valid,complete:false,stopReason:'maximum-ticks'},
    {...valid,stopReason:'driver-not-advanced'},
    {...valid,terminalScene:2},
    {...valid,build:{...build,instrumented:false}},
    {...valid,replay:{...valid.replay,sha256:'9'.repeat(64)}},
    {...valid,errors:['runtime exception']},
  ])assert.notEqual(assessTh08ReplayCapture(changed,{lane:'quick',id:'demo0',expectedSha256:hash,expectedBytes:123}),null);
});

test('daily candidate row artifacts cannot escape their capture directory',async()=>{
  const root=await mkdtemp(join(tmpdir(),'th08-replay-capture-'));
  try{
    const metadata=join(root,'lunatic.json');
    assert.equal(resolveTh08CaptureRowsPath(root,metadata,'lunatic.rows.jsonl'),resolve(root,'lunatic.rows.jsonl'));
    assert.throws(()=>resolveTh08CaptureRowsPath(root,metadata,'../outside.rows.jsonl'),/escapes/);
  }finally{await rm(root,{recursive:true,force:true});}
});

test('TH08 daily lane keeps private-input absences explicit and natural terminal differences fail',async()=>{
  const root=await mkdtemp(join(tmpdir(),'th08-replay-lane-'));
  try{
    const suite=await createTh08ReplaySuite({lane:'daily',captureRoot:root,
      replayFiles:{lunatic:join(root,'missing-lunatic.rpy'),extra:join(root,'missing-extra.rpy')}});
    assert.deepEqual(suite.expectedIds,['lunatic','extra']);
    assert.deepEqual(suite.cases.map(item=>item.status),['missing-replay','missing-replay']);
    const result=await runReplaySuite(suite);
    assert.equal(result.status,'INCOMPLETE');assert.deepEqual(result.results.map(item=>item.id),['lunatic','extra']);
    const divergence=await suite.adapter.validateCandidate({fixture:{capture:{stages:[8],finalScore:1,terminalMatch:false},
      goldenAsset:{stages:[8],finalScore:2}}});
    assert.equal(divergence.status,'DIVERGED');assert.equal(divergence.reason,'natural-terminal-state');
    const inconsistent=await suite.adapter.validateCandidate({fixture:{capture:{stages:[8],finalScore:2,terminalMatch:false},
      goldenAsset:{stages:[8],finalScore:2}}});
    assert.equal(inconsistent.status,'INCOMPATIBLE');
  }finally{await rm(root,{recursive:true,force:true});}
});
