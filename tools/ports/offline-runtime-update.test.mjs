import test from 'node:test';
import assert from 'node:assert/strict';
import {validateOfflineUpdateManifest,assertOfflineUpdateManifestIdentity} from './offline-runtime-update.mjs';

const data={bytes:12,sha256:'1'.repeat(64)};
function sample(){return{schema:'eagler-touhou/offline-package-updates/1',games:{th11:{
  from:{revision:'base-revision-1',archiveSha256:'2'.repeat(64),data},
  to:{path:'D:/packages/th11-update.zip',revision:'update-revision-2',archiveSha256:'3'.repeat(64),data,
    ogg:[{name:'th11_00.ogg',bytes:24,sha256:'4'.repeat(64)}]},
}}};}

test('explicit package generations allow a descriptor-only change with stable DATA and verified OGG inventory',()=>{
  const value=validateOfflineUpdateManifest(sample());
  const result=assertOfflineUpdateManifestIdentity({game:'th11',manifestEntry:value.games.th11,basePath:'D:/packages/th11-base.zip',
    baseRevision:'base-revision-1',baseArchiveSha256:'2'.repeat(64),baseData:data,
    update:{path:'D:/packages/th11-update.zip',archiveSha256:'3'.repeat(64),archiveBytes:900,
      descriptor:{revision:'update-revision-2',files:{'game-data':data}},ogg:[{name:'th11_00.ogg',bytes:24,sha256:'4'.repeat(64)}]}});
  assert.equal(result.updateRevision,'update-revision-2');
  assert.deepEqual(result.data,data);
});

test('update manifests reject same-generation, changed-DATA, and ambiguous OGG identities',()=>{
  const same=sample();same.games.th11.to.revision=same.games.th11.from.revision;
  assert.throws(()=>validateOfflineUpdateManifest(same),/revision must differ/);
  const changed=sample();changed.games.th11.to.data={bytes:13,sha256:'5'.repeat(64)};
  assert.throws(()=>validateOfflineUpdateManifest(changed),/DATA identity/);
  const duplicate=sample();duplicate.games.th11.to.ogg.push({...duplicate.games.th11.to.ogg[0]});
  assert.throws(()=>validateOfflineUpdateManifest(duplicate),/duplicate OGG/);
});

test('selected update evidence is bound to the actual base ZIP identity',()=>{
  const value=validateOfflineUpdateManifest(sample());
  assert.throws(()=>assertOfflineUpdateManifestIdentity({game:'th11',manifestEntry:value.games.th11,basePath:'D:/packages/th11-base.zip',
    baseRevision:'other-revision',baseArchiveSha256:'2'.repeat(64),baseData:data,
    update:{path:'D:/packages/th11-update.zip',archiveSha256:'3'.repeat(64),archiveBytes:900,
      descriptor:{revision:'update-revision-2',files:{'game-data':data}},ogg:[{name:'th11_00.ogg',bytes:24,sha256:'4'.repeat(64)}]}}),/base descriptor differs/);
});
