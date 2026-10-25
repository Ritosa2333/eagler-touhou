import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {existsSync,openSync,closeSync} from 'node:fs';
import {mkdtemp,readFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {dirname,isAbsolute,join,resolve} from 'node:path';
import {WASI} from 'node:wasi';

/** Compile production code which owns wasm32 layouts, then execute it through Node's WASI host. */
export async function runWasiCpp({source,includes=[],sources=[],environment=process.env,captureStdout=false}){
  const sdk=environment.EAGLER_WASI_SDK??environment.WASI_SDK_PATH;
  assert.ok(typeof sdk==='string'&&isAbsolute(sdk),'Set EAGLER_WASI_SDK or WASI_SDK_PATH to an absolute WASI SDK directory');
  const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
  assert.ok(existsSync(compiler),`WASI C++ compiler is missing: ${compiler}`);
  const directory=await mkdtemp(join(tmpdir(),'eagler-wasi-cpp-'));
  let stdout;
  try{
    const wasm=join(directory,'fixture.wasm');
    execFileSync(compiler,['--target=wasm32-wasip1','-O2','-std=c++17','-fno-exceptions','-fno-rtti',
      ...includes.flatMap(path=>['-I',resolve(path)]),resolve(source),...sources.map(path=>resolve(path)),'-o',wasm],
    {encoding:'utf8',windowsHide:true,timeout:120000});
    if(captureStdout)stdout=openSync(join(directory,'stdout.txt'),'w');
    const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true,...(captureStdout?{stdout}:{})});
    const {instance}=await WebAssembly.instantiate(await readFile(wasm),wasi.getImportObject());
    assert.equal(wasi.start(instance),0,'The compiled production fixture failed');
    if(captureStdout){closeSync(stdout);stdout=undefined;return{stdout:await readFile(join(directory,'stdout.txt'),'utf8')};}
  }finally{
    if(stdout!==undefined)closeSync(stdout);
    if(dirname(resolve(directory))!==resolve(tmpdir()))throw Error('Unexpected WASI fixture temporary directory');
    await rm(directory,{recursive:true,force:true});
  }
}
