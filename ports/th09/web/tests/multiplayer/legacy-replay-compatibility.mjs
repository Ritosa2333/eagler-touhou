// Compare real legacy Replay playback with the retained pre-feature binary.
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {presentationServer} from '../../scripts/presentation-server.mjs';
const root=resolve(import.meta.dirname,'../..'),label=process.env.RUN_LABEL;
assert.match(label||'',/^[\w-]+$/);
for(const key of ['PC_BUILD','BASELINE_BUILD','TH09_REPLAY_FILE','BROWSER_TEST_PACKAGE','BROWSER_EXECUTABLE'])assert.ok(process.env[key],key);
const out=resolve(root,'artifacts/multiplayer-tests',label);assert.ok(!existsSync(out));mkdirSync(out,{recursive:true});
const baseline=resolve(process.env.BASELINE_BUILD),candidate=resolve(process.env.PC_BUILD),fixture=readFileSync(process.env.TH09_REPLAY_FILE);
const sha=b=>createHash('sha256').update(b).digest('hex');
const identity={baseline:sha(readFileSync(baseline+'/th09-presentation.wasm')),candidate:sha(readFileSync(candidate+'/th09-presentation.wasm')),fixture:sha(fixture),test:sha(readFileSync(import.meta.filename))};
const hosts=await Promise.all([baseline,candidate].map(artifactDirectory=>presentationServer(0,{artifactDirectory})));
const require=createRequire(process.env.BROWSER_TEST_PACKAGE),browser=await require('puppeteer-core').launch({headless:true,
 executablePath:process.env.BROWSER_EXECUTABLE,protocolTimeout:600000,args:['--enable-unsafe-swiftshader']});
const result={identity};
try{
 const pages=await Promise.all(Array.from({length:3},()=>browser.newPage()));
 for(let i=0;i<3;i++){pages[i].on('console',m=>{if(m.text().startsWith('Replay '))console.log(i+' '+m.text());});
  await pages[i].goto(hosts[i===0?0:1].url);await pages[i].evaluate(()=>openProbe(2,2,2,1,true));}
 const playback=async(bytes,limit,stage)=>Promise.all(pages.map((page,i)=>page.evaluate(async({bytes,limit,mode,stage})=>{
  const c=core;if(c._th09_local_charge_priority&&!c._th09_local_charge_priority(mode))throw Error('charge preference');
  c.HEAPU8.set(bytes,c._th09_import_buffer(bytes.length));
  if(!c._th09_probe_replay(bytes.length,stage))throw Error('import '+probeError());
  const trace=new Int32Array(limit*56),stages=[];let ticks=0,previous=-1;
  for(;ticks<limit;++ticks){
   if(!c._th09_game_tick(0))throw Error('tick '+ticks+' '+probeError());
   const s=Array.from(c.HEAP32.subarray(c._th09_session_status()/4,c._th09_session_status()/4+8));
   const p=c._th09_probe_world()/4;trace.set(c.HEAP32.subarray(p,p+48),ticks*56);trace.set(s,ticks*56+48);
   if(s[5]!==previous){previous=s[5];stages.push(previous);}
   if(s[0]!==1){++ticks;break;}
   if(ticks%10000===9999)console.log('Replay '+(ticks+1));
   if(ticks%240===239)await new Promise(r=>setTimeout(r,0));
  }
  const digest=await crypto.subtle.digest('SHA-256',trace.subarray(0,ticks*56));
  const final=Array.from(c.HEAP32.subarray(c._th09_session_status()/4,c._th09_session_status()/4+8));
  return {ticks,stages,final,bridges:c._th09_probe_charge_bridges?.()??0,
   sha256:Array.from(new Uint8Array(digest),b=>b.toString(16).padStart(2,'0')).join('')};
 },{bytes:Array.from(bytes),limit,stage,mode:i===2?1:0})));
 result.native=await playback(fixture,90000,0);
 for(const row of result.native){assert.deepEqual(row,result.native[0]);assert.deepEqual(row.stages,[0,1,2,3,4,5,6,7,8]);assert.equal(row.final[0],5);assert.equal(row.final[1],71637);assert.equal(row.bridges,0);}
 // The baseline creates an authentic motion-only Replay using the old writer.
 const motion=await pages[0].evaluate(async()=>{
  await openProbe(2,2,2,1,false);const c=core;c._th09_touch_options(1,3,1,0,0);c._th09_touch_controls(1,1,0,0,0);
  for(let n=0;n<1800;++n){c._th09_touch_stick(Math.sin(n*.025)*14000,Math.cos(n*.031)*12000);
   if(!c._th09_game_tick(0))throw Error(probeError());if(n%240===239)await new Promise(r=>setTimeout(r,0));}
  c._th09_touch_controls(0,0,0,0,0);c._th09_touch_stick(0,0);
  c.FS.mkdirTree('/save/replay');if(!c._th09_probe_save_replay())throw Error('save '+probeError());
  return Array.from(c.FS.readFile('/save/replay/th9_25.rpy'));
 });
 const bytes=Buffer.from(motion);assert.equal(bytes.subarray(-24,-16).toString(),'THMOTION');assert.ok(!bytes.includes(Buffer.from('T9CHARGE')));
 result.motionFixture={bytes:bytes.length,sha256:sha(bytes)};
 // Fresh modules also reset diagnostic call counters before the comparison.
 for(const page of pages)await page.evaluate(()=>openProbe(2,2,2,1,true));
 result.motion=await playback(bytes,1800,9);for(const row of result.motion)assert.deepEqual(row,result.motion[0]);
 result.passed=true;console.log(JSON.stringify(result));
}finally{writeFileSync(out+'/report.json',JSON.stringify(result,null,2)+'\n');await browser.close();
 for(const h of hosts){h.netplay.close();await new Promise(r=>h.server.close(r));}}
