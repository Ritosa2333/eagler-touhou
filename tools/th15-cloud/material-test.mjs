// Native and optional real Wasm contract checks. No renderer/FPS acceptance.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync,mkdtempSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'../..'),source=resolve(import.meta.dirname,'material-contract.cpp');
const artifacts=resolve(root,'artifacts/th15-material');mkdirSync(artifacts,{recursive:true});
const out=mkdtempSync(resolve(artifacts,'contract-'));
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const receipt={schema:'th15-material-contract/1',startedAt:new Date().toISOString(),commands:[],sources:{[source]:sha(source)},results:[],passed:false,
 scope:{gpu:false,gameplay:false,fps:false,requestedWasm:process.argv.includes('--wasm')}};
function run(name,program,args,env={}){
 const result=spawnSync(program,args,{cwd:root,env:{...process.env,...env},encoding:'utf8',timeout:60000,maxBuffer:4*1024*1024});
 writeFileSync(resolve(out,name+'.log'),(result.stdout||'')+(result.stderr||''));
 receipt.commands.push({name,program,args,exitCode:result.status,signal:result.signal,error:result.error?String(result.error):null});
 if(result.error||result.status!==0)throw Error(name+' failed: '+String(result.error||result.stderr||result.stdout));
 return result.stdout;
}
function verify(name,output){
 const line=output.split('\n').find(s=>s.startsWith('ANM_MATERIAL_CONTRACT '));assert.ok(line,'No actual contract result');
 const result=JSON.parse(line.slice('ANM_MATERIAL_CONTRACT '.length));assert.equal(result.checks,266896);assert.equal(result.matches,138410);
 receipt.results.push({name,...result});process.stdout.write(name+' '+line+'\n');
}
try{
 const compiler=process.env.CXX||(process.platform==='win32'?'g++':'c++');
 receipt.compiler=run('version',compiler,['--version']).split('\n')[0];
 let sdk,emcc;
 if(receipt.scope.requestedWasm){
  assert.ok(process.env.TH15_EMSDK||process.env.EMSDK,'Set TH15_EMSDK or EMSDK');sdk=resolve(process.env.TH15_EMSDK||process.env.EMSDK);
  emcc=['install','upstream'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);assert.ok(emcc,'Compiler missing in selected SDK');
 }
 const headers=['th15','th15-mp'].map(port=>resolve(root,'ports',port,'web/cpp/game/AnmMaterial.hpp'));
 assert.equal(sha(headers[0]),sha(headers[1]),'normal/MP material helper differs');
 for(const port of ['th15','th15-mp']){
  const include=resolve(root,'ports',port,'web/cpp/game'),header=resolve(include,'AnmMaterial.hpp');
  receipt.sources[header]=sha(header);const pipeline=resolve(root,'ports',port,'portable/sdl/GraphicsState.hpp');receipt.sources[pipeline]=sha(pipeline);
  const flags=['-O2','-std=c++17','-ffp-contract=off','-fno-exceptions','-fno-rtti','-I'+include];
  const binary=resolve(out,port+(process.platform==='win32'?'.exe':''));
  run(port+'-native-compile',compiler,[...flags,source,'-o',binary]);verify(port+'-native',run(port+'-native-run',binary,[]));
  if(receipt.scope.requestedWasm){
   const loader=resolve(out,port+'.cjs');
   run(port+'-wasm-compile',process.env.PYTHON||'python',[emcc,...flags,source,'-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',loader],{EM_CONFIG:resolve(sdk,'.emscripten')});
   verify(port+'-wasm',run(port+'-wasm-run',process.execPath,[loader]));
  }
 }
 receipt.passed=true;
}catch(error){receipt.failure=String(error);process.stderr.write(receipt.failure+'\n');process.exitCode=1;}
finally{receipt.completedAt=new Date().toISOString();writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');console.log('Material evidence: '+out);}
