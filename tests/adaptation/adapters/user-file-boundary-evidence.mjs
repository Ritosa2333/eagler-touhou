import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {resolveAdaptationAdapter} from './registry.mjs';
import {deriveReplayStorageLocations,parseReplayStoragePolicy} from '../../../tools/ports/runtime-storage-contract.mjs';
import {matchExpectedOfflinePathTraversal} from '../../../tools/ports/offline-runtime-errors.mjs';
import {assessNativeUserDataJourney} from '../../../tools/ports/runtime-user-data-journey.mjs';
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');

export function assertUserFileBoundary({driver,e,product,adapter=resolveAdaptationAdapter(product.game,product.variant)}){
  assert.equal(driver.schema,'eagler-touhou/offline-runtime-browser/1');assert.equal(product.variant,'normal');
  assert.equal(driver.scope.game,product.game);assert.equal(driver.scope.variant,product.variant);
  const policyPath=resolve(import.meta.dirname,'../../../launcher/src/launcher/replay-storage-policy.mts'),source=readFileSync(policyPath);
  assert.equal(e.policySource.sha256,digest(source));assert.equal(e.policySource.bytes,source.length);
  const locations=deriveReplayStorageLocations({game:product.game,product:PRODUCT_GAMES[product.game],policy:parseReplayStoragePolicy(source.toString('utf8')),language:e.language});
  assert.deepEqual(e.locations,locations);assert.deepEqual(e.mapping,adapter.runtime);
  assert.equal(e.packageSha256,driver.runtimePackages.normal.packageSha256);
  assert.equal(e.mount.available,true);assert.equal(e.mount.idbfs,true);assert.equal(e.mount.mountpoint,locations.normal.database);
  assert.equal(e.mount.expectedDatabase,locations.normal.database);assert.equal(e.mount.runtimeGlobal,adapter.runtime.global);
  const native=driver.nativeUserData.observation;
  assessNativeUserDataJourney(adapter.storage,native,'SAVE-04');
  assert.equal(e.snapshot.root,locations.normal.root);assert.equal(e.snapshot.document.epoch,native.lifecycle.afterEpoch);
  assert.ok(e.snapshot.document.app>0);assert.equal(Number(new URL(e.snapshot.document.url).searchParams.get('runtimeEpoch')),native.lifecycle.afterEpoch);
  const expected=[...native.reopened.files,driver.userFiles.sync];
  assert.equal(e.snapshot.files.length,expected.length);assert.equal(new Set(e.snapshot.files.map(file=>file.path)).size,expected.length);
  for(const reference of expected){
    const path=reference.path??reference.runtimePath,actual=e.snapshot.files.find(file=>file.path===path);assert.ok(actual,`Native file ${path} is absent from its save root`);
    assert.equal(actual.absolutePath,locations.normal.root+'/'+path);assert.equal(actual.resolvedPath,actual.absolutePath);
    assert.equal(actual.mountpoint,locations.normal.database);
    const bytes=readFileSync(actual.artifact);assert.equal(bytes.length,actual.bytes);assert.equal(digest(bytes),actual.sha256);
    assert.equal(actual.bytes,reference.bytes);assert.equal(actual.sha256,reference.sha256);
    assert.deepEqual(bytes,Buffer.from(reference.contentBase64,'base64'));
  }
  assert.deepEqual(e.lists.before,e.lists.after,'Rejected file requests changed the Runtime file list');
  assert.equal(e.negatives.length,3);assert.deepEqual(e.negatives.map(row=>row.classification.command),['read','write','remove']);
  assert.equal(new Set(e.negatives.map(row=>row.response.request)).size,3);
  for(const row of e.negatives){
    const c=row.classification;assert.equal(c.game,product.game);assert.equal(c.epoch,e.beforeTraversal.document.epoch);
    const actual=matchExpectedOfflinePathTraversal(row.response,{game:c.game,epoch:c.epoch,command:c.command,path:c.path,requestId:c.requestId});
    assert.ok(actual,'Directory escape did not receive an exact production path rejection');assert.deepEqual(c,actual);
  }
  assert.equal(e.beforeTraversal.document.epoch,e.afterTraversal.document.epoch);
  for(const snapshot of [e.beforeTraversal,e.afterTraversal,e.snapshot]){
    assert.equal(snapshot.root,locations.normal.root);assert.deepEqual(snapshot.outside,{path:locations.normal.root+'/../outside-offline-acceptance.dat',exists:false});
  }
  return{paths:expected.map(file=>file.path??file.runtimePath),root:locations.normal.root};
}

export function assessUserFileBoundaryEvidence({driver,entry,product}={}){
  if(driver?.schema!=='eagler-touhou/offline-runtime-browser/1'||!['SAVE-01','SAVE-02'].includes(entry?.id))return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'native-user-file-root-and-allowlist',checks:[entry.id]};
  try{
    assert.equal(driver.checks.filter(row=>row.id===entry.id).length,1);assert.ok(driver.checks.includes(entry));
    assert.equal(entry.status,'passed');assert.equal(entry.complete,true);assert.deepEqual(entry.evidenceRefs,['/userFileBoundary']);
    const envelope=driver.userFileBoundary;assert.equal(envelope.game,product.game);assert.equal(envelope.variant,product.variant);
    assert.ok(envelope.adaptationIds.includes(entry.id));assertUserFileBoundary({driver,e:envelope.observation,product});result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
