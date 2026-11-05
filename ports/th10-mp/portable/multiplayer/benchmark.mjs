// Serial, immutable Runtime A/B runs. Every invocation keeps failures and raw
// evidence; neither the game nor a golden corpus is changed by the benchmark.
import {fork,spawn} from 'node:child_process';
import {createServer} from 'node:net';
import {createHash,randomUUID} from 'node:crypto';
import {readFileSync,writeFileSync,mkdirSync,copyFileSync} from 'node:fs';
import {resolve} from 'node:path';

const root=resolve(import.meta.dirname,'../..'),argv=process.argv.slice(2);
const label=argv.find(a=>a.startsWith('--label='))?.slice(8);
const nativeCase=argv.find(a=>a.startsWith('--native-case='))?.slice(14);
const local=argv.includes('--local-dense')||!!nativeCase;
if(!label||!/^[-a-zA-Z0-9_]+$/.test(label))throw Error('Supply --label=FROZEN_RUNTIME_LABEL');
const workspace=resolve(process.env.EAGLER_WORKSPACE||root+'/../..');
const launcher=resolve(workspace,'eagler-touhou');
const runtime=resolve(root,'artifacts/rollback-performance',label,'runtime');
const frozen=JSON.parse(readFileSync(resolve(runtime,'frozen.json')));
const profile=frozen.profile||'multiplayer-fixtures';
if(!['multiplayer','multiplayer-fixtures'].includes(profile))throw Error('Invalid frozen profile');
if(profile==='multiplayer'&&(local||argv.includes('--dense')))
 throw Error('Fixture workload cannot run on the production Runtime');
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const validate=()=>{for(const [file,hash] of Object.entries(frozen.files))
 if(sha(resolve(runtime,file))!==hash)throw Error('Frozen Runtime changed: '+file);};
validate();
const out=resolve(root,'artifacts/rollback-performance/runs',label+'-'+randomUUID());
const fixture=resolve(out,'fixture');mkdirSync(fixture,{recursive:true});
const fixtureFiles=['smoke.html','smoke.mjs','performance-driver.mjs','replay-robot.mjs','replay-audio-seek.mjs'];
const sources={};
for(const file of [...fixtureFiles,'check-performance.py','check-dense.py','check-native-fixtures.py','rollback_testkit.py','benchmark.mjs','serve.mjs']){
 const from=resolve(import.meta.dirname,file),to=resolve(fixture,file);
 copyFileSync(from,to);sources[file]=sha(to);
}
const report={passed:false,label,kind:nativeCase?'native-fixture':local?'dense-fixed-input':'native-raf-real-rtc',frozen,sources};
const port=()=>new Promise((done,reject)=>{const probe=createServer();probe.once('error',reject);
 probe.listen(0,'127.0.0.1',()=>{const value=probe.address().port;probe.close(e=>e?reject(e):done(value));});});
let server,relay;
try{
 const httpPort=await port(),relayPort=local?null:await port();
 if(!local){
  relay=fork(resolve(launcher,'server/netplay-relay.mjs'),[],{cwd:launcher,
   env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(relayPort),
    EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe','ipc']});
  relay.stderr.on('data',b=>process.stderr.write(b));
  await new Promise((done,reject)=>{const timer=setTimeout(()=>reject(Error('Relay startup timeout')),15000);
   relay.once('error',reject);relay.once('exit',c=>reject(Error('Relay exited '+c)));
   relay.stdout.on('data',b=>{if(String(b).includes('relay listening')){clearTimeout(timer);done();}});});
 }
 server=fork(resolve(import.meta.dirname,'serve.mjs'),[],{cwd:root,
  env:{...process.env,PORT:String(httpPort),EAGLER_WORKSPACE:workspace,TH10_MP_PROFILE:profile,
   TH10_MP_BUILD_ROOT:runtime,TH10_MP_FIXTURE_ROOT:fixture},stdio:['ignore','pipe','pipe','ipc']});
 server.stdout.on('data',b=>process.stdout.write(b));server.stderr.on('data',b=>process.stderr.write(b));
 await new Promise((done,reject)=>{const timer=setTimeout(()=>reject(Error('Fixture startup timeout')),15000);
  server.once('error',reject);server.once('exit',c=>reject(Error('Fixture exited '+c)));
  server.on('message',m=>{if(m?.type==='ready'){clearTimeout(timer);done();}});});
 const output=resolve(out,'result.json');
 const extra=argv.filter(a=>!a.startsWith('--label=')&&!a.startsWith('--native-case=')&&a!=='--local-dense');
 const code=await new Promise((done,reject)=>{
  const child=spawn(process.env.TH_PYTHON||'python',['-u',resolve(import.meta.dirname,nativeCase?'check-native-fixtures.py':local?'check-dense.py':'check-performance.py'),
   '--url','http://127.0.0.1:'+httpPort+'/',...(!local?['--relay','ws://127.0.0.1:'+relayPort]:[]),
   '--output',output,...(nativeCase?['--case',nativeCase]:[]),...extra],{cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe']});
  let stdout='',stderr='';
  child.stdout.on('data',b=>{stdout+=b;process.stdout.write(b);});child.stderr.on('data',b=>{stderr+=b;process.stderr.write(b);});
  child.once('error',reject);child.once('exit',code=>{report.stdout=stdout;report.stderr=stderr;done(code);});
 });
 report.result=output;report.code=code;validate();
 for(const [file,hash] of Object.entries(sources))
  if(sha(resolve(import.meta.dirname,file))!==hash)throw Error('Harness changed during benchmark: '+file);
 if(code!==0||!JSON.parse(readFileSync(output)).passed)throw Error('Benchmark failed; raw report retained');
 report.passed=true;
}catch(error){report.error=String(error);process.exitCode=1;}
finally{
 server?.kill();relay?.kill();writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2)+'\n');
 console.log(JSON.stringify({passed:report.passed,report:resolve(out,'suite.json'),error:report.error}));
}
