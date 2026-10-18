// Exercise the real journal in WASI and Emscripten (including its JS bridge).
import {spawnSync} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {pathToFileURL} from 'node:url';
import {WASI} from 'node:wasi';
if(process.argv.includes('--run-wasi')){
  const file=process.argv[process.argv.indexOf('--run-wasi')+1];
  const wasi=new WASI({version:'preview1',args:[file],returnOnExit:true});
  const instance=await WebAssembly.instantiate(await WebAssembly.compile(readFileSync(file)),{wasi_snapshot_preview1:wasi.wasiImport});
  process.exitCode=wasi.start(instance);
}else{
  const root=resolve(import.meta.dirname,'../..'),at=process.argv.indexOf('--output');
  if(at<0||!process.argv[at+1])throw Error('Fresh --output required');
  const out=resolve(process.argv[at+1]);if(existsSync(resolve(out,'receipt.json')))throw Error('Existing receipt');
  mkdirSync(out,{recursive:true});
  const common=resolve(root,'third_party/eagler-common'),sdk=resolve(root,'../../toolchains/emsdk');
  const source=resolve(common,'src/netplay/RollbackJournal.cpp'),header=resolve(common,'include/eagler/netplay/RollbackJournal.hpp');
  const fixtures=[resolve(common,'tests/rollback-journal-test.cpp'),resolve(common,'tests/rollback-readonly-batch-test.cpp')];
  const sha=f=>createHash('sha256').update(readFileSync(f)).digest('hex');
  const r={schema:'th11-journal-readonly-batch/1',startedAt:new Date().toISOString(),completedAt:null,passed:false,
    scope:'Real journal and explicit read-only contract; Emscripten bridge, heap growth, existing regression. Not gameplay performance.',
    sourceFiles:Object.fromEntries([source,header,...fixtures,import.meta.filename].map(f=>[f,sha(f)])),commands:[]};
  const save=()=>writeFileSync(resolve(out,'receipt.json'),JSON.stringify(r,null,2)+'\n');
  const stable=()=>{for(const [f,h]of Object.entries(r.sourceFiles))if(sha(f)!==h)throw Error('Source changed: '+f);};
  function run(label,program,args,env=process.env){
    stable();const x=spawnSync(program,args,{cwd:root,env,encoding:'utf8',windowsHide:true,maxBuffer:8*1024*1024,timeout:180000});
    writeFileSync(resolve(out,label+'.stdout.log'),x.stdout||'');writeFileSync(resolve(out,label+'.stderr.log'),x.stderr||'');
    r.commands.push({label,program,args,exitCode:x.status,signal:x.signal,error:x.error?String(x.error):null});save();
    if(x.status!==0||x.error)throw Error(label+' failed; retained logs');
    if(label.endsWith('run')){if(!x.stdout.includes('PASS'))throw Error(label+' never reached test completion');console.log(x.stdout.trim());}
  }
  save();
  try{
    const flags=['-O2','-g0','-UNDEBUG','-std=c++17','-ffp-contract=off','-fno-exceptions','-fno-rtti','-I'+resolve(common,'include')];
    for(const [i,fixture]of fixtures.entries()){
      const name=i?'batch':'legacy',wasm=resolve(out,name+'.wasm');
      run(name+'-wasi-build',resolve(root,'../../toolchains/wasi-sdk-34.0-x86_64-windows/bin/clang++.exe'),
        ['--target=wasm32-wasip1',...flags,fixture,source,'-Wl,-z,stack-size=1048576','-o',wasm]);
      run(name+'-wasi-run',process.execPath,[import.meta.filename,'--run-wasi',wasm]);
      const js=resolve(out,name+'.mjs');
      run(name+'-js-build','python',[resolve(sdk,'upstream/emscripten/emcc.py'),...flags,fixture,source,
        '-sDEFAULT_TO_CXX=1','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432','-sMAXIMUM_MEMORY=268435456',
        '-sSTACK_SIZE=1048576','-sENVIRONMENT=node','-sMODULARIZE=1','-sEXPORT_ES6=1','-sEXIT_RUNTIME=1','-o',js],{...process.env,EMSDK:sdk,EM_CONFIG:resolve(sdk,'.emscripten'),EMCC_CORES:'1'});
      run(name+'-js-run',process.execPath,['--input-type=module','--eval',
        'const {default:create}=await import('+JSON.stringify(pathToFileURL(js).href)+');await create();']);
    }
    stable();r.sourceStable=true;r.passed=true;
  }catch(error){r.error=error.stack;process.exitCode=1;console.error(String(error));}
  finally{r.completedAt=new Date().toISOString();save();console.log(resolve(out,'receipt.json'));}
}
