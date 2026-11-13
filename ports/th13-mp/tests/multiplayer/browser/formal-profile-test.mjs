import {test} from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,readFile} from 'node:fs/promises';
import {join} from 'node:path';
import {tmpdir} from 'node:os';
import {startCpuProfiles} from './formal-profile.mjs';
test('a hung profiler fails its seat while preserving another complete profile',async()=>{
  const dir=await mkdtemp(join(tmpdir(),'profile-deadline-'));
  const pages=[true,false].map(()=>({evaluate:async()=>({runtime:null})}));
  const sessions=[true,false].map(hung=>({
    send:async name=>name==='Profiler.stop'?(hung?new Promise(()=>{}):{profile:{startTime:1,endTime:2,samples:[1],nodes:[{}]}}):{},
    detach:async()=>{},
  }));
  const profile=await startCpuProfiles({pages,sessions,directory:dir,seconds:30,deadlineMs:10});
  const report=await profile.finish();assert.equal(report.passed,false);
  assert.match(report.seats[0].error,/timed out/);assert.equal(report.seats[1].samples,1);
  assert((await readFile(join(dir,'cpu-seat-1.cpuprofile'))).length>0);
  assert.equal(JSON.parse(await readFile(join(dir,'cpu-profiles.json'),'utf8')).passed,false);
});
test('uses only existing throttle session and never detaches it on finish or setup failure',async()=>{
  const dir=await mkdtemp(join(tmpdir(),'profile-session-'));
  let detaches=0,created=0;const calls=[];
  const page={evaluate:async()=>({runtime:null}),context:()=>({newCDPSession(){created++;throw Error('must not create');}})};
  let fail=false;
  const session={send:async method=>{calls.push(method);if(fail&&method==='Profiler.start')throw Error('start failed');
    if(method==='Profiler.stop')return {profile:{startTime:1,endTime:2,samples:[1],nodes:[{}]}};return {};},detach:async()=>{detaches++;}};
  const profile=await startCpuProfiles({pages:[page],sessions:[session],directory:dir,seconds:30,deadlineMs:50});
  assert.equal((await profile.finish()).passed,true);assert.equal(detaches,0);assert.equal(created,0);
  fail=true;await assert.rejects(startCpuProfiles({pages:[page],sessions:[session],directory:dir,seconds:30,deadlineMs:50}),/start failed/);
  assert.equal(detaches,0);assert.equal(created,0);assert(calls.includes('Profiler.disable'));
  await assert.rejects(startCpuProfiles({pages:[page],directory:dir,seconds:30}),/existing per-page/);
});
