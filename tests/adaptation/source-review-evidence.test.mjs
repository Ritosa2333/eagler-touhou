import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtemp,readFile,rm,writeFile} from 'node:fs/promises';
import {join} from 'node:path';
import {tmpdir} from 'node:os';
import test from 'node:test';
import {inspectSourceReviewNow} from '../../tools/ports/adaptation-source-review.mjs';
import {assessCompleteEvidence} from './complete-evidence.mjs';
import {assessDriverReport} from './results.mjs';

const hash=value=>createHash('sha256').update(value).digest('hex');
const product={game:'th08',variant:'normal'};

async function fixture(t,{wrongGoldenIdentity=false}={}){
  const root=await mkdtemp(join(tmpdir(),'source-review-evidence-'));t.after(()=>rm(root,{recursive:true,force:true}));
  const put=async(path,content)=>{
    const bytes=Buffer.from(content);await writeFile(join(root,path),bytes);
    return{path,bytes:bytes.length,sha256:hash(bytes)};
  };
  const originalExecutable=await put('original.exe','independent original executable');
  const originalData=await put('original.dat','original game DATA');
  const font=await put('font.bin','approved font bytes');
  const notice=await put('NOTICE','source attribution and license');
  const producer=await put('capture.mjs','independent original capture producer');
  const trace=await put('accepted.rows','original trace bytes');
  const sourceReviewEvidence=await put('source-review.txt','independent source and material review');
  const referenceReviewEvidence=await put('reference-review.txt','independent original reference review');
  const golden=await put('golden.json',JSON.stringify({game:'th08',policy:{candidateMayUpdateGolden:false},
    identity:{executableSha256:originalExecutable.sha256,
      resourceSha256:wrongGoldenIdentity?'f'.repeat(64):originalData.sha256},
    assets:{demo0:{...trace,comparedTicks:91,replaySha256:hash('original replay identity')}}}));
  const source={upstream:'https://example.test/original',revision:'a'.repeat(40),author:'Original author',
    license:'Declared source terms',notice,
    externalInputs:[{...originalData,role:'game-data',origin:'original installation',distribution:'external private data'},
      {...font,role:'font',origin:'approved font distribution',distribution:'external font'}],
    review:{decision:'accepted',reviewer:'source reviewer',reviewedAt:'2026-10-21',evidence:[sourceReviewEvidence]}};
  const reference={candidateMayUpdateGolden:false,maintenanceWritesAcceptedReference:false,
    provider:'independent-original',captureMethod:'Run the original executable into staging',
    acceptanceMethod:'Separate review and acceptance',producer,originalExecutable,originalData,
    acceptedManifest:golden,review:{decision:'accepted',reviewer:'reference reviewer',
      reviewedAt:'2026-10-21',evidence:[referenceReviewEvidence]}};
  const manifest=join(root,'source-review.json');
  const save=()=>writeFile(manifest,JSON.stringify({schema:'eagler-touhou/adaptation-source-review/1',
    products:{th08:{normal:{source,reference}}}}));
  await save();return{root,manifest,source,reference,save};
}

function report(result){
  return{schema:'eagler-touhou/adaptation-source-review-result/1',runId:'review-run',
    startedAt:'2026-10-21T00:00:00.000Z',finishedAt:'2026-10-21T00:00:01.000Z',
    status:'passed',passed:true,scope:product,inputs:result.inputs,checks:result.checks};
}

test('BUILD-06 and VERIFY-05 recompute accepted records and bind every reviewed file',async t=>{
  const f=await fixture(t),value=report(inspectSourceReviewNow({manifest:f.manifest,...product}));
  for(const entry of value.checks){
    const result=assessCompleteEvidence({driver:value,entry,product});
    assert.deepEqual(result,{available:true,valid:true,reason:null,
      kind:entry.id==='BUILD-06'?'reviewed-source-provenance':'accepted-original-reference'});
    assert.equal(entry.evidence.observation.inputIdentity,value.inputs);
  }
  assert.equal(value.checks[0].evidence.observation.review.reviewer,'source reviewer');
  assert.equal(value.checks[1].evidence.observation.review.reviewer,'reference reviewer');
  const normalized=assessDriverReport({game:product.game,variant:product.variant,group:'source-review',
    checksMapped:['BUILD-06','VERIFY-05'],scenarioIds:['reviewed-source-and-original-reference']},
  {code:0,status:'passed',startedAt:'2026-10-20T23:59:59.000Z'},value);
  assert.equal(normalized.status,'passed');assert.ok(normalized.checks.every(check=>check.complete));
});

test('complete flags without review envelopes, accepted decisions or real source files fail closed',async t=>{
  const f=await fixture(t),value=report(inspectSourceReviewNow({manifest:f.manifest,...product}));
  const placeholder={id:'BUILD-06',status:'passed',complete:true,
    evidence:{game:product.game,variant:product.variant,adaptationIds:['BUILD-06'],observation:{note:'checked'}}};
  const emptyDriver={...value,checks:[placeholder]};
  assert.equal(assessCompleteEvidence({driver:emptyDriver,entry:placeholder,product}).valid,false);

  const accepted=value.checks.find(item=>item.id==='BUILD-06');
  accepted.evidence.observation.inputIdentity.files.pop();
  value.inputs.files.pop();
  const omitted=assessCompleteEvidence({driver:value,entry:accepted,product});
  assert.equal(omitted.available,true);assert.equal(omitted.valid,false);

  const changed=await fixture(t),changedReport=report(inspectSourceReviewNow({manifest:changed.manifest,...product}));
  await writeFile(join(changed.root,'NOTICE'),'changed source attribution');
  const stale=assessCompleteEvidence({driver:changedReport,
    entry:changedReport.checks.find(item=>item.id==='BUILD-06'),product});
  assert.equal(stale.available,true);assert.equal(stale.valid,false);
});

test('accepted golden data must match the identified original executable and DATA',async t=>{
  const f=await fixture(t),value=report(inspectSourceReviewNow({manifest:f.manifest,...product,checks:['VERIFY-05']}));
  const goldenPath=join(f.root,f.reference.acceptedManifest.path);
  const golden=JSON.parse(await readFile(goldenPath,'utf8'));
  golden.identity.resourceSha256='f'.repeat(64);
  const goldenBytes=Buffer.from(JSON.stringify(golden));await writeFile(goldenPath,goldenBytes);
  f.reference.acceptedManifest={...f.reference.acceptedManifest,bytes:goldenBytes.length,sha256:hash(goldenBytes)};
  await f.save();
  const result=assessCompleteEvidence({driver:value,entry:value.checks[0],product});
  assert.equal(result.available,true);assert.equal(result.valid,false);
  assert.match(result.reason,/Golden has another original DATA/);
});

test('a changed accepted/reviewed decision cannot reuse its earlier completion',async t=>{
  const f=await fixture(t),value=report(inspectSourceReviewNow({manifest:f.manifest,...product,checks:['VERIFY-05']}));
  f.reference.maintenanceWritesAcceptedReference=true;await f.save();
  const result=assessCompleteEvidence({driver:value,entry:value.checks[0],product});
  assert.equal(result.available,true);assert.equal(result.valid,false);
  assert.match(result.reason,/staging separately/);
});
