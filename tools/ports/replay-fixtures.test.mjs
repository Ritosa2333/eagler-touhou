import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createHash} from 'node:crypto';
import {mkdtemp,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {loadRuntimeReplayFixture} from './replay-fixtures.mjs';

test('replay input is explicit and preserves all bytes with pre/post identity checks',async()=>{
  const directory=await mkdtemp(resolve(tmpdir(),'runtime-replay-input-'));
  try {
    const path=resolve(directory,'reference.rpy'),contents=Buffer.from([0,255,128,13,10,0]);
    await writeFile(path,contents);
    const replay={path,bytes:contents.length,sha256:createHash('sha256').update(contents).digest('hex')};
    const target={id:'fixture-normal',replay};
    assert.deepEqual((await loadRuntimeReplayFixture(target)).contents,contents);
    await writeFile(path,Buffer.from([0,255,128,13,10,1]));
    await assert.rejects(loadRuntimeReplayFixture(target),/bytes changed/);
    await writeFile(path,Buffer.from([0]));
    await assert.rejects(loadRuntimeReplayFixture(target),/size changed/);
  } finally {await rm(directory,{recursive:true,force:true});}
});

test('missing or incomplete replay inputs cannot become a synthetic successful check',async()=>{
  for(const replay of [undefined,{}, {path:'relative.rpy',bytes:1,sha256:'a'.repeat(64)},
    {path:resolve('missing.rpy'),bytes:0,sha256:'a'.repeat(64)},
    {path:resolve('missing.rpy'),bytes:1,sha256:'invalid'}])
    await assert.rejects(loadRuntimeReplayFixture({id:'fixture-normal',replay}));
});
