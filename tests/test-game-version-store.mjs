import assert from 'node:assert/strict';
import {IDBFactory, IDBKeyRange} from 'fake-indexeddb';
import {createHash} from 'node:crypto';
import {installPackageFromAcquisition} from '../package/package-installer.mjs';
import {readCurrentPackageGeneration, readPackageObject, garbageCollectPackageStore} from '../package/package-store.mjs';

globalThis.indexedDB = new IDBFactory();
globalThis.IDBKeyRange = IDBKeyRange;
const digest = b => createHash('sha256').update(b).digest('hex');
function descriptor(version, data, music = new Uint8Array([9, 8, 7])) {
  const file = (name, bytes) => ({source: `games/th06/${name}`, target: `/${name}`, bytes: bytes.length, sha256: digest(bytes), revision: digest(bytes).slice(0,16)});
  return {schema:'eagler-touhou/package/1',game:'th06',revision:`${version}-${digest(data).slice(0,16)}`,
    ...(version === 'beauty' ? {gameVersion:version} : {}),
    runtimeRequirement:{protocol:'eagler-touhou/1',target:'th06',dataFile:'game-data',dataLayout:'shared-layout'},
    files:{'game-data':file('th06.data',data),music:file('bgm.ogg',music)},base:{files:['game-data','music']},components:{}};
}
async function install(version, data, options = {}) {
  const d=descriptor(version,data);
  return installPackageFromAcquisition({descriptor:d,source:'local',reuseCurrent:false,desiredFileIds:d.base.files,
    acquire:async id=>id==='game-data'?data:new Uint8Array([9,8,7]),...options});
}
const original=await install('original',new Uint8Array([1,2,3]));
const beauty=await install('beauty',new Uint8Array([4,5,6]));
assert.notEqual(original.generation.id,beauty.generation.id);
assert.equal(original.generation.files.music.objectId,beauty.generation.files.music.objectId,'identical music is shared immutably');
await garbageCollectPackageStore();
for(const [version,expected]of [['original',original],['beauty',beauty]]) {
  const result=await readCurrentPackageGeneration('th06',{gameVersion:version});
  assert.equal(result.generation.id,expected.generation.id,'GC retains each version');
  assert.ok(await readPackageObject(result.generation.files['game-data'].objectId));
}
const updated=await install('beauty',new Uint8Array([6,5,4]));
assert.equal((await readCurrentPackageGeneration('th06')).generation.id,original.generation.id,'updating beauty never moves original');
await assert.rejects(install('beauty',new Uint8Array([7,7,7]),{acquire:async()=>{throw Error('failed transfer');}}),/failed transfer/);
assert.equal((await readCurrentPackageGeneration('th06',{gameVersion:'beauty'})).generation.id,updated.generation.id,'failed beauty update retains beauty');
assert.equal((await readCurrentPackageGeneration('th06')).generation.id,original.generation.id,'failed beauty update retains original');
const [nextOriginal,nextBeauty]=await Promise.all([install('original',new Uint8Array([11,12,13])),install('beauty',new Uint8Array([14,15,16]))]);
await garbageCollectPackageStore();
assert.equal((await readCurrentPackageGeneration('th06')).generation.id,nextOriginal.generation.id);
assert.equal((await readCurrentPackageGeneration('th06',{gameVersion:'beauty'})).generation.id,nextBeauty.generation.id);
assert.equal((await readCurrentPackageGeneration('th06')).installation.pendingGeneration,null);
assert.equal((await readCurrentPackageGeneration('th06',{gameVersion:'beauty'})).installation.pendingGeneration,null);
console.log('Independent original/beauty installs, immutable deduplication, GC, failed update and concurrent commits: PASS');
