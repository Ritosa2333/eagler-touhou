// Reproducible SP regression gate: a fresh title-owned Demo capture against
// immutable original-JIT golden, with bounded child lifetimes and saved logs.
import {fork,spawn,execFileSync} from 'node:child_process';
import {createServer} from 'node:net';
import {createHash,randomUUID} from 'node:crypto';
import {existsSync,mkdirSync,readFileSync,writeFileSync,createWriteStream} from 'node:fs';
import {resolve} from 'node:path';

const root=resolve(import.meta.dirname,'../..');
const id=process.argv.find(value=>value.startsWith('--run-id='))?.slice(9)||randomUUID();
if(!/^[A-Za-z0-9_-]{1,80}$/.test(id))throw Error('Invalid quick run identifier');
const out=resolve(root,'artifacts/replay-verifier/quick-'+id);
if(existsSync(out))throw Error('Quick directory already exists; preserve earlier evidence');
mkdirSync(out,{recursive:true});
const hash=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const runtime=resolve(root,'artifacts/presentation-lab/runtime');
const build=JSON.parse(readFileSync(resolve(root,'th10_web/artifacts/presentation-lab/build.json')));
const golden=resolve(root,'tools/replay-verifier/golden/manifest.json');
const report={schema:'th10/replay-quick-run/v1',id,passed:false,phase:'starting',pid:process.pid,
  wasm:build.sha256,sourceDigest:build.sourceDigest,runtimeInventory:hash(resolve(runtime,'runtime-files.json')),
  goldenManifest:hash(golden),tests:[]};
const save=()=>writeFileSync(resolve(out,'run.json'),JSON.stringify(report,null,2)+'\n');
save();
let child=null,server=null;
function terminate(value){
  if(!value||value.exitCode!==null||value.signalCode!==null)return;
  if(process.platform==='win32'){
    try{execFileSync('taskkill',['/PID',String(value.pid),'/T','/F'],{stdio:'ignore',windowsHide:true});}catch{}
  }else value.kill('SIGTERM');
}
async function run(name,executable,args,timeoutMs){
  report.phase=name;save();
  const log=createWriteStream(resolve(out,name+'.log'),{flags:'wx'}),started=Date.now();
  const result=await new Promise((done,reject)=>{
    child=spawn(executable,args,{cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe']});
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
  if(build.profile!=='presentation-lab'||build.variant!=='normal'||!build.diagnostic||
     hash(resolve(runtime,'th10-sdl.wasm'))!==build.sha256)throw Error('Mixed quick Runtime');
  const verifySource=()=>{
    for(const [name,expected] of Object.entries(build.sourceFiles))
      if(hash(resolve(root,name))!==expected)throw Error('Modified candidate source: '+name);
    if(hash(resolve(runtime,'runtime-files.json'))!==report.runtimeInventory||
       hash(resolve(runtime,'th10-sdl.wasm'))!==build.sha256)throw Error('Runtime changed during quick suite');
    if(hash(golden)!==report.goldenManifest)throw Error('Golden manifest changed during quick suite');
  };
  verifySource();
  await run('verify-golden',process.execPath,['tools/replay-verifier/verify-golden.mjs'],120000);
  const workspace=process.env.EAGLER_WORKSPACE||[resolve(root,'..'),resolve(root,'../..')]
    .find(path=>existsSync(resolve(path,'toolchains/emsdk')));
  if(!workspace)throw Error('Set EAGLER_WORKSPACE to the local workspace root');
  const port=await new Promise((done,reject)=>{
    const probe=createServer();probe.once('error',reject);
    probe.listen(0,'127.0.0.1',()=>{const value=probe.address().port;probe.close(error=>error?reject(error):done(value));});
  });
  server=fork(resolve(root,'portable/presentation-lab/serve.mjs'),[],{cwd:root,
    env:{...process.env,PORT:String(port),
      TH10_LAB_FONT:process.env.TH10_LAB_FONT||resolve(workspace,'th06-eagler/assets/msgothic.ttc'),
      TH10_LAB_DATA:process.env.TH10_LAB_DATA||resolve(workspace,'games/web-content/th10/th10.data')},
    stdio:['ignore','pipe','pipe','ipc']});
  server.stdout.on('data',bytes=>process.stdout.write(bytes));server.stderr.on('data',bytes=>process.stderr.write(bytes));
  await new Promise((done,reject)=>{
    const timeout=setTimeout(()=>reject(Error('Quick server startup timed out')),15000);
    server.once('error',error=>{clearTimeout(timeout);reject(error);});
    server.once('exit',code=>{clearTimeout(timeout);reject(Error('Quick server exited '+code));});
    server.on('message',message=>{if(message?.type==='ready'){
      clearTimeout(timeout);report.serverPid=server.pid;report.port=message.port;save();done();
    }});
  });
  const capture=resolve(out,'capture');
  await run('capture',process.env.TH_PYTHON||'python',[
    '-u','tools/replay-verifier/capture-current-demos.py','--url','http://127.0.0.1:'+port+'/',
    '--output',capture],20*60*1000);
  const evidence=JSON.parse(readFileSync(resolve(capture,'suite.json')));
  if(!evidence.complete||evidence.build?.wasm!==build.sha256||
     evidence.build?.runtimeInventory!==report.runtimeInventory)throw Error('Quick capture identity/completion mismatch');
  await run('compare',process.execPath,['tools/replay-verifier/run-demo-gate.mjs',
    '--candidate',resolve(capture,'suite.json'),'--common-root','third_party/eagler-common',
    '--report',resolve(out,'quick-result.json')],120000);
  if(!JSON.parse(readFileSync(resolve(out,'quick-result.json'))).passed)throw Error('Quick comparison did not pass');
  verifySource();report.passed=true;report.phase='completed';
}catch(error){report.error=String(error);report.phase='failed';process.exitCode=1;}
finally{
  terminate(child);terminate(server);save();
  console.log(JSON.stringify({passed:report.passed,report:resolve(out,'run.json'),error:report.error}));
}
