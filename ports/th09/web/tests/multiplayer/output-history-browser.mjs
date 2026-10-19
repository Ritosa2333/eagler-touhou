import assert from 'node:assert/strict';
import {existsSync,mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {createRequire} from 'node:module';
// Actual Release worlds and input packets. Keep delayed-recovery injection
// explicit; this gate is not a public-network latency measurement.
const game=fileURLToPath(new URL('../../',import.meta.url));
const label=process.env.RUN_LABEL;if(!/^[\w-]+$/.test(label||''))throw Error('Unique RUN_LABEL required');
const out=resolve(game,'artifacts/multiplayer-tests',label);if(existsSync(out))throw Error('Retained output-history run already exists');mkdirSync(out,{recursive:true});
const build=resolve(process.env.PC_BUILD||resolve(game,'artifacts/sdl-release'));
const {presentationServer}=await import('../../scripts/presentation-server.mjs');
const {chromium}=createRequire(game+'/package.json')('playwright');
const host=await presentationServer(0,{release:true,artifactDirectory:build});
const expectedBroken=process.argv.includes('--expect-broken');
const browser=await chromium.launch({headless:true,executablePath:process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE,args:['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required']});
const report={scope:'Real Release WASM and native game worlds, real native input protocol over controlled in-page links. Delayed recovery is injected by synchronous native ticks before yielding to timers. Not a measured public-network reproduction.',wasmSha256:createHash('sha256').update(readFileSync(build+'/th09.wasm')).digest('hex'),browser:browser.version(),runs:[]};
try{
 for(const after of [false,true]){
  const page=await browser.newPage();page.setDefaultTimeout(120000);
  await page.goto(host.url);await page.waitForFunction(()=>typeof openProbe==='function');
  const result=await page.evaluate(async after=>{
   const peers=[];
   for(let n=0;n<2;++n){const f=document.createElement('iframe');f.src='/';document.body.append(f);await new Promise(r=>f.onload=r);peers.push(f.contentWindow);}
   for(const w of peers)await w.openProbe(0,1,2,3,true);
   const read=w=>{const at=w.core._th09_rollback_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+8));};
   const error=w=>{const c=w.core,at=c._th09_error();return new TextDecoder().decode(c.HEAPU8.subarray(at,c.HEAPU8.indexOf(0,at)));};
   const pending=[],held=[],published=[],captured=[new Map(),new Map()];let withholding=false;
   peers[0].core.onNetworkSpectatorFrame=(...args)=>published.push(args.slice(0,3));
   for(let side=0;side<2;++side){const w=peers[side];const dc=()=>({readyState:'open',bufferedAmount:0,send:raw=>{
     const bytes=new Uint8Array(raw),isInput=bytes[0]===69&&bytes[5]===1;
     if(isInput){const v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),first=v.getUint32(32,true),count=bytes[42];
      for(let n=0;n<count;++n){const frame=first+n,bits=v.getUint16(44+n*12,true),old=captured[side].get(frame);
       if(old!==undefined&&old!==bits)throw Error('Captured input rewritten');captured[side].set(frame,bits);}}
     (withholding&&side===1&&isInput?held:pending).push({to:1-side,bytes:new Uint8Array(bytes)});
   }});
    w.__eaglerPeerTransport={route:'rtc',localPlayer:side,playerCount:2,received:[],receivedHead:0,peers:new Map([[1-side,{inputOpen:true,controlOpen:true,inputDc:dc(),controlDc:dc()}]])};
    if(!w.core._th09_rollback_recovery_mode(+after)||!w.core._th09_rollback_begin(12345,side,3,0,1,0x1234,0x5678,0x901,0))throw Error('begin '+error(w));
   }
   const deliver=()=>{while(pending.length){const p=pending.shift();peers[p.to].__eaglerPeerTransport.received.push(p.bytes);}};
   for(let attempt=0;attempt<64&&!peers.every(w=>read(w)[6]);++attempt){deliver();for(const w of peers)if(!w.core._th09_rollback_pump())throw Error('handshake '+error(w));}
   if(!peers.every(w=>read(w)[6]))throw Error('Handshake incomplete');
   const tick=w=>{const ok=w.core._th09_game_tick(0);if(!ok)throw Error('tick '+error(w));return ok;};
   // Right captures first, so left consumes exact input and publishes its entire
   // prefix. Use unmodified native game logic, without probe exports or cheats.
   for(let n=0;n<180;++n){deliver();tick(peers[1]);deliver();tick(peers[0]);deliver();}
   const left=peers[0],right=peers[1],base=read(left)[0],trace=[];
   const snap=name=>trace.push({name,left:read(left),right:read(right),error:error(left)});
   if(read(left)[7]!==base||read(right)[0]!==base)throw Error('Warmup frontiers not aligned');
   if(left.core.HEAP32[left.core._th09_session_status()/4]!==1)throw Error('Not native match phase');
   snap('exact-published-prefix');
   // This sets the production eligibility flag. All calls below remain in one
   // JS task, matching multiple due ticks before RAF queues post-Present work.
   left.core._th09_loop_start();withholding=true;
   for(let n=0;n<8;++n){tick(left);deliver();}
   snap('eight-predictions-waiting-for-input');
   // First five predictions were correct. The first mismatch is later than
   // the oldest unpublished output, so the correction-distance guard differs
   // from the output-record retention requirement.
   for(let n=0;n<8;++n){right.core._th09_key(44,+(n>=5));tick(right);deliver();}
   while(held.length)pending.push(held.shift());withholding=false;deliver();
   snap('remote-burst-queued');
   for(let n=0;n<40&&read(left)[0]<base+10;++n){tick(left);deliver();}
   snap('two-visible-ticks-before-recovery');
   const at=left.core._th09_confirmed_frame_hash(base)/4;
   const oldestOutputOwner=left.core.HEAPU32[at];
   // Stop cancels queued presentation work and makes the next native tick
   // finish recovery. This does not clear the network session or output ring.
   left.core._th09_loop_stop();let ok=1;
   for(let n=0;n<40&&ok&&read(left)[7]<base+8;++n)ok=left.core._th09_game_tick(0);
   snap('publication-after-recovery');
   for(const w of peers)w.core._th09_loop_stop();
   const publicationIsContiguous=published.every((p,n)=>p[0]===n&&p[1]===captured[0].get(n)&&p[2]===captured[1].get(n));
   const confirmedHash=w=>{const at=w.core._th09_confirmed_frame_hash(base+7)/4;return Array.from(w.core.HEAPU32.subarray(at,at+2));};
   return {after,base,oldestOutputOwner,ok,error:error(left),trace,publicationIsContiguous,publishedFrames:published.length,hashes:peers.map(confirmedHash),uniqueCaptures:captured.map(c=>c.size)};
  },after);
  report.runs.push(result);writeFileSync(out+'/report.json',JSON.stringify(report,null,2)+'\n');
  console.log(JSON.stringify(result));await page.close();
 }
 for(const run of report.runs){
  if(expectedBroken&&run.after){assert.equal(run.ok,0);assert.equal(run.error,'Confirmed output history expired');continue;}
  assert.notEqual(run.ok,0);assert.equal(run.error,'');
  assert.equal(run.publicationIsContiguous,true,'confirmed output once, in order, with exact wire inputs');
  assert.ok(run.publishedFrames>=run.base+8,'confirmed prefix drained');
  assert.equal(run.hashes[0][0],run.base+7);assert.deepEqual(run.hashes[0],run.hashes[1],'corrected native world matches exact peer');
  const end=run.trace.at(-1).left;assert.equal(end[4],run.uniqueCaptures[0]);
  assert.ok(end[2]>0,'real correction performed');
 }
 report.passed=true;
}catch(error){report.passed=false;report.failure=String(error.stack);throw error;}
finally{writeFileSync(out+'/report.json',JSON.stringify(report,null,2)+'\n');await browser.close();host.netplay.close();await new Promise(r=>host.server.close(r));}
