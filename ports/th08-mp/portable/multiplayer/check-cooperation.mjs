// Own the diagnostic server and identify the exact native artifact under test.
import {fork,spawn} from 'node:child_process';
import {createServer} from 'node:net';
import {createHash,randomUUID} from 'node:crypto';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'../..');
const selected=process.argv.find(arg=>arg.startsWith('--case='))?.slice(7)||'all';
const buildRoot=resolve(root,'th08_web/artifacts/multiplayer-fixtures');
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json')));
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
if(build.variant!=='multiplayer-fixtures'||sha(resolve(buildRoot,'th08-sdl.wasm'))!==build.sha256)throw Error('Wrong/stale fixture build');
for(const [name,hash] of Object.entries(build.sourceFiles))if(sha(resolve(root,name))!==hash)throw Error('Source changed: '+name);
const out=resolve(root,'artifacts/multiplayer-tests/cooperation-'+randomUUID());mkdirSync(out,{recursive:true});
const report={passed:false,wasm:build.sha256,sourceDigest:build.sourceDigest,case:selected};
const port=await new Promise((done,reject)=>{const p=createServer();p.once('error',reject);p.listen(0,'127.0.0.1',()=>{const value=p.address().port;p.close(error=>error?reject(error):done(value));});});
let server;
try{
 server=fork(resolve(import.meta.dirname,'serve.mjs'),[],{cwd:root,env:{...process.env,PORT:String(port),TH08_MP_FIXTURES:'1'},stdio:['ignore','pipe','pipe','ipc']});
 server.stdout.on('data',bytes=>process.stdout.write(bytes));server.stderr.on('data',bytes=>process.stderr.write(bytes));
 await new Promise((done,reject)=>{const timer=setTimeout(()=>reject(Error('Fixture server timeout')),15000);
  server.once('error',reject);server.once('exit',code=>reject(Error('Fixture server exited '+code)));
  server.on('message',message=>{if(message?.type==='ready'){clearTimeout(timer);done();}});
 });
 const output=resolve(out,'cases.json');
 report.process=await new Promise((done,reject)=>{
  const child=spawn(process.env.TH_PYTHON||'python',['-u',resolve(import.meta.dirname,'check-cooperation.py'),
   '--url','http://127.0.0.1:'+port+'/','--output',output,'--case',selected],{cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe']});
  child.stdout.on('data',bytes=>process.stdout.write(bytes));child.stderr.on('data',bytes=>process.stderr.write(bytes));
  child.once('error',reject);child.once('exit',(code,signal)=>done({code,signal}));
 });
 const evidence=JSON.parse(readFileSync(output));
 if(report.process.code!==0||!evidence.passed)throw Error('Native cooperation failed');
 if(!evidence.runtimeIdentities?.length||evidence.runtimeIdentities.some(value=>value.wasmSha256!==build.sha256||!value.fixtureBuild))throw Error('Mixed candidate identities');
 for(const [name,hash] of Object.entries(build.sourceFiles))if(sha(resolve(root,name))!==hash)throw Error('Source changed during test: '+name);
 report.passed=true;
}catch(error){report.error=String(error);process.exitCode=1;}
finally{server?.kill();writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({passed:report.passed,report:resolve(out,'suite.json'),error:report.error}));}
