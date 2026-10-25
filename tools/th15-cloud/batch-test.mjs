// CPU packing contracts compile the same RendererBatch.inc as the production
// owner. The terminal issue sink cannot prove GPU pixels or frame rate.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync,mkdtempSync,existsSync} from 'node:fs';
import {spawnSync} from 'node:child_process';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import os from 'node:os';
const root=resolve(import.meta.dirname,'../..'),source=resolve(import.meta.dirname,'batch-contract.cpp');
const artifacts=resolve(process.env.TH15_BATCH_EVIDENCE||resolve(root,'artifacts/th15-batch'));mkdirSync(artifacts,{recursive:true});
const out=mkdtempSync(resolve(artifacts,'contract-'));
const hash=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const receipt={schema:'th15-ordered-batch-contract/1',head:process.env.GITHUB_SHA??null,startedAt:new Date().toISOString(),
 host:{platform:process.platform,arch:process.arch,cpu:os.cpus()[0]?.model},sources:{},commands:[],results:[],passed:false,
 scope:{gpu:false,gameplay:false,fps:false,requestedWasm:process.argv.includes('--wasm'),sanitizer:process.argv.includes('--sanitize')}};
function run(name,program,args,env={}){
 const p=spawnSync(program,args,{cwd:root,env:{...process.env,...env},encoding:'utf8',timeout:120000,maxBuffer:8*1024*1024});
 writeFileSync(resolve(out,name+'.log'),(p.stdout||'')+(p.stderr||''));receipt.commands.push({name,program,args,exitCode:p.status,signal:p.signal,error:p.error?String(p.error):null});
 if(p.error||p.status!==0)throw Error(name+' failed: '+String(p.error||p.stderr||p.stdout));return p.stdout;
}
function verify(name,output){
 const line=output.split('\n').find(s=>s.startsWith('TH15_ORDERED_BATCH '));assert.ok(line,'Missing actual packing result');
 const result=JSON.parse(line.slice('TH15_ORDERED_BATCH '.length));assert.equal(result.scenarios,146);assert.equal(result.logicalDraws,25100);assert.ok(result.checks>=2700000);assert.equal(result.gpuValidated,false);
 receipt.results.push({name,...result});process.stdout.write(name+' '+line+'\n');
}
try{
 const cxx=process.env.CXX||(process.platform==='win32'?'g++':'c++');receipt.compiler=run('compiler',cxx,['--version']).split('\n')[0];
 receipt.sources[source]=hash(source);
 const stubs=resolve(import.meta.dirname,'batch-stubs');
 for(const h of ['SDL3/SDL.h','GLES3/gl3.h'])receipt.sources[h]=hash(resolve(stubs,h));
 assert.equal(hash(resolve(root,'ports/th15/portable/sdl/RendererBatch.inc')),hash(resolve(root,'ports/th15-mp/portable/sdl/RendererBatch.inc')),'Port batching algorithm differs');
 let sdk,compiler;
 if(receipt.scope.requestedWasm){sdk=resolve(process.env.TH15_EMSDK||process.env.EMSDK||'missing-sdk');compiler=['install','upstream'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);assert.ok(compiler,'Set TH15_EMSDK or EMSDK');}
 for(const port of ['th15','th15-mp']){
  const include=resolve(root,'ports',port,'portable/sdl');
  for(const h of ['Renderer.hpp','RendererBatch.inc','GraphicsState.hpp'])receipt.sources[port+'/'+h]=hash(resolve(include,h));
  const flags=['-O2','-std=c++17','-ffp-contract=off','-fno-exceptions','-fno-rtti','-I'+stubs,'-I'+include];
  const sanitizer=receipt.scope.sanitizer?['-fsanitize=address,undefined','-fno-omit-frame-pointer']:[];
  const binary=resolve(out,port+(process.platform==='win32'?'.exe':''));
  run(port+'-compile',cxx,[...flags,...sanitizer,source,'-o',binary]);verify(port+'-native',run(port+'-execute',binary,[]));receipt.sources[port+'-native-binary']=hash(binary);
  if(receipt.scope.requestedWasm){
   const loader=resolve(out,port+'.cjs');
   run(port+'-wasm-compile',process.env.PYTHON||'python',[compiler,...flags,source,'-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',loader],{EM_CONFIG:resolve(sdk,'.emscripten')});
   verify(port+'-wasm',run(port+'-wasm-execute',process.execPath,[loader]));receipt.sources[port+'-wasm-binary']=hash(resolve(out,port+'.wasm'));
  }
 }
 receipt.passed=true;
}catch(error){receipt.error=String(error);process.exitCode=1;process.stderr.write(receipt.error+'\n');}
finally{receipt.completedAt=new Date().toISOString();writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');console.log('Ordered batch evidence: '+out);}
