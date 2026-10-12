// Local real-RTC performance/lifecycle runner. The relay is signaling/local fallback,
// not a WAN test. --output names the checker JSON; logs/receipt sit beside it.
import {fork,spawn} from 'node:child_process';
import {createHash,randomUUID} from 'node:crypto';
import {appendFileSync,mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {dirname,resolve} from 'node:path';
import {createServer} from 'node:net';

const root=resolve(import.meta.dirname,'../..');
const workspace=resolve(process.env.EAGLER_WORKSPACE||resolve(root,'../..'));
const launcher=resolve(process.env.EAGLER_LAUNCHER_ROOT||resolve(workspace,'eagler-touhou'));
const python=process.env.TH_PYTHON||'python';
const forwarded=[];let outputArg,buildArg,argumentError,checkerArg='performance';
const argv=process.argv.slice(2);
for(let i=0;i<argv.length;i++){
 const key=argv[i].split('=')[0];
 if(!['--build-root','--output','--checker'].includes(key)){forwarded.push(argv[i]);continue;}
 const value=argv[i].includes('=')?argv[i].slice(key.length+1):argv[++i];
 if(!value||value.startsWith('--')){argumentError='Missing value for '+key;break;}
 if(key==='--build-root')buildArg=value;
 else if(key==='--checker')checkerArg=value;
 else outputArg=value;
}
if(!['performance','manual-restart'].includes(checkerArg))argumentError='Unknown checker: '+checkerArg;
const output=resolve(outputArg||resolve(root,'artifacts/rollback-smoothness/performance-'+randomUUID()+'.json'));
const receipt=output+'.runner.json',logRoot=output+'.logs';
mkdirSync(dirname(output),{recursive:true});mkdirSync(logRoot,{recursive:true});
const report={schemaVersion:1,passed:false,startedAt:new Date().toISOString(),output,receipt,
 scope:'local independent browsers and local Launcher relay; actual RTC gated by checker, not WAN',
 passScope:'checker run, route, state and confirmed-audio gates; cadence and final-pixel motion are separate',
 workspace,launcher,python,checker:checkerArg,forwarded,logs:{},children:{},integrityVerifiedOnCompletion:false};
for(const name of ['runner','relay','server','checker'])for(const stream of ['stdout','stderr']){
 const path=resolve(logRoot,name+'.'+stream+'.log');report.logs[name+'.'+stream]=path;writeFileSync(path,'');
}
const log=(name,stream,bytes)=>{
 appendFileSync(report.logs[name+'.'+stream],bytes);
 process[stream].write(bytes);
};
const save=()=>writeFileSync(receipt,JSON.stringify(report,null,2)+'\n');
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const owned=[];let rejectInterrupted;
const interrupted=new Promise((_,reject)=>{rejectInterrupted=reject;});interrupted.catch(()=>{});
const signalHandler=signal=>{report.interruptedBy=signal;rejectInterrupted(Error('Runner interrupted by '+signal));};
const onInt=()=>signalHandler('SIGINT'),onTerm=()=>signalHandler('SIGTERM');
process.once('SIGINT',onInt);process.once('SIGTERM',onTerm);
const wait=promise=>Promise.race([promise,interrupted]);
// The shared HTTP server requires a concrete unprivileged port. Select an
// unused local port as its existing runner does; its ready IPC remains the
// authority. A bind race fails startup without borrowing somebody's service.
const unusedPort=()=>new Promise((done,reject)=>{
 const probe=createServer();probe.once('error',reject);
 probe.listen(0,'127.0.0.1',()=>{const port=probe.address().port;
  probe.close(error=>error?reject(error):done(port));});
});
function track(name,child){
 const row=report.children[name]={pid:child.pid??null,startedAt:new Date().toISOString(),closed:false};
 const item={name,child,row};owned.push(item);
 child.stdout.on('data',bytes=>log(name,'stdout',bytes));
 child.stderr.on('data',bytes=>log(name,'stderr',bytes));
 child.on('error',error=>{row.error=String(error);});
 item.done=new Promise(done=>child.once('close',(code,signal)=>{
  Object.assign(row,{closed:true,code,signal,closedAt:new Date().toISOString()});done({code,signal});
 }));
 return item;
}
function ready(item,kind){
 return new Promise((done,reject)=>{
  let tail='';
  const finish=(error,value)=>{
   clearTimeout(timer);item.child.removeListener('error',onError);item.child.removeListener('close',onClose);
   item.child.stdout.removeListener('data',onData);item.child.removeListener('message',onMessage);
   if(error)reject(error);else done(value);
  };
  const onError=error=>finish(error);
  const onClose=(code,signal)=>finish(Error(kind+' closed before ready: '+code+' '+signal));
  const onData=bytes=>{
   tail=(tail+String(bytes)).slice(-8192);
   const match=tail.match(/relay listening ws:\/\/127\.0\.0\.1:(\d+)[^\r\n]*\r?\n/);
   if(kind==='relay'&&match)finish(null,{port:Number(match[1]),listeningLine:match[0].trim()});
  };
  const onMessage=message=>{if(kind==='server'&&message?.type==='ready')finish(null,message);};
  const timer=setTimeout(()=>finish(Error(kind+' startup timeout')),15000);
  item.child.once('error',onError);item.child.once('close',onClose);
  item.child.stdout.on('data',onData);item.child.on('message',onMessage);
  if(item.row.closed)onClose(item.row.code,item.row.signal);
 });
}
async function stop(item){
 if(item.row.closed)return;
 item.row.stopRequested=true;
 const bounded=ms=>new Promise(done=>{
  const timer=setTimeout(done,ms);item.done.then(()=>{clearTimeout(timer);done();});
 });
 try{item.child.kill();}catch(error){item.row.stopError=String(error);}
 await bounded(2500);
 if(!item.row.closed){
  item.row.forceStopRequested=true;
  try{item.child.kill('SIGKILL');}catch(error){item.row.forceStopError=String(error);}
  await bounded(2500);
 }
 if(!item.row.closed)throw Error('Owned '+item.name+' did not close');
}
save();
try{
 if(argumentError)throw Error(argumentError);
 const defaultProfile=process.env.TH10_MP_PROFILE||'multiplayer';
 const buildRoot=resolve(buildArg||process.env.TH10_MP_BUILD_ROOT||resolve(root,'th10_web/artifacts',defaultProfile));
 const buildPath=resolve(buildRoot,'build.json'),build=JSON.parse(readFileSync(buildPath));
 if(build.variant!=='multiplayer'||!['multiplayer','multiplayer-fixtures'].includes(build.profile))throw Error('Wrong multiplayer build/profile');
 const wasmPath=resolve(buildRoot,'th10-sdl.wasm'),modulePath=resolve(buildRoot,'th10-sdl.mjs');
 const relayPath=resolve(launcher,'server/netplay-relay.mjs'),serverPath=resolve(import.meta.dirname,'serve.mjs');
 const checkerPath=resolve(import.meta.dirname,checkerArg==='manual-restart'?'check-manual-restart.py':'check-performance.py');
 const identityPaths=[buildPath,wasmPath,modulePath,relayPath,serverPath,checkerPath,
  resolve(import.meta.dirname,'run-smoothness.mjs'),resolve(import.meta.dirname,'performance-driver.mjs'),
  resolve(import.meta.dirname,'browser-evidence.cjs'),
  resolve(import.meta.dirname,'smoke.mjs'),resolve(root,'third_party/eagler-common/testkit/rtc-input-impairment.cjs'),
  resolve(root,'third_party/eagler-common/testkit/presentation-lab/server-core.mjs')];
 report.sourceHashes=Object.fromEntries(identityPaths.map(path=>[path,sha(path)]));
 if(report.sourceHashes[wasmPath]!==build.sha256)throw Error('Frozen runtime WASM disagrees with build.json');
 if(build.loaderSha256&&report.sourceHashes[modulePath]!==build.loaderSha256)
  throw Error('Frozen runtime loader disagrees with build.json');
 report.runtime={buildRoot,profile:build.profile,wasmSha256:build.sha256,moduleSha256:report.sourceHashes[modulePath],
  buildJsonSha256:report.sourceHashes[buildPath],sourceDigest:build.sourceDigest,builtAt:build.builtAt,
  sourceScope:'archived runtime identity; current native source may intentionally differ from frozen build'};
 report.relayIdentity={path:relayPath,sha256:report.sourceHashes[relayPath],
  hashScope:'relay entry source only; imported relay modules not individually attested'};
 // Relay assigns port zero itself. Neither service borrows an existing
 // listener; do not carry check-network's forced first-packet drop.
 const relay=track('relay',fork(relayPath,[],{cwd:launcher,env:{...process.env,
  EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''},
  stdio:['ignore','pipe','pipe','ipc'],windowsHide:true}));
 report.relayReady=await wait(ready(relay,'relay'));
 const httpPort=await wait(unusedPort());
 const server=track('server',fork(serverPath,[],{cwd:root,env:{...process.env,PORT:String(httpPort),
  EAGLER_WORKSPACE:workspace,TH10_MP_PROFILE:build.profile,TH10_MP_BUILD_ROOT:buildRoot,
  TH10_MP_FIXTURE_ROOT:import.meta.dirname},stdio:['ignore','pipe','pipe','ipc'],windowsHide:true}));
 report.serverReady=await wait(ready(server,'server'));
 for(const value of [report.relayReady.port,report.serverReady.port])
  if(!Number.isInteger(value)||value<1||value>65535)throw Error('Invalid actual listener port');
 if(report.serverReady.sha256!==build.sha256||report.serverReady.profile!==build.profile)throw Error('Wrong server-ready runtime identity');
 const url='http://127.0.0.1:'+report.serverReady.port+'/',relayUrl='ws://127.0.0.1:'+report.relayReady.port;
 // Runner-owned endpoints are last so a forwarded duplicate cannot silently
 // make this receipt attest a different server or relay.
 const childArgs=['-u',checkerPath,...forwarded,'--url',url,'--relay',relayUrl,'--output',output];
 report.command={executable:python,args:childArgs,cwd:root};save();
 const checker=track('checker',spawn(python,childArgs,{cwd:root,stdio:['ignore','pipe','pipe'],windowsHide:true}));
 report.checkerExit=await wait(checker.done);
 if(report.checkerExit.code!==0)throw Error(checkerArg+' checker failed: '+JSON.stringify(report.checkerExit));
 const result=JSON.parse(readFileSync(output));
 if(!result.passed)throw Error(checkerArg+' correctness/route gates failed');
 if(result.options?.url!==url||result.options?.relay!==relayUrl)throw Error('Checker used different endpoints');
 if(!Array.isArray(result.identities)||result.identities.length<2||
    result.identities.some(identity=>identity.wasmSha256!==build.sha256))throw Error('Actual peer runtime identity mismatch');
 for(const [path,hash] of Object.entries(report.sourceHashes))if(sha(path)!==hash)throw Error('Source/runtime changed during run: '+path);
 report.integrityVerifiedOnCompletion=true;report.passed=true;
}catch(error){
 report.error=String(error?.stack||error);log('runner','stderr',report.error+'\n');
}finally{
 for(const item of [...owned].reverse()){
  try{await stop(item);}catch(error){
   report.passed=false;(report.cleanupErrors??=[]).push(String(error));log('runner','stderr',String(error)+'\n');
  }
 }
 process.removeListener('SIGINT',onInt);process.removeListener('SIGTERM',onTerm);
 report.finishedAt=new Date().toISOString();save();
 if(!report.passed)process.exitCode=1;
 log('runner','stdout',JSON.stringify({passed:report.passed,report:output,receipt,logs:logRoot,error:report.error})+'\n');
}
