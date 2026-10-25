// Numeric contract against the frozen preceding camera implementation.
// Production game objects are used; this is not GPU or gameplay acceptance.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync,mkdtempSync,existsSync} from 'node:fs';
import {spawnSync} from 'node:child_process';
import {resolve,basename} from 'node:path';
import {createHash} from 'node:crypto';
const variant=process.argv.find(a=>a.startsWith('--variant='))?.slice(10);
assert.ok(['normal','multiplayer'].includes(variant),'Use --variant=normal or --variant=multiplayer');
const multiplayer=variant==='multiplayer',repo=resolve(import.meta.dirname,'../..');
const root=resolve(repo,'ports',multiplayer?'th15-mp':'th15'),production=resolve(root,'web/artifacts',multiplayer?'sdl-multiplayer':'sdl-release');
const artifacts=resolve(repo,'artifacts/th15-camera');mkdirSync(artifacts,{recursive:true});const out=mkdtempSync(resolve(artifacts,variant+'-'));
const hash=b=>createHash('sha256').update(b).digest('hex');
const receipt={schema:'th15-camera-contract/1',head:process.env.GITHUB_SHA??null,variant,startedAt:new Date().toISOString(),commands:[],passed:false,scope:{wasm:true,productionGameObjects:true,gpu:false,gameplay:false}};
let sdk;
function run(name,program,args){
 const p=spawnSync(program,args,{cwd:root,env:{...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),EMCC_CORES:'1'},encoding:'utf8',timeout:180000,maxBuffer:4*1024*1024});
 writeFileSync(resolve(out,name+'.log'),(p.stdout||'')+(p.stderr||''));receipt.commands.push({name,program,args,exitCode:p.status,signal:p.signal,error:p.error?String(p.error):null});
 if(p.error||p.status!==0)throw Error(name+' failed: '+String(p.error||p.stderr||p.stdout));return p.stdout;
}
try{
 assert.ok(process.env.TH15_EMSDK||process.env.EMSDK,'Set TH15_EMSDK or EMSDK');sdk=resolve(process.env.TH15_EMSDK||process.env.EMSDK);
 const compiler=['install','upstream'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);assert.ok(compiler,'Missing selected compiler');
 const build=JSON.parse(readFileSync(resolve(production,'build.json'),'utf8'));assert.equal(build.kind,'th15-sdl3-application-release');assert.equal(build.features.multiplayer,multiplayer);
 for(const [p,sha] of Object.entries({...build.sources,...build.headers}))assert.equal(hash(readFileSync(resolve(root,p))),sha,'Changed release input '+p);
 assert.equal(hash(readFileSync(resolve(production,'th15.wasm'))),build.sha256);receipt.productionWasm=build.sha256;
 const frozen=readFileSync(resolve(import.meta.dirname,'camera-reference.inc'));
 receipt.referenceBlob=createHash('sha1').update('blob '+frozen.length+'\0').update(frozen).digest('hex');assert.equal(receipt.referenceBlob,'feb788ccb4558a0e9b773bf0147be662807e16ae');
 receipt.sources=Object.fromEntries(['camera-check.cpp','camera-reference.cpp','camera-reference.inc'].map(p=>[p,hash(readFileSync(resolve(import.meta.dirname,p)))]));
 const sources=Object.keys(build.sources),deps=multiplayer?['WriteJournal.cpp','SimulationHeap.cpp'].map(name=>{const found=sources.filter(p=>basename(p)===name);assert.equal(found.length,1);return found[0];}):[];
 const objects=[...sources.filter(p=>p.startsWith('web/cpp/game/')),...deps].map(p=>resolve(production,'objects',p.replaceAll('/','_')+'.o'));
 for(const p of objects)assert.ok(existsSync(p),'Missing release object '+p);receipt.productionObjectCount=objects.length;
 const flags=['-O2','-std=c++17','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-DTH_NATIVE_PLATFORM=1','-DTH15_MULTIPLAYER='+Number(multiplayer),'-DTH15_WRITE_JOURNAL='+Number(multiplayer),'-DEAGLER_WRITE_JOURNAL='+Number(multiplayer),'-DTH_ENABLE_THPRAC='+Number(build.features.thprac===true),'-I'+resolve(root,'web/cpp/game'),'-I'+resolve(repo,'common/include')];
 const fixtures=[];
 for(const name of ['camera-check','camera-reference']){const object=resolve(out,name+'.o');run('compile-'+name,process.env.PYTHON||'python',[compiler,...flags,'-c',resolve(import.meta.dirname,name+'.cpp'),'-o',object]);fixtures.push(object);}
 const loader=resolve(out,'camera.cjs'),rsp=resolve(out,'link.rsp');writeFileSync(rsp,[...flags,'--use-port=sdl3','-sDEFAULT_TO_CXX=1','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=8388608','-sINITIAL_MEMORY=134217728',...objects,...fixtures,'-o',loader].map(v=>JSON.stringify(v)).join('\n'));
 run('link',process.env.PYTHON||'python',[compiler,'@'+rsp]);receipt.wasmSha256=hash(readFileSync(resolve(out,'camera.wasm')));receipt.loaderSha256=hash(readFileSync(loader));
 const stdout=run('execute',process.execPath,[loader]);process.stdout.write(stdout);const line=stdout.split('\n').find(s=>s.startsWith('TH15_CAMERA_REFERENCE '));assert.ok(line,'Missing executed math result');
 receipt.result=JSON.parse(line.slice('TH15_CAMERA_REFERENCE '.length));assert.equal(receipt.result.cases,100000);assert.equal(receipt.result.checks,7400001);assert.equal(receipt.result.bitwiseVertices,true);assert.equal(receipt.result.parentPostState,true);receipt.passed=true;
}catch(error){receipt.error=String(error);process.stderr.write(receipt.error+'\n');process.exitCode=1;}
finally{receipt.completedAt=new Date().toISOString();writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');console.log('Camera evidence: '+out);}
