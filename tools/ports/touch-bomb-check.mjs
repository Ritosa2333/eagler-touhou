import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';
import {readFileSync,mkdirSync,writeFileSync,renameSync,statSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
import {workspacePath} from '../../launcher/lib/workspace-layout.mjs';

const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
export function wasiCompiler(environment=process.env,platform=process.platform){
  const bin=environment.WASI_SDK_BIN||(environment.WASI_SDK_ROOT&&resolve(environment.WASI_SDK_ROOT,'bin'));
  if(!bin)throw Error('Set WASI_SDK_BIN to the WASI SDK bin directory, or WASI_SDK_ROOT to its root');
  const compiler=resolve(bin,platform==='win32'?'clang++.exe':'clang++');
  if(!statSync(compiler,{throwIfNoEntry:false})?.isFile())throw Error(`Configured WASI compiler is missing: ${compiler}`);
  return compiler;
}

/** Compile and execute the unchanged title-owned production-module fixture. */
export async function checkTouchBomb({sourceRoot,checks,before=false,environment=process.env}){
  const output=resolve(sourceRoot,'artifacts/touch-bomb');mkdirSync(output,{recursive:true});
  const reportPath=resolve(output,before?'before.json':'report.json');
  const report={schema:'eagler-touhou/touch-bomb-module/1',status:'running',passed:false,startedAt:new Date().toISOString(),
    driverSha256:hash(readFileSync(new URL(import.meta.url))),
    scope:'Production C++ touch module through WASI; browser wiring and complete adaptation rows are separate checks',checks};
  const save=()=>{writeFileSync(reportPath+'.pending',JSON.stringify(report,null,2)+'\n');renameSync(reportPath+'.pending',reportPath);};
  save();
  try{
    const compiler=wasiCompiler(environment),common=resolve(environment.EAGLER_COMMON_ROOT||workspacePath('common'));
    const source=resolve(sourceRoot,'portable/input/touch-bomb-check.cpp');
    const inputs=[source,resolve(sourceRoot,'portable/input/TouchController.hpp'),resolve(common,'include/eagler/input/DialogueGesture.hpp')];
    report.inputs=inputs.map(path=>({path,sha256:hash(readFileSync(path))}));
    const version=spawnSync(compiler,['--version'],{encoding:'utf8',windowsHide:true,timeout:10000});
    if(version.error||version.status!==0)throw version.error??Error(version.stderr||'Cannot query WASI compiler version');
    report.compiler={path:compiler,version:version.stdout.trim()};
    const wasm=resolve(output,'verify.wasm');
    const args=['--target=wasm32-wasip1','-O2','-std=c++17','-fno-exceptions','-fno-rtti','-mexec-model=reactor',
      '-Wl,--no-entry','-Wl,--export-memory','-I',resolve(common,'include'),source,'-o',wasm];
    report.command={file:compiler,args};
    const built=spawnSync(compiler,args,{encoding:'utf8',windowsHide:true,timeout:180000,
      env:{...environment,CPATH:'',CPLUS_INCLUDE_PATH:''}});
    if(built.error||built.status!==0)throw built.error??Error(`WASI compilation failed (${built.status})\n${built.stdout}${built.stderr}`);
    const bytes=readFileSync(wasm);report.wasmSha256=hash(bytes);
    const wasi=new WASI({version:'preview1',args:[],env:{}});
    const {instance}=await WebAssembly.instantiate(bytes,{wasi_snapshot_preview1:wasi.wasiImport});wasi.initialize(instance);
    assert.equal(typeof instance.exports.verify,'function','Touch fixture must export verify');
    report.failure=instance.exports.verify();assert.equal(report.failure,0,'Production touch module assertion failed');
    for(const input of report.inputs)assert.equal(hash(readFileSync(input.path)),input.sha256,'Touch module input changed during verification');
    report.status='passed';report.passed=true;
  }catch(error){report.status='failed';report.error=String(error.stack||error);throw error;}
  finally{report.finishedAt=new Date().toISOString();save();console.log(JSON.stringify(report));}
  return report;
}
