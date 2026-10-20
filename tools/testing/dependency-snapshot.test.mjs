import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,mkdir,writeFile,rm} from 'node:fs/promises';
import {relative,resolve} from 'node:path';
import {dependencySnapshot,identityDigest} from './dependency-snapshot.mjs';

const root=resolve(import.meta.dirname,'../..');
test('transitive literal imports invalidate identities while unrelated documents do not',async()=>{
  await mkdir(resolve(root,'.local'),{recursive:true});const directory=await mkdtemp(resolve(root,'.local/dependency-test-'));
  try{
    await writeFile(resolve(directory,'entry.mjs'),"export {value} from './helper.mjs'; await import('./dynamic.mjs');\n");
    await writeFile(resolve(directory,'helper.mjs'),'export const value=1;');
    await writeFile(resolve(directory,'dynamic.mjs'),'export const other=2;');
    const read=()=>dependencySnapshot(root,[relative(root,resolve(directory,'entry.mjs'))]);
    const first=await read();assert.equal(first.length,3);
    await writeFile(resolve(directory,'notes.md'),'documentation');assert.equal(identityDigest(await read()),identityDigest(first));
    await writeFile(resolve(directory,'helper.mjs'),'export const value=9;');assert.notEqual(identityDigest(await read()),identityDigest(first));
    await writeFile(resolve(directory,'entry.mjs'),"import './missing.mjs';");await assert.rejects(read,/unresolved local test dependency/);
  }finally{await rm(directory,{recursive:true,force:true});}
});

test('browser callback imports bind to actual Launcher modules and repository URL dependencies',async()=>{
  const snapshot=await dependencySnapshot(root,['tools/ports/offline-runtime-browser.mjs']);
  assert.ok(snapshot.some(file=>file.path==='launcher/package/package-store.mjs'));
  assert.ok(snapshot.some(file=>file.path==='tools/testing/cli-report.mjs'));
});
