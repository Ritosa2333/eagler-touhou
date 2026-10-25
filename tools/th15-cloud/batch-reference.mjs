// Reuse the existing seven browser cases, but serve the frozen pre-merge GPU
// renderer to the exact-input peer. Candidate and reference own separate Wasm
// instances; no reference path calls the candidate RendererBatch.inc.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync,rmSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {runCloudCommand} from './cloud-command-runner.mjs';
const repo=resolve(import.meta.dirname,'../..'),port=resolve(repo,'ports/th15-mp');
const referenceRepo=resolve(repo,'.batch-reference'),reference=resolve(referenceRepo,'ports/th15-mp/web/artifacts/sdl-multiplayer/rollback');
const out=resolve(repo,'artifacts/th15-batch-reference');mkdirSync(out,{recursive:true});
const sha=b=>createHash('sha256').update(b).digest('hex');
const file=p=>({path:p,sha256:sha(readFileSync(p))});
const receipt={schema:'th15-frozen-renderer-browser-reference/1',head:process.env.GITHUB_SHA??null,startedAt:new Date().toISOString(),
 baseline:'b7e7bed8a9c5f1161f8fececdc7c0026300d8a40',scope:{gpu:true,softwareRenderer:true,userHardware:false,performance:false},commands:[],cases:[],passed:false};
const save=()=>writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');
function run(name,args,env={}){
 const result=runCloudCommand(process.execPath,args,{cwd:repo,env:{...process.env,...env},log:resolve(out,name+'.log'),timeout:1200000});
 receipt.commands.push({name,...result});save();
 if(!result.cleanup.verified)throw Error('Unverified child cleanup '+name);
 if(result.error||result.exitCode!==0)throw Error(name+' failed');
}
const driver=resolve(port,'portable/multiplayer/rollback-browser.mjs');
const temporary=resolve(port,'portable/multiplayer/.batch-reference-'+process.pid+'.mjs');
try{
 for(const name of ['TH15_TEST_ARCHIVE','TH15_TEST_FONT','TH15_TEST_UNICODE_FONT','EAGLER_FONT_ROOT','EAGLER_CHROME_PATH','TH15_EMSDK'])assert.ok(process.env[name],'Missing '+name);
 run('build-candidate-fixture',[resolve(port,'portable/multiplayer/build-rollback.mjs')]);
 run('build-reference-fixture',[resolve(referenceRepo,'ports/th15-mp/portable/multiplayer/build-rollback.mjs')]);
 receipt.fixture={candidate:JSON.parse(readFileSync(resolve(port,'web/artifacts/sdl-multiplayer/rollback/build.json'))),reference:JSON.parse(readFileSync(resolve(reference,'build.json')))};
 receipt.sourceDriver=file(driver);receipt.referenceFiles=['th15.mjs','th15.wasm','build.json'].map(n=>file(resolve(reference,n)));
 for(const pair of [
  [resolve(port,'web/artifacts/sdl-multiplayer/rollback'),receipt.fixture.candidate],
  [reference,receipt.fixture.reference]]){
   assert.equal(sha(readFileSync(resolve(pair[0],'th15.wasm'))),pair[1].wasmSha256);
   assert.equal(sha(readFileSync(resolve(pair[0],'th15.mjs'))),pair[1].loaderSha256);
 }
 let source=readFileSync(driver,'utf8');
 function replace(before,after){assert.equal(source.split(before).length,2,'One frozen driver anchor required: '+before.slice(0,100));source=source.replace(before,after);}
 replace("import createModule from '/th15.mjs';", "const {default:createModule}=await import(new URL(location.href).searchParams.has('reference')?'/reference/th15.mjs':'/th15.mjs');");
 replace("await page.goto(base+'/?scale='+scale);", "await page.goto(base+'/?scale='+scale+(mode===1?'&reference=1':''));");
 replace("if(!['/th15.mjs','/th15.wasm'].includes(path)){", `if(['/reference/th15.mjs','/reference/th15.wasm'].includes(path)){res.setHeader('Content-Type',path.endsWith('.mjs')?'application/javascript':'application/wasm');res.end(await readFile(resolve(${JSON.stringify(reference)},path.slice('/reference/'.length))));return;}\n if(!['/th15.mjs','/th15.wasm'].includes(path)){`);
 replace("oracle:'independent v7 pure-delay exact-input WASM'", "oracle:'frozen b7e7bed GPU renderer; independent exact-input WASM'");
 receipt.generatedDriverSha256=sha(Buffer.from(source));writeFileSync(temporary,source,{flag:'wx'});save();
 for(const [name,flags,groups] of [['rollback',[],5],['lifecycle',['--lifecycle-only'],1],['presentation',['--presentation-toggle'],1]]){
  const caseOut=resolve(out,name);run(name,[temporary,...flags,'--output='+caseOut]);
  const resultFile=resolve(caseOut,'report.json');const result=JSON.parse(readFileSync(resultFile));
  assert.equal(result.status,'PASS');assert.equal(result.reports.length,groups);
  receipt.cases.push({name,groups,report:file(resultFile)});save();
 }
 assert.equal(receipt.cases.reduce((n,c)=>n+c.groups,0),7);
 assert.equal(file(driver).sha256,receipt.sourceDriver.sha256,'Canonical driver changed');
 for(const f of receipt.referenceFiles)assert.equal(file(f.path).sha256,f.sha256,'Frozen reference changed');
 receipt.passed=true;
}catch(error){receipt.error=String(error);process.stderr.write(receipt.error+'\n');process.exitCode=1;}
finally{rmSync(temporary,{force:true});receipt.completedAt=new Date().toISOString();save();console.log(JSON.stringify({out,passed:receipt.passed,groups:receipt.cases.reduce((n,c)=>n+c.groups,0),error:receipt.error}));}
