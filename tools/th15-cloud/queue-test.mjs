// Production queue/storage contracts with independent terminal state semantics.
// These checks are not GLES pixel tests and cannot establish gameplay or FPS.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync,mkdtempSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'../..'),artifacts=resolve(root,'artifacts/th15-queue');mkdirSync(artifacts,{recursive:true});
const out=mkdtempSync(resolve(artifacts,'contract-'));
const hash=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const receipt={schema:'th15-deferred-gpu-contract/1',head:process.env.GITHUB_SHA??null,startedAt:new Date().toISOString(),commands:[],sources:{},results:[],passed:false,
 scope:{gpuPixels:false,gameplay:false,fps:false,wasm:process.argv.includes('--wasm'),sanitizer:process.argv.includes('--sanitize')}};
function run(name,program,args,env={}){
 const p=spawnSync(program,args,{cwd:root,env:{...process.env,...env},encoding:'utf8',timeout:120000,maxBuffer:8*1024*1024});
 writeFileSync(resolve(out,name+'.log'),(p.stdout||'')+(p.stderr||''));receipt.commands.push({name,program,args,exitCode:p.status,signal:p.signal,error:p.error?String(p.error):null});
 if(p.error||p.status!==0)throw Error(name+' failed: '+String(p.error||p.stderr||p.stdout));return p.stdout;
}
function verify(name,kind,output){
 const prefix=kind==='queue-contract'?'TH15_DEFERRED_COMMAND ':'GPU_LIVENESS ';
 const line=output.split('\n').find(s=>s.startsWith(prefix));assert.ok(line,'Missing actual model result');
 const result=JSON.parse(line.slice(prefix.length));
 assert.equal(result.sequences,kind==='queue-contract'?1013:4005);
 assert.equal(result.checks,kind==='queue-contract'?358407:24006);
 if(kind==='queue-contract'){assert.equal(result.queuedDraws,36265);assert.equal(result.prunedDraws,397);assert.equal(result.gpuPixels,false);}
 else {assert.equal(result.removedDraws,7647);assert.equal(result.pixels,false);}
 receipt.results.push({name,...result});process.stdout.write(name+' '+prefix+JSON.stringify(result)+'\n');
}
try{
 const cxx=process.env.CXX||(process.platform==='win32'?'g++':'c++');receipt.compiler=run('compiler',cxx,['--version']).split('\n')[0];
 let sdk,compiler;
 if(receipt.scope.wasm){sdk=resolve(process.env.TH15_EMSDK||process.env.EMSDK||'missing-sdk');compiler=['install','upstream'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);assert.ok(compiler,'Set TH15_EMSDK or EMSDK');}
 for(const header of ['RendererQueue.inc','GpuCommandLiveness.hpp'])assert.equal(hash(resolve(root,'ports/th15/portable/sdl',header)),hash(resolve(root,'ports/th15-mp/portable/sdl',header)),'Normal/MP queue implementation differs');
 for(const port of ['th15','th15-mp']){
  const include=resolve(root,'ports',port,'portable/sdl');
  for(const header of ['Renderer.hpp','RendererBatch.inc','RendererQueue.inc','GpuCommandLiveness.hpp','GraphicsState.hpp'])receipt.sources[port+'/'+header]=hash(resolve(include,header));
  const flags=['-O2','-std=c++17','-ffp-contract=off','-fno-exceptions','-fno-rtti','-DTH15_MULTIPLAYER='+Number(port==='th15-mp'),'-I'+resolve(import.meta.dirname,'batch-stubs'),'-I'+include];
  for(const kind of ['queue-contract','queue-liveness']){
   const source=resolve(import.meta.dirname,kind+'.cpp');receipt.sources[kind]=hash(source);
   const name=port+'-'+kind,binary=resolve(out,name+(process.platform==='win32'?'.exe':''));
   const sanitizer=receipt.scope.sanitizer?['-fsanitize=address,undefined','-fno-omit-frame-pointer']:[];
   run(name+'-compile',cxx,[...flags,...sanitizer,source,'-o',binary]);verify(name+'-native',kind,run(name+'-execute',binary,[]));
   receipt.sources[name+'-binary']=hash(binary);
   if(receipt.scope.wasm){
    const loader=resolve(out,name+'.cjs');
    run(name+'-wasm-compile',process.env.PYTHON||'python',[compiler,...flags,source,'-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',loader],{EM_CONFIG:resolve(sdk,'.emscripten')});
    verify(name+'-wasm',kind,run(name+'-wasm-execute',process.execPath,[loader]));receipt.sources[name+'-wasm']=hash(resolve(out,name+'.wasm'));
   }
  }
 }
 receipt.passed=true;
}catch(error){receipt.error=String(error);process.stderr.write(receipt.error+'\n');process.exitCode=1;}
finally{receipt.completedAt=new Date().toISOString();writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');console.log('Queue evidence: '+out);}
