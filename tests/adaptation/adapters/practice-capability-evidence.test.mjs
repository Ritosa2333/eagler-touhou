import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,writeFileSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {createHash} from 'node:crypto';
import {assertPracticeBuildFeatureIdentity,assertPracticeCapabilityEvidence} from './practice-capability-evidence.mjs';
const product={game:'th08',variant:'normal'},hash=bytes=>createHash('sha256').update(bytes).digest('hex');
function fixture(t,feature=true){
  const directory=mkdtempSync(join(tmpdir(),'practice-build-feature-'));t.after(()=>rmSync(directory,{recursive:true,force:true}));
  const plan={game:'th08',variant:'normal',features:{thprac:feature},environment:{TH_ENABLE_THPRAC:feature?'1':'0'}},
    manifest={game:'th08',features:{thprac:feature}},evidence={};
  for(const [key,value] of Object.entries({plan,manifest})){
    const bytes=Buffer.from(JSON.stringify(value)),path=join(directory,key+'.json');writeFileSync(path,bytes);
    evidence[key]={path,bytes:bytes.length,sha256:hash(bytes)};
  }
  return{identity:{runtime:{sourceBuild:{planReceiptSha256:evidence.plan.sha256},manifestSha256:evidence.manifest.sha256}},feature,evidence,product,plan,manifest};
}
test('current practice build identity accepts enabled and legal disabled packages',t=>{
  for(const feature of [true,false])assert.equal(assertPracticeBuildFeatureIdentity(fixture(t,feature)).feature,feature);
});
for(const [name,mutate] of Object.entries({
  'different compiled plan':a=>a.identity.runtime.sourceBuild.planReceiptSha256='a'.repeat(64),
  'different packaged manifest':a=>a.identity.runtime.manifestSha256='a'.repeat(64),
  'changed plan bytes':a=>writeFileSync(a.evidence.plan.path,'{}'),
  'declared feature disagrees':a=>a.feature=false,
  'wrong mode':a=>a.product={game:'th08',variant:'multiplayer'},
  'wrong game':a=>a.product={game:'th10',variant:'normal'},
  'product capability exceeded':a=>a.products={th08:{features:{thprac:false}}},
}))test(`practice build identity rejects ${name}`,t=>{const args=fixture(t);mutate(args);assert.throws(()=>assertPracticeBuildFeatureIdentity(args));});
test('valid build metadata cannot substitute for the real Host/native UI matrix',t=>{
  const args=fixture(t),target={...product,status:'passed',feature:true,identity:args.identity,postflight:args.identity,
    buildFeatureEvidence:args.evidence,finalErrorAudit:{status:'passed'},launcherPractice:{status:'observed',passed:true,scenes:[]}};
  assert.throws(()=>assertPracticeCapabilityEvidence({target,product}));
});
