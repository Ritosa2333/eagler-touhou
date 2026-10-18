import {fork,spawn} from 'node:child_process';
import {createServer} from 'node:net';
import {createHash,randomUUID} from 'node:crypto';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';

const root=resolve(import.meta.dirname,'../..');
const workspace=resolve(process.env.EAGLER_WORKSPACE||root+'/../..');
const launcher=resolve(process.env.EAGLER_LAUNCHER_ROOT||resolve(workspace,'eagler-touhou'));
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const retry=process.argv.includes('--retry'),death=process.argv.includes('--p1-death');
const rescueDeath=process.argv.includes('--p1-rescue-death');
const profile=retry||death||rescueDeath?'multiplayer-fixtures':'multiplayer';
const buildRoot=resolve(root,'th10_web/artifacts',profile),build=JSON.parse(readFileSync(resolve(buildRoot,'build.json')));
if(build.profile!==profile||sha(resolve(buildRoot,'th10-sdl.wasm'))!==build.sha256)throw Error('Wrong/stale network build');
for(const [name,hash] of Object.entries(build.sourceFiles))if(sha(resolve(root,name))!==hash)throw Error('Source changed: '+name);
const only=process.argv.find(arg=>arg.startsWith('--only='))?.slice(7);
const cases=[['rtc-2p','rtc',2,false],['rtc-3p-loss','rtc',3,true],['relay-2p','relay',2,false],['relay-3p','relay',3,false]]
 .filter(value=>(!(retry||death||rescueDeath)||value[2]===2)&&(!only||value[0]===only));
if(!cases.length)throw Error('Unknown network case');
const out=resolve(root,'artifacts/multiplayer-tests/network-'+randomUUID());mkdirSync(out,{recursive:true});
const report={passed:false,wasm:build.sha256,sourceDigest:build.sourceDigest,retry,p1Death:death,p1RescueDeath:rescueDeath,profile,cases:[]};
const port=()=>new Promise((done,reject)=>{const server=createServer();server.once('error',reject);server.listen(0,'127.0.0.1',()=>{const value=server.address().port;server.close(error=>error?reject(error):done(value));});});
const httpPort=await port(),relayPort=await port();let server,relay;
try{
 relay=fork(resolve(launcher,'server/netplay-relay.mjs'),[],{cwd:launcher,
  env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(relayPort),
   EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_RELAY_DROP_FIRST_INPUT_PER_EDGE:'1'},stdio:['ignore','pipe','pipe','ipc']});
 relay.stderr.on('data',bytes=>process.stderr.write(bytes));
 await new Promise((done,reject)=>{const timer=setTimeout(()=>reject(Error('Relay startup timeout')),15000);
  relay.once('error',reject);relay.once('exit',code=>reject(Error('Relay exited '+code)));
  relay.stdout.on('data',bytes=>{const text=String(bytes);if(text.includes('relay listening')){clearTimeout(timer);done();}});
 });
 server=fork(resolve(import.meta.dirname,'serve.mjs'),[],{cwd:root,env:{...process.env,PORT:String(httpPort),TH10_MP_PROFILE:profile},stdio:['ignore','pipe','pipe','ipc']});
 server.stdout.on('data',bytes=>process.stdout.write(bytes));server.stderr.on('data',bytes=>process.stderr.write(bytes));
 await new Promise((done,reject)=>{const timer=setTimeout(()=>reject(Error('MP server startup timeout')),15000);
  server.once('error',reject);server.once('exit',code=>reject(Error('MP server exited '+code)));
  server.on('message',message=>{if(message?.type==='ready'){clearTimeout(timer);done();}});
 });
 for(const [name,mode,players,drop] of cases){
  console.log('BEGIN',name);
  const output=resolve(out,name+'.json');
  const result=await new Promise((done,reject)=>{
   const child=spawn(process.env.TH_PYTHON||'python',['-u',resolve(import.meta.dirname,'check-network.py'),
    '--url','http://127.0.0.1:'+httpPort+'/','--relay','ws://127.0.0.1:'+relayPort,
    '--output',output,'--mode',mode,'--players',String(players),...(drop?['--drop-fast']:[]),...(retry?['--retry']:[]),...(death?['--p1-death']:[]),...(rescueDeath?['--p1-rescue-death']:[])],
    {cwd:root,stdio:['ignore','pipe','pipe'],windowsHide:true});
   child.stdout.on('data',bytes=>process.stdout.write(bytes));child.stderr.on('data',bytes=>process.stderr.write(bytes));
   child.once('error',reject);child.once('exit',(code,signal)=>done({code,signal}));
  });
  report.cases.push({name,...result,report:output});
  writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2)+'\n');
  if(result.code!==0||!JSON.parse(readFileSync(output)).passed)throw Error(name+' failed');
 }
 for(const [name,hash] of Object.entries(build.sourceFiles))if(sha(resolve(root,name))!==hash)throw Error('Source changed during network test: '+name);
 report.passed=true;
}catch(error){report.error=String(error);process.exitCode=1;}
finally{server?.kill();relay?.kill();writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({passed:report.passed,report:resolve(out,'suite.json'),error:report.error}));}
