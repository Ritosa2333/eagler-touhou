import {fork,spawn} from 'node:child_process';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash,randomUUID} from 'node:crypto';
import {createServer} from 'node:net';

const root=resolve(import.meta.dirname,'../..');
const profile=process.argv.includes('--fixtures')?'multiplayer-fixtures':'multiplayer';
const selected=process.argv.find(value=>value.startsWith('--only='))?.slice(7).split(',');
const normal=['check-rollback','check-rollback-3p','check-rollback-bomb',
              'check-rollback-stall','check-rollback-pause','check-peer-seats'];
const fixtures=['check-native-fixtures','check-generation','check-dense'];
const tests=selected??(profile==='multiplayer'?normal:fixtures);
if(!tests.length||tests.some(name=>![...normal,...fixtures].includes(name)))throw Error('Unknown runtime test');
const buildPath=resolve(root,'th10_web/artifacts',profile,'build.json');
const wasmPath=resolve(root,'th10_web/artifacts',profile,'th10-sdl.wasm');
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const build=JSON.parse(readFileSync(buildPath));
if(build.sha256!==sha(wasmPath)||build.profile!==profile)throw Error('Stale/mixed build');
for(const [path,hash] of Object.entries(build.sourceFiles))
 if(sha(resolve(root,path))!==hash)throw Error('Source changed since build: '+path);
const module=new WebAssembly.Module(readFileSync(wasmPath));
const hasFixture=WebAssembly.Module.exports(module).some(value=>value.name==='mp_fixture_prepare');
if(hasFixture!==(profile==='multiplayer-fixtures'))throw Error('Fixture export/profile mismatch');
const id=randomUUID(),out=resolve(root,'artifacts/multiplayer-tests/suites',id);
mkdirSync(out,{recursive:true});
const report={passed:false,id,profile,wasmSha256:build.sha256,sourceDigest:build.sourceDigest,tests:[]};
// Common test server intentionally requires a concrete loopback Host/port.
// Probe an OS-assigned free port, then pass that explicit value; any intervening
// bind race fails startup, never silently connects to an existing service.
const port=await new Promise((resolve,reject)=>{
 const probe=createServer();probe.once('error',reject);
 probe.listen(0,'127.0.0.1',()=>{const port=probe.address().port;probe.close(error=>error?reject(error):resolve(port));});
});
const server=fork(resolve(import.meta.dirname,'serve.mjs'),[],{cwd:root,
 env:{...process.env,PORT:String(port),TH10_MP_PROFILE:profile},stdio:['ignore','pipe','pipe','ipc']});
server.stdout.on('data',data=>process.stdout.write(data));server.stderr.on('data',data=>process.stderr.write(data));
try{
 const url=await new Promise((resolve,reject)=>{
  const timer=setTimeout(()=>reject(Error('Local fixture server startup timed out')),15000);
  const fail=error=>{clearTimeout(timer);reject(error);};
  server.once('error',fail);server.once('exit',code=>fail(Error('Local server exited '+code)));
  server.on('message',value=>{if(value?.type==='ready'){
   clearTimeout(timer);resolve('http://127.0.0.1:'+value.port+'/');
  }});
 });
 for(const name of tests){
  const output=resolve(out,name+'.json'),start=performance.now();
  console.log('BEGIN '+name+' '+profile);
  const result=await new Promise((done,reject)=>{
   const child=spawn(process.env.TH_PYTHON||'python',[
    resolve(import.meta.dirname,name+'.py'),'--url',url,'--output',output
   ],{cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe']});
   let stdout='',stderr='';
   child.stdout.on('data',data=>{stdout+=data;process.stdout.write(data);});
   child.stderr.on('data',data=>{stderr+=data;process.stderr.write(data);});
   child.once('error',reject);child.once('exit',(code,signal)=>done({code,signal,stdout,stderr}));
  });
  report.tests.push({name,...result,milliseconds:performance.now()-start,report:output});
  writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2)+'\n');
  if(result.code!==0)throw Error(name+' failed: '+result.code);
  if(!JSON.parse(readFileSync(output)).passed)throw Error(name+' did not produce passing evidence');
  if(sha(wasmPath)!==build.sha256)throw Error('WASM changed during suite');
  console.log('PASS '+name);
 }
 for(const [path,hash] of Object.entries(build.sourceFiles))
  if(sha(resolve(root,path))!==hash)throw Error('Source changed during suite: '+path);
 report.passed=true;
}catch(error){report.error=String(error);process.exitCode=1;console.error(error);}
finally{
 server.kill();
 writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2)+'\n');
 console.log(JSON.stringify({passed:report.passed,profile,suite:resolve(out,'suite.json')}));
}
