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
const characters=(process.env.CHARACTERS||'2,2').split(',').map(Number);
assert.ok(characters.length===2&&characters.every(n=>Number.isInteger(n)&&n>=0&&n<16));
const localCharge=process.env.LOCAL_CHARGE==='1',expectRegression=process.env.EXPECT_REGRESSION==='1';
const expectResumeWait=process.env.EXPECT_RESUME_WAIT==='1';
const auditPrediction=process.env.AUDIT_PREDICTION!=='0',delayControl=process.env.DELAY_CONTROL==='1';
const expectContinuous=process.env.EXPECT_CONTINUOUS==='1';
const auditRefresh=process.env.AUDIT_RECOVERY_REFRESH==='1',expectStaleRecovery=process.env.EXPECT_STALE_RECOVERY==='1';
const arrivalRecovery=process.env.ARRIVAL_RECOVERY==='1',expectMissingArrival=process.env.EXPECT_MISSING_ARRIVAL==='1';
const auditDeclaration=process.env.AUDIT_DECLARATION==='1';
const inputDropEvery=Number(process.env.INPUT_DROP_EVERY||0),controlStallAt=Number(process.env.CONTROL_STALL_AT||900),controlStallTicks=Number(process.env.CONTROL_STALL_TICKS||0);
assert.ok([inputDropEvery,controlStallAt,controlStallTicks].every(n=>Number.isInteger(n)&&n>=0));
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
  const result=await page.evaluate(async({limit,delay,jitter,warmup,chained,characters,localCharge,expectRegression,auditPrediction,delayControl,inputDropEvery,controlStallAt,controlStallTicks,auditRefresh,arrivalRecovery,auditDeclaration})=>{
    const windows=[],pending=[],tapes=[[],[]],captured=[[],[]],presented=[[],[]],exact=[],exactDraw=[],charges=[[],[]];
    let clock=0;
    const refreshAudit={corrections:0,stale:0,stopCorrections:0,pixelChanges:0,examples:[]};
    const arrivalAudit={opportunities:0,corrections:0,stopCorrections:0,changed:0,stale:0,pixelSamples:0,pixelChanges:0};
    const wire=[0,1].map(()=>({input:0,dropped:0,control:0,stallAt:null})),controlDue=[0,0];let packetSerial=0;
    for(let side=0;side<3;++side){
      const frame=document.createElement('iframe');frame.src='/';document.body.append(frame);
      await new Promise(r=>frame.onload=r);windows.push(frame.contentWindow);
    }
    const retainCanvas=w=>{
      // Recovery Presents happen between callbacks. Retain that submitted
      // buffer for readback after compositing; never redraw to take a sample.
      if(auditDeclaration){const canvas=w.document.querySelector('canvas'),get=canvas.getContext.bind(canvas);
        canvas.getContext=(kind,options)=>get(kind,{...options,preserveDrawingBuffer:true});}
    };
    for(const w of windows){retainCanvas(w);await w.openProbe(2,2,2,3,true);}
    const peers=windows.slice(0,2);let ref=windows[2];
    const values=(w,name,count)=>{const c=w.core,p=c[name]()/4;return Array.from(c.HEAP32.subarray(p,p+count));};
    const status=w=>values(w,'_th09_rollback_info',8),world=w=>values(w,'_th09_probe_world',48);
    const float=n=>new Float32Array(new Int32Array([n]).buffer)[0];
    const presentations=w=>values(w,'_th09_game_metrics',10)[9];
    const load=w=>values(w,'_th09_probe_load',8);
    const draw=w=>w.core._th09_probe_stop_draw?values(w,'_th09_probe_stop_draw',9):null;
    const send=(side,bytes,lane)=>{
      const input=bytes[0]===69&&bytes[5]===1;
      const charge=bytes[0]===84&&bytes[1]===57&&bytes[2]===67&&(bytes[3]===80||bytes[3]===66);
      if(charge){
        const v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),batch=bytes[3]===66,count=batch?v.getUint32(20,true):1;
        for(let i=0;i<count;++i){const offset=batch?24+i*44:12,f=v.getUint32(offset,true),start=batch?offset+4:20,raw=Array.from(bytes.subarray(start,start+40));
          if(charges[side][f]&&charges[side][f].some((b,j)=>b!==raw[j]))throw Error('Rewritten charge '+f);charges[side][f]=raw;}
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
      let due=clock+(input||charge||delayControl?delay+clock%(jitter+1):0);
      if(lane==='input'){
        ++wire[side].input;
        if(inputDropEvery&&wire[side].input%inputDropEvery===0){++wire[side].dropped;return;}
      }else{
        ++wire[side].control;
        if(controlStallTicks&&clock>=controlStallAt&&wire[side].stallAt===null){wire[side].stallAt=clock;controlDue[side]=clock+controlStallTicks;}
        due=Math.max(due,controlDue[side]);controlDue[side]=due;
      }
      pending.push({to:1-side,due,serial:++packetSerial,bytes:new Uint8Array(bytes)});
    };
    for(let side=0;side<3;++side){
      const w=windows[side],dc=lane=>({readyState:'open',bufferedAmount:0,send:b=>{if(side<2)send(side,b,lane);}});
      w.__eaglerPeerTransport={route:'rtc',localPlayer:side%2,playerCount:2,received:[],receivedHead:0,
        peers:new Map([[1-side%2,{inputOpen:true,controlOpen:true,inputDc:dc('input'),controlDc:dc('control')}]])};
      if(side<2&&!w.core._th09_rollback_recovery_mode(1))throw Error('Recovery mode');
      if(localCharge&&!w.core._th09_local_charge_priority)throw Error('Local charge ABI required');
      if(w.core._th09_local_charge_priority&&!w.core._th09_local_charge_priority(+(localCharge&&side<2)))throw Error('Charge policy');
      if(!expectRegression&&!w.core._th09_probe_stop_draw)throw Error('Draw observation required');
      if(!w.core._th09_adonis_configure(0)||!w.core._th09_rollback_begin(12345,side%2,3,...characters,0x1234,0x5678,0x901,0))
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
    const observers=[],visibleFrames=[0,0],loopResults=[1,1];
    const declarationPictures=[[],[]],declarationStates=[{bits:0,start:0,resume:-100},{bits:0,start:0,resume:-100}];
    for(let side=0;side<2;++side){
      const w=peers[side];w.queue=[];w.requestAnimationFrame=cb=>{w.queue.push(cb);return w.queue.length;};
      let lastPresent=presentations(w);
      const observe=ok=>{
        if(!ok)throw Error('Native loop '+w.probeError()+' '+JSON.stringify({side,clock,status:status(w)}));
        const count=presentations(w);if(count===lastPresent)return;lastPresent=count;
        visibleFrames[side]=status(w)[0];
        presented[side].push({clock,frame:status(w)[0],confirmed:status(w)[1],world:world(w),load:load(w),draw:draw(w),
          visual:w.core._th09_probe_charge_visual?values(w,'_th09_probe_charge_visual',8):null});
        if(auditDeclaration){
          const row=presented[side].at(-1),state=declarationStates[side],bits=row.draw[8]&0x1800;
          if(bits!==state.bits){state.start=row.frame;if(!bits&&state.bits)state.resume=row.frame;}
          const age=bits?row.frame-state.start:row.frame-state.resume;
          // AttackController clears the flag AFTER player/bullet updates.
          // Age zero at exit is still frozen; observe the next eight ticks too.
          if(bits?[0,4,8,16,28,30].includes(age):age>=0&&age<=8)
            declarationPictures[side].push({frame:row.frame,clock,confirmed:row.confirmed,bits,age,
              image:w.document.querySelector('canvas').toDataURL()});
          state.bits=bits;
        }
      };
      w.core.onGameFrame=ok=>{loopResults[side]=ok;observe(ok);};
      observers.push(()=>observe(1));
      w.core._th09_loop_start();
    }
    const setKeys=(w,bits)=>{for(const [bit,scan] of [[1,44],[2,45],[4,42],[64,203],[128,205],[256,29]])w.core._th09_key(scan,Number(!!(bits&bit)));};
    for(;clock<limit*3;++clock){
      const ready=[];
      for(let i=pending.length-1;i>=0;--i)if(pending[i].due<=clock)ready.push(pending.splice(i,1)[0]);
      // The reliable SCTP stream retains send order even after a blocked head.
      ready.sort((a,b)=>a.due-b.due||a.serial-b.serial);
      for(const packet of ready)peers[packet.to].__eaglerPeerTransport.received.push(packet.bytes);
      // Deliver through the production arrival entry points after the prior
      // post-Present timer drained and before this clock's rAF. Restrict this
      // audit to completed callbacks: no outstanding due tick may advance.
      if(arrivalRecovery&&ready.length&&loopResults.every(n=>n===1)&&peers.every(w=>status(w)[0]<limit)){
        const before=peers.map(w=>({info:status(w),draw:draw(w),presentations:presentations(w),
          callbacks:values(w,'_th09_rollback_recovery_info',5)[2],
          image:arrivalAudit.pixelChanges<4&&draw(w)[8]?w.document.querySelector('canvas').toDataURL():null}));
        for(const w of peers){if(!w.core._th09_rollback_pump())throw Error(w.probeError());
          w.core._th09_rollback_wake();w.core._th09_rollback_wake();}
        await new Promise(r=>setTimeout(r,0));
        for(let side=0;side<2;++side){
          const w=peers[side],a=before[side],info=status(w),picture=draw(w);++arrivalAudit.opportunities;
          if(info[4]!==a.info[4]||info[0]>a.info[0])throw Error('Packet arrival earned fresh input or time');
          if(values(w,'_th09_rollback_recovery_info',5)[2]-a.callbacks>1)throw Error('Duplicate wake scheduled duplicate recovery');
          if(presentations(w)!==a.presentations&&info[0]!==a.info[0])throw Error('Arrival presented an incomplete recovery');
          if(a.image){++arrivalAudit.pixelSamples;if(a.image!==w.document.querySelector('canvas').toDataURL())++arrivalAudit.pixelChanges;}
          if(info[3]>a.info[3]&&info[0]===a.info[0]){
            ++arrivalAudit.corrections;if(picture[8]|a.draw[8])++arrivalAudit.stopCorrections;
            if(picture.some((n,i)=>n!==a.draw[i])){++arrivalAudit.changed;if(presentations(w)===a.presentations)++arrivalAudit.stale;}
          }
          observers[side]();
        }
      }
      for(let side=0;side<2;++side){
        const w=peers[side];setKeys(w,tapes[side][status(w)[4]]||0);
        const callback=w.queue.shift();if(callback)callback(clock*1000/60);
      }
      // Deliver the real native post-Present timer between manual rAF callbacks.
      // Read the actual canvas around recovery without forcing another Draw.
      // Logical/capture frontiers must stay put when an endpoint is repainted.
      const beforeRecovery=auditRefresh?peers.map((w,side)=>({info:status(w),visibleFrame:visibleFrames[side],draw:draw(w),presentations:presentations(w),
        image:refreshAudit.pixelChanges<4&&draw(w)[8]?w.document.querySelector('canvas').toDataURL():null})):null;
      await new Promise(r=>setTimeout(r,0));
      if(auditRefresh)for(let side=0;side<2;++side){
        const w=peers[side],a=beforeRecovery[side],info=status(w),picture=draw(w),count=presentations(w);
        if(info[4]!==a.info[4])throw Error('Recovery captured fresh physical input');
        if(count!==a.presentations&&info[0]!==a.visibleFrame)throw Error('Presented an incomplete recovery frontier');
        observers[side]();
        if(info[3]<=a.info[3]||info[0]!==a.info[0]||!picture.some((n,i)=>n!==a.draw[i]))continue;
        ++refreshAudit.corrections;const stale=count===a.presentations,stopped=!!(picture[8]|a.draw[8]);
        if(stale)++refreshAudit.stale;if(stopped)++refreshAudit.stopCorrections;
        if(!stale&&a.image&&a.image!==w.document.querySelector('canvas').toDataURL())++refreshAudit.pixelChanges;
        if(refreshAudit.examples.length<12||stopped)refreshAudit.examples.push({side,clock,frame:info[0],stopped,stale,
          presentations:count-a.presentations,before:a.draw,after:picture});
      }
      if(!auditRefresh)for(const observe of observers)observe();
      if(peers.every(w=>status(w)[0]===limit&&status(w)[1]===limit))break;
    }
    for(const w of peers)w.core._th09_loop_stop();
    const stopWaits=peers.map(w=>w.core._th09_probe_stop_boundary_waits?values(w,'_th09_probe_stop_boundary_waits',2):null);
    const capturesComplete=captured.every(t=>t.length===limit&&Array.from({length:limit},(_,f)=>Number.isInteger(t[f])).every(Boolean));
    if(!capturesComplete)throw Error('Captured input gap');
    // The physical keyboard adapter can retain a brief stroke during a wait.
    // The exact oracle consumes the actual captured wire input, just as Replay
    // does, so input buffering cannot masquerade as a rollback state error.
    // Tape generation executes a complete match and changes shared animation
    // resources. Start the exact picture oracle with fresh resource owners,
    // just as the two peers did, rather than reusing the generator's caches.
    const referenceTransport=ref.__eaglerPeerTransport,oldFrame=ref.frameElement;
    ref.core._th09_network_end();ref.core._th09_probe_close();oldFrame.remove();
    const referenceFrame=document.createElement('iframe');referenceFrame.src='/';document.body.append(referenceFrame);
    await new Promise(r=>referenceFrame.onload=r);ref=referenceFrame.contentWindow;windows[2]=ref;
    retainCanvas(ref);await ref.openProbe(2,2,2,3,true);ref.__eaglerPeerTransport=referenceTransport;
    if(localCharge&&!ref.core._th09_local_charge_priority(1))throw Error('Reference policy');
    if(!ref.core._th09_rollback_begin(12345,0,3,...characters,0x1234,0x5678,0x901,0))throw Error('Reference restart '+ref.probeError());
    if(localCharge&&!ref.core._th09_probe_charge_reference_mode(1))throw Error('Reference commands');
    const pictureFrames=new Set(declarationPictures.flat().map(row=>row.frame)),exactPictures=new Map();
    for(let f=0;f<limit;++f){
      if(localCharge)for(let side=0;side<2;++side){if(!charges[side][f])throw Error('Missing charge record '+f);ref.core.HEAPU8.set(charges[side][f],ref.core._th09_probe_charge_reference_buffer()+40*side);}
      if(!ref.core._th09_probe_rollback_step(captured[0][f],captured[1][f],+pictureFrames.has(f+1)))throw Error('Exact replay '+ref.probeError());
      exact.push(world(ref));
      exactDraw.push(draw(ref));
      if(pictureFrames.has(f+1))exactPictures.set(f+1,ref.document.querySelector('canvas').toDataURL());
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
        const stopped=!!stopBits,wasStopped=prior,edge=stopBits!==priorBits;
        if(stopped&&!prior)++starts;prior=stopped;priorBits=stopBits;
        const expected=exact[row.frame-1];
        if(row.frame<=row.confirmed&&row.world.some((n,i)=>n!==expected[i]))confirmedErrors.push({frame:row.frame});
        const expectedBits=exactDraw[row.frame-1]?.[8]??(expected[2]&0x1800);
        const expectedStop=!!expectedBits,resume=!stopped&&wasStopped;
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
      // A forward picture and a recovery picture can share one clock. Score
      // the last complete submission once per opportunity, including field
      // time-stop and its exit. This measures poses, not monitor scanout.
      const lastSubmitted=[...new Map(rows.map(row=>[row.clock,row])).values()];
      const stats=()=>({samples:0,different:0,totalPixels:0,maxPixels:0});
      const display={global:stats(),field:stats(),resume:stats(),declarationRecovery:stats()};let lastStop=0,lastGlobal=0,declarationResume=-100;
      for(const row of lastSubmitted){
        const bits=row.draw?.[8]??(row.world[2]&0x1800),expected=exactDraw[row.frame-1];
        if(!row.draw||!expected)continue;
        const error=Math.max(...[0,1].map(p=>Math.hypot(float(row.draw[p*4+2])-float(expected[p*4+2]),float(row.draw[p*4+3])-float(expected[p*4+3]))));
        const group=bits&0x1800?display.global:bits?display.field:lastStop?display.resume:null;
        if(group){++group.samples;if(error>0)++group.different;group.totalPixels+=error;group.maxPixels=Math.max(group.maxPixels,error);}
        const global=bits&0x1800;
        if(lastGlobal&&!global)declarationResume=row.frame;
        if(!global&&row.frame>declarationResume&&row.frame<=declarationResume+8){
          const group=display.declarationRecovery;++group.samples;if(error>0)++group.different;
          group.totalPixels+=error;group.maxPixels=Math.max(group.maxPixels,error);
        }
        lastGlobal=global;
        lastStop=bits;
      }
      return {display,cadence,presentations:rows.length,frozen,resumed,edges,starts,maxBullets,maxPose,maxDraw,unseenStopFrames,unseenMaxPose,
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
    const declarationAudit=declarationPictures.map(rows=>{
      const group=()=>({samples:0,different:0});
      const groups={start:group(),middle:group(),end:group(),settled:group(),exitFlag:group(),recovery:group()},differences=[];
      for(const row of rows){
        const phase=row.bits?(row.age<8?'start':row.age<28?'middle':'end'):row.age?'recovery':'exitFlag';
        const bucket=groups[phase];++bucket.samples;
        const different=row.image!==exactPictures.get(row.frame);
        // Sixteen ticks span two complete prediction windows. At this point
        // the scene frozen by the declaration must match the accepted inputs.
        if(row.bits&&row.age>=16){++groups.settled.samples;if(different)++groups.settled.different;}
        if(different){++bucket.different;
          const {image,...sample}=row;differences.push({...sample,phase,expectedBits:exactDraw[row.frame-1][8]&0x1800});}
      }
      return {groups,differences};
    });
    return {declarationAudit,arrivalAudit,refreshAudit,clock,summary,hashes,audits,imagesEqual:new Set(images).size===1,metrics,pairs,shortGaps,captureDifferences,capturesComplete,stopWaits,wire,
      capturesMatch:captured.every((t,side)=>t.length===limit&&t.every((bits,f)=>bits===tapes[side][f]))};
  },{limit,delay,jitter,warmup,chained,characters,localCharge,expectRegression,auditPrediction,delayControl,inputDropEvery,controlStallAt,controlStallTicks,auditRefresh,arrivalRecovery,auditDeclaration});
  const report={identity,parameters:{limit,delay,jitter,warmup,chained,characters,localCharge,expectRegression,expectResumeWait,auditPrediction,delayControl,expectContinuous,inputDropEvery,controlStallAt,controlStallTicks,auditRefresh,expectStaleRecovery,arrivalRecovery,expectMissingArrival,auditDeclaration},browser:await browser.version(),
    scope:'Authored game, native rAF/post-Present drivers and synthetic delayed packets. Presentation cadence, predicted poses and confirmed worlds across global cut-ins, field time-stop and resume. Recovery refresh auditing reads actual canvas pixels without an extra Draw. Exact final worlds and framebuffer use captured inputs and immutable charge records as the replay oracle. This is not device performance evidence.',result,errors};
  writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');
  assert.deepEqual(errors,[]);
  if(auditDeclaration)for(const audit of result.declarationAudit){
    assert.ok(audit.groups.middle.samples>0&&audit.groups.end.samples>0&&audit.groups.recovery.samples>0,'Declaration interior and actual movement recovery sampled');
    assert.ok(audit.groups.settled.samples>0,'Settled declaration scene sampled');
    if(!expectRegression)assert.equal(audit.groups.settled.different,0,'Settled declaration scene matches captured commands');
  }
  if(arrivalRecovery){
    assert.ok(result.arrivalAudit.opportunities>100);
    if(expectMissingArrival)assert.equal(result.arrivalAudit.corrections,0,'Baseline waits for another rAF');
    else{assert.ok(result.arrivalAudit.changed>0&&result.arrivalAudit.stopCorrections>0,'Arrival corrects stopped pictures before rAF');
      // A correction can change keys or a covered sprite without changing
      // visible pixels. Submission and exact-scene checks own correctness.
      assert.ok(result.arrivalAudit.pixelSamples>0,'Stopped canvas sampled around arrival recovery');
      assert.equal(result.arrivalAudit.stale,0,'Completed arrival corrections are presented');}
  }
  if(auditRefresh){
    assert.ok(result.refreshAudit.corrections>0&&result.refreshAudit.stopCorrections>0,'Completed corrections exercised during skill stops');
    if(expectStaleRecovery)assert.ok(result.refreshAudit.stale>0,'Baseline leaves corrected state unpresented');
    else{
      assert.equal(result.refreshAudit.stale,0,'Every completed changed frontier is repainted');
      assert.ok(result.refreshAudit.pixelChanges>0,'Actual stopped canvas changes without a new logical tick');
    }
  }
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
  console.log(JSON.stringify({passed:true,identity,parameters:report.parameters,pairs:result.pairs,shortGaps:result.shortGaps,stopWaits:result.stopWaits,refreshAudit:result.refreshAudit,metrics:result.metrics}));
}finally{
  await browser.close();host.netplay.close();await new Promise(r=>host.server.close(r));
}
