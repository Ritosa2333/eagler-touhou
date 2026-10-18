import {fork,spawn} from 'node:child_process';
import {createServer} from 'node:net';
import {createHash,randomUUID} from 'node:crypto';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';

const root=resolve(import.meta.dirname,'../..');
const workspace=resolve(process.env.EAGLER_WORKSPACE||root+'/../..');
const launcher=resolve(process.env.EAGLER_LAUNCHER_ROOT||resolve(workspace,'worktrees/eagler-multiplayer'));
const fixtures=process.argv.includes('--fixtures'),profile=fixtures?'multiplayer-fixtures':'multiplayer';
const buildRoot=resolve(root,'th10_web/artifacts',profile);
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json')));
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
if(build.profile!==profile||sha(resolve(buildRoot,'th10-sdl.wasm'))!==build.sha256)throw Error('Wrong/stale spectator build');
for(const [name,hash] of Object.entries(build.sourceFiles))if(sha(resolve(root,name))!==hash)throw Error('Source changed: '+name);
const only=process.argv.find(arg=>arg.startsWith('--only='))?.slice(7);
const modes=['rtc','relay'].filter(value=>!only||value===only);if(!modes.length)throw Error('Unknown spectator case');
const out=resolve(root,'artifacts/multiplayer-tests/spectator-'+randomUUID());mkdirSync(out,{recursive:true});
const report={passed:false,profile,wasm:build.sha256,sourceDigest:build.sourceDigest,cases:[]};
const freePort=()=>new Promise((done,reject)=>{const server=createServer();server.once('error',reject);server.listen(0,'127.0.0.1',()=>{const value=server.address().port;server.close(error=>error?reject(error):done(value));});});
const httpPort=await freePort(),relayPort=await freePort();let server,relay;
try{
 relay=fork(resolve(launcher,'server/netplay-relay.mjs'),[],{cwd:launcher,env:{...process.env,
  EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(relayPort),
  EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe','ipc']});
 relay.stderr.on('data',bytes=>process.stderr.write(bytes));
 await new Promise((done,reject)=>{const timer=setTimeout(()=>reject(Error('Relay startup timeout')),15000);
  relay.once('error',reject);relay.once('exit',code=>reject(Error('Relay exited '+code)));
  relay.stdout.on('data',bytes=>{if(String(bytes).includes('relay listening')){clearTimeout(timer);done();}});});
 server=fork(resolve(import.meta.dirname,'serve.mjs'),[],{cwd:root,env:{...process.env,PORT:String(httpPort),TH10_MP_PROFILE:profile},stdio:['ignore','pipe','pipe','ipc']});
 server.stdout.on('data',bytes=>process.stdout.write(bytes));server.stderr.on('data',bytes=>process.stderr.write(bytes));
 await new Promise((done,reject)=>{const timer=setTimeout(()=>reject(Error('MP server startup timeout')),15000);
  server.once('error',reject);server.once('exit',code=>reject(Error('MP server exited '+code)));
  server.on('message',message=>{if(message?.type==='ready'){clearTimeout(timer);done();}});});
 for(const mode of modes){
  console.log('BEGIN spectator-'+mode);
  const output=resolve(out,mode+'.json');
  const result=await new Promise((done,reject)=>{
   const child=spawn(process.env.TH_PYTHON||'python',['-u',resolve(import.meta.dirname,'check-spectator.py'),
    '--url','http://127.0.0.1:'+httpPort+'/','--relay','ws://127.0.0.1:'+relayPort,
    '--output',output,'--mode',mode],{cwd:root,stdio:['ignore','pipe','pipe'],windowsHide:true});
   child.stdout.on('data',bytes=>process.stdout.write(bytes));child.stderr.on('data',bytes=>process.stderr.write(bytes));
   child.once('error',reject);child.once('exit',(code,signal)=>done({code,signal}));
  });
  report.cases.push({mode,...result,report:output});writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2)+'\n');
  if(result.code!==0||!JSON.parse(readFileSync(output)).passed)throw Error('spectator '+mode+' failed');
 }
 report.passed=true;
}catch(error){report.error=String(error);process.exitCode=1;}
finally{server?.kill();relay?.kill();writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2)+'\n');
 console.log(JSON.stringify({passed:report.passed,report:resolve(out,'suite.json'),error:report.error}));}
