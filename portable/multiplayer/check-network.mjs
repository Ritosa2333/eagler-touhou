import {fork,spawn} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {createHash,randomUUID} from 'node:crypto';
import {createServer} from 'node:net';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'../..'),fixtures=process.argv.includes('--fixtures');
const profile=fixtures?'multiplayer-fixtures':'multiplayer';
const buildRoot=resolve(root,'th08_web/artifacts',profile),build=JSON.parse(readFileSync(resolve(buildRoot,'build.json')));
const hash=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const verify=()=>{if(build.variant!==profile||hash(resolve(buildRoot,'th08-sdl.wasm'))!==build.sha256)throw Error('Mismatched build');
 for(const [name,sha] of Object.entries(build.sourceFiles))if(hash(resolve(root,name))!==sha)throw Error('Stale source: '+name);};
verify();
const only=process.argv.find(s=>s.startsWith('--only='))?.slice(7);
const choices=['packets','rtc','relay','host','spectator-rtc','spectator-relay'];
const modes=only?only.split(','):['packets','rtc','relay'];
if(!modes.length||modes.some(s=>!choices.includes(s)))throw Error('No/unknown cases');
const hostMode=modes.some(m=>m==='host'||m.startsWith('spectator-'));
const count=process.argv.find(s=>s.startsWith('--players='))?.slice(10)||'all';
const analog=process.argv.includes('--analog');
const browserBlocked=new Set([2049,3659,4045,5060,5061,6000,6566,6665,6666,6667,6668,6669,6697,10080]);
const free=async()=>{for(let tries=0;tries<32;++tries){const p=await new Promise((ok,fail)=>{const s=createServer();s.once('error',fail);s.listen(0,'127.0.0.1',()=>{const port=s.address().port;s.close(e=>e?fail(e):ok(port));});});if(p>=1024&&!browserBlocked.has(p))return p;}throw Error('No browser-safe loopback port');};
const out=resolve(root,'artifacts/multiplayer-tests/network-'+randomUUID());mkdirSync(out,{recursive:true});
const report={passed:false,profile,analog,wasm:build.sha256,harness:{},cases:[]};
for(const file of ['check-network.mjs','check-network.py','smoke.mjs','serve.mjs'])report.harness[file]=hash(resolve(import.meta.dirname,file));
if(hostMode)for(const file of ['check-runtime-host.py','runtime-host.html','host-resources.json'])report.harness[file]=hash(resolve(import.meta.dirname,file));
if(modes.some(m=>m.startsWith('spectator-')))report.harness['check-spectator.py']=hash(resolve(import.meta.dirname,'check-spectator.py'));
if(hostMode)for(const file of ['th08.html','shell.mjs','eagler-host.mjs','save-storage.mjs','multiplayer-host.mjs','practice.mjs','practice-config.mjs','practice-sections.mjs']){
 const key='../../th08_web/sdl-runtime/'+file;report.harness[key]=hash(resolve(import.meta.dirname,key));
}
let server,relay;
const children=[];
const ready=(child,pattern)=>new Promise((done,reject)=>{const t=setTimeout(()=>reject(Error('Server timeout')),15000);
 child.once('error',reject);child.once('exit',c=>reject(Error('Server exit '+c)));
 child.stdout.on('data',b=>{process.stdout.write(b);if(pattern&&String(b).includes(pattern)){clearTimeout(t);done();}});
 child.stderr.on('data',b=>process.stderr.write(b));
 child.on('message',m=>{if(!pattern&&m?.type==='ready'){clearTimeout(t);done();}});});
try{
 const port=await free(),relayPort=await free();
 if(modes.some(m=>m!=='packets')){
  const launcher=resolve(process.env.EAGLER_LAUNCHER_ROOT||resolve(root,'../eagler-multiplayer'));
  const file=resolve(launcher,'server/netplay-relay.mjs');if(!existsSync(file))throw Error('Relay missing');
  report.relay={path:file,sha256:hash(file)};
  relay=fork(file,[],{cwd:launcher,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(relayPort),EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe','ipc']});
  await ready(relay,'relay listening');
 }
 server=fork(resolve(import.meta.dirname,'serve.mjs'),[],{cwd:root,env:{...process.env,PORT:String(port),TH08_MP_FIXTURES:fixtures?'1':'0'},stdio:['ignore','pipe','pipe','ipc']});await ready(server);
 for(const mode of modes){
  const output=resolve(out,mode+'.json');console.log('BEGIN TH08 '+mode+' players='+count);
  const code=await new Promise((done,reject)=>{
   const c=spawn(process.env.TH_PYTHON||'python',['-u',resolve(import.meta.dirname,mode==='host'?'check-runtime-host.py':mode.startsWith('spectator-')?'check-spectator.py':'check-network.py'),'--url','http://127.0.0.1:'+port+'/','--relay','ws://127.0.0.1:'+relayPort,'--mode',mode,'--players',count,'--output',output,...(analog?['--analog']:[])],{cwd:root,stdio:['ignore','pipe','pipe'],windowsHide:true});
   children.push(c);c.stdout.on('data',b=>process.stdout.write(b));c.stderr.on('data',b=>process.stderr.write(b));c.once('error',reject);c.once('exit',done);
  });
  report.cases.push({mode,code,report:output});
  writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2));
  if(code!==0||!JSON.parse(readFileSync(output)).passed)throw Error('Failed '+mode);
 }
 verify();for(const [name,sha] of Object.entries(report.harness))if(hash(resolve(import.meta.dirname,name))!==sha)throw Error('Harness changed: '+name);
 report.passed=true;
}catch(e){report.error=String(e);process.exitCode=1;}
finally{for(const c of children)if(c.exitCode===null)c.kill();server?.kill();relay?.kill();writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({passed:report.passed,report:resolve(out,'suite.json'),error:report.error}));}
