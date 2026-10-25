import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,writeFileSync,readFileSync,rmSync} from 'node:fs';
import {join} from 'node:path';
import {tmpdir} from 'node:os';
import {createHash} from 'node:crypto';
import {assertReplayStorageEvidence,assessReplayStorageEvidence} from './replay-storage-evidence.mjs';
import {resolveAdaptationAdapter} from './registry.mjs';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {parseReplayStoragePolicy,deriveReplayStorageLocations} from '../../../tools/ports/runtime-storage-contract.mjs';

const product={game:'th08',variant:'normal'};
function fixture(t){
  const directory=mkdtempSync(join(tmpdir(),'replay-storage-proof-'));t.after(()=>rmSync(directory,{recursive:true,force:true}));
  // Format acceptance belongs to the native validator receipt; these test bytes exercise file identity.
  const bytes=Buffer.from('synthetic replay storage fixture'),path=join(directory,'fixture.rpy'),read=join(directory,'read.bin'),restored=join(directory,'restored.bin');
  for(const file of [path,read,restored])writeFileSync(file,bytes);
  const sha256=createHash('sha256').update(bytes).digest('hex'),mapping=resolveAdaptationAdapter('th08','normal').runtime;
  const policy=parseReplayStoragePolicy(readFileSync(new URL('../../../launcher/src/launcher/replay-storage-policy.mts',import.meta.url),'utf8'));
  const locations=deriveReplayStorageLocations({game:'th08',product:PRODUCT_GAMES.th08,policy});
  const identity={id:'th08',...product,targetId:'th08-normal',packageSha256:'a'.repeat(64),declaredSaveRoot:'/savesth08',runtimeMapping:mapping};
  const replayPath='replay/th8_udabcd.rpy';
  const e={schema:'eagler-touhou/runtime-replay-storage-journey/1',product:identity,fixture:{path,bytes:bytes.length,sha256},path:replayPath,
    storage:{normal:locations.normal,multiplayer:locations.multiplayer},sessions:[3,4,5].map(epoch=>({...product,targetId:identity.targetId,packageSha256:identity.packageSha256,runtimeMapping:mapping,epoch,
      runtimeUrl:`https://127.0.0.1/runtime/th08-normal/th08.html?runtimeEpoch=${epoch}`,readySaveRoot:null,
      mount:{available:true,idbfs:true,expectedDatabase:'/savesth08',mountpoint:'/savesth08',runtimeGlobal:mapping.global,moduleField:mapping.module}})),
    operations:[{operation:'write',path:replayPath,bytes:bytes.length,sha256},{operation:'read',path:replayPath,bytes:bytes.length,sha256,artifact:read},
      {operation:'list',path:replayPath,size:bytes.length},{operation:'sync',status:'passed'},
      {operation:'fresh-epoch-read',path:replayPath,bytes:bytes.length,sha256,artifact:restored,epoch:4},{operation:'remove',path:replayPath,sha256},
      {operation:'sync-after-remove',status:'passed'},{operation:'fresh-epoch-list-after-delete',path:replayPath,epoch:5,absent:true},
      {operation:'fresh-epoch-read-after-delete',path:replayPath,epoch:5,error:'File not found'}],
    lists:{beforeWrite:[],afterWrite:[{path:replayPath,size:bytes.length}],afterRestore:[{path:replayPath,size:bytes.length}],afterDelete:[]},
    fixtureValidation:{status:'passed',method:'official-runtime-inspection',path:replayPath,stageCount:1,description:{stages:[{id:1}]},naturalEndpointClaimed:false},
    negativeControls:[{kind:'path-escape',rejected:['read','write','remove'].map(command=>({command,error:'Invalid save path'}))},
      {kind:'malformed-replay',status:'passed',rejectedAt:'official-runtime-validator',path:'replay/th8_ud1234.rpy',error:'Invalid Replay'}],
    cleanup:{validReplayAbsentAfterDelete:true,malformedReplayAbsentAfterControl:true},scope:{observedVariants:['normal'],multiplayerRuntimeExecuted:false,naturalReplayEndpointClaimed:false}};
  const entry={id:'replay-file-roundtrip-persistence',status:'passed',observation:{product:identity}},packageIdentity={packageSha256:identity.packageSha256},dataIdentity={sha256:'b'.repeat(64)};
  const target={...product,checks:[entry],replayFileJourney:e,finalErrorAudit:{status:'passed'},packageIdentity,dataIdentity,inputPostflight:{packageIdentity,dataIdentity}};
  return{driver:{schema:'eagler-touhou/runtime-browser/1',targets:[target]},entry,e,target,restored};
}
test('Replay file proof reopens both actual readbacks and completes only the selected normal file journey',t=>{
  const f=fixture(t);assert.doesNotThrow(()=>assertReplayStorageEvidence(f.driver,f.entry,product));
  assert.deepEqual(assessReplayStorageEvidence({...f,product}).checks,['REPLAY-05']);
});
const mutations={
  'changed restored bytes with unchanged summary':f=>writeFileSync(f.restored,'changed'),
  'readback artifact missing':f=>delete f.e.operations[1].artifact,
  'restoration in the old epoch':f=>f.e.sessions[1].epoch=3,
  'wrong mount':f=>f.e.sessions[1].mount.mountpoint='/savesth08-multiplayer',
  'wrong Runtime document':f=>f.e.sessions[1].runtimeUrl='https://127.0.0.1/runtime/th10-normal/th10.html?runtimeEpoch=4',
  'missing sync':f=>f.e.operations.splice(3,1),
  'deleted file returns after reopen':f=>f.e.lists.afterDelete=f.e.lists.afterWrite,
  'restore list omits the Replay':f=>f.e.lists.afterRestore=[],
  'malformed file rejected at write instead of validator':f=>f.e.negativeControls[1].rejectedAt='write',
  'validator accepts no stage':f=>f.e.fixtureValidation.description.stages=[],
  'path escape was only a timeout':f=>f.e.negativeControls[0].rejected[0].error='Timeout: request',
  'deletion read was only a timeout':f=>f.e.operations.at(-1).error='Timeout: read',
  'late Runtime error':f=>f.target.finalErrorAudit.status='failed',
  'package changes during the journey':f=>f.target.inputPostflight={...f.target.inputPostflight,packageIdentity:{packageSha256:'c'.repeat(64)}},
};
for(const [name,mutate] of Object.entries(mutations))test(`Replay file proof rejects ${name}`,t=>{
  const f=fixture(t);mutate(f);assert.throws(()=>assertReplayStorageEvidence(f.driver,f.entry,product));
});
