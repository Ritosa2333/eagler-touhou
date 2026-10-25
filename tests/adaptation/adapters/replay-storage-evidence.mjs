import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {resolveAdaptationAdapter} from './registry.mjs';
import {deriveReplayStorageLocations,parseReplayStoragePolicy,runtimeReplayFilePath,validateReplayInspection} from '../../../tools/ports/runtime-storage-contract.mjs';

const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const error=value=>assert.ok(typeof value==='string'&&value.trim()&&!/timeout|unsupported|unavailable/i.test(value),'Expected an explicit production rejection');

/** Reopen the actual readbacks; a hash printed by the producer is insufficient. */
export function assertReplayStorageEvidence(driver,entry,product){
  assert.equal(driver.schema,'eagler-touhou/runtime-browser/1');
  assert.equal(product.variant,'normal','Other modes need their own executed storage journey');
  const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
  const target=targets[0];assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1);
  assert.equal(entry.status,'passed');assert.equal(target.finalErrorAudit?.status,'passed');
  assert.deepEqual(target.inputPostflight.packageIdentity,target.packageIdentity);assert.deepEqual(target.inputPostflight.dataIdentity,target.dataIdentity);
  const e=target.replayFileJourney,p=PRODUCT_GAMES[product.game],mapping=resolveAdaptationAdapter(product.game,product.variant).runtime;
  assert.equal(e.schema,'eagler-touhou/runtime-replay-storage-journey/1');
  const identity={id:product.game,game:product.game,variant:product.variant,targetId:`${product.game}-${product.variant}`,
    packageSha256:target.packageIdentity.packageSha256,declaredSaveRoot:p.storage.saveRoot,runtimeMapping:mapping};
  assert.deepEqual(e.product,identity);assert.deepEqual(entry.observation.product,identity);
  const policy=parseReplayStoragePolicy(readFileSync(new URL('../../../launcher/src/launcher/replay-storage-policy.mts',import.meta.url),'utf8'));
  const locations=deriveReplayStorageLocations({game:product.game,product:p,policy,language:'ja'});
  assert.deepEqual(e.storage.normal,locations.normal);assert.deepEqual(e.storage.multiplayer,locations.multiplayer);
  const bytes=readFileSync(e.fixture.path);assert.ok(bytes.length>0&&bytes.length<=64*1024*1024);
  assert.equal(bytes.length,e.fixture.bytes);assert.equal(digest(bytes),e.fixture.sha256);
  const suffix=e.path.match(/_ud([a-z0-9]{4})\.[a-z0-9]+$/)?.[1];
  assert.equal(e.path,runtimeReplayFilePath({product:p,fixturePath:e.fixture.path,suffix}));
  assert.equal(e.sessions.length,3);const epochs=e.sessions.map(session=>session.epoch);
  assert.ok(epochs.every((epoch,index)=>Number.isSafeInteger(epoch)&&epoch>0&&(!index||epoch>epochs[index-1])));
  for(const session of e.sessions){
    for(const key of ['game','variant','targetId','packageSha256','runtimeMapping'])assert.deepEqual(session[key],identity[key]);
    const url=new URL(session.runtimeUrl);assert.ok(url.pathname.startsWith(`/runtime/${identity.targetId}/`));
    assert.equal(url.searchParams.get('runtimeEpoch'),String(session.epoch));
    if(session.readySaveRoot!==null)assert.equal(session.readySaveRoot,locations.normal.database);
    assert.equal(session.mount.available,true);assert.equal(session.mount.idbfs,true);
    assert.equal(session.mount.expectedDatabase,locations.normal.database);assert.equal(session.mount.mountpoint,locations.normal.database);
    assert.equal(session.mount.runtimeGlobal,mapping.global);assert.equal(session.mount.moduleField,mapping.module);
  }
  const operations=['write','read','list','sync','fresh-epoch-read','remove','sync-after-remove','fresh-epoch-list-after-delete','fresh-epoch-read-after-delete'];
  assert.deepEqual(e.operations.map(row=>row.operation),operations);
  for(const row of e.operations){
    if(['sync','sync-after-remove'].includes(row.operation)){assert.equal(row.status,'passed');continue;}
    assert.equal(row.path,e.path);
    if(['write','read','fresh-epoch-read'].includes(row.operation)){assert.equal(row.bytes,bytes.length);assert.equal(row.sha256,digest(bytes));}
    if(['read','fresh-epoch-read'].includes(row.operation))assert.deepEqual(readFileSync(row.artifact),bytes,'Runtime readback differs from the original fixture');
    if(row.operation==='list')assert.equal(row.size,bytes.length);
    if(row.operation==='fresh-epoch-read')assert.equal(row.epoch,epochs[1]);
    if(row.operation.startsWith('fresh-epoch-')&&row.operation!=='fresh-epoch-read')assert.equal(row.epoch,epochs[2]);
    if(row.operation==='fresh-epoch-read-after-delete')error(row.error);
    if(row.operation==='fresh-epoch-list-after-delete')assert.equal(row.absent,true);
  }
  for(const name of ['beforeWrite','afterWrite','afterRestore','afterDelete']){
    const files=e.lists[name];assert.ok(Array.isArray(files));const matches=files.filter(file=>file.path===e.path);
    assert.equal(matches.length,['afterWrite','afterRestore'].includes(name)?1:0);
    if(matches.length)assert.equal(matches[0].size,bytes.length);
  }
  const validity=e.fixtureValidation;assert.equal(validity.status,'passed');assert.equal(validity.path,e.path);
  if(validity.method==='official-runtime-inspection'){
    const inspected=validateReplayInspection({description:validity.description,inspectedPath:validity.path,expectedPath:e.path});
    assert.equal(validity.stageCount,inspected.stageCount);
  }else{
    assert.equal(validity.method,'official-runtime-open-pause');assert.ok(validity.opened&&typeof validity.opened==='object');
  }
  assert.equal(validity.naturalEndpointClaimed,false);
  const escape=e.negativeControls.filter(row=>row.kind==='path-escape');assert.equal(escape.length,1);
  assert.deepEqual(escape[0].rejected.map(row=>row.command),['read','write','remove']);escape[0].rejected.forEach(row=>error(row.error));
  const malformed=e.negativeControls.filter(row=>row.kind==='malformed-replay');assert.equal(malformed.length,1);
  assert.equal(malformed[0].status,'passed');assert.equal(malformed[0].rejectedAt,'official-runtime-validator');
  assert.notEqual(malformed[0].path,e.path);error(malformed[0].error);
  assert.equal(e.cleanup.validReplayAbsentAfterDelete,true);assert.equal(e.cleanup.malformedReplayAbsentAfterControl,true);
  assert.deepEqual(e.scope.observedVariants,[product.variant]);assert.equal(e.scope.multiplayerRuntimeExecuted,false);
  assert.equal(e.scope.naturalReplayEndpointClaimed,false);
}

export function assessReplayStorageEvidence({driver,entry,product}={}){
  if(entry?.id!=='replay-file-roundtrip-persistence')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'native-replay-storage',checks:['REPLAY-05']};
  try{assertReplayStorageEvidence(driver,entry,product);result.valid=true;}catch(error){result.reason=error.message;}
  return result;
}
