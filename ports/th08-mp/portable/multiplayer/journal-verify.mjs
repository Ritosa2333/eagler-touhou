import {spawn,spawnSync} from 'node:child_process';
import {mkdirSync,existsSync,readFileSync,writeFileSync,openSync,closeSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {createServer} from 'node:net';

const root=resolve(import.meta.dirname,'../..'),label=process.env.RUN_LABEL;
if(!label||!/^[A-Za-z0-9_-]+$/.test(label))throw Error('Set unique RUN_LABEL');
const out=resolve(root,'artifacts/rollback-journal-4x',label);
if(existsSync(out))throw Error('Run already exists');mkdirSync(out,{recursive:true});
const suites={
  enemy:['check-enemy-journal.py'],
  world:['check-world-journal.py','--bomb-lazy','--shot-sparse','--record-sparse'],
  world2:['check-world-journal.py','--bomb-lazy','--shot-sparse','--record-sparse','--players','2'],
  world3:['check-world-journal.py','--bomb-lazy','--shot-sparse','--record-sparse','--players','3'],
  'world-focused':['check-world-journal.py','--bomb-lazy','--shot-sparse','--record-sparse','--scenario','focused-bombs','--players','2'],
  bullets:['check-live-bullets.py'],
};
const names=process.argv.slice(2);
if(!names.length||names.some(name=>!Object.hasOwn(suites,name)))throw Error('Choose '+Object.keys(suites).join(', '));
const sha=b=>createHash('sha256').update(b).digest('hex');
const buildPath=resolve(root,'th08_web/artifacts/multiplayer-fixtures/build.json');
const build=JSON.parse(readFileSync(buildPath,'utf8'));
const verify=()=>Object.entries(build.sourceFiles).every(([name,expected])=>sha(readFileSync(resolve(root,name)))===expected);
if(!verify())throw Error('Fixture source drift before run');
const fd=openSync(resolve(out,'server.log'),'wx');
const selectedPort=await new Promise((done,reject)=>{
  const probe=createServer();probe.once('error',reject);
  probe.listen(0,'127.0.0.1',()=>{const port=probe.address().port;probe.close(()=>done(port));});
});
const server=spawn(process.execPath,['portable/multiplayer/serve.mjs'],{cwd:root,
  env:{...process.env,TH08_MP_FIXTURES:'1',PORT:String(selectedPort)},stdio:['ignore',fd,fd,'ipc']});
const report={passed:false,buildSha256:build.sha256,buildRecordSha256:sha(readFileSync(buildPath)),
  sourceStableBefore:true,suites:[],teardown:null};
try{
  const port=await new Promise((done,reject)=>{
    const timer=setTimeout(()=>reject(Error('Fixture server readiness timeout')),20000);
    server.once('error',error=>{clearTimeout(timer);reject(error);});
    server.once('exit',code=>{clearTimeout(timer);reject(Error('Fixture server exited '+code));});
    server.on('message',message=>{if(message?.type==='ready'){clearTimeout(timer);done(message.port);}});
  });
  for(const name of names){
    const [script,...extra]=suites[name],output=resolve(out,name+'.json');
    const args=[resolve(import.meta.dirname,script),'--url','http://127.0.0.1:'+port,'--output',output,...extra];
    const log=openSync(resolve(out,name+'.log'),'wx'),start=Date.now();
    const result=spawnSync(process.env.TH_PYTHON||'python',args,{cwd:root,stdio:['ignore',log,log],timeout:name.startsWith('world')?480000:300000});
    closeSync(log);
    const entry={name,args,status:result.status,error:result.error?.message,elapsedMs:Date.now()-start,
      scriptSha256:sha(readFileSync(resolve(import.meta.dirname,script)))};
    report.suites.push(entry);console.log(JSON.stringify(entry));
    if(result.status||result.error)throw Error('Suite failed: '+name);
  }
  report.sourceStableAfter=verify();report.passed=report.sourceStableAfter;
}catch(error){report.error=String(error);process.exitCode=1;}
finally{
  server.kill();
  if(server.exitCode===null&&server.signalCode===null)await new Promise(done=>{
    const timeout=setTimeout(done,5000);server.once('exit',()=>{clearTimeout(timeout);done();});
  });
  if(server.exitCode===null&&server.signalCode===null)server.kill('SIGKILL');
  report.teardown={serverPid:server.pid,exitCode:server.exitCode,signal:server.signalCode,connected:server.connected};
  closeSync(fd);writeFileSync(resolve(out,'run.json'),JSON.stringify(report,null,2)+'\n');
}
console.log(JSON.stringify({passed:report.passed,error:report.error,teardown:report.teardown}));
if(!report.passed)process.exitCode=1;
