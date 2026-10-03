// Run the two immutable-golden daily lanes against one identified SP Runtime.
// No oracle operation, build mutation, expected-data update or tick cropping.
import {fork,spawn,execFileSync} from 'node:child_process';
import {createServer} from 'node:net';
import {createHash,randomUUID} from 'node:crypto';
import {existsSync,mkdirSync,readFileSync,writeFileSync,createWriteStream} from 'node:fs';
import {resolve} from 'node:path';

const root=resolve(import.meta.dirname,'../..');
const flag=process.argv.find(value=>value.startsWith('--run-id='));
const id=flag?.slice(9)||randomUUID();
if(!/^[A-Za-z0-9_-]{1,80}$/.test(id))throw Error('Invalid daily run identifier');
const out=resolve(root,'artifacts/replay-verifier/daily-'+id);
if(existsSync(out))throw Error('Daily run directory already exists; preserve earlier evidence');
mkdirSync(out,{recursive:true});
const hash=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const buildPath=resolve(root,'th10_web/artifacts/presentation-lab/build.json');
const build=JSON.parse(readFileSync(buildPath));
const runtime=resolve(root,'artifacts/presentation-lab/runtime');
const fixtureRoot=resolve(process.env.TH10_REPLAY_FIXTURE_ROOT||resolve(root,'tools/replay-verifier/fixtures/original'));
const corpus=JSON.parse(readFileSync(resolve(root,'tools/replay-verifier/corpus.json')));
const report={schema:'th10/replay-daily-run/v1',id,passed:false,phase:'starting',pid:process.pid,
  wasm:build.sha256,sourceDigest:build.sourceDigest,runtimeInventory:hash(resolve(runtime,'runtime-files.json')),tests:[]};
const save=()=>writeFileSync(resolve(out,'run.json'),JSON.stringify(report,null,2)+'\n');
save();
if(build.profile!=='presentation-lab'||build.variant!=='normal'||!build.diagnostic||
   hash(resolve(runtime,'th10-sdl.wasm'))!==build.sha256)throw Error('Mixed daily Runtime');
for(const [name,expected] of Object.entries(build.sourceFiles))
  if(hash(resolve(root,name))!==expected)throw Error('Rebuild candidate source: '+name);
const workspace=process.env.EAGLER_WORKSPACE||[resolve(root,'..'),resolve(root,'../..')]
  .find(path=>existsSync(resolve(path,'toolchains/emsdk')));
if(!workspace)throw Error('Set EAGLER_WORKSPACE to the local workspace root');
const port=await new Promise((done,reject)=>{
  const probe=createServer();probe.once('error',reject);
  probe.listen(0,'127.0.0.1',()=>{const value=probe.address().port;probe.close(error=>error?reject(error):done(value));});
});
let child=null,server=null;
function terminate(process){
  if(!process||process.exitCode!==null||process.signalCode!==null)return;
  if(globalThis.process.platform==='win32'){
    try{execFileSync('taskkill',['/PID',String(process.pid),'/T','/F'],{stdio:'ignore',windowsHide:true});}catch{}
  }else process.kill('SIGTERM');
}
async function run(name,args,timeoutMs){
  report.phase=name;save();
  const log=createWriteStream(resolve(out,name+'.log'),{flags:'wx'});
  const started=Date.now();
  const result=await new Promise((done,reject)=>{
    child=spawn(name.startsWith('capture-')?(process.env.TH_PYTHON||'python'):process.execPath,args,
      {cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe']});
    let timedOut=false;
    const timeout=setTimeout(()=>{timedOut=true;terminate(child);},timeoutMs);
    const output=bytes=>{log.write(bytes);process.stdout.write(bytes);};
    child.stdout.on('data',output);child.stderr.on('data',output);
    child.once('error',error=>{clearTimeout(timeout);reject(error);});
    child.once('exit',(code,signal)=>{clearTimeout(timeout);done({code,signal,timedOut,milliseconds:Date.now()-started});});
  }).finally(()=>log.end());
  report.tests.push({name,...result});save();child=null;
  if(result.code!==0||result.timedOut)throw Error(name+' failed: '+JSON.stringify(result));
}
try{
  const fixtures=[['lunatic','th10_ud1aef.rpy'],['extra','th10_ud1b2a.rpy']].map(([name,file])=>{
    const path=resolve(fixtureRoot,file),expected=corpus.cases.find(value=>value.kind===name);
    if(!expected||!existsSync(path)||hash(path)!==expected.replaySha256)
      throw Error(name+' Replay missing or hash mismatch; run fetch-fixtures.mjs or set TH10_REPLAY_FIXTURE_ROOT');
    return {name,path,sha256:expected.replaySha256};
  });
  report.fixtures=fixtures;save();
  server=fork(resolve(root,'portable/presentation-lab/serve.mjs'),[],{cwd:root,
    env:{...process.env,PORT:String(port),
      TH10_LAB_FONT:process.env.TH10_LAB_FONT||resolve(workspace,'th06-eagler/assets/msgothic.ttc'),
      TH10_LAB_DATA:process.env.TH10_LAB_DATA||resolve(workspace,'games/web-content/th10/th10.data')},
    stdio:['ignore','pipe','pipe','ipc']});
  server.stdout.on('data',bytes=>process.stdout.write(bytes));server.stderr.on('data',bytes=>process.stderr.write(bytes));
  await new Promise((done,reject)=>{
    const timeout=setTimeout(()=>reject(Error('Daily server startup timed out')),15000);
    server.once('error',error=>{clearTimeout(timeout);reject(error);});
    server.once('exit',code=>{clearTimeout(timeout);reject(Error('Daily server exited '+code));});
    server.on('message',message=>{if(message?.type==='ready'){
      clearTimeout(timeout);report.serverPid=server.pid;report.port=message.port;save();done();
    }});
  });
  const url='http://127.0.0.1:'+port+'/';
  for(const {name,path} of fixtures){
    await run('capture-'+name,['-u','tools/replay-verifier/capture-current-replay.py',
      '--url',url,'--replay',path,
      '--output',resolve(out,name+'.json')],45*60*1000);
    const evidence=JSON.parse(readFileSync(resolve(out,name+'.json')));
    if(!evidence.complete||evidence.build?.wasm!==build.sha256||
       evidence.build?.runtimeInventory!==report.runtimeInventory)throw Error(name+' capture identity/completion mismatch');
  }
  await run('compare',['tools/replay-verifier/run-daily-gate.mjs','--capture-root',out,
    '--common-root','third_party/eagler-common','--report',resolve(out,'daily-result.json')],120000);
  if(!JSON.parse(readFileSync(resolve(out,'daily-result.json'))).passed)throw Error('Daily comparison did not pass');
  if(hash(resolve(runtime,'th10-sdl.wasm'))!==build.sha256||
     hash(resolve(runtime,'runtime-files.json'))!==report.runtimeInventory)throw Error('Runtime changed during daily suite');
  report.passed=true;report.phase='completed';
}catch(error){report.error=String(error);report.phase='failed';process.exitCode=1;}
finally{
  terminate(child);terminate(server);save();
  console.log(JSON.stringify({passed:report.passed,report:resolve(out,'run.json'),error:report.error}));
}
