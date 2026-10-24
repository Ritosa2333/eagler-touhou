import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,mkdtempSync,existsSync,rmSync} from 'node:fs';
import {spawnSync} from 'node:child_process';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {fileURLToPath} from 'node:url';
import {runInNewContext} from 'node:vm';
import {assertBeforeDeadline,awaitBeforeDeadline} from './performance-input-proof.mjs';
import {installPersistenceSyncMarkers} from './performance-persistence-proof.mjs';

const driver=new URL('./performance-browser.mjs',import.meta.url),source=readFileSync(driver,'utf8');
const hostSource=source.slice(source.indexOf('function host('),source.indexOf('\n// Read the actual IDBFS database'));
const waitSource=source.slice(source.indexOf('async function waitNaturalSaveAfterCopy('),source.indexOf('\nasync function stableProofEvidence('));
function naturalEvents({stack='at http://test/runtime/shell.mjs:347:53',complete=true,fsId=5,error=null}={}){
  const events=[{kind:'native-request',token:5,result:5,entered:60132.2,at:60132.9,stack},
    {kind:'native-poll',token:5,result:1,at:60133},
    {kind:'fs-sync-start',id:5,populate:false,at:60133.2}];
  if(complete)events.push({kind:'fs-sync-complete',id:fsId,populate:false,error,at:60193.4});
  return events;
}
function fixture(events,{saveOnly=true}={}){
  let disposed=false,waitCalls=0;
  const frame={waitForFunction:async(predicate,args,options)=>{
    ++waitCalls;assert.equal(options.polling,100);assert.ok(options.timeout>0);
    const value=runInNewContext('('+predicate.toString()+')(args)',{args,
      __th15Performance:{state:{saveEvents:[{kind:'old-save'},...events]}},
      document:{querySelector:()=>({textContent:''})}});
    if(!value)throw Error('predicate is still waiting for a real natural FS completion');
    return {jsonValue:async()=>value,dispose:async()=>{disposed=true;}};
  }};
  const context={frames:[frame],assert,performance,assertBeforeDeadline,awaitBeforeDeadline,
    saveValidationOnly:saveOnly,saveEvidenceSupersessionTest:true};
  runInNewContext(waitSource,context);
  return {run:()=>context.waitNaturalSaveAfterCopy(0,{metadata:{saveEventCount:1}},performance.now()+1000,[347]),
    disposed:()=>disposed,waitCalls:()=>waitCalls};
}

test('the actual CLI rejects the deliberate supersession option before creating a performance output',()=>{
  const directory=mkdtempSync(join(tmpdir(),'th15-save-evidence-option-'));
  try{
    const output=join(directory,'must-not-exist');
    const result=spawnSync(process.execPath,[fileURLToPath(driver),'--save-evidence-supersession-test','--output='+output],{encoding:'utf8'});
    assert.notEqual(result.status,0);
    assert.match(result.stderr,/--save-evidence-supersession-test requires --save-validation-only/);
    assert.equal(existsSync(output),false);
  }finally{rmSync(directory,{recursive:true,force:true});}
});

test('the actual regression wait requires an existing natural timer native token and its matching FS completion',async()=>{
  const f=fixture(naturalEvents()),result=await f.run();
  assert.equal(result.token,5);assert.equal(result.fsId,5);
  assert.equal(result.filesystemCompletedAt,60193.4);assert.equal(f.disposed(),true);
});

test('a Host save, native completion alone, and an unrelated FS completion do not satisfy the natural-save regression',async()=>{
  for(const events of [naturalEvents({stack:'at command (http://test/runtime/shell.mjs:314:19)'}),
                       naturalEvents({complete:false}),naturalEvents({fsId:4})]){
    await assert.rejects(fixture(events).run(),/still waiting for a real natural FS completion/);
  }
});

test('the actual regression wait propagates real FS failures and cannot run in a performance case',async()=>{
  await assert.rejects(fixture(naturalEvents({error:'real IDB error'})).run(),/observed a save failure/);
  const f=fixture(naturalEvents(),{saveOnly:false});
  await assert.rejects(f.run(),/restricted to save-only validation/);assert.equal(f.waitCalls(),0);
});

