// Compare the disabled feature with a retained pre-feature authored build.
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {existsSync,mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {presentationServer} from '../../scripts/presentation-server.mjs';
const root=resolve(import.meta.dirname,'../..'),label=process.env.RUN_LABEL;
assert.match(label||'',/^[\w-]+$/);assert.ok(process.env.PC_BUILD&&process.env.BASELINE_BUILD);
const build=resolve(process.env.PC_BUILD),baseline=resolve(process.env.BASELINE_BUILD),out=resolve(root,'artifacts/multiplayer-tests',label);
assert.ok(!existsSync(out));mkdirSync(out,{recursive:true});
const sha=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const identity={candidate:sha(resolve(build,'th09-presentation.wasm')),baseline:sha(resolve(baseline,'th09-presentation.wasm')),test:sha(import.meta.filename)};
const host=await presentationServer(0,{artifactDirectory:build}),old=await presentationServer(0,{artifactDirectory:baseline});
const require=createRequire(process.env.BROWSER_TEST_PACKAGE||resolve(root,'package.json'));
const browser=await require('puppeteer-core').launch({headless:true,executablePath:process.env.BROWSER_EXECUTABLE,protocolTimeout:600000,args:['--enable-unsafe-swiftshader']});
try{
  const pages=await Promise.all([browser.newPage(),browser.newPage()]);
  await Promise.all(pages.map((p,i)=>p.goto([host.url,old.url][i])));
  const run=async(p,candidate)=>p.evaluate(async candidate=>{
    await openProbe(2,2,2,3,true);
    const c=core,world=()=>Array.from(c.HEAP32.subarray(c._th09_probe_world()/4,c._th09_probe_world()/4+48));
    globalThis.__eaglerPeerTransport={route:'rtc',localPlayer:0,playerCount:2,received:[],receivedHead:0,peers:new Map([[1,{inputOpen:true,controlOpen:true,inputDc:{readyState:'open',send(){}},controlDc:{readyState:'open',send(){}}}]])};
    if(candidate&&!c._th09_local_charge_priority(0))throw Error('Disable');
    if(!c._th09_adonis_configure(0)||!c._th09_rollback_begin(12345,0,3,2,2,0x1234,0x5678,0x901,0))throw Error(probeError());
    const audits=[];let starts=0,previous=false;
    for(let f=0;f<6000;++f){
      const v=world(),float=n=>new Float32Array(new Int32Array([n]).buffer)[0];
      const input=side=>{const at=6+side*20,charge=float(v[at+4]),available=float(v[at+5]);return (f<140?256:0)|4|((Math.floor(f/24)+side)%2?64:128)|(available>=205?(charge>=202?0:1):((f+side)%6<3?1:0));};
      if(!c._th09_probe_rollback_step(input(0),input(1),0))throw Error(probeError());
      const row=world(),stopped=!!(row[2]&0x1800);if(stopped&&!previous)++starts;previous=stopped;audits.push(row);
    }
    const hash=c._th09_network_hash()>>>0;
    c.FS.mkdirTree('/save/replay');if(!c._th09_probe_save_replay())throw Error(probeError());
    return {audits,hash,starts,replay:Array.from(c.FS.readFile('/save/replay/th9_25.rpy'))};
  },candidate);
  const [candidate,reference]=await Promise.all([run(pages[0],true),run(pages[1],false)]);
  const first=candidate.audits.findIndex((row,f)=>row.some((v,i)=>v!==reference.audits[f][i]));
  const result={identity,frames:candidate.audits.length,firstDivergence:first,hashes:[candidate.hash,reference.hash],starts:[candidate.starts,reference.starts],replayBytes:candidate.replay.length,replayIdentical:Buffer.from(candidate.replay).equals(Buffer.from(reference.replay))};
  writeFileSync(resolve(out,'report.json'),JSON.stringify(result,null,2)+'\n');
  assert.equal(first,-1);assert.equal(candidate.hash,reference.hash);assert.ok(result.replayIdentical);assert.ok(candidate.starts>=5);
  assert.equal(sha(resolve(build,'th09-presentation.wasm')),identity.candidate);console.log(JSON.stringify({passed:true,...result}));
}finally{await browser.close();for(const h of [host,old]){h.netplay.close();await new Promise(r=>h.server.close(r));}}
