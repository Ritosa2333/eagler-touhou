import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
const {server,netplay,url}=await presentationServer();
const browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
const page=await browser.newPage(),errors=[];
page.on('pageerror',e=>errors.push(e.stack));
try {
  await page.goto(url);
  const result=await page.evaluate(async()=>{
    const frames=[];
    for(let i=0;i<3;++i){const frame=document.createElement('iframe');frame.src='/';document.body.append(frame);await new Promise(r=>frame.onload=r);frames.push(frame.contentWindow);}
    for(const w of frames)await w.openProbe(0,1,2,3,true);
    const peers=frames.slice(0,2),pending=[];let clock=0,sequence=0;
    const stats=w=>{const at=w.core._th09_rollback_info()/4;return Array.from(w.core.HEAPU32.subarray(at,at+8));};
    const error=w=>{const c=w.core,at=c._th09_error();return new TextDecoder().decode(c.HEAPU8.subarray(at,c.HEAPU8.indexOf(0,at)));};
    const send=(side,reliable,bytes)=>{
      const input=bytes[0]===69&&bytes[5]===1;
      const n=++sequence;
      if(input&&!reliable&&n%7===0)return;
      let delay=input?2+n%3:0;
      // Both lanes experience the same long spike, then recover.
      if(input&&clock>=220&&clock<245)delay+=48;
      pending.push({to:1-side,due:clock+delay,bytes:new Uint8Array(bytes)});
      if(input&&n%11===0)pending.push({to:1-side,due:clock+delay+5,bytes:new Uint8Array(bytes)});
    };
    for(let side=0;side<3;++side){const w=frames[side];const dc=reliable=>({readyState:'open',bufferedAmount:0,send:bytes=>{if(side<2)send(side,reliable,bytes);}});
      w.__eaglerPeerTransport={route:'rtc',localPlayer:side%2,playerCount:2,received:[],receivedHead:0,peers:new Map([[1-side%2,{inputOpen:true,controlOpen:true,inputDc:dc(false),controlDc:dc(true)}]])};
      if(!w.core._th09_rollback_begin(12345,side%2,3,0,1,0x1234,0x5678,0x901))throw Error('begin '+error(w));
    }
    const keys=(side,f)=> (f<140?256:0) | (f%18<12?1:0) | ((Math.floor(f/37)+side)%2?64:128) | (f%217===0?2:0);
    const physical=(w,bits)=>{for(const [bit,scan] of [[1,44],[2,45],[64,203],[128,205],[256,29]])w.core._th09_key(scan,+(!!(bits&bit)));};
    const limit=900;for(const w of peers)w.core._th09_probe_frame_limit(limit);
    const ticks=[0,0],waits=[0,0],milliseconds=[0,0];let iterations=0;
    while(iterations++<4000){
      for(let i=pending.length-1;i>=0;--i)if(pending[i].due<=clock){const packet=pending.splice(i,1)[0];peers[packet.to].__eaglerPeerTransport.received.push(packet.bytes);}
      for(let side=0;side<2;++side){const w=peers[side],s=stats(w);physical(w,keys(side,s[0]));const begin=performance.now();
        if(s[0]<=limit){const ok=w.core._th09_game_tick(0);if(!ok)throw Error(`peer ${side} frame ${s[0]}: ${error(w)}`);if(ok===2)++waits[side];++ticks[side];}
        else if(!w.core._th09_rollback_pump())throw Error(error(w));
        milliseconds[side]+=performance.now()-begin;
      }
      ++clock;
      const s=peers.map(stats);if(s.every(v=>v[0]===limit&&v[1]===limit))break;
      if(iterations%8===0)await new Promise(r=>setTimeout(r,0));
    }
    const summary=peers.map(stats);if(!summary.every(v=>v[0]===limit&&v[1]===limit&&v[4]===limit&&v[2]>0))throw Error('incomplete '+JSON.stringify(summary));
    const reference=frames[2];for(let f=0;f<limit;++f)if(!reference.core._th09_probe_rollback_step(keys(0,f),keys(1,f),0))throw Error('reference '+error(reference));
    const hashes=frames.map(w=>w.core._th09_network_hash()>>>0);
    if(new Set(hashes).size!==1)throw Error('reference divergence '+JSON.stringify(hashes));
    const audited=frames.map(w=>{const c=w.core,at=c._th09_probe_world()/4;return Array.from(c.HEAP32.subarray(at,at+48));});
    if(new Set(audited.map(a=>JSON.stringify(a))).size!==1)throw Error('world audit differs from exact-input reference');
    const images=frames.map(w=>{w.core._th09_game_draw();return w.document.querySelector('canvas').toDataURL();});
    if(new Set(images).size!==1)throw Error('framebuffer differs from exact-input reference');
    const replays=frames.map(w=>{if(!w.core._th09_probe_save_replay())throw Error('replay '+error(w));return w.core.FS.readFile('/save/replay/th9_25.rpy');});
    if(!replays.slice(1).every(b=>b.length===replays[0].length&&b.every((v,i)=>v===replays[0][i])))throw Error('replay differs from exact-input reference');
    return {frames:limit,iterations,summary,hashes,audited,framebuffersEqual:true,replayBytes:replays[0].length,ticks,waits,milliseconds};
  });
  assert.equal(errors.length,0,errors.join('\n'));
  const wasm=JSON.parse(readFileSync(resolve(root,'artifacts/sdl3/build.json'))).sha256;
  writeFileSync(resolve(root,'artifacts/multiplayer-tests/peer-browser-report.json'),JSON.stringify({passed:true,wasm,scope:'Real TH09 browser worlds and shared packet/session implementation; deterministic injected RTC lane faults, not public TURN',result,errors},null,2));
  console.log(JSON.stringify(result));
} catch(error) {
  writeFileSync(resolve(root,'artifacts/multiplayer-tests/peer-browser-failure.json'),JSON.stringify({error:error.stack,errors},null,2));throw error;
} finally {await browser.close();netplay.close();await new Promise(r=>server.close(r));}