function syncFixture({phase='complete',fail=false,replaceRuntime=false}={}){
  const runtimeClock={at:12},hostClock={at:67},listeners=new Set(),commands=[];
  const probe={state:{phase,timeOrigin:20000,saveApi:true,saveEvents:[],overflow:[]},snapshot:()=>({at:runtimeClock.at})};
  const runtimeContext={__th15Performance:probe,performance:{timeOrigin:20000,now:()=>runtimeClock.at}};
  runInNewContext('('+installPersistenceSyncMarkers.toString()+')()',runtimeContext);
  let serial=0,currentChild;
  const child={__th15Performance:probe,postMessage:(message,origin)=>{
    commands.push(message);assert.equal(origin,'https://same-origin.test');
    assert.equal(probe.state.persistenceMarkers.length,1);
    runtimeClock.at=44;hostClock.at=90;
    probe.state.saveEvents.push(...naturalEvents({stack:'at command'}));
    if(replaceRuntime)currentChild={__th15Performance:{}};
    for(const listener of [...listeners])listener({source:child,origin,data:{protocol:'eagler-touhou/1',request:message.request,
      ok:!fail,...(fail?{error:'native save failed'}:{})}});
  }};
  currentChild=child;
  const context={performance:{timeOrigin:10000,now:()=>hostClock.at},setTimeout,clearTimeout,
    crypto:{randomUUID:()=>('actual-host-request-'+(++serial))},location:{origin:'https://same-origin.test'},
    addEventListener:(type,callback)=>{assert.equal(type,'message');listeners.add(callback);},
    removeEventListener:(type,callback)=>{assert.equal(type,'message');listeners.delete(callback);},
    document:{querySelector:()=>({contentWindow:currentChild}),createElement:()=>({}),body:{append(){}}}};
  context.window=context;
  runInNewContext('('+hostSource+')(0)',context);
  return {context,probe,commands,runtimeClock,hostClock,listeners};
}

test('actual serialized proofSync records its real request in Runtime-clock markers before send and after ACK',async()=>{
  const f=syncFixture(),result=await f.context.proofSync('evidence-sync');
  assert.equal(f.commands.length,1);assert.equal(f.commands[0].command,'sync');
  assert.equal(result.request,f.commands[0].request);assert.equal(result.runtimeWindow.request,result.request);
  assert.equal(result.timeOrigin,10000);assert.equal(result.start,67);assert.equal(result.ackAt,90);
  assert.equal(result.runtimeWindow.start.timeOrigin,20000);assert.equal(result.runtimeWindow.start.at,12);
  assert.equal(result.runtimeWindow.ack.timeOrigin,20000);assert.equal(result.runtimeWindow.ack.at,44);
  assert.equal(result.runtimeWindow.start.saveEventCount,0);assert.equal(result.runtimeWindow.ack.saveEventCount,4);
  assert.equal(result.snapshot.at,44);assert.equal(f.listeners.size,1);
  assert.deepEqual(JSON.parse(JSON.stringify(f.probe.state.persistenceMarkers)),JSON.parse(JSON.stringify([result.runtimeWindow.start,result.runtimeWindow.ack])));
  result.runtimeWindow.start.at=999;assert.equal(f.probe.state.persistenceMarkers[0].at,12);
});

test('actual proofSync retains only the start marker after a failed save or Runtime replacement',async()=>{
  for(const options of [{fail:true},{replaceRuntime:true}]){
    const f=syncFixture(options);
    await assert.rejects(f.context.proofSync('evidence-sync'),/native save failed|Runtime changed/);
    assert.equal(f.probe.state.persistenceMarkers.length,1);assert.equal(f.probe.state.persistenceMarkers[0].edge,'start');
    assert.equal(f.listeners.size,1);
  }
});

test('Runtime marker installation and recording reject measurement phase, wrong identity, duplicate ACK and clock replacement',()=>{
  assert.throws(()=>syncFixture({phase:'measure'}),/not ready/);
  const f=syncFixture(),mark=f.probe.markPersistenceSync;
  assert.throws(()=>mark({request:'request',label:'sync',edge:'ack'}),/request order/);
  assert.throws(()=>mark({request:'',label:'sync',edge:'start'}),/identity/);
  mark({request:'request',label:'sync',edge:'start'});
  assert.throws(()=>mark({request:'request',label:'sync',edge:'start'}),/request order/);
  assert.throws(()=>mark({request:'request',label:'other',edge:'ack'}),/request order/);
  mark({request:'request',label:'sync',edge:'ack'});
  assert.throws(()=>mark({request:'request',label:'sync',edge:'ack'}),/request order/);
  f.probe.state.timeOrigin=20001;
  assert.throws(()=>mark({request:'another',label:'sync',edge:'start'}),/changed or overflowed/);
  assert.equal(f.probe.state.persistenceMarkers.length,2);
});
