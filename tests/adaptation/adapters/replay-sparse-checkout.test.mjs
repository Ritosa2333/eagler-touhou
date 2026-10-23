import test from 'node:test';
import assert from 'node:assert/strict';
import {copyFile,mkdir,mkdtemp,rm,writeFile} from 'node:fs/promises';
import {dirname,resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {dependencySnapshot} from '../../../tools/testing/dependency-snapshot.mjs';

const repository=resolve(import.meta.dirname,'../../..');

test('generic Replay driver dependencies import from a sparse checkout with no TH08 tree',async()=>{
  await mkdir(resolve(repository,'launcher/.local'),{recursive:true});
  const sparse=await mkdtemp(resolve(repository,'launcher/.local/replay-sparse-checkout-'));
  try{
    await mkdir(resolve(sparse,'launcher'),{recursive:true});
    await writeFile(resolve(sparse,'launcher/package.json'),'{}');
    for(const path of ['tools/ports/replay-browser-semantics.mjs','tools/browser/final-error-audit.mjs',
      'tests/adaptation/adapters/replay.mjs','tests/adaptation/adapters/titles/th08-replay-format.mjs']){
      const destination=resolve(sparse,path);await mkdir(dirname(destination),{recursive:true});await copyFile(resolve(repository,path),destination);
    }
    assert.equal(await import(pathToFileURL(resolve(sparse,'tests/adaptation/adapters/replay.mjs'))).then(module=>
      module.replayTerminalPolicy({game:'th08',variant:'normal'}).kind),'parsed-capacity-diagnostic');
    const sparseSnapshot=await dependencySnapshot(sparse,['tools/ports/replay-browser-semantics.mjs']);
    assert.deepEqual(sparseSnapshot.map(file=>file.path).sort(),[
      'tests/adaptation/adapters/replay.mjs','tests/adaptation/adapters/titles/th08-replay-format.mjs',
      'tools/browser/final-error-audit.mjs','tools/ports/replay-browser-semantics.mjs',
    ]);
    assert.equal(sparseSnapshot.some(file=>file.path.startsWith('ports/th08/')),false);

    const driverSnapshot=await dependencySnapshot(repository,['tools/ports/replay-browser.mjs']);
    assert.equal(driverSnapshot.some(file=>file.path.startsWith('ports/th08/')),false);
    assert.equal(driverSnapshot.some(file=>file.path==='tests/adaptation/adapters/titles/th08-replay.mjs'),false);
  }finally{await rm(sparse,{recursive:true,force:true});}
});
