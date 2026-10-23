import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtempSync,writeFileSync,rmSync} from 'node:fs';
import {join} from 'node:path';
import {tmpdir} from 'node:os';
import {readProductionPresentationEvidence} from './presentation-paired-report.mjs';
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
function fixture(t){
  const root=mkdtempSync(join(tmpdir(),'presentation-production-artifacts-'));t.after(()=>rmSync(root,{recursive:true,force:true}));
  const save=(name,value)=>{const path=join(root,name),bytes=Buffer.from(JSON.stringify(value));writeFileSync(path,bytes);return {path,bytes:bytes.length,sha256:hash(bytes)};};
  const build={packageSha256:hash('package'),wasmSha256:hash('wasm'),manifestSha256:hash('manifest')},data={bytes:12,sha256:hash('DATA')};
  const target={game:'th08',variant:'normal',packageIdentity:build,dataIdentity:data,checks:[{id:'production-replay-capture',status:'passed'}],
    inputPostflight:{packageIdentity:build,dataIdentity:data},errors:[],consoleErrors:[],httpErrors:[],protocolErrors:[],finalErrorAudit:{status:'passed'},
    presentationProduction:save('production.json',{schema:'raw-replay-fixture',samples:[1,2]}),
    presentationFirstFrames:[save('cold.json',{kind:'cold'}),save('reload.json',{kind:'reload'})]};
  const report={schema:'eagler-touhou/runtime-browser/1',finishedAt:new Date().toISOString(),targets:[target],httpFailures:[],
    harnessSha256:hash('driver'),harnessPostflightSha256:hash('driver')};
  return {root,target,report,save};
}
test('reopen hash-bound normal Replay and both compositor captures',t=>{
  const f=fixture(t),ref=f.save('report.json',f.report),loaded=readProductionPresentationEvidence(ref);
  assert.equal(loaded.run.samples.length,2);assert.equal(loaded.firstFrameRuns.length,2);assert.deepEqual(loaded.build,f.target.packageIdentity);
});
for(const [label,mutate] of [
  ['unfinished report',f=>{delete f.report.finishedAt;}],
  ['failed browser check',f=>{f.target.checks[0].status='failed';}],
  ['changed package',f=>{f.target.inputPostflight.packageIdentity={packageSha256:hash('other')};}],
  ['late browser error',f=>{f.target.consoleErrors.push('late audio error');}],
  ['changed driver',f=>{f.report.harnessPostflightSha256=hash('changed');}],
  ['missing raw Replay',f=>{delete f.target.presentationProduction;}]
])test(`reject ${label}`,t=>{const f=fixture(t);mutate(f);assert.throws(()=>readProductionPresentationEvidence(f.save('report.json',f.report)));});
test('changed raw file cannot inherit the browser report identity',t=>{
  const f=fixture(t),ref=f.save('report.json',f.report);writeFileSync(f.target.presentationProduction.path,'{}');
  assert.throws(()=>readProductionPresentationEvidence(ref),/bytes differ/);
});
test('changed parent report cannot inherit its saved identity',t=>{
  const f=fixture(t),ref=f.save('report.json',f.report);writeFileSync(ref.path,'{}');
  assert.throws(()=>readProductionPresentationEvidence(ref),/bytes differ/);
});
