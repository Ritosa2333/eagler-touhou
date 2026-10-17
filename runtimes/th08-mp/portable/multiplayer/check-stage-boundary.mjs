import {fork,spawn} from 'node:child_process';
import {createServer} from 'node:net';
import {createHash,randomUUID} from 'node:crypto';
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'../..');
const buildRoot=resolve(root,'th08_web/artifacts/multiplayer-fixtures');
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json')));
const hash=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const verify=()=>{if(build.variant!=='multiplayer-fixtures'||hash(resolve(buildRoot,'th08-sdl.wasm'))!==build.sha256)throw Error('Mismatched build');for(const [name,sha] of Object.entries(build.sourceFiles))if(hash(resolve(root,name))!==sha)throw Error('Stale source: '+name);};
verify();
const free=()=>new Promise((ok,fail)=>{const s=createServer();s.once('error',fail);s.listen(0,'127.0.0.1',()=>{const p=s.address().port;s.close(e=>e?fail(e):ok(p));});});
const out=resolve(root,'artifacts/multiplayer-tests/stage-boundary-'+randomUUID());mkdirSync(out,{recursive:true});
const report={passed:false,wasm:build.sha256,cases:[]};let relay,server;
const ready=(child,pattern)=>new Promise((ok,fail)=>{const timer=setTimeout(()=>fail(Error('Server timeout')),15000);child.once('error',fail);child.once('exit',c=>fail(Error('Server exit '+c)));child.stdout.on('data',b=>{process.stdout.write(b);if(pattern&&String(b).includes(pattern)){clearTimeout(timer);ok();}});child.stderr.on('data',b=>process.stderr.write(b));child.on('message',m=>{if(!pattern&&m?.type==='ready'){clearTimeout(timer);ok();}});});
try{
 const port=await free(),relayPort=await free();
 const launcher=resolve(process.env.EAGLER_LAUNCHER_ROOT||resolve(root,'../eagler-multiplayer'));
 const file=resolve(launcher,'server/netplay-relay.mjs');if(!existsSync(file))throw Error('Relay missing');
 relay=fork(file,[],{cwd:launcher,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(relayPort),EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe','ipc']});await ready(relay,'relay listening');
 server=fork(resolve(import.meta.dirname,'serve.mjs'),[],{cwd:root,env:{...process.env,PORT:String(port),TH08_MP_FIXTURES:'1'},stdio:['ignore','pipe','pipe','ipc']});await ready(server);
 for(const players of [2,3]){
  const output=resolve(out,players+'p.json');
  const code=await new Promise((ok,fail)=>{const c=spawn(process.env.TH_PYTHON||'python',['-u',resolve(import.meta.dirname,'check-stage-boundary.py'),'--url','http://127.0.0.1:'+port+'/','--relay','ws://127.0.0.1:'+relayPort,'--players',String(players),'--skip-resim-visual','--skip-resim-geometry','--back-metadata-only','--output',output],{cwd:root,stdio:['ignore','pipe','pipe'],windowsHide:true});c.stdout.on('data',b=>process.stdout.write(b));c.stderr.on('data',b=>process.stderr.write(b));c.once('error',fail);c.once('exit',ok);});
  report.cases.push({players,code,output});if(code!==0||!JSON.parse(readFileSync(output)).passed)throw Error('Failed '+players+'P');
 }
 verify();report.passed=true;
}catch(e){report.error=String(e);process.exitCode=1;}
finally{server?.kill();relay?.kill();writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({passed:report.passed,report:resolve(out,'suite.json'),error:report.error}));}
