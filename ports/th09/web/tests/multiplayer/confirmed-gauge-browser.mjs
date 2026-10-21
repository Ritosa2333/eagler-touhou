// Authored WASM worlds | native rAF and post-Present recovery | synthetic wire.
// Compare every presented gauge with the exact confirmed source frame.
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
  const result=await page.evaluate(async({limit,delay,jitter,warmup,chained})=>{
    const windows=[],pending=[],tapes=[[],[]],captured=[[],[]],presented=[[],[]],exact=[],toggles=[null,null];
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
    const gauges=w=>values(w,'_th09_probe_gauge_draw',136);
    const live=w=>values(w,'_th09_probe_gauge_live',136);
    const send=(side,bytes)=>{
      const input=bytes[0]===69&&bytes[5]===1;
      if(input){
        const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
        const start=view.getUint32(32,true),count=bytes[42];
        for(let i=0;i<count;++i){
          const frame=start+i,bits=view.getUint16(44+i*12,true);
          if(captured[side][frame]!==undefined&&captured[side][frame]!==bits)throw Error('Rewritten input '+frame);
          captured[side][frame]=bits;
        }
      }
      pending.push({to:1-side,due:clock+(input?delay+clock%(jitter+1):0),bytes:new Uint8Array(bytes)});
    };
    for(let side=0;side<3;++side){
      const w=windows[side],dc=()=>({readyState:'open',bufferedAmount:0,send:b=>{if(side<2)send(side,b);}});
      w.__eaglerPeerTransport={route:'rtc',localPlayer:side%2,playerCount:2,received:[],receivedHead:0,
        peers:new Map([[1-side%2,{inputOpen:true,controlOpen:true,inputDc:dc(),controlDc:dc()}]])};
      if(side<2&&!w.core._th09_rollback_recovery_mode(1))throw Error('Recovery mode');
      if(!w.core._th09_adonis_configure(0)||!w.core._th09_rollback_begin(12345,side%2,3,2,2,0x1234,0x5678,0x901,0))
        throw Error('Begin '+w.probeError());
      if(!w.core._th09_confirmed_gauge_mode(+(side<2)))throw Error('Gauge mode');
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
        if(!ok)throw Error('Native loop '+w.probeError());
        const count=presentations(w);if(count===lastPresent)return;lastPresent=count;
        presented[side].push({clock,frame:status(w)[0],confirmed:status(w)[1],world:world(w),load:load(w),gauges:gauges(w)});
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
      for(let side=0;side<2;++side){
        const w=peers[side],p=6+side*20,offset=side*68;
        if(toggles[side]||float(world(w)[p+4])<150||!gauges(w)[offset])continue;
        const hash=w.core._th09_network_hash(),before=status(w),actual=live(w).slice(offset+8,offset+68);
        if(w.core._th09_confirmed_gauge_mode(2)!==0||!w.core._th09_confirmed_gauge_mode(0))throw Error('Disable mode');
        w.core._th09_game_draw();
        const disabled=gauges(w),restored=JSON.stringify(disabled.slice(offset+8,offset+68))===JSON.stringify(actual);
        if(!w.core._th09_confirmed_gauge_mode(1))throw Error('Enable mode');
        w.core._th09_game_draw();
        toggles[side]={restored,disabled:disabled[offset]===0,enabled:gauges(w)[offset]===1,
          hashUnchanged:w.core._th09_network_hash()===hash,statusUnchanged:JSON.stringify(before)===JSON.stringify(status(w))};
      }
      if(peers.every(w=>status(w)[0]===limit&&status(w)[1]===limit))break;
    }
    for(const w of peers)w.core._th09_loop_stop();
    const capturesComplete=captured.every(t=>t.length===limit&&Array.from({length:limit},(_,f)=>Number.isInteger(t[f])).every(Boolean));
    if(!capturesComplete)throw Error('Captured input gap');
    // The physical keyboard adapter can retain a brief stroke during a wait.
    // The exact oracle consumes the actual captured wire input, just as Replay
    // does, so input buffering cannot masquerade as a rollback state error.
    ref.core._th09_network_end();
    if(!ref.core._th09_rollback_begin(12345,0,3,2,2,0x1234,0x5678,0x901,0))throw Error('Reference restart '+ref.probeError());
    for(let f=0;f<limit;++f){
      if(!ref.core._th09_probe_rollback_step(captured[0][f],captured[1][f],0))throw Error('Exact replay '+ref.probeError());
      exact.push({world:world(ref),gauges:live(ref)});
    }
    const summary=peers.map(status),hashes=windows.map(w=>w.core._th09_network_hash()>>>0),audits=windows.map(world);
    const images=windows.map(w=>{w.core._th09_confirmed_gauge_mode(0);w.core._th09_game_draw();return w.document.querySelector('canvas').toDataURL();});
    const metrics=presented.map((rows,side)=>{
      const p=6+side*20,offset=side*68,errors=[];let frozen=0,maxBullets=0,starts=0,prior=false,checked=0,lastSource=0,maxDelay=0,rawCorrections=0;
      for(const row of rows){
        const stopped=!!(row.world[2]&0x1800);if(stopped&&!prior)++starts;prior=stopped;
        maxBullets=Math.max(maxBullets,row.load[0]+row.load[1]);if(stopped)++frozen;
        if(row.frame&&row.world[p+4]!==exact[row.frame-1].world[p+4])++rawCorrections;
        const g=row.gauges.slice(offset,offset+68);
        if(!g[0]){if(float(row.world[p+4])>0)errors.push({frame:row.frame,reason:'Unconfirmed charged appearance'});continue;}
        ++checked;const source=g[1],expected=exact[source-1];maxDelay=Math.max(maxDelay,row.frame-source);
        if(source<lastSource||source>row.confirmed||!expected)errors.push({frame:row.frame,source,reason:'Confirmation order'});
        else if(g[2]!==expected.world[p+4]||g[3]!==expected.world[p+5]||
          JSON.stringify(g.slice(8))!==JSON.stringify(expected.gauges.slice(offset+8,offset+68)))
          errors.push({frame:row.frame,source,reason:'Displayed gauge differs from exact confirmed frame'});
        lastSource=source;
      }
      return {presentations:rows.length,checked,frozen,starts,maxBullets,maxDelay,rawCorrections,errors};
    });
    let lastEnd=-1,prior=false,shortGaps=0;
    for(let f=0;f<exact.length;++f){
      const stopped=!!(exact[f].world[2]&0x1800);
      if(stopped&&!prior&&lastEnd>=0&&f-lastEnd-1<=3)++shortGaps;
      if(!stopped&&prior)lastEnd=f-1;prior=stopped;
    }
    const captureDifferences=captured.map((t,side)=>({length:t.length,
      different:t.flatMap((bits,f)=>bits===tapes[side][f]?[]:[{frame:f,planned:tapes[side][f],captured:bits}]).slice(0,20)}));
    return {clock,summary,hashes,audits,imagesEqual:new Set(images).size===1,metrics,pairs,shortGaps,toggles,captureDifferences,capturesComplete,
      capturesMatch:captured.every((t,side)=>t.length===limit&&t.every((bits,f)=>bits===tapes[side][f]))};
  },{limit,delay,jitter,warmup,chained});
  const report={identity,parameters:{limit,delay,jitter,warmup,chained},browser:await browser.version(),
    scope:'Authored game | two native rollback drivers | all presented gauge slots compared with exact captured input at their confirmed source frame | in-match display rollback switch | controlled delayed packets',result,errors};
  writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');
  assert.deepEqual(errors,[]);
  assert.ok(result.capturesComplete,'Every captured input retained on the wire');
  for(const state of result.summary){assert.equal(state[0],limit);assert.equal(state[1],limit);assert.equal(state[4],limit,'One physical input capture per logical frame');assert.ok(state[2]>0&&state[3]>0);}
  assert.equal(new Set(result.hashes).size,1,'Both final worlds match exact input');
  assert.deepEqual(result.audits[0],result.audits[2]);assert.deepEqual(result.audits[1],result.audits[2]);
  assert.ok(result.imagesEqual,'Final framebuffer matches exact input');
  for(const m of result.metrics){assert.ok(m.frozen>100&&m.starts>=10,'Repeated level-2 cut-ins exercised');assert.ok(m.checked>1000&&m.maxDelay>0);assert.deepEqual(m.errors,[],'Every displayed gauge matches confirmed history');}
  for(const toggle of result.toggles){assert.ok(toggle,'Toggled while charging');for(const value of Object.values(toggle))assert.equal(value,true,'Immediate display rollback preserves the game and input history');}
  if(chained)assert.ok(result.pairs>=5&&result.shortGaps>=5,'Both players chained level-2 skills within three ticks');
  assert.equal(sha(resolve(build,'th09-presentation.wasm')),identity.wasm,'Build stayed frozen');
  console.log(JSON.stringify({passed:true,identity,parameters:report.parameters,pairs:result.pairs,shortGaps:result.shortGaps,metrics:result.metrics}));
}finally{
  await browser.close();host.netplay.close();await new Promise(r=>host.server.close(r));
}
