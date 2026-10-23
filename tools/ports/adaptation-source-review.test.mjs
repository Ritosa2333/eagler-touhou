import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {mkdtempSync,readFileSync,rmSync,writeFileSync} from 'node:fs';
import {mkdtemp,readFile,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join,resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import test from 'node:test';
import {inspectSourceReview} from './adaptation-source-review.mjs';

const hash=value=>createHash('sha256').update(value).digest('hex');
async function fixture(t){
  const root=await mkdtemp(join(tmpdir(),'adaptation-provenance-'));t.after(()=>rm(root,{recursive:true,force:true}));
  const put=async(path,content)=>{const bytes=Buffer.from(content);await writeFile(join(root,path),bytes);return{path,bytes:bytes.length,sha256:hash(bytes)};};
  const exe=await put('original.exe','independent original executable'),data=await put('original.dat','original game data');
  const trace=await put('demo.rows','accepted original trace'),font=await put('font.bin','external font');
  const notice=await put('NOTICE','authors and source license'),producer=await put('capture.mjs','independent original capture');
  const acceptance=await put('acceptance.txt','review of original provider, capture and provenance');
  const golden=await put('golden.json',JSON.stringify({game:'th08',policy:{candidateMayUpdateGolden:false},
    identity:{executableSha256:exe.sha256,resourceSha256:data.sha256},
    assets:{demo0:{...trace,comparedTicks:100,replaySha256:hash('input')}}}));
  const review={decision:'accepted',reviewer:'test reviewer',reviewedAt:'2026-10-21',evidence:[acceptance]};
  const product={source:{upstream:'https://example.test/original',revision:'a'.repeat(40),author:'Original author',license:'Declared source terms',notice,
    externalInputs:[{...data,role:'game-data',origin:'original installation',distribution:'external-private'},
      {...font,role:'font',origin:'font distribution',distribution:'external'}],review},
    reference:{candidateMayUpdateGolden:false,maintenanceWritesAcceptedReference:false,provider:'independent-original',
      captureMethod:'Run original executable into staging',acceptanceMethod:'Separate reviewed acceptance',
      producer,originalExecutable:exe,originalData:data,acceptedManifest:golden,review}};
  const manifest=join(root,'review.json');
  const save=()=>writeFile(manifest,JSON.stringify({schema:'eagler-touhou/adaptation-source-review/1',products:{th08:{normal:product}}}));
  await save();return{root,product,manifest,save};
}

test('review inspection binds source, assets and accepted independent original traces without recapturing',async t=>{
  const f=await fixture(t),before=await readFile(f.manifest),r=await inspectSourceReview({manifest:f.manifest,game:'th08',variant:'normal'});
  assert.deepEqual(r.checks.map(c=>[c.id,c.complete]),[['BUILD-06',true],['VERIFY-05',true]]);
  assert.equal(r.checks[1].evidence.observation.recapturedThisRun,false);assert.ok(r.inputs.files.length>=8);
  assert.deepEqual(await readFile(f.manifest),before);
});
test('a new or changed source notice cannot reuse an old source review',async t=>{
  const f=await fixture(t);await writeFile(join(f.root,'NOTICE'),'changed terms');
  await assert.rejects(inspectSourceReview({manifest:f.manifest,game:'th08',variant:'normal',checks:['BUILD-06']}),/changed since review/);
});
test('source review requires both external game DATA and font provenance',async t=>{
  const f=await fixture(t);f.product.source.externalInputs.pop();await f.save();
  await assert.rejects(inspectSourceReview({manifest:f.manifest,game:'th08',variant:'normal',checks:['BUILD-06']}),/game-data and font/);
});
test('a pending review cannot certify source or original-reference acceptance',async t=>{
  const f=await fixture(t);f.product.reference.review.decision='pending';await f.save();
  await assert.rejects(inspectSourceReview({manifest:f.manifest,game:'th08',variant:'normal',checks:['VERIFY-05']}),/explicit accepted review/);
});
test('candidate replacement and maintenance writes to the accepted reference are rejected',async t=>{
  const f=await fixture(t);f.product.reference.candidateMayUpdateGolden=true;await f.save();
  await assert.rejects(inspectSourceReview({manifest:f.manifest,game:'th08',variant:'normal',checks:['VERIFY-05']}),/must not accept/);
  f.product.reference.candidateMayUpdateGolden=false;f.product.reference.maintenanceWritesAcceptedReference=true;await f.save();
  await assert.rejects(inspectSourceReview({manifest:f.manifest,game:'th08',variant:'normal',checks:['VERIFY-05']}),/staging separately/);
});
test('changed original executable and changed accepted trace both invalidate maintenance evidence',async t=>{
  const f=await fixture(t);await writeFile(join(f.root,'original.exe'),'candidate executable');
  await assert.rejects(inspectSourceReview({manifest:f.manifest,game:'th08',variant:'normal',checks:['VERIFY-05']}),/changed since review/);
  await writeFile(join(f.root,'original.exe'),'independent original executable');await writeFile(join(f.root,'demo.rows'),'candidate output');
  await assert.rejects(inspectSourceReview({manifest:f.manifest,game:'th08',variant:'normal',checks:['VERIFY-05']}),/changed since review/);
});
test('selected product and variant cannot borrow another reviewed record',async t=>{
  const f=await fixture(t);await assert.rejects(inspectSourceReview({manifest:f.manifest,game:'th08',variant:'multiplayer'}),/no th08\/multiplayer/);
});

test('public inspection rejects explicit blockers for the selected checks with their reasons',async t=>{
  const f=await fixture(t);
  await writeFile(f.manifest,JSON.stringify({schema:'eagler-touhou/adaptation-source-review/1',products:{th08:{normal:{
    status:'blocked',blockers:[
      {check:'BUILD-06',reason:'Source and font origins need independent review.'},
      {check:'VERIFY-05',reason:'The original executable matching the golden hash is unavailable.'},
    ]}}}}));
  await assert.rejects(inspectSourceReview({manifest:f.manifest,game:'th08',variant:'normal',checks:['VERIFY-05']}),error=>{
    assert.match(error.message,/Source review blocked: VERIFY-05:/);
    assert.match(error.message,/The original executable matching the golden hash is unavailable\./);
    assert.doesNotMatch(error.message,/BUILD-06|Source and font origins/);
    return true;
  });
});

test('CLI preserves explicit pending human review as a blocked report',t=>{
  const root=mkdtempSync(join(tmpdir(),'source-review-blocked-'));t.after(()=>rmSync(root,{recursive:true,force:true}));
  const manifest=join(root,'review.json'),output=join(root,'output');
  writeFileSync(manifest,JSON.stringify({schema:'eagler-touhou/adaptation-source-review/1',products:{th08:{normal:{
    status:'blocked',blockers:[
      {check:'BUILD-06',reason:'Source license and external font/data origins need independent review.'},
      {check:'VERIFY-05',reason:'The original executable matching the golden hash is not available for inspection.'},
    ]}}}}));
  const cli=fileURLToPath(new URL('./adaptation-source-review.mjs',import.meta.url));
  const child=spawnSync(process.execPath,[cli,`--manifest=${manifest}`,'--game=th08','--variant=normal',`--output=${output}`],
    {encoding:'utf8',timeout:10000});
  assert.equal(child.error,undefined,child.error?.message);assert.equal(child.signal,null,child.stderr);assert.equal(child.status,1,child.stderr);
  const report=JSON.parse(readFileSync(join(output,'report.json'),'utf8'));
  assert.equal(report.status,'blocked');assert.equal(report.passed,false);assert.equal(report.blocked,true);
  assert.deepEqual(report.checks.map(check=>[check.id,check.status,check.complete]),
    [['BUILD-06','blocked',false],['VERIFY-05','blocked',false]]);
  assert.deepEqual(report.blockers.map(blocker=>blocker.check),['BUILD-06','VERIFY-05']);
  assert.equal(report.inputs.manifest.path,resolve(manifest));
});
