// Link the actual release game/journal objects, then instrument and execute.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync,existsSync,renameSync,mkdtempSync} from 'node:fs';
import {spawnSync} from 'node:child_process';
import {resolve,basename} from 'node:path';
import {createHash} from 'node:crypto';
const repo=resolve(import.meta.dirname,'../..'),root=resolve(repo,'ports/th15-mp');
const production=resolve(root,'web',process.env.TH15_OUTPUT||'artifacts/sdl-multiplayer');
const artifacts=resolve(repo,'artifacts/th15-draw-scratch');mkdirSync(artifacts,{recursive:true});
const out=mkdtempSync(resolve(artifacts,'wasm-')),hash=b=>createHash('sha256').update(b).digest('hex');
const receipt={schema:'th15-draw-scratch/1',head:process.env.GITHUB_SHA??null,startedAt:new Date().toISOString(),commands:[],passed:false,scope:{instrumentedWasm:true,productionObjects:true,syntheticResources:true,gpu:false,fps:false}};
function run(name,program,args){
 const result=spawnSync(program,args,{cwd:root,env:{...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),EMCC_CORES:'1'},encoding:'utf8',timeout:180000,maxBuffer:8*1024*1024});
 writeFileSync(resolve(out,name+'.log'),(result.stdout||'')+(result.stderr||''));
 receipt.commands.push({name,program,args,exitCode:result.status,signal:result.signal,error:result.error?String(result.error):null});
 if(result.error||result.status!==0)throw Error(name+' failed: '+String(result.error||result.stderr||result.stdout));
 return result.stdout;
}
let sdk;
try{
 assert.ok(process.env.TH15_EMSDK||process.env.EMSDK,'Set TH15_EMSDK or EMSDK');sdk=resolve(process.env.TH15_EMSDK||process.env.EMSDK);
 const compiler=['install','upstream'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);assert.ok(compiler,'Missing Emscripten compiler');
 const build=JSON.parse(readFileSync(resolve(production,'build.json'),'utf8'));receipt.productionWasm=build.sha256;
 assert.equal(build.netplay?.stateBackend,'write-journal-32');assert.equal(build.netplay.inlineDirtyBlocks,false);assert.equal(build.netplay.scalarHooks,false);
 assert.equal(hash(readFileSync(resolve(production,'th15.wasm'))),build.sha256);
 for(const [path,sha] of Object.entries({...build.sources,...build.headers}))assert.equal(hash(readFileSync(resolve(root,path))),sha,'Changed release input '+path);
 const files=Object.keys(build.sources),dependencies=['WriteJournal.cpp','SimulationHeap.cpp'].map(name=>{
  const paths=files.filter(p=>basename(p)===name);assert.equal(paths.length,1,'Ambiguous release dependency '+name);return paths[0];});
 const objects=[...files.filter(p=>p.startsWith('web/cpp/game/')),...dependencies].map(p=>resolve(production,'objects',p.replaceAll('/','_')+'.o'));
 for(const p of objects)assert.ok(existsSync(p),'Missing release object '+p);
 const source=resolve(import.meta.dirname,'draw-scratch-check.cpp');receipt.sourceSha256=hash(readFileSync(source));receipt.productionObjectCount=objects.length;
 const flags=['-O2','-std=c++17','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-DTH_NATIVE_PLATFORM=1','-DTH15_MULTIPLAYER=1','-DTH15_WRITE_JOURNAL=1','-DEAGLER_WRITE_JOURNAL=1','-DTH_ENABLE_THPRAC=0','-I'+resolve(root,'web/cpp/game'),'-I'+resolve(repo,'common/include')];
 const fixture=resolve(out,'fixture.o'),loader=resolve(out,'scratch.cjs'),wasm=resolve(out,'scratch.wasm'),instrumented=resolve(out,'instrumented.wasm');
 run('compile-fixture',process.env.PYTHON||'python',[compiler,...flags,'-c',source,'-o',fixture]);
 const rsp=resolve(out,'link.rsp');writeFileSync(rsp,[...flags,'--use-port=sdl3','-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=8388608','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824',...objects,fixture,'-o',loader].map(v=>JSON.stringify(v)).join('\n'));
 run('link',process.env.PYTHON||'python',[compiler,'@'+rsp]);
 const pass=resolve(repo,'tools/rollback/instrument.mjs');run('instrument',process.execPath,[pass,'--native-filter',wasm,instrumented]);renameSync(instrumented,wasm);
 run('verify',process.execPath,[pass,'--verify','--native-filter',wasm]);receipt.wasmSha256=hash(readFileSync(wasm));receipt.loaderSha256=hash(readFileSync(loader));
 const output=run('execute',process.execPath,[loader]);process.stdout.write(output);
 const line=output.split('\n').find(s=>s.startsWith('TH15_DRAW_SCRATCH '));assert.ok(line,'No executed ownership result');
 receipt.result=JSON.parse(line.slice('TH15_DRAW_SCRATCH '.length));assert.ok(receipt.result.checks>=400);assert.equal(receipt.result.growthIterations,18);assert.equal(receipt.result.parentUndo,true);assert.equal(receipt.result.errorUndo,true);assert.equal(receipt.result.unfinishedBatchDiscard,true);receipt.passed=true;
}catch(error){receipt.error=String(error);process.stderr.write(receipt.error+'\n');process.exitCode=1;}
finally{receipt.completedAt=new Date().toISOString();writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');console.log('Draw scratch evidence: '+out);}
