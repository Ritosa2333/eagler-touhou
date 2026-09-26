import {fork,spawn} from 'node:child_process';
import {createServer} from 'node:net';
import {createHash,randomUUID} from 'node:crypto';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'../..');
const correction=process.argv.includes('--native-correction');
const product=process.argv.includes('--product');
const loadouts=process.argv.includes('--world-loadouts');
const worldJournal=process.argv.includes('--world-journal')||loadouts||correction;
const enemyJournal=process.argv.includes('--enemy-journal')||worldJournal||product;
const variant=enemyJournal?'multiplayer-fixtures':'multiplayer';
const buildRoot=resolve(root,'th08_web/artifacts',variant);
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json')));
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const caseScript=product?'check-product.py':correction?'check-correction.py':worldJournal?'check-world-journal.py':enemyJournal?'check-enemy-journal.py':'check-admission.py';
const harness=Object.fromEntries(['check-admission.mjs',caseScript,'smoke.mjs','serve.mjs'].map(name=>[name,sha(resolve(import.meta.dirname,name))]));
const inventory=()=>{if(build.variant!==variant||sha(resolve(buildRoot,'th08-sdl.wasm'))!==build.sha256)throw Error('Wrong/stale TH08 MP build');
 for(const [name,hash] of Object.entries(build.sourceFiles))if(sha(resolve(root,name))!==hash)throw Error('Source changed: '+name);
 for(const [name,hash] of Object.entries(harness))if(sha(resolve(import.meta.dirname,name))!==hash)throw Error('Harness changed during verification: '+name);};
inventory();
const port=await new Promise((done,reject)=>{const s=createServer();s.once('error',reject);s.listen(0,'127.0.0.1',()=>{const p=s.address().port;s.close(error=>error?reject(error):done(p));});});
const out=resolve(root,'artifacts/multiplayer-tests/'+(product?'product-':correction?'native-correction-':worldJournal?'world-journal-':enemyJournal?'enemy-journal-':'admission-')+randomUUID());mkdirSync(out,{recursive:true});
const report={passed:false,wasm:build.sha256,harness};let server;
try{
 server=fork(resolve(import.meta.dirname,'serve.mjs'),[],{cwd:root,env:{...process.env,PORT:String(port),TH08_MP_FIXTURES:enemyJournal?'1':'0'},stdio:['ignore','pipe','pipe','ipc']});
 server.stdout.on('data',v=>process.stdout.write(v));server.stderr.on('data',v=>process.stderr.write(v));
 await new Promise((done,reject)=>{const timer=setTimeout(()=>reject(Error('Server startup timeout')),15000);server.once('error',reject);server.once('exit',code=>reject(Error('Server exited '+code)));server.on('message',v=>{if(v?.type==='ready'){clearTimeout(timer);done();}});});
 const output=resolve(out,'cases.json');
 report.process=await new Promise((done,reject)=>{const c=spawn(process.env.TH_PYTHON||'python',['-u',resolve(import.meta.dirname,caseScript),'--url','http://127.0.0.1:'+port+'/','--output',output,...(loadouts?['--loadouts']:[])],{cwd:root,stdio:['ignore','pipe','pipe'],windowsHide:true});
 c.stdout.on('data',v=>process.stdout.write(v));c.stderr.on('data',v=>process.stderr.write(v));c.once('error',reject);c.once('exit',(code,signal)=>done({code,signal}));});
 const evidence=JSON.parse(readFileSync(output));
 const identities=product||worldJournal?evidence.identities:enemyJournal?[evidence.identity]:evidence.identities;
 if(report.process.code!==0||!evidence.passed||identities?.length!==(product?2:correction?5:worldJournal?(loadouts?20:8):enemyJournal?1:5)||identities.some(x=>!x||x.wasmSha256!==build.sha256||!!x.fixtureBuild!==enemyJournal))throw Error('Native admission/owner test failed or mismatched identity');
 inventory();report.passed=true;
}catch(error){report.error=String(error);process.exitCode=1;}
finally{server?.kill();writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({passed:report.passed,report:resolve(out,'suite.json'),error:report.error}));}
