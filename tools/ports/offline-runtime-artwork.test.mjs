import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,mkdir,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {dirname,resolve} from 'node:path';
import {inspectOfflineArtwork} from './offline-runtime-artwork.mjs';

test('single-game offline input still requires every visible catalog cover',async()=>{
  const directory=await mkdtemp(resolve(tmpdir(),'eagler-artwork-'));
  try{
    const empty=await inspectOfflineArtwork(directory);
    assert.equal(empty.available,false);
    assert.ok(empty.missing.some(file=>file.name==='th075-card.webp'));
    for(const file of empty.missing){
      await mkdir(dirname(file.path),{recursive:true});
      await writeFile(file.path,file.name==='th075-card.webp'?'':`synthetic artwork ${file.name}`);
    }
    const partial=await inspectOfflineArtwork(directory);
    assert.equal(partial.available,false);
    assert.deepEqual(partial.missing.map(file=>file.name),['th075-card.webp']);
    await writeFile(resolve(directory,'th075-card.webp'),'synthetic artwork th075');
    const complete=await inspectOfflineArtwork(directory);
    assert.equal(complete.available,true);
    assert.equal(complete.files.length,empty.missing.length);
    assert.ok(complete.files.every(file=>file.bytes>0&&/^[a-f0-9]{64}$/.test(file.sha256)));
  }finally{await rm(directory,{recursive:true,force:true});}
});
