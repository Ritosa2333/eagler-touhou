import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {mkdtempSync,readFileSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join,resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import test from 'node:test';
import {assessMinimalRuntimeJourney} from './minimal-runtime-browser.mjs';

function evidence(){
  const scope={game:'th08',variant:'normal'},base={scope,startedAt:'2026-10-20T23:59:59Z',finishedAt:'2026-10-21T00:00:00Z'},sha='a'.repeat(64);
  return{...scope,checkout:{...base,schema:'eagler-touhou/build-acceptance/1',runId:'checkout-run',status:'incomplete',passed:false,
    stage:'checkout',observations:{checkout:{status:'passed',
    targetSourceRoot:'ports/th08',unexpectedOtherProductOwners:[],absentOtherProductOwners:['ports/th10']}}},
    build:{...base,schema:'eagler-touhou/build-acceptance/1',runId:'build-run',status:'passed',passed:true,
      stage:'verify-existing',checks:['BUILD-01','BUILD-02','BUILD-03','BUILD-04'].map(id=>({id,status:'passed',complete:true,
      evidence:{observation:{package:{current:{package:{packageSha256:sha}}}}}}))},
    offline:{...base,schema:'eagler-touhou/offline-runtime-browser/1',runId:'offline-run',status:'passed',passed:true,
      checks:[{id:'RUNTIME-05',status:'passed',complete:true}],runtimePackages:{normal:{packageSha256:sha}},
      onlineRuntime:{observation:{first:{event:'first-frame',epoch:1},dataRead:{bytes:10,sha256:'b'.repeat(64)}}}}};
}
const complete=()=>({available:true,valid:true,reason:null});

test('single-game acceptance retains the nested journey and complete product envelope',()=>{
  const result=assessMinimalRuntimeJourney({...evidence(),assessEvidence:complete});
  assert.equal(result.id,'BUILD-05');assert.equal(result.complete,true);
  assert.deepEqual(result.evidence.adaptationIds,['BUILD-05']);
  assert.deepEqual(Object.keys(result.evidence.observation.journey),['checkout','build','offline']);
});
test('a full checkout, unsealed report or other product cannot supply single-game evidence',()=>{
  const e=evidence();e.checkout.observations.checkout.unexpectedOtherProductOwners=['ports/th10'];
  assert.throws(()=>assessMinimalRuntimeJourney({...e,assessEvidence:complete}),/Another game/);
  const unsealed=evidence();delete unsealed.offline.finishedAt;
  assert.throws(()=>assessMinimalRuntimeJourney({...unsealed,assessEvidence:complete}),/not sealed/);
  const wrong=evidence();wrong.offline.scope={game:'th10',variant:'normal'};
  assert.throws(()=>assessMinimalRuntimeJourney({...wrong,assessEvidence:complete}),/another product/);
});
test('an old package or missing managed DATA cannot satisfy the same-run minimal journey',()=>{
  const e=evidence();e.offline.runtimePackages.normal.packageSha256='c'.repeat(64);
  assert.throws(()=>assessMinimalRuntimeJourney({...e,assessEvidence:complete}),/minimal checkout package/);
  const noData=evidence();delete noData.offline.onlineRuntime.observation.dataRead;
  assert.throws(()=>assessMinimalRuntimeJourney({...noData,assessEvidence:complete}),/installed DATA/);
});
test('a smoke or failed source build cannot be promoted to a complete BUILD-05',()=>{
  const e=evidence();e.offline.checks[0].complete=false;
  assert.throws(()=>assessMinimalRuntimeJourney({...e,assessEvidence:complete}));
  const bad=evidence();bad.build.status='failed';
  assert.throws(()=>assessMinimalRuntimeJourney({...bad,assessEvidence:complete}),/did not pass/);
  const duplicate=evidence();duplicate.build.checks.push(duplicate.build.checks[0]);
  assert.throws(()=>assessMinimalRuntimeJourney({...duplicate,assessEvidence:complete}),/duplicate/);
});
test('a missing assessor or rejected nested proof keeps BUILD-05 incomplete',()=>{
  assert.throws(()=>assessMinimalRuntimeJourney(evidence()),/assessor is required/);
  const reject=()=>({available:true,valid:false,reason:'fixture missing'});
  assert.throws(()=>assessMinimalRuntimeJourney({...evidence(),assessEvidence:reject}),/failed recomputation/);
});

test('CLI imports the complete assessor through the module cycle without a top-level-await hang',t=>{
  const output=mkdtempSync(join(tmpdir(),'minimal-runtime-import-cycle-'));
  t.after(()=>rmSync(output,{recursive:true,force:true}));
  const cli=fileURLToPath(new URL('./minimal-runtime-browser.mjs',import.meta.url));
  const child=spawnSync(process.execPath,[cli,`--output=${output}`,'--game=th08','--variant=normal'],
    {encoding:'utf8',timeout:10000});
  assert.equal(child.error,undefined,child.error?.message);
  assert.equal(child.signal,null,child.stderr);
  assert.equal(child.status,1,child.stderr);
  const report=JSON.parse(readFileSync(resolve(output,'report.json'),'utf8'));
  assert.equal(report.schema,'eagler-touhou/minimal-runtime-browser/1');
  assert.equal(report.status,'blocked');
  assert.match(report.reason,/checkout input manifest/i);
});
