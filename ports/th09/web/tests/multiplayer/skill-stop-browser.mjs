// Authored WASM worlds | native rAF and post-Present recovery | synthetic wire.
// Measure cadence and predicted poses at skill-stop and resume boundaries.
// Confirmed worlds and the final replay oracle must match exactly.
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {existsSync, mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {presentationServer} from '../../scripts/presentation-server.mjs';

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
const localCharge=process.env.LOCAL_CHARGE==='1',expectRegression=process.env.EXPECT_REGRESSION==='1';
const expectResumeWait=process.env.EXPECT_RESUME_WAIT==='1';
const auditPrediction=process.env.AUDIT_PREDICTION!=='0',delayControl=process.env.DELAY_CONTROL==='1';
const expectContinuous=process.env.EXPECT_CONTINUOUS==='1';
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
  const result=await page.evaluate(async({limit,delay,jitter,warmup,chained,localCharge,expectRegression,auditPrediction,delayControl})=>{
    const windows=[],pending=[],tapes=[[],[]],captured=[[],[]],presented=[[],[]],exact=[],exactDraw=[],charges=[[],[]];
    let clock=0;
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
    const draw=w=>w.core._th09_probe_stop_draw?values(w,'_th09_probe_stop_draw',9):null;
    const send=(side,bytes)=>{
      const input=bytes[0]===69&&bytes[5]===1;
      const charge=bytes[0]===84&&bytes[1]===57&&bytes[2]===67&&bytes[3]===80;
      if(charge){const v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),f=v.getUint32(12,true),raw=Array.from(bytes.subarray(20));
        if(charges[side][f]&&charges[side][f].some((b,i)=>b!==raw[i]))throw Error('Rewritten charge '+f);charges[side][f]=raw;}
      if(input){
        const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
        const start=view.getUint32(32,true),count=bytes[42];
        for(let i=0;i<count;++i){
          const frame=start+i,bits=view.getUint16(44+i*12,true);
          if(captured[side][frame]!==undefined&&captured[side][frame]!==bits)throw Error('Rewritten input '+frame);
          captured[side][frame]=bits;
        }
      }
      pending.push({to:1-side,due:clock+(input||charge||delayControl?delay+clock%(jitter+1):0),bytes:new Uint8Array(bytes)});
    };
    for(let side=0;side<3;++side){
      const w=windows[side],dc=()=>({readyState:'open',bufferedAmount:0,send:b=>{if(side<2)send(side,b);}});
      w.__eaglerPeerTransport={route:'rtc',localPlayer:side%2,playerCount:2,received:[],receivedHead:0,
        peers:new Map([[1-side%2,{inputOpen:true,controlOpen:true,inputDc:dc(),controlDc:dc()}]])};
      if(side<2&&!w.core._th09_rollback_recovery_mode(1))throw Error('Recovery mode');
      if(localCharge&&!w.core._th09_local_charge_priority)throw Error('Local charge ABI required');
      if(w.core._th09_local_charge_priority&&!w.core._th09_local_charge_priority(+(localCharge&&side<2)))throw Error('Charge policy');
      if(!expectRegression&&!w.core._th09_probe_stop_draw)throw Error('Draw observation required');
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
      }
      if(!ref.core._th09_probe_rollback_step(tapes[0][f],tapes[1][f],0))throw Error('Reference '+ref.probeError());
    }
    for(let side=0;side<2;++side){
      const w=peers[side];w.queue=[];w.requestAnimationFrame=cb=>{w.queue.push(cb);return w.queue.length;};
      let lastPresent=presentations(w);
      w.core.onGameFrame=ok=>{
        if(!ok)throw Error('Native loop '+w.probeError()+' '+JSON.stringify({side,clock,status:status(w)}));
        const count=presentations(w);if(count===lastPresent)return;lastPresent=count;
        presented[side].push({clock,frame:status(w)[0],confirmed:status(w)[1],world:world(w),load:load(w),draw:draw(w),
          visual:w.core._th09_probe_charge_visual?values(w,'_th09_probe_charge_visual',8):null});
      };
      w.core._th09_loop_start();
    }
    const setKeys=(w,bits)=>{for(const [bit,scan] of [[1,44],[2,45],[4,42],[64,203],[128,205],[256,29]])w.core._th09_key(scan,Number(!!(bits&bit)));};
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
    const stopWaits=peers.map(w=>w.core._th09_probe_stop_boundary_waits?values(w,'_th09_probe_stop_boundary_waits',2):null);
    const capturesComplete=captured.every(t=>t.length===limit&&Array.from({length:limit},(_,f)=>Number.isInteger(t[f])).every(Boolean));
    if(!capturesComplete)throw Error('Captured input gap');
    // The physical keyboard adapter can retain a brief stroke during a wait.
    // The exact oracle consumes the actual captured wire input, just as Replay
    // does, so input buffering cannot masquerade as a rollback state error.
    ref.core._th09_network_end();
    if(localCharge&&!ref.core._th09_local_charge_priority(1))throw Error('Reference policy');
    if(!ref.core._th09_rollback_begin(12345,0,3,2,2,0x1234,0x5678,0x901,0))throw Error('Reference restart '+ref.probeError());
    if(localCharge&&!ref.core._th09_probe_charge_reference_mode(1))throw Error('Reference commands');
    for(let f=0;f<limit;++f){
      if(localCharge)for(let side=0;side<2;++side){if(!charges[side][f])throw Error('Missing charge record '+f);ref.core.HEAPU8.set(charges[side][f],ref.core._th09_probe_charge_reference_buffer()+40*side);}
      if(!ref.core._th09_probe_rollback_step(captured[0][f],captured[1][f],0))throw Error('Exact replay '+ref.probeError());
      exact.push(world(ref));
      exactDraw.push(draw(ref));
    }
    const summary=peers.map(status),hashes=windows.map(w=>w.core._th09_network_hash()>>>0),audits=windows.map(world);
    const images=windows.map(w=>{w.core._th09_game_draw();return w.document.querySelector('canvas').toDataURL();});
    const metrics=presented.map((rows,side)=>{
      const gaps=rows.slice(1).map((row,i)=>({clock:row.clock,frame:row.frame,gap:row.clock-rows[i].clock}));
      const cadence={maxGap:Math.max(0,...gaps.map(g=>g.gap)),missedOpportunities:gaps.reduce((n,g)=>n+Math.max(0,g.gap-1),0),longGaps:gaps.filter(g=>g.gap>1).slice(0,24)};
      const errors=[],gaugeErrors=[],confirmedErrors=[];let frozen=0,resumed=0,edges=0,maxBullets=0,maxPose=0,maxDraw=0,starts=0,prior=false,priorBits=0,unseenStopFrames=0,unseenMaxPose=0,unconfirmedChargeFrames=0;
      for(const row of rows){
        if(!expectRegression){
          if(!row.visual)throw Error('Charge observation required');
          const charge=float(row.world[6+side*20+4]),scale=float(row.visual[side*4+1]);
          if(scale!==Math.fround(charge*Math.fround(.0025)))gaugeErrors.push({frame:row.frame,charge,scale});
          if(row.frame>row.confirmed&&charge>0)++unconfirmedChargeFrames;
        }
        const stopBits=row.draw?row.draw[8]:row.world[2]&0x1800;
        const stopped=!!(row.world[2]&0x1800),wasStopped=prior,edge=stopBits!==priorBits;
        if(stopped&&!prior)++starts;prior=stopped;priorBits=stopBits;
        const expected=exact[row.frame-1];
        if(row.frame<=row.confirmed&&row.world.some((n,i)=>n!==expected[i]))confirmedErrors.push({frame:row.frame});
        const expectedBits=exactDraw[row.frame-1]?.[8]??(expected[2]&0x1800);
        const expectedStop=!!(expected[2]&0x1800),resume=!stopped&&wasStopped;
        maxBullets=Math.max(maxBullets,row.load[0]+row.load[1]);
        // A not-yet-received opponent release cannot be known to zero-delay
        // rollback. Retain that separate prediction diagnostic; the acceptance
        // diagnostics cover displayed stops and their first displayed resume.
        if(!stopped&&expectedStop&&!resume&&!edge){++unseenStopFrames;
          for(let player=0;player<2;++player){const p=6+player*20;unseenMaxPose=Math.max(unseenMaxPose,Math.hypot(float(row.world[p])-float(expected[p]),float(row.world[p+1])-float(expected[p+1])));}continue;}
        if(stopped)++frozen;if(resume)++resumed;if(edge)++edges;
        if(!stopped&&!resume&&!edge)continue;
        if(stopBits!==expectedBits)errors.push({frame:row.frame,kind:'stop-flags',actual:stopBits,expected:expectedBits});
        for(let player=0;player<2;++player){const p=6+player*20;
          const pose=Math.hypot(float(row.world[p])-float(expected[p]),float(row.world[p+1])-float(expected[p+1]));
          const a=row.draw,b=exactDraw[row.frame-1];
          const appearance=a&&b?Math.hypot(float(a[player*4+2])-float(b[player*4+2]),float(a[player*4+3])-float(b[player*4+3])):0;
          maxPose=Math.max(maxPose,pose);maxDraw=Math.max(maxDraw,appearance);
          if(pose||appearance)errors.push({frame:row.frame,clock:row.clock,player,resume,pose,appearance});
        }
      }
      return {cadence,presentations:rows.length,frozen,resumed,edges,starts,maxBullets,maxPose,maxDraw,unseenStopFrames,unseenMaxPose,
        confirmedErrors,unconfirmedChargeFrames,gaugeErrorCount:gaugeErrors.length,gaugeErrors:gaugeErrors.slice(0,12),errorCount:errors.length,errors:errors.slice(0,12)};
    });
    let lastEnd=-1,prior=false,shortGaps=0;
    for(let f=0;f<exact.length;++f){
      const stopped=!!(exact[f][2]&0x1800);
      if(stopped&&!prior&&lastEnd>=0&&f-lastEnd-1<=3)++shortGaps;
      if(!stopped&&prior)lastEnd=f-1;prior=stopped;
    }
    const captureDifferences=captured.map((t,side)=>({length:t.length,
      different:t.flatMap((bits,f)=>bits===tapes[side][f]?[]:[{frame:f,planned:tapes[side][f],captured:bits}]).slice(0,20)}));
    return {clock,summary,hashes,audits,imagesEqual:new Set(images).size===1,metrics,pairs,shortGaps,captureDifferences,capturesComplete,stopWaits,
      capturesMatch:captured.every((t,side)=>t.length===limit&&t.every((bits,f)=>bits===tapes[side][f]))};
  },{limit,delay,jitter,warmup,chained,localCharge,expectRegression,auditPrediction,delayControl});
  const report={identity,parameters:{limit,delay,jitter,warmup,chained,localCharge,expectRegression,expectResumeWait,auditPrediction,delayControl,expectContinuous},browser:await browser.version(),
    scope:'Authored game, native rAF/post-Present drivers and synthetic delayed packets. Presentation cadence, predicted poses and confirmed worlds across global cut-ins, field time-stop and resume. Exact final worlds and framebuffer use captured inputs and immutable charge records as the replay oracle. This is not device performance evidence.',result,errors};
  writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');
  assert.deepEqual(errors,[]);
  assert.ok(result.stopWaits.every(Boolean),'Native boundary observations required');
  if(expectResumeWait)assert.ok(result.stopWaits.some(row=>row[1]>0),'Baseline repeats confirmation after a completed stop');
  else assert.ok(result.stopWaits.every(row=>row[1]===0),'A completed stop adds no confirmation fence to the following ordinary frame');
  assert.ok(result.capturesComplete,'Every captured input retained on the wire');
  for(const state of result.summary){assert.equal(state[0],limit);assert.equal(state[1],limit);assert.equal(state[4],limit,'One physical input capture per logical frame');assert.ok(state[2]>0&&state[3]>0);}
  assert.equal(new Set(result.hashes).size,1,'Both final worlds match exact input');
  assert.deepEqual(result.audits[0],result.audits[2]);assert.deepEqual(result.audits[1],result.audits[2]);
  assert.ok(result.imagesEqual,'Final framebuffer matches exact input');
  for(const m of result.metrics){
    if(expectContinuous){
      assert.equal(m.cadence.missedOpportunities,0,'Skill transitions preserve every due presentation');
      assert.ok(result.stopWaits.every(row=>row.every(n=>n===0)),'Skill state never introduces a confirmation fence');
    }
    assert.ok(m.frozen>100&&m.starts>=5&&m.resumed>0,'Repeated skill stops and resumes exercised');
    if(!expectRegression){
      assert.deepEqual(m.confirmedErrors,[],'Every presented confirmed world matches exact commands');
      if(!auditPrediction)assert.deepEqual(m.errors,[],'Both player poses match exact commands at stops and resume');
      assert.deepEqual(m.gaugeErrors,[],'Charge HUD follows current game state');
      assert.ok(m.unconfirmedChargeFrames>100,'Charge display advances before input confirmation');
    }
  }
  if(expectRegression)assert.ok(result.metrics.some(m=>m.errorCount>0),'Retained pre-fix build reproduces the position regression');
  if(chained){
    assert.ok(result.pairs>=5&&result.shortGaps>=5,'Both players chained level-2 skills within three ticks');
    assert.ok(result.metrics.every(m=>m.maxBullets>=100),'Dense authored bullet workload exercised');
  }
  assert.equal(sha(resolve(build,'th09-presentation.wasm')),identity.wasm,'Build stayed frozen');
  console.log(JSON.stringify({passed:true,identity,parameters:report.parameters,pairs:result.pairs,shortGaps:result.shortGaps,stopWaits:result.stopWaits,metrics:result.metrics}));
}finally{
  await browser.close();host.netplay.close();await new Promise(r=>host.server.close(r));
}
