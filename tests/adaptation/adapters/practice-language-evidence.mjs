import assert from 'node:assert/strict';
import {basename,resolve,dirname} from 'node:path';
import {resolvePracticeAdapter} from './registry.mjs';
import {readSceneReviewArtifact as readPracticeReviewArtifact} from './scene-review.mjs';
export {readPracticeReviewArtifact};

function clearAudit(audit){
  assert.equal(audit?.status,'passed');
  assert.deepEqual(Object.keys(audit.channels).sort(),['pageErrors','consoleErrors','httpErrors','protocolErrors','responseErrors','teardownErrors'].sort());
  for(const values of Object.values(audit.channels))assert.deepEqual(values,[],'Raw practice error channels are not clear');
}
/** Combine one human scene-set conclusion with the exact native locale run and reviewed pixels. */
export function assertPracticeLanguageReview({sourceReport,review,product,adapter=resolvePracticeAdapter(product?.game,product?.variant)}={}){
  const contract=adapter?.localeReview;assert.ok(contract,'Declare the practice locale/font review contract');
  const source=JSON.parse(readPracticeReviewArtifact(sourceReport)),human=JSON.parse(readPracticeReviewArtifact(review));
  assert.equal(source.schema,'eagler-touhou/practice-browser/1');assert.equal(source.status,'passed');
  assert.ok(source.finishedAt&&Date.parse(source.finishedAt)>=Date.parse(source.startedAt));
  clearAudit(source.finalErrorAudit);assert.deepEqual(source.sharedInputsPostflight,source.sharedInputs);
  assert.deepEqual(source.teardownErrors,[]);assert.deepEqual(source.httpFailures,[]);
  assert.ok(['eagler-touhou/scene-review/1','eagler/local-user-review-confirmation/1'].includes(human.schema));
  assert.equal(human.game,product.game);assert.equal(human.variant,product.variant);
  assert.ok(typeof human.reviewer==='string'&&human.reviewer.trim());assert.ok(Number.isFinite(Date.parse(human.recordedAt)));
  assert.ok(Date.parse(human.recordedAt)>=Date.parse(source.finishedAt),'The scene set was approved before its capture finished');
  const groups=human.groups.filter(group=>group.id==='practice-text');assert.equal(groups.length,1);
  const group=groups[0];assert.equal(group.verdict,'passed');
  assert.deepEqual(group.materials.map(item=>item.locale).sort(),[...contract.locales].sort(),'Review must cover exactly the declared locale set');
  const targets=source.targets.filter(target=>target.game===product.game&&target.variant===product.variant);assert.equal(targets.length,1);
  const target=targets[0];assert.equal(target.status,'passed');clearAudit(target.finalErrorAudit);
  assert.deepEqual(target.identity,target.postflight);assert.equal(target.feature,true);
  const font=source.sharedInputs[contract.font];assert.ok(font,'Practice font is absent from actual inputs');readPracticeReviewArtifact(font);
  const observations=[];
  for(const locale of contract.locales){
    const rows=target.locales.filter(row=>row.enabled===true&&row.locale===locale);assert.equal(rows.length,1);const row=rows[0];
    assert.equal(row.status,'passed');assert.equal(row.before.open,false);assert.equal(row.opened.open,true);assert.equal(row.closed.open,false);
    assert.deepEqual(row.opened.options,{thpracEnabled:true,thpracLocale:locale});
    assert.ok(row.opened.events.some(event=>event.open===true));assert.ok(row.closed.events.slice(row.opened.events.length).some(event=>event.open===false));
    assert.equal(row.gameplay.game,product.game);assert.equal(row.gameplay.phase.name,'stage');
    // The C++ menu-open publication is recorded for every locale. The full
    // parameter snapshot is an additional observation in the parameter case.
    if(row.practiceSnapshot)assert.equal(row.practiceSnapshot.state.enabled,true);
    const responses=row.responseIntegrity.responses.filter(item=>new URL(item.url).pathname===contract.font);
    assert.ok(responses.length>0,'No actual practice-font response');
    for(const response of responses){assert.equal(response.status,200);assert.equal(response.contentIdentity,'passed');
      assert.deepEqual(response.actual,{bytes:font.bytes,sha256:font.sha256});}
    const material=group.materials.find(item=>item.locale===locale);
    assert.equal(basename(material.path),row.screenshot);assert.equal(resolve(material.path),resolve(dirname(sourceReport.path),row.screenshot),
      'Reviewed screenshot belongs to another execution');
    const image=readPracticeReviewArtifact(material);assert.equal(image.subarray(0,8).toString('hex'),'89504e470d0a1a0a');
    assert.ok(image.readUInt32BE(16)>0&&image.readUInt32BE(20)>0);
    observations.push({locale,epoch:row.epoch,image:{...material},font:{bytes:font.bytes,sha256:font.sha256}});
  }
  return{sourceRun:{startedAt:source.startedAt,finishedAt:source.finishedAt,identity:target.identity},
    reviewer:human.reviewer,reviewedAt:human.recordedAt,observations};
}

export function assessPracticeLanguageReviewEvidence({driver,entry,product}={}){
  if(driver?.schema!=='eagler-touhou/practice-language-review/1'||entry?.id!=='PRACTICE-07')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'reviewed-native-practice-locales',checks:['PRACTICE-07']};
  try{
    assert.deepEqual(driver.scope,product);assert.equal(entry.status,'passed');assert.equal(entry.complete,true);
    assert.ok(driver.checks.includes(entry)&&driver.checks.filter(row=>row.id===entry.id).length===1);
    assert.deepEqual(entry.observation,assertPracticeLanguageReview({...driver.evidence,product}));result.valid=true;
  }catch(error){result.reason=error.message;}return result;
}
