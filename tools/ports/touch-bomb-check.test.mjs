import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,mkdir,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {wasiCompiler,checkTouchBomb} from './touch-bomb-check.mjs';

test('WASI compiler aliases follow the shared precedence and never fall back from an invalid selected path',async()=>{
  const root=await mkdtemp(resolve(tmpdir(),'wasi-module-tools-'));
  try{
    async function sdk(name){const path=resolve(root,name),bin=resolve(path,'bin');await mkdir(bin,{recursive:true});
      for(const compiler of ['clang++','clang++.exe'])await writeFile(resolve(bin,compiler),'compiler fixture');return path;}
    const preferred=await sdk('preferred'),secondary=await sdk('secondary'),legacyRoot=await sdk('legacy-root');
    const legacyBin=resolve(root,'legacy-bin');await mkdir(legacyBin);
    for(const compiler of ['clang++','clang++.exe'])await writeFile(resolve(legacyBin,compiler),'compiler fixture');
    assert.equal(wasiCompiler({EAGLER_WASI_SDK:preferred,WASI_SDK_PATH:secondary,WASI_SDK_BIN:legacyBin,WASI_SDK_ROOT:legacyRoot},'win32'),resolve(preferred,'bin','clang++.exe'));
    assert.equal(wasiCompiler({WASI_SDK_PATH:secondary,WASI_SDK_BIN:legacyBin,WASI_SDK_ROOT:legacyRoot},'linux'),resolve(secondary,'bin','clang++'));
    assert.equal(wasiCompiler({WASI_SDK_BIN:legacyBin,WASI_SDK_ROOT:legacyRoot},'linux'),resolve(legacyBin,'clang++'));
    assert.equal(wasiCompiler({WASI_SDK_ROOT:legacyRoot},'linux'),resolve(legacyRoot,'bin','clang++'));
    assert.throws(()=>wasiCompiler({}),/Set EAGLER_WASI_SDK or WASI_SDK_PATH/);
    assert.throws(()=>wasiCompiler({EAGLER_WASI_SDK:resolve(root,'missing'),WASI_SDK_PATH:secondary}),/Configured WASI compiler is missing/);
    assert.throws(()=>wasiCompiler({WASI_SDK_PATH:'relative-sdk',WASI_SDK_BIN:legacyBin}),/WASI_SDK_PATH must be an absolute path/);
    assert.throws(()=>wasiCompiler({WASI_SDK_BIN:resolve(root,'missing-bin'),WASI_SDK_ROOT:legacyRoot}),/Configured WASI compiler is missing/);
  }finally{await rm(root,{recursive:true,force:true});}
});

test('missing toolchain replaces stale successful evidence and honors the suite-owned report identity',async()=>{
  const root=await mkdtemp(resolve(tmpdir(),'wasi-module-result-'));
  try{
    const out=resolve(root,'reports','th08');await mkdir(out,{recursive:true});
    await writeFile(resolve(out,'report.json'),JSON.stringify({passed:true}));
    const environment={EAGLER_TOUCH_BOMB_REPORT_DIRECTORY:out,EAGLER_TOUCH_BOMB_TASK_ID:'touch-bomb-th08',
      EAGLER_TOUCH_BOMB_SOURCE_OWNER:'th08',EAGLER_TOUCH_BOMB_VARIANTS:'normal'};
    await assert.rejects(checkTouchBomb({sourceRoot:root,checks:['fixture'],environment}),/Set EAGLER_WASI_SDK or WASI_SDK_PATH/);
    const {readFile}=await import('node:fs/promises');
    const report=JSON.parse(await readFile(resolve(out,'report.json'),'utf8'));
    assert.equal(report.passed,false);assert.equal(report.status,'failed');assert.ok(report.finishedAt);
    assert.deepEqual(report.task,{id:'touch-bomb-th08',sourceOwner:'th08',variants:['normal']});
    await assert.rejects(readFile(resolve(root,'artifacts/touch-bomb/report.json')),{code:'ENOENT'});
  }finally{await rm(root,{recursive:true,force:true});}
});
