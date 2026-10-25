import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {readSceneReviewArtifact} from './scene-review.mjs';
import {assessViewOptionsEvidence} from './launcher-lifecycle-evidence.mjs';

/** Reopen the original machine proof before preparing its three native views. */
export function viewOptionsReviewMaterials({sourceReport,product}){
  const source=JSON.parse(readSceneReviewArtifact(sourceReport));
  assert.equal(source.schema,'eagler-touhou/offline-runtime-browser/1');assert.equal(source.status,'passed');
  assert.equal(source.scope.game,product.game);assert.equal(source.scope.variant,product.variant);
  assert.ok(source.finishedAt&&Date.parse(source.finishedAt)>=Date.parse(source.startedAt));
  const entries=source.checks.filter(row=>row.id==='RUNTIME-06');assert.equal(entries.length,1);
  const proof=assessViewOptionsEvidence({driver:source,entry:entries[0],product});assert.equal(proof.valid,true,proof.reason);
  const materials=source.viewOptions.observation.screenshots.map(({name,path,bytes,sha256})=>({name,path,bytes,sha256}));
  return{sourceRun:{startedAt:source.startedAt,finishedAt:source.finishedAt,runtime:source.runtimePackages.normal,data:source.runtimePackages.data},materials};
}

export function assertViewOptionsReviewApproval({human,materials,product,finishedAt}){
  assert.equal(human.schema,'eagler-touhou/scene-review/1');assert.equal(human.game,product.game);assert.equal(human.variant,product.variant);
  assert.ok(typeof human.reviewer==='string'&&human.reviewer.trim());
  assert.ok(Number.isFinite(Date.parse(human.recordedAt))&&Date.parse(human.recordedAt)>=Date.parse(finishedAt));
  const groups=human.groups.filter(row=>row.id==='player-marker');assert.equal(groups.length,1);
  const group=groups[0];assert.equal(group.verdict,'passed');
  assert.deepEqual(materials.map(row=>row.name),['disabled','focused','enabled']);
  assert.equal(new Set(group.materials.map(row=>row.name)).size,3);assert.equal(group.materials.length,3);
  for(const expected of materials){
    const item=group.materials.find(row=>row.name===expected.name);assert.ok(item);
    assert.equal(resolve(item.path),resolve(expected.path));assert.equal(item.bytes,expected.bytes);assert.equal(item.sha256,expected.sha256);
    const image=readSceneReviewArtifact(item);assert.equal(image.subarray(0,8).toString('hex'),'89504e470d0a1a0a');
    assert.equal(image.subarray(12,16).toString(),'IHDR');assert.ok(image.readUInt32BE(16)>0&&image.readUInt32BE(20)>0);
  }
  return{reviewer:human.reviewer,reviewedAt:human.recordedAt,group:'player-marker',materials};
}

export function assertViewOptionsReview({sourceReport,review,product}){
  const prepared=viewOptionsReviewMaterials({sourceReport,product}),human=JSON.parse(readSceneReviewArtifact(review));
  return{sourceRun:prepared.sourceRun,...assertViewOptionsReviewApproval({human,materials:prepared.materials,product,finishedAt:prepared.sourceRun.finishedAt})};
}

export function assessViewOptionsReviewEvidence({driver,entry,product}={}){
  if(driver?.schema!=='eagler-touhou/view-options-review/1'||entry?.id!=='RUNTIME-06')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'reviewed-native-player-marker',checks:['RUNTIME-06']};
  try{
    assert.equal(driver.status,'passed');assert.deepEqual(driver.scope,product);assert.equal(driver.newBrowserRun,false);
    assert.equal(entry.status,'passed');assert.equal(entry.complete,true);
    assert.ok(driver.checks.includes(entry));assert.equal(driver.checks.filter(row=>row.id===entry.id).length,1);
    assert.deepEqual(entry.observation,assertViewOptionsReview({...driver.evidence,product}));result.valid=true;
  }catch(error){result.reason=error.message;}return result;
}
