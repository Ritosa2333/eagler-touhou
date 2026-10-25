import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {assessLauncherPracticeJourney} from '../../../tools/ports/launcher-practice-journey.mjs';
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
function readJson(receipt,expected){
  const bytes=readFileSync(receipt.path);assert.equal(bytes.length,receipt.bytes);assert.equal(hash(bytes),receipt.sha256);
  assert.equal(receipt.sha256,expected,'Practice feature input differs from the verified build/package identity');return JSON.parse(bytes);
}

export function assertPracticeBuildFeatureIdentity({identity,feature,evidence,product,products=PRODUCT_GAMES}){
  assert.equal(typeof feature,'boolean');
  const plan=readJson(evidence.plan,identity.runtime.sourceBuild.planReceiptSha256),manifest=readJson(evidence.manifest,identity.runtime.manifestSha256);
  assert.equal(plan.game,product.game);assert.equal(plan.variant,product.variant);assert.equal(manifest.game,product.game);
  assert.equal(plan.features.thprac,feature);assert.equal(plan.environment.TH_ENABLE_THPRAC,feature?'1':'0');
  assert.equal(manifest.features.thprac,feature,'Published practice feature disagrees with compiled build plan');
  if(feature)assert.equal(products[product.game].features.thprac,true,'Published package exceeds product practice support');
  return{feature,planSha256:evidence.plan.sha256,manifestSha256:evidence.manifest.sha256};
}

export function assertPracticeCapabilityEvidence({target,product}){
  assert.equal(target.game,product.game);assert.equal(target.variant,product.variant);assert.equal(target.status,'passed');
  assert.equal(target.finalErrorAudit.status,'passed');assert.deepEqual(target.postflight,target.identity);
  const built=assertPracticeBuildFeatureIdentity({identity:target.identity,feature:target.feature,evidence:target.buildFeatureEvidence,product});
  const ui=target.launcherPractice;assert.equal(assessLauncherPracticeJourney(ui).passed,true,'Actual Launcher feature/settings matrix did not complete');
  assert.deepEqual(ui.identity,target.identity);assert.deepEqual(ui.postflight,target.postflight);
  assert.ok(ui.scenes.every(scene=>scene.packageFeature===built.feature));
  return built;
}

export function assessPracticeCapabilityEvidence({driver,entry,product}={}){
  if(driver?.schema!=='eagler-touhou/practice-browser/1'||entry?.id!=='PRACTICE-01')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'compiled-published-host-practice-capability',checks:['PRACTICE-01']};
  try{
    const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);const target=targets[0];
    assert.ok(target.checks.includes(entry));assert.equal(target.checks.filter(row=>row.id===entry.id).length,1);assert.equal(entry.complete,true);assert.equal(entry.status,'passed');
    assert.deepEqual(entry.observation.observation.buildIdentity,assertPracticeCapabilityEvidence({target,product}));result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
