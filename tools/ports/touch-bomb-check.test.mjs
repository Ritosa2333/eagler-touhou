import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,mkdir,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {wasiCompiler,checkTouchBomb} from './touch-bomb-check.mjs';

test('WASI compiler selection is explicit, platform-specific, and never falls back past an invalid explicit path',async()=>{
  const root=await mkdtemp(resolve(tmpdir(),'wasi-module-tools-'));
  try{
    const bin=resolve(root,'bin');await mkdir(bin);
    for(const name of ['clang++','clang++.exe'])await writeFile(resolve(bin,name),'compiler fixture');
    assert.equal(wasiCompiler({WASI_SDK_ROOT:root},'linux'),resolve(bin,'clang++'));
    assert.equal(wasiCompiler({WASI_SDK_ROOT:root},'win32'),resolve(bin,'clang++.exe'));
    assert.throws(()=>wasiCompiler({}),/Set WASI_SDK_BIN/);
    assert.throws(()=>wasiCompiler({WASI_SDK_BIN:resolve(root,'missing'),WASI_SDK_ROOT:root}),/Configured WASI compiler is missing/);
  }finally{await rm(root,{recursive:true,force:true});}
});

test('missing toolchain replaces stale successful evidence with a failed report',async()=>{
  const root=await mkdtemp(resolve(tmpdir(),'wasi-module-result-'));
  try{
    const out=resolve(root,'artifacts/touch-bomb');await mkdir(out,{recursive:true});
    await writeFile(resolve(out,'report.json'),JSON.stringify({passed:true}));
    await assert.rejects(checkTouchBomb({sourceRoot:root,checks:['fixture'],environment:{}}),/Set WASI_SDK_BIN/);
    const {readFile}=await import('node:fs/promises');
    const report=JSON.parse(await readFile(resolve(out,'report.json'),'utf8'));
    assert.equal(report.passed,false);assert.equal(report.status,'failed');assert.ok(report.finishedAt);
  }finally{await rm(root,{recursive:true,force:true});}
});
