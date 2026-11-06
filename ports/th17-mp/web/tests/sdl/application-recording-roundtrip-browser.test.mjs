import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,root,report,sha} from '../cpp/helpers.mjs';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {applicationServer,applicationPage} from './application-fixture.mjs';

test('Live recorded gameplay survives export and fresh per-stage playback',{timeout:7200000},async()=>{
 const path=process.env.TH17_ROUNDTRIP_SOURCE??resolve(root,'../[th17] 东方鬼形兽（汉化版+日文版）/th17_01.rpy'),bytes=readFileSync(path),c=await core(),r=c.replay_create(),p=c.allocate(bytes.length),entry=c.allocate(0x158),streams={};
 try{memory(c,p,bytes.length).set(bytes);assert.equal(c.replay_open(r,p,bytes.length),1);for(let stage=1;stage<=6;stage++)if(c.replay_initial_state(r,stage,entry)){const d=new DataView(c.memory.buffer),n=d.getUint32(entry+4,true);c.replay_select(r,stage);streams[stage]=[];for(let frame=0;frame<n;frame++)streams[stage].push(new DataView(c.memory.buffer).getUint16(c.replay_tick(r,1),true));}}finally{c.deallocate(entry);c.deallocate(p);c.replay_delete(r);}
 const playbackHash=sha(readFileSync(resolve(root,'artifacts/sdl-application/th17-application.wasm')));
 const resume=process.env.TH17_ROUNDTRIP_RESUME?resolve(root,process.env.TH17_ROUNDTRIP_RESUME):null;
 const capture=resume?JSON.parse(readFileSync(resolve(resume,'capture.json'),'utf8')):null;
 if(capture){for(const [name,hash]of Object.entries(capture.files))assert.equal(sha(readFileSync(resolve(resume,name))),hash,name+' capture identity');assert.equal(capture.sourceSha256,sha(bytes),'Live input source identity');assert.equal(playbackHash,process.env.TH17_ROUNDTRIP_PLAYBACK_BUILD??capture.wasmSha256,'Explicit playback build identity');}
 const resumed=capture?{bytes:Array.from(readFileSync(resolve(resume,'live-roundtrip.rpy'))),states:JSON.parse(readFileSync(resolve(resume,'live-states.json'),'utf8')),diagnostics:JSON.parse(readFileSync(resolve(resume,'live-diagnostics.json'),'utf8'))}:null;
 const capturedDirectory=resume??resolve(root,'artifacts/replay-roundtrip');
 const server=await applicationServer({'/recording/capture.rpy':resolve(capturedDirectory,'live-roundtrip.rpy'),'/recording/states.json':resolve(capturedDirectory,'live-states.json'),'/recording/diagnostics.json':resolve(capturedDirectory,'live-diagnostics.json')}),observations=[],errors=[];let browser;
 try{
  browser=await launchBrowser();let page=await applicationPage(browser,server);page.on('pageerror',e=>errors.push(String(e)));page.on('console',m=>{if(m.type()==='error')console.log('browser error',m.text());});
  if(!resumed)await page.evaluate(({bytes,streams})=>{window.sourceStreams=streams;window.recordedStates={};window.recordedDiagnostics={};window.diagnostic=()=>{const players=Array.from({length:4},(_,i)=>{const p=fixture._application_test_player_diagnostic(i);return Array.from(fixture.HEAPU8.subarray(p,p+fixture._application_test_player_diagnostic_size(i)));}),bullets=[];for(let slot=fixture._application_test_bullet_next(0xffffffff)>>>0;slot!==0xffffffff;slot=fixture._application_test_bullet_next(slot)>>>0){const p=fixture._application_test_bullet_diagnostic(slot);bullets.push([slot,Array.from(fixture.HEAPU8.subarray(p,p+fixture._application_test_bullet_diagnostic_size()))]);}return{players,bullets};};fixture.FS.writeFile('/save/replay/source.rpy',new Uint8Array(bytes));const p=fixture._malloc(32);fixture.HEAPU8.set(new TextEncoder().encode('source.rpy\0'),p);if(!fixture._application_test_live_from_replay(p,1))throw Error('Live setup failed');fixture._free(p);step();}, {bytes:Array.from(bytes),streams});
  const maxStage=Number(process.env.TH17_ROUNDTRIP_STAGES??2),lastLimit=Number(process.env.TH17_ROUNDTRIP_LAST_LIMIT??3000),limits=Object.values(streams).slice(0,maxStage).reduce((n,a)=>n+a.length,0)+maxStage*400;
  let simulated=0,finished=!!resumed;
  while(simulated<limits&&!finished){
   const value=await page.evaluate(({maxStage,lastLimit})=>{
    const out=fixture._malloc(26*4),held=fixture._malloc(2);let done=false;
    try{for(let tick=0;tick<240;tick++){
     const s=state(),previousStage=fixture._application_test_recording_stage(),starting=s[2]!==previousStage&&s[5]===30,stage=starting?s[2]:(previousStage||s[2]),clock=starting?0:fixture._application_test_recording_frames();if(s[0]!==1||pause()[0]!==0||stage>maxStage||stage===maxStage&&clock>=lastLimit){done=true;break;}
     const source=sourceStreams[stage];if(!source||clock>=source.length&&s[2]===stage){done=true;break;}
     new DataView(fixture.HEAPU8.buffer).setUint16(held,source[Math.max(0,clock)]??0,true);
     if(!fixture._application_test_live_batch(held,1,out))throw Error(decode(fixture._application_error()));
     const v=Array.from(fixture.HEAP32.subarray(out>>>2,(out>>>2)+26));
     if(v[24]===stage&&v[25]>clock&&v[25]>0){const hp=fixture._application_test_simulation_hashes()>>>2;(recordedStates[stage]??=[])[v[25]-1]=[...v.slice(0,15),...v.slice(16,18),...fixture.HEAPU32.subarray(hp,hp+2)];if(stage===2&&[1,3363,3364,3566,3567].includes(v[25]))recordedDiagnostics[v[25]]=diagnostic();}
    }}finally{document.querySelector('canvas').getContext('webgl2').finish();fixture._free(out);fixture._free(held);}return{done,stage:state()[2],clock:fixture._application_test_recording_frames(),counts:Object.fromEntries(Object.entries(recordedStates).map(([s,a])=>[s,a.length]))};
   },{maxStage,lastLimit});simulated+=240;finished=value.done;if(simulated%2400===0||finished)console.log('live',JSON.stringify(value));
  }
  const saved=resumed?{bytes:resumed.bytes,counts:Object.fromEntries(Object.entries(resumed.states).map(([s,a])=>[s,a.length]))}:await page.evaluate(()=>{if(!fixture._application_prepare_replay(0))throw Error(decode(fixture._application_error()));const p=fixture._malloc(16);fixture.HEAPU8.set(new TextEncoder().encode('ROUNDTRP\0'),p);if(!fixture._application_save_replay(0,p))throw Error(decode(fixture._application_error()));fixture._free(p);return{bytes:Array.from(fixture.FS.readFile('/save/replay/th17_01.rpy')),counts:Object.fromEntries(Object.entries(recordedStates).map(([s,a])=>[s,a.length]))};});
  if(!resumed){const dir=resolve(root,'artifacts/replay-roundtrip');mkdirSync(dir,{recursive:true});writeFileSync(resolve(dir,'live-roundtrip.rpy'),Buffer.from(saved.bytes));writeFileSync(resolve(dir,'live-states.json'),JSON.stringify(await page.evaluate(()=>recordedStates)));writeFileSync(resolve(dir,'live-diagnostics.json'),JSON.stringify(await page.evaluate(()=>recordedDiagnostics)));writeFileSync(resolve(dir,'capture.json'),JSON.stringify({wasmSha256:sha(readFileSync(resolve(root,'artifacts/sdl-application/th17-application.wasm'))),sourceSha256:sha(bytes),files:Object.fromEntries(['live-roundtrip.rpy','live-states.json','live-diagnostics.json'].map(name=>[name,sha(readFileSync(resolve(dir,name)))]))},null,2)+'\n');}
  if(!resumed){const replay=c.replay_create(),p=c.allocate(saved.bytes.length);try{memory(c,p,saved.bytes.length).set(saved.bytes);assert.equal(c.replay_open(replay,p,saved.bytes.length),1);assert.equal(c.replay_web_rules(replay),2,'Actual application recordings carry the current web rules marker');}finally{c.deallocate(p);c.replay_delete(replay);}}
  assert.ok(saved.counts[maxStage]>=Math.min(1200,lastLimit,streams[maxStage].length-1),'Live test must actually reach and record the requested final stage');
  for(const [number,count]of Object.entries(saved.counts)){
   if(process.env.TH17_ROUNDTRIP_PLAYBACK_STAGES&&!process.env.TH17_ROUNDTRIP_PLAYBACK_STAGES.split(',').includes(number))continue;
   // A fresh Wasm instance and browser context per selected stage avoids
   // sharing any live run pools, GL context, input or static platform state.
   await browser.close();browser=await launchBrowser();page=await applicationPage(browser,server);page.on('pageerror',e=>errors.push(String(e)));page.on('console',m=>{if(m.type()==='error')console.log('browser error',m.text());});
   const expected=await page.evaluate(async stage=>{window.recordedStates=await(await fetch('/recording/states.json')).json();window.recordedDiagnostics=await(await fetch('/recording/diagnostics.json')).json();fixture.FS.writeFile('/save/replay/th17_01.rpy',new Uint8Array(await(await fetch('/recording/capture.rpy')).arrayBuffer()));window.diagnostic=()=>null;window.nextExpectedFrame=0;const rows=recordedStates[stage],terminalSceneTransition=rows.at(-1).every(x=>x===0);const p=fixture._malloc(32);fixture.HEAPU8.set(new TextEncoder().encode('th17_01.rpy\0'),p);if(!fixture._application_test_replay(p,stage))throw Error('Saved replay startup failed');fixture._free(p);step();return{count:rows.length-(terminalSceneTransition?1:0),terminalSceneTransition};},Number(number));
   const stage=Number(number);
   console.log('playback starting',stage,count);
   let checked=0,stalled=0,liveEndingFrames=0;
   while(checked<expected.count){
    const result=await page.evaluate(({stage,count})=>{
     const p=fixture._malloc(26*4);let checked=0,nativeCompletion=false,liveEndingFrames=0;
     try{for(let tick=0;tick<240;tick++){
      // Original GameProgression::next_stage opens the replay completion
      // menu at a final-stage clear. Live play alone continues the HUD wait
      // and ending fade. Those captured inputs are not replay gameplay.
      if((state()[6]&0x4000)&&pause()[0]===1){
       if(stage!==6&&stage!==7||!nextExpectedFrame)throw Error('Unexpected replay clear');
       const last=recordedStates[stage][nextExpectedFrame-1];
       for(let frame=nextExpectedFrame;frame<count;frame++)if(recordedStates[stage][frame].some((x,i)=>i&&x!==last[i]))throw Error('Uncompared gameplay changes after native replay completion at '+frame);
       nativeCompletion=true;liveEndingFrames=count-nextExpectedFrame;break;
      }
      const before=fixture._application_state(),s=Array.from(fixture.HEAP32.subarray(before>>>2,(before>>>2)+16));
      const selected=fixture._application_test_replay_stage();if(selected&&selected!==stage)break;if(!fixture._application_test_replay_trace(1,p))throw Error(decode(fixture._application_error()));
      const v=Array.from(fixture.HEAP32.subarray(p>>>2,(p>>>2)+26)),frame=v[25]-1;if(v[24]!==stage||frame<0||frame>=count)continue;
      if(frame<nextExpectedFrame)continue;if(frame!==nextExpectedFrame)throw Error('Skipped captured frame '+nextExpectedFrame+'; actual '+frame);
      const hp=fixture._application_test_simulation_hashes()>>>2,wanted=recordedStates[stage][frame],actual=[...v.slice(0,15),...v.slice(16,18),...fixture.HEAPU32.subarray(hp,hp+2)];if(!wanted)continue;
      if(actual.some((x,i)=>x!==wanted[i]))return{mismatch:{stage,frame,actual,wanted,diagnostic:diagnostic(),recordedDiagnostic:recordedDiagnostics[frame+1]},checked};checked++;nextExpectedFrame++;if(frame+1===count)break;
     }}finally{document.querySelector('canvas').getContext('webgl2').finish();fixture._free(p);}return{checked,nextExpectedFrame,nativeCompletion,liveEndingFrames,clock:world()[0],state:state(),pause:pause(),stage:fixture._application_test_replay_stage()||state()[2]};
    },{stage,count:expected.count});
    if(result.mismatch){writeFileSync(resolve(root,'artifacts/replay-roundtrip/mismatch.json'),JSON.stringify(result.mismatch));assert.fail(JSON.stringify({stage:result.mismatch.stage,frame:result.mismatch.frame,actual:result.mismatch.actual,wanted:result.mismatch.wanted}));}checked+=result.checked;
    if(result.nativeCompletion){liveEndingFrames=result.liveEndingFrames;break;}
    if(!result.checked){console.log('playback waiting',JSON.stringify({stage,totalChecked:checked,...result}));assert.ok(++stalled<=2,'Replay stopped consuming input before captured gameplay ended: '+JSON.stringify(result));}else stalled=0;
    assert.ok(result.checked>0||result.stage===stage,'Replay left stage before consuming saved inputs');
   }
   assert.equal(checked+liveEndingFrames,expected.count,'Each gameplay input frame must be checked exactly once through native replay completion');observations.push({stage,recordedInputFrames:count,frames:checked,liveEndingFrames,terminalSceneTransition:expected.terminalSceneTransition});console.log('roundtrip',JSON.stringify(observations.at(-1)));
  }
  assert.deepEqual(errors,[]);report(process.env.TH17_ROUNDTRIP_REPORT??'sdl-recording-roundtrip',{passed:true,wasmSha256:playbackHash,liveWasmSha256:capture?.wasmSha256??playbackHash,sourceSha256:sha(bytes),recordedSha256:sha(Buffer.from(saved.bytes)),observations,scope:'Real live input and recorder through normal stage transitions, SDL3 drawing, export, reopen and fresh per-stage browser/Wasm instances; no invulnerability or forced stage completion. Compares distinct gameplay frames through native replay completion: player/stocks/score/enemy count/game RNG and hashes of all active bullet motion, lifecycle, flags and hitboxes plus enemy motion, life, flags and script registers. The live-only ending wait/fade and terminal scene transition are excluded and counted separately; their captured simulation fields except the clock must remain unchanged. Source inputs do not make this newly recorded run an independent original-EXE comparison.'});
 }finally{await browser?.close();await server.close();}
});
