import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {runNativeCpp} from '../../../launcher/tests/support/native-cpp.mjs';

test('SDL sampler preserves sparse/rewind output, moving stopped VMs and allocation-free steady tracks',async()=>{
  const source=await readFile(resolve(import.meta.dirname,'native/sdl-animation-performance.cpp'),'utf8');
  const output=await runNativeCpp('#include "Anm.cpp"\n#include "Animation.cpp"\n'+source,
    {includes:[resolve(import.meta.dirname,'../portable/sdl')]});
  const result=JSON.parse(output);
  assert.equal(result.steadyAllocations,0);
  assert.equal(result.steadyFrames,600);
  assert.equal(result.steadyTracks,512);
  assert.match(result.outputHash,/^[0-9a-f]{16}$/);
});
