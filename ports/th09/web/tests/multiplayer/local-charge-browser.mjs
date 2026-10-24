// Authored WASM worlds | native rAF and post-Present recovery | synthetic wire.
// Compare accepted local gauges, exact-command simulation and saved Replay.
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {existsSync, mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {validateChargeReplay,exportReplayName} from '../../sdl-runtime/motion-replay.mjs';

const root=resolve(import.meta.dirname,'../..');
const require=createRequire(process.env.BROWSER_TEST_PACKAGE||resolve(root,'package.json'));
const puppeteer=require('puppeteer-core');
const label=process.env.RUN_LABEL;
assert.match(label||'',/^[\w-]+$/,'Set a fresh RUN_LABEL');
assert.ok(process.env.PC_BUILD,'Set PC_BUILD to a development build');
assert.ok(process.env.BROWSER_EXECUTABLE,'Set BROWSER_EXECUTABLE to Chromium or Edge');
const build=resolve(process.env.PC_BUILD),out=resolve(root,'artifacts/multiplayer-tests',label);
assert.ok(!existsSync(out),'Retain previous runs under their original labels');
mkdirSync(out,{recursive:true});
const limit=Number(process.env.FRAMES||7200),delay=Number(process.env.PACKET_DELAY||5);
const jitter=Number(process.env.PACKET_JITTER||0),warmup=Number(process.env.SKILL_WARMUP||0);
const chained=process.env.CHAINED_SKILLS==='1';
const pauseCycle=process.env.PAUSE_CYCLE==='1';
assert.ok(Number.isInteger(limit)&&limit>=2400&&limit<=24000);
assert.ok(Number.isInteger(delay)&&delay>=1&&Number.isInteger(jitter)&&jitter>=0&&delay+jitter<=7);
assert.ok(Number.isInteger(warmup)&&warmup>=0&&warmup<2400);
const sha=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const identity={wasm:sha(resolve(build,'th09-presentation.wasm')),test:sha(import.meta.filename)};
const host=await presentationServer(0,{artifactDirectory:build});
const browser=await puppeteer.launch({headless:true,executablePath:process.env.BROWSER_EXECUTABLE,
  protocolTimeout:600000,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
const errors=[];
try{
  const page=await browser.newPage();page.on('pageerror',e=>errors.push(e.message));await page.goto(host.url);
  const result=await page.evaluate(async({limit,delay,jitter,warmup,chained,pauseCycle})=>{
    const windows=[],pending=[],tapes=[[],[]],captured=[[],[]],presented=[[],[]],exact=[],charges=[[],[]];
    let clock=0,repeatedCharge=0;
    for(let side=0;side<3;++side){
      const frame=document.createElement('iframe');frame.src='/';document.body.append(frame);
      await new Promise(r=>frame.onload=r);windows.push(frame.contentWindow);
    }
    for(const w of windows)await w.openProbe(2,2,2,3,true);
    const peers=windows.slice(0,2),ref=windows[2];
    const values=(w,name,count)=>{const c=w.core,p=c[name]()/4;return Array.from(c.HEAP32.subarray(p,p+count));};
    const status=w=>values(w,'_th09_rollback_info',8),world=w=>values(w,'_th09_probe_world',48);
    const float=n=>new Float32Array(new Int32Array([n]).buffer)[0];
    const presentations=w=>values(w,'_th09_game_metrics',10)[9];
    const load=w=>values(w,'_th09_probe_load',8);
    const send=(side,bytes)=>{
      const input=bytes[0]===69&&bytes[5]===1;
      const charge=bytes[0]===84&&bytes[1]===57&&bytes[2]===67&&(bytes[3]===80||bytes[3]===66);
      if(charge){
        const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),batch=bytes[3]===66,count=batch?view.getUint32(20,true):1;
        for(let i=0;i<count;++i){
          const offset=batch?24+i*44:12,frame=view.getUint32(offset,true),start=batch?offset+4:20,raw=Array.from(bytes.subarray(start,start+40));
          if(charges[side][frame]){
            if(charges[side][frame].raw.some((b,j)=>b!==raw[j]))throw Error('Rewritten charge '+frame);
            ++repeatedCharge;
          }else charges[side][frame]={charge:view.getInt32(start,true),active:view.getUint32(start+36,true),raw};
        }
      }
      if(input){
        const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
        const start=view.getUint32(32,true),count=bytes[42];
        for(let i=0;i<count;++i){
          const frame=start+i,bits=view.getUint16(44+i*12,true);
          if(captured[side][frame]!==undefined&&captured[side][frame]!==bits)throw Error('Rewritten input '+frame);
          captured[side][frame]=bits;
        }
      }
      pending.push({to:1-side,due:clock+(input||charge?delay+clock%(jitter+1):0),bytes:new Uint8Array(bytes)});
    };
    for(let side=0;side<3;++side){
      const w=windows[side],dc=()=>({readyState:'open',bufferedAmount:0,send:b=>{if(side<2)send(side,b);}});
      w.__eaglerPeerTransport={route:'rtc',localPlayer:side%2,playerCount:2,received:[],receivedHead:0,
        peers:new Map([[1-side%2,{inputOpen:true,controlOpen:true,inputDc:dc(),controlDc:dc()}]])};
      if(side<2&&!w.core._th09_rollback_recovery_mode(1))throw Error('Recovery mode');
      if(!w.core._th09_local_charge_priority(+(side<2)))throw Error('Charge mode');
      if(!w.core._th09_adonis_configure(0)||!w.core._th09_rollback_begin(12345,side%2,3,2,2,0x1234,0x5678,0x901,0))
        throw Error('Begin '+w.probeError());
      w.core._th09_probe_frame_limit(limit);
    }
    // Generate both players' physical input independently of either prediction.
    // Tap to build meter, then release at level 2. Optional shot-only intervals
    // let the authored enemy/bullet workload build up before the next exchange.
    let pairPhase=0,firstSide=0,pairs=0;
    for(let f=0;f<limit;++f){
      const v=world(ref);
      const stopped=!!(v[2]&0x1800);
      if(chained){
        if(pairPhase===0&&!stopped&&f%2400>=warmup&&[0,1].every(s=>float(v[6+s*20+4])>=202))pairPhase=1;
        else if(pairPhase===1&&stopped)pairPhase=2;
        else if(pairPhase===2&&!stopped)pairPhase=3;
        else if(pairPhase===3&&stopped)pairPhase=4;
        else if(pairPhase===4&&!stopped){pairPhase=0;firstSide=1-firstSide;++pairs;}
      }
      for(let side=0;side<2;++side){
        const p=6+side*20,charge=float(v[p+4]),available=float(v[p+5]);
        const charging=available>=205&&f%2400>=warmup;
        let shot=charging?(chained?1:charge>=202?0:1):((f+side)%6<3?1:0);
        if(chained&&((pairPhase===1&&side===firstSide)||(pairPhase===3&&side!==firstSide)))shot=0;
        if(chained&&pairPhase===2&&side!==firstSide)shot=1;
        const direction=(Math.floor(f/24)+side)%4;
        tapes[side][f]=(f<140?256:0)|shot|(direction===0?64:direction===2?128:0)|4;
        if(pauseCycle&&f>=600&&f<=720)tapes[side][f]=side===0&&(f===600||f===700)?8:0;
      }
      if(!ref.core._th09_probe_rollback_step(tapes[0][f],tapes[1][f],0))throw Error('Reference '+ref.probeError());
    }
    for(let side=0;side<2;++side){
      const w=peers[side];w.queue=[];w.requestAnimationFrame=cb=>{w.queue.push(cb);return w.queue.length;};
      let lastPresent=presentations(w);
      w.core.onGameFrame=ok=>{
        if(!ok)throw Error('Native loop '+w.probeError()+' '+JSON.stringify({side,clock,status:status(w),world:world(w)}));
        const count=presentations(w);if(count===lastPresent)return;lastPresent=count;
        presented[side].push({clock,frame:status(w)[0],world:world(w),load:load(w),visual:values(w,'_th09_probe_charge_visual',8)});
      };
      w.core._th09_loop_start();
    }
    const setKeys=(w,bits)=>{for(const [bit,scan] of [[1,44],[2,45],[4,42],[8,1],[64,203],[128,205],[256,29]])w.core._th09_key(scan,Number(!!(bits&bit)));};
    for(;clock<limit*3;++clock){
      for(let i=pending.length-1;i>=0;--i)if(pending[i].due<=clock){
        const packet=pending.splice(i,1)[0];peers[packet.to].__eaglerPeerTransport.received.push(packet.bytes);
      }
      for(let side=0;side<2;++side){
        const w=peers[side];setKeys(w,tapes[side][status(w)[4]]||0);
        const callback=w.queue.shift();if(callback)callback(clock*1000/60);
      }
      // Deliver the real native post-Present timer between manual rAF callbacks.
      await new Promise(r=>setTimeout(r,0));
      if(peers.every(w=>status(w)[0]===limit&&status(w)[1]===limit))break;
    }
    for(const w of peers)w.core._th09_loop_stop();
    const capturesComplete=captured.every(t=>t.length===limit&&Array.from({length:limit},(_,f)=>Number.isInteger(t[f])).every(Boolean));
    if(!capturesComplete)throw Error('Captured input gap');
    // The physical keyboard adapter can retain a brief stroke during a wait.
    // The exact oracle consumes the actual captured wire input, just as Replay
    // does, so input buffering cannot masquerade as a rollback state error.
    const summary=peers.map(status),hashes=peers.map(w=>w.core._th09_network_hash()>>>0),audits=peers.map(world);
    const images=peers.map(w=>{w.core._th09_game_draw();return w.document.querySelector('canvas').toDataURL();});
    const recordings=peers.map(w=>values(w,'_th09_probe_charge_recording',5));
    const saves=peers.map(w=>{
      w.core.FS.mkdirTree('/save/replay');
      if(!w.core._th09_probe_save_replay())throw Error('Save '+w.probeError());
      return Array.from(w.core.FS.readFile('/save/replay/th9_25.rpy'));
    });
    ref.core._th09_network_end();
    if(!ref.core._th09_local_charge_priority(1))throw Error('Exact mode');
    if(!ref.core._th09_rollback_begin(12345,0,3,2,2,0x1234,0x5678,0x901,0))throw Error('Reference restart '+ref.probeError());
    if(!ref.core._th09_probe_charge_reference_mode(1))throw Error('Exact command mode');
    const recordedWorlds=[];let priorRecorded=0;
    for(let f=0;f<limit;++f){
      for(let side=0;side<2;++side){
        const sample=charges[side][f];if(!sample)throw Error('Charge gap '+side+':'+f);
        ref.core.HEAPU8.set(sample.raw,ref.core._th09_probe_charge_reference_buffer()+side*40);
      }
      if(!ref.core._th09_probe_rollback_step(captured[0][f],captured[1][f],0))throw Error('Exact replay '+ref.probeError());
      exact.push(world(ref));
      const count=values(ref,'_th09_probe_charge_recording',5)[1];
      if(count<priorRecorded)recordedWorlds.length=0;
      if(count>priorRecorded||(count<priorRecorded&&count>0))recordedWorlds[count]=world(ref);priorRecorded=count;
    }
    const exactHash=ref.core._th09_network_hash()>>>0,exactAudit=world(ref);
    ref.core._th09_game_draw();const exactImage=ref.document.querySelector('canvas').toDataURL();
    const replayErrors=[],replayFields=(v,calls=v[1])=>v.map((n,i)=>i===2?n&~8:i===1?calls:n);
    ref.core._th09_network_end();ref.core._th09_probe_charge_reference_mode(0);
    ref.core.HEAPU8.set(saves[0],ref.core._th09_import_buffer(saves[0].length));
    if(!ref.core._th09_probe_replay(saves[0].length,9))throw Error('Play '+ref.probeError());
    let replayFrames=0,priorReplayCalls=world(ref)[1];
    for(let tick=0;tick<limit*2;++tick){
      if(!ref.core._th09_probe_rollback_step(0,0,0))throw Error('Playback '+ref.probeError());
      // Live capture resets its call counter each tick. Replay retains the
      // counter, so compare the per-tick delta while checking the seed exactly.
      const raw=world(ref),calls=raw[1]-priorReplayCalls;priorReplayCalls=raw[1];
      const count=values(ref,'_th09_replay_view_info',9)[1];
      if(count>replayFrames){
        const actual=replayFields(raw,calls),expected=recordedWorlds[count]&&replayFields(recordedWorlds[count]);
        if(!expected||actual.some((v,i)=>v!==expected[i])){
          if(replayErrors.length<5)replayErrors.push({tick,frame:count,actual,expected});
        }
        replayFrames=count;
      }
      if(replayFrames===recordings[0][1])break;
    }
    const spectatorErrors=[];
    // Feed the production spectator ABI, then step fixed seek targets so each
    // resulting world can be compared with the accepted-command reference.
    if(!ref.core._th09_room_return())throw Error('Return before spectator');
    if(ref.core._th09_probe_charge_bridges())throw Error('Replay charge bridges survived room return');
    if(!ref.core._th09_room_preview(0))throw Error('Leave room preview before spectator');
    if(!ref.core._th09_spectator_begin(12345,3,2,2))throw Error('Spectator begin '+ref.probeError());
    for(let f=0;f<limit;++f){
      for(let side=0;side<2;++side)ref.core.HEAPU8.set(charges[side][f].raw,ref.core._th09_spectator_charge_buffer()+side*40);
      if(!ref.core._th09_spectator_feed_charge(f,captured[0][f],captured[1][f],0,0,0,0,0,0))throw Error('Spectator feed '+f);
    }
    if(!ref.core._th09_spectator_finish(limit))throw Error('Spectator end');
    for(let f=0;f<limit;++f){
      if(!ref.core._th09_spectator_control(4,f+1)||!ref.core._th09_game_tick(0))throw Error('Spectator tick '+f+' '+ref.probeError());
      const actual=world(ref),expected=exact[f];
      if(actual.some((v,i)=>v!==expected[i])&&spectatorErrors.length<5)spectatorErrors.push({frame:f,actual,expected});
    }
    const spectatorFrames=ref.core._th09_spectator_frame();
    ref.core._th09_spectator_end();
    if(ref.core._th09_probe_charge_bridges())throw Error('Spectator charge bridges survived departure');
    const metrics=presented.map((rows,side)=>{
      const p=6+side*20,errors=[],drawErrors=[];let frozen=0,maxBullets=0,starts=0,prior=false;
      for(const row of rows){
        const stopped=!!(row.world[2]&0x1800);if(stopped&&!prior)++starts;prior=stopped;
        maxBullets=Math.max(maxBullets,row.load[0]+row.load[1]);if(stopped)++frozen;
        const sample=charges[side][row.frame-1];
        if(sample?.active&&sample.charge!==row.world[p+4])errors.push({frame:row.frame,clock:row.clock,charge:float(row.world[p+4]),accepted:float(sample.charge)});
        const expected=Math.fround(float(row.world[p+4])*Math.fround(.0025));
        if(sample?.active&&float(row.visual[side*4+1])!==expected)drawErrors.push({frame:row.frame,actual:float(row.visual[side*4+1]),expected});
      }
      return {presentations:rows.length,frozen,starts,maxBullets,errors:errors.slice(0,10),drawErrors:drawErrors.slice(0,10)};
    });
    let lastEnd=-1,prior=false,shortGaps=0;
    for(let f=0;f<exact.length;++f){
      const stopped=!!(exact[f][2]&0x1800);
      if(stopped&&!prior&&lastEnd>=0&&f-lastEnd-1<=3)++shortGaps;
      if(!stopped&&prior)lastEnd=f-1;prior=stopped;
    }
    const captureDifferences=captured.map((t,side)=>({length:t.length,
      different:t.flatMap((bits,f)=>bits===tapes[side][f]?[]:[{frame:f,planned:tapes[side][f],captured:bits}]).slice(0,20)}));
    return {clock,summary,hashes,audits,exactHash,exactAudit,imagesEqual:[...images,exactImage].every(i=>i===exactImage),metrics,pairs,shortGaps,captureDifferences,capturesComplete,chargeCounts:charges.map(v=>v.length),repeatedCharge,
      recordings,replayFrames,replayErrors,spectatorFrames,spectatorErrors,savesEqual:saves[0].length===saves[1].length&&saves[0].every((b,i)=>b===saves[1][i]),replayBytes:saves[0].length,
      encodedReplay:btoa(saves[0].map(b=>String.fromCharCode(b)).join('')),
      capturesMatch:captured.every((t,side)=>t.length===limit&&t.every((bits,f)=>bits===tapes[side][f]))};
  },{limit,delay,jitter,warmup,chained,pauseCycle});
  const replay=new Uint8Array(Buffer.from(result.encodedReplay,'base64'));delete result.encodedReplay;
  writeFileSync(resolve(out,'th9_25.rpyx'),replay);
  result.chargeReplayEnvelope=validateChargeReplay(replay,9);
  result.exportedName=exportReplayName('th9_25.rpy',replay,9);
  const report={identity,parameters:{limit,delay,jitter,warmup,chained,pauseCycle},browser:await browser.version(),
    scope:'Authored game, two native rollback drivers, manual rAF clock, real post-Present timer and synthetic delayed packets. Local accepted gauge and drawn scale, exact accepted-command simulation, native Replay input-clock comparison. Synthetic transport timing does not measure device performance.',result,errors};
  writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');
  assert.deepEqual(errors,[]);
  assert.ok(result.capturesComplete,'Every captured input retained on the wire');
  for(const state of result.summary){assert.equal(state[0],limit);assert.equal(state[1],limit);assert.equal(state[4],limit,'One physical input capture per logical frame');assert.ok(state[2]>0&&state[3]>0);}
  assert.equal(new Set([...result.hashes,result.exactHash]).size,1,'Both final worlds match exact accepted commands');
  assert.deepEqual(result.audits[0],result.audits[1]);
  assert.deepEqual(result.audits[0],result.exactAudit);
  assert.deepEqual(result.chargeCounts,[limit,limit]);
  assert.ok(result.imagesEqual,'Final framebuffer matches exact input');
  for(const m of result.metrics){assert.ok(m.frozen>100&&m.starts>=5,'Repeated level-2 cut-ins exercised');assert.deepEqual(m.errors,[],'Local accepted charge is preserved');assert.deepEqual(m.drawErrors,[],'HUD draws the accepted charge');}
  assert.ok(result.savesEqual,'Both players save identical Replay files');
  assert.equal(result.chargeReplayEnvelope,true);assert.equal(result.exportedName,'th9_25.rpyx');
  assert.equal(result.replayFrames,result.recordings[0][1]);
  assert.deepEqual(result.replayErrors,[],'Replay matches every recorded input tick');
  if(pauseCycle)assert.ok(result.recordings[0][4]>result.recordings[0][1],'Paused charge rows were preserved outside the native input clock');
  assert.equal(result.spectatorFrames,limit);assert.deepEqual(result.spectatorErrors,[],'Spectator matches every accepted-command frame');
  if(chained)assert.ok(result.pairs>=5&&result.shortGaps>=5,'Both players chained level-2 skills within three ticks');
  assert.equal(sha(resolve(build,'th09-presentation.wasm')),identity.wasm,'Build stayed frozen');
  console.log(JSON.stringify({passed:true,identity,parameters:report.parameters,pairs:result.pairs,shortGaps:result.shortGaps,metrics:result.metrics}));
}finally{
  await browser.close();host.netplay.close();await new Promise(r=>host.server.close(r));
}
