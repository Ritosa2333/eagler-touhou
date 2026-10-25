import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {readSceneReviewArtifact} from './scene-review.mjs';
import {assertLanguageEvidence} from './language-evidence.mjs';

/** Machine proof and six bound native views form one human-readable scene set. */
export function languageReviewMaterials({sourceReport,product}){
  const source=JSON.parse(readSceneReviewArtifact(sourceReport));
  assert.equal(source.schema,'eagler-touhou/language-browser/1');assert.equal(source.status,'passed');
  assert.ok(source.finishedAt&&Date.parse(source.finishedAt)>=Date.parse(source.startedAt));
  assert.deepEqual(source.scope.game,product.game);assert.deepEqual(source.scope.variant,product.variant);
  const targets=source.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
  const claims=targets[0].checks.filter(row=>row.id==='LANG-05');assert.equal(claims.length,1);
  assert.equal(claims[0].machineComplete,true);assertLanguageEvidence(source,claims[0],product);
  const scenes=['japanese-offline',...Object.keys(source.inputs.languages.packs).map(id=>'translated-'+id)];
  const materials=scenes.flatMap(id=>{
    const matches=targets[0].scenes.filter(scene=>scene.id===id);assert.equal(matches.length,1);const scene=matches[0];
    return [['statistics',scene.consumers.statistics.image],['dialogue',scene.consumers.dialogueCapture.image]]
      .map(([kind,image])=>({locale:scene.language,scene:kind,path:image.path,bytes:image.bytes,sha256:image.sha256}));
  });
  return{sourceReport,sourceRun:{startedAt:source.startedAt,finishedAt:source.finishedAt,source:source.sourceSnapshotStart,
    data:source.inputs.data,runtime:source.site.runtimeGroup},materials};
}

/** One approval covers exactly the supplied scene set; no per-character conclusions. */
export function assertLanguageReviewApproval({human,product,materials,finishedAt}){
  assert.equal(human.schema,'eagler-touhou/scene-review/1');assert.equal(human.game,product.game);assert.equal(human.variant,product.variant);
  assert.ok(typeof human.reviewer==='string'&&human.reviewer.trim());assert.ok(Number.isFinite(Date.parse(human.recordedAt)));
  assert.ok(Date.parse(human.recordedAt)>=Date.parse(finishedAt),'Review precedes the actual scene capture');
  const groups=human.groups.filter(group=>group.id==='text-presentation');assert.equal(groups.length,1);const group=groups[0];
  assert.equal(group.verdict,'passed');
  const key=material=>material.locale+'|'+material.scene;
  assert.equal(new Set(materials.map(key)).size,materials.length);
  assert.deepEqual(group.materials.map(key).sort(),materials.map(key).sort(),'Review must cover exactly the prepared language scenes');
  for(const expected of materials){
    const material=group.materials.find(row=>key(row)===key(expected));
    assert.equal(resolve(material.path),resolve(expected.path));assert.equal(material.bytes,expected.bytes);assert.equal(material.sha256,expected.sha256);
    const bytes=readSceneReviewArtifact(material);assert.equal(bytes.subarray(0,8).toString('hex'),'89504e470d0a1a0a');
    assert.ok(bytes.readUInt32BE(16)>0&&bytes.readUInt32BE(20)>0);
  }
  return{reviewer:human.reviewer,reviewedAt:human.recordedAt,group:'text-presentation',materials};
}

export function assertLanguageReview({sourceReport,review,product}){
  const prepared=languageReviewMaterials({sourceReport,product}),human=JSON.parse(readSceneReviewArtifact(review));
  return{sourceRun:prepared.sourceRun,...assertLanguageReviewApproval({human,product,materials:prepared.materials,finishedAt:prepared.sourceRun.finishedAt})};
}

export function assessLanguageReviewEvidence({driver,entry,product}={}){
  if(driver?.schema!=='eagler-touhou/language-review/1'||entry?.id!=='LANG-05')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'reviewed-native-language-scenes',checks:['LANG-05']};
  try{
    assert.deepEqual(driver.scope,product);assert.equal(driver.newBrowserRun,false);assert.equal(entry.status,'passed');assert.equal(entry.complete,true);
    assert.ok(driver.checks.includes(entry)&&driver.checks.filter(row=>row.id===entry.id).length===1);
    assert.deepEqual(entry.observation,assertLanguageReview({...driver.evidence,product}));result.valid=true;
  }catch(error){result.reason=error.message;}return result;
}
