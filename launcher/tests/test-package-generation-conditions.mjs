import {test, beforeEach} from 'vitest';
import assert from 'node:assert/strict';
import {IDBFactory, IDBKeyRange} from 'fake-indexeddb';
import {installPackageFromAcquisition} from '../package/package-installer.mjs';
import {planPackageGeneration} from '../package/package-generation.mjs';
import {stagePendingPackageGeneration, putPendingPackageObject, commitPendingPackageGeneration,
  readCurrentPackageGeneration, readPackageObject, cancelPendingPackageGeneration} from '../package/package-store.mjs';

beforeEach(() => {globalThis.indexedDB = new IDBFactory(); globalThis.IDBKeyRange = IDBKeyRange;});
const descriptor = revision => ({schema:'eagler-touhou/package/1',game:'th06',revision,
  runtimeRequirement:{protocol:'eagler-touhou/1',target:'th06',dataFile:'data'},
  files:{data:{source:'th06.data',target:'/th06.data',revision,bytes:1}},base:{files:['data']},components:{}});
const install = (revision, value, extra = {}) => installPackageFromAcquisition({descriptor:descriptor(revision),
  source:'local',reuseCurrent:false,desiredFileIds:['data'],acquire:async()=>new Uint8Array([value]),...extra});
async function currentBytes() {
  const current = await readCurrentPackageGeneration('th06');
  const object = await readPackageObject(current.generation.files.data.objectId);
  return {current, bytes:[...new Uint8Array(await object.blob.arrayBuffer())]};
}

test('a checked update waiting for another client staging cannot replace its newer import without Web Locks', async () => {
  const initial = await install('r1', 1);
  const foreign = planPackageGeneration({current:initial.generation,descriptor:descriptor('r3'),desiredFileIds:['data'],generationId:'foreign-r3'}).generation;
  const owner = {operationId:'foreign-import'};
  await stagePendingPackageGeneration(foreign,{...owner,source:'local'});
  let checked;
  const observed = new Promise(resolve=>{checked=resolve;});
  const older = install('r2',2,{reuseCurrent:true,expectedGenerationId:initial.generation.id,
    desiredFileIds:current=>{assert.equal(current.generation.id,initial.generation.id);checked();return ['data'];}})
    .then(result=>({result}),error=>({error}));
  await observed;
  await putPendingPackageObject('th06',foreign.id,'data',new Uint8Array([3]),owner);
  await commitPendingPackageGeneration('th06',foreign.id,owner);
  const outcome = await older;
  assert.match(outcome.error?.message ?? '', /Package changed before background update/);
  const {current,bytes} = await currentBytes();
  assert.equal(current.generation.id,foreign.id);
  assert.equal(current.installation.pendingGeneration,null);
  assert.deepEqual(bytes,[3]);
});

test('the durable commit compares the expected installation in its own transaction', async () => {
  const initial = await install('r1',1);
  const pending = planPackageGeneration({current:initial.generation,descriptor:descriptor('r2'),desiredFileIds:['data'],generationId:'conditional-r2'}).generation;
  const owner = {operationId:'conditional-update'};
  await stagePendingPackageGeneration(pending,{...owner,source:'remote'});
  await putPendingPackageObject('th06',pending.id,'data',new Uint8Array([2]),owner);
  await assert.rejects(commitPendingPackageGeneration('th06',pending.id,{...owner,expectedGenerationId:'superseded-r0'}),/Package changed before background update/);
  assert.equal((await readCurrentPackageGeneration('th06')).generation.id,initial.generation.id);
  await cancelPendingPackageGeneration('th06',{...owner,generationId:pending.id});
});
