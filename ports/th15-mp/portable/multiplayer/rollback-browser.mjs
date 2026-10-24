// Deterministic browser acceptance: release game objects, independent exact
// lockstep oracle, real bounded correction driver and full-resolution pixels.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {launchBrowser} from '../../../../tools/browser/launch.mjs';
const root=resolve(import.meta.dirname,'../..');
const runtime=resolve(root,'web',process.env.TH15_OUTPUT||'artifacts/sdl-multiplayer','rollback');
const out=resolve(process.argv.find(v=>v.startsWith('--output='))?.slice(9)||runtime);await mkdir(out,{recursive:true});
const lifecycleOnly=process.argv.includes('--lifecycle-only');
const cp932=process.env.TH15_TEST_CP932_FONT||(process.env.EAGLER_FONT_ROOT&&resolve(process.env.EAGLER_FONT_ROOT,'cp932.bin'));
const font=process.env.TH15_TEST_FONT,unicode=process.env.TH15_TEST_UNICODE_FONT;
for(const [name,value] of Object.entries({TH15_TEST_ARCHIVE:process.env.TH15_TEST_ARCHIVE,cp932,TH15_TEST_FONT:font,TH15_TEST_UNICODE_FONT:unicode}))assert.ok(value,'Missing '+name);
const paths=[['/data',process.env.TH15_TEST_ARCHIVE],['/fonts/cp932.bin',cp932],['/msgothic.ttc',font],['/unifont.otf',unicode]];
const resources=new Map(await Promise.all(paths.map(async([url,path])=>[url,await readFile(path)])));
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const provenance={fixture:JSON.parse(await readFile(resolve(runtime,'build.json'),'utf8')),driver:sha(await readFile(import.meta.filename)),
 inputs:Object.fromEntries(paths.map(([url,path])=>[url,{path,sha256:sha(resources.get(url))}]))};
await writeFile(resolve(out,'inputs.json'),JSON.stringify(provenance,null,2)+'\n');
const html=`<!doctype html><meta charset="utf-8"><style>body{margin:0;background:#000}canvas{display:block}</style><canvas id="canvas" width="640" height="480"></canvas><script type="module">
import createModule from '/th15.mjs';
window.moduleReady=(async()=>{
 window.Module=await createModule({canvas:document.querySelector('canvas'),eaglerOptions:{netplaySpectator:true},printErr:console.error});
 const m=Module;m.FS.mkdirTree('/fonts');
 for(const [path,url] of [['/th15.dat','/data'],['/fonts/cp932.bin','/fonts/cp932.bin'],['/msgothic.ttc','/msgothic.ttc'],['/unifont.otf','/unifont.otf']])m.FS.writeFile(path,new Uint8Array(await(await fetch(url)).arrayBuffer()));
 m._th15_render_scale(Number(new URL(location.href).searchParams.get('scale'))||1);m._th15_music_enabled(0);
 if(!m._th15_initialize())throw Error('Native initialization failed');return true;
})();</script>`;
const server=createServer(async(req,res)=>{try{
 const path=new URL(req.url,'http://localhost').pathname;
 if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
 if(resources.has(path)){res.end(resources.get(path));return;}
 if(!['/th15.mjs','/th15.wasm'].includes(path)){res.writeHead(404).end();return;}
 res.setHeader('Content-Type',path.endsWith('.mjs')?'application/javascript':'application/wasm');res.end(await readFile(resolve(runtime,path.slice(1))));
}catch(error){res.writeHead(500).end(String(error));}});
await new Promise(yes=>server.listen(0,'127.0.0.1',yes));const base='http://127.0.0.1:'+server.address().port;
let browser;const reports=[],errors=[];
async function initialize(page,count,mode,scale){
 page.on('pageerror',error=>errors.push(String(error)));await page.setViewportSize({width:640*scale,height:480*scale});
 await page.goto(base+'/?scale='+scale);await page.evaluate(()=>moduleReady);
 await page.evaluate(({count,mode,lifecycleOnly})=>{
  const m=Module,text=p=>new TextDecoder().decode(m.HEAPU8.subarray(p,m.HEAPU8.indexOf(0,p)));
  const error=()=>text(m._th15_mp_error())||text(m._th15_spectator_lab_error());
  const check=(ok,label)=>{if(!ok)throw Error(label+': '+error());};
  check(m._th15_rollback_lab_begin(count,mode),'native begin');
  const words=(pointer,length)=>Array.from(m.HEAPU32.subarray(pointer>>>2,(pointer>>>2)+length));
  const status=()=>words(m._th15_mp_rollback_status(),20),details=()=>words(m._th15_rollback_lab_details(),10);
  const lifecycle=()=>words(m._th15_rollback_lab_lifecycle(),8);
  let lastCapture=0,lastCommit=0;
  const audit=()=>{const s=status();check(s[3]>=lastCapture,'physical capture rewound');lastCapture=s[3];
   if(s[4]!==0xffffffff){check(s[4]>=lastCommit&&s[4]<s[2],'confirmed output watermark');lastCommit=s[4];}
   check(s[5]<=12,'retained rollback window');const d=details();check(d[1]===(s[4]===0xffffffff?0:s[4]+1),'Replay escaped committed prefix');return s;};
  const input=(frame,seat)=>{
   let held=1,motion=0,x=0,y=0;
   if(frame>=180)held|=((Math.floor(frame/24)+seat)%2?0x40:0x80);
   if(lifecycleOnly)return [held,motion,x,y];
   if((frame>=240&&frame<248)||(frame>=600&&frame<608)){
    if(seat===1){motion=1;x=frame%2?132:-132;y=320+frame%3*12;held=1|(frame===244?2:0);}
   }
   if((frame===300||frame===420)&&seat===count-1)held|=0x100;
   if((frame===350||frame===470)&&seat===0)held|=0x100;
   return [held,motion,x,y];
  };
  const remote=(frame,seat)=>check(m._th15_rollback_lab_remote(seat,frame,...input(frame,seat)),'remote '+seat+'/'+frame);
  const tick=(frame,physical=true,override=null)=>{check(m._th15_rollback_lab_step(...(override||input(frame,0)),physical?1:0),'driver '+frame);return audit();};
  const state=()=>({world:words(m._th15_spectator_lab_state(),16),groups:words(m._th15_spectator_lab_groups(),6)});
  const forward=(frame,withhold=0)=>{for(let seat=1;seat<count;++seat)if(!(withhold&(1<<seat)))remote(frame,seat);
   const before=status();check(before[2]===frame&&!before[1],'unexpected simulation frontier '+frame);const after=tick(frame);
   check(after[2]===frame+1&&!after[1],'forward frame did not advance '+JSON.stringify({frame,before,after,details:details()}));return state();};
  const drain=target=>{let attempts=0;while(status()[2]<target||status()[1]||details()[8]!==0xffffffff){
   const s=tick(status()[2],false);check(s[2]<=target,'correction passed saved frontier');check(++attempts<128,'correction stalled '+JSON.stringify({target,status:s,details:details()}));
  }return {...state(),status:audit(),details:details()};};
  const snapshot=async pixels=>{const result={...state(),status:audit(),details:details(),lifecycle:lifecycle(),phase:m._th15_phase()};
   if(pixels){const p=m._th15_rollback_lab_pixels(),size=m._th15_rollback_lab_pixels_size();check(p&&size,'framebuffer readback');result.pixelBytes=size;result.pixelSha=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',m.HEAPU8.slice(p,p+size)))).map(v=>v.toString(16).padStart(2,'0')).join('');}
   return result;
  };
  const replay=()=>{const size=m._th15_rollback_lab_replay_size();check(size,'confirmed replay encode');return Array.from(m.HEAPU8.subarray(m._th15_rollback_lab_replay_bytes(),m._th15_rollback_lab_replay_bytes()+size));};
  const textureSnapshot=async(edited=false)=>{
   const pointer=m._th15_rollback_lab_texture_observe(edited?1:0);check(pointer,'texture observation');
   const value=words(pointer,13);check(value[0]===1,'texture capture');const images=[];
   for(let index=0;index<4;++index){const size=value[9+index],p=m._th15_rollback_lab_texture_bytes(edited?1:0,index);check(p&&size,'texture bytes');
    const bytes=m.HEAPU8.slice(p,p+size);images.push({size,allZero:bytes.every(v=>v===0),sha256:Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))).map(v=>v.toString(16).padStart(2,'0')).join('')});
   }
   return {executions:value[1],mutationFrame:value[2],journalOpenAtMutation:value[3]===1,retainedAtMutation:value[4],
    gameOwned:value.slice(5,7),uploadedBytes:{edited:value[7],untouched:value[8]},images};
  };
  window.rollbackLab={count,mode,input,remote,tick,status,details,lifecycle,audit,state,forward,drain,snapshot,replay,textureSnapshot};
 },{count,mode,lifecycleOnly});
}
function compare(expected,actual,label){
 assert.deepEqual(actual.world,expected.world,label+' world');assert.deepEqual(actual.groups,expected.groups,label+' script/projectile groups');
 if(expected.pixelSha){assert.equal(actual.pixelBytes,expected.pixelBytes,label+' framebuffer extent');assert.equal(actual.pixelSha,expected.pixelSha,label+' full-resolution pixels');}
}
async function pairedExact(exact,late,first,end,trajectory){
 for(let frame=first;frame<end;frame+=30){const until=Math.min(end,frame+30);
  const run=page=>page.evaluate(({first,end})=>{const result=[];for(let frame=first;frame<end;++frame)result.push(rollbackLab.forward(frame));return result;},{first:frame,end:until});
  const expected=await run(exact),actual=await run(late);for(let i=0;i<expected.length;++i)compare(expected[i],actual[i],'future frame '+(frame+i));
  trajectory.push({first:frame,end:until,hash:sha(Buffer.from(JSON.stringify(actual)))});
 }
}
function replayFrames(bytes){const data=Uint8Array.from(bytes),view=new DataView(data.buffer),description=view.getUint32(24,true),chapters=view.getUint32(28,true),frames=view.getUint32(32,true);
 return {frames,score:view.getUint32(64,true),complete:view.getUint32(68,true),chapters:Array.from(data.subarray(40+description,40+description+chapters*8)),samples:Array.from(data.subarray(40+description+chapters*8))};}
async function lifecycleAcceptance(exact,late,report){
 await pairedExact(exact,late,0,200,report.trajectory);let frame=200;
 const request=async name=>{for(const page of [exact,late])assert.equal(await page.evaluate(name=>Module[name](),name),1,name);};
 const boundary=async(kind,reached)=>{
  for(let attempts=0;attempts<600;++attempts,++frame){
   const before=await late.evaluate(()=>rollbackLab.snapshot(false));
   let expected=await exact.evaluate(async frame=>{rollbackLab.forward(frame);return rollbackLab.snapshot(false);},frame);
   if(!reached(expected)){
    const actual=await late.evaluate(async frame=>{rollbackLab.forward(frame);return rollbackLab.snapshot(false);},frame);
    compare(expected,actual,kind+' before boundary '+frame);continue;
   }
   expected=await exact.evaluate(()=>rollbackLab.snapshot(true));
   const blocked=await late.evaluate(frame=>{
    const lab=rollbackLab;for(let seat=1;seat<lab.count-1;++seat)lab.remote(frame,seat);
    lab.tick(frame);const waiting=lab.status();for(let n=0;n<3;++n)lab.tick(frame,false);
    return {waiting,status:lab.status(),details:lab.details(),lifecycle:lab.lifecycle(),phase:Module._th15_phase()};
   },frame);
   assert.equal(blocked.status[2],frame,kind+' advanced without exact remote input');
   assert.equal(blocked.status[4],frame-1,kind+' published unconfirmed output');
   assert.equal(blocked.status[17],frame,kind+' did not retain confirmed boundary');
   assert.equal(blocked.status[3],blocked.waiting[3],kind+' recaptured while waiting');
   assert.equal(blocked.status[5],0,kind+' retained journal frame at boundary');
   assert.equal(blocked.lifecycle[6],0,kind+' retained undo log at boundary');
   assert.equal(blocked.details[3],before.details[3],kind+' changed loading state while waiting');
   assert.equal(blocked.lifecycle[0],before.lifecycle[0],kind+' changed ending owner while waiting');
   assert.equal(blocked.phase,before.phase,kind+' changed native phase while waiting');
   const actual=await late.evaluate(async frame=>{rollbackLab.remote(frame,rollbackLab.count-1);rollbackLab.drain(frame+1);return rollbackLab.snapshot(true);},frame);
   compare(expected,actual,kind+' confirmed boundary');
   assert.deepEqual(actual.lifecycle,expected.lifecycle,kind+' native lifecycle state');
   assert.equal(actual.phase,expected.phase,kind+' native phase');
   report.checks.push({kind,frame,before,blocked,confirmed:actual});++frame;return;
  }
  throw Error(kind+' did not reach a native lifecycle boundary');
 };
 await request('_th15_rollback_lab_next_stage');
 await boundary('stage-2-entry',state=>state.details[3]===2);
 await pairedExact(exact,late,frame,frame+180,report.trajectory);frame+=180;
 let state=await late.evaluate(()=>rollbackLab.snapshot(true));
 assert.equal(state.details[5],2,'Native stage 2 was not constructed');assert.equal(state.details[3],0);assert.equal(state.phase,1);
 compare(await exact.evaluate(()=>rollbackLab.snapshot(true)),state,'stage 2 active');
 report.checks.push({kind:'stage-2-active',frame,state});
 await request('_th15_rollback_lab_request_ending');
 await boundary('ending-entry',state=>state.details[3]===1);
 for(let attempts=0;attempts<180;++attempts){
  await pairedExact(exact,late,frame,frame+1,report.trajectory);++frame;state=await late.evaluate(()=>rollbackLab.snapshot(false));
  if(state.lifecycle[0]&&state.details[3]===0&&state.lifecycle[3]===0xffffffff)break;
 }
 assert.equal(state.lifecycle[0],1,'Native ending was not constructed');assert.equal(state.lifecycle[1],2,'Multiplayer ending history was not normalized');
 assert.equal(state.phase,3);assert.equal(state.details[3],0);assert.equal(state.lifecycle[3],0xffffffff);
 report.checks.push({kind:'ending-active',frame,state});
 await request('_th15_rollback_lab_finish_ending');
 await boundary('results-entry',state=>state.phase===0&&!state.lifecycle[0]);
 const retired=[];
 for(const page of [exact,late])retired.push(await page.evaluate(async()=>{
  const lab=rollbackLab;lab.tick(lab.status()[2],false);const before=await lab.snapshot(false),replay=lab.replay();
  for(let n=0;n<5;++n)lab.tick(lab.status()[2],false);
  return {before,after:await lab.snapshot(true),replay};
 }));
 for(const record of retired){
  assert.equal(record.after.lifecycle[7],1,'Transport did not retire at confirmed results');
  for(const index of [2,3,4])assert.equal(record.after.status[index],record.before.status[index],'Retired netplay frontier advanced');
  assert.equal(record.after.status[5],0);assert.equal(record.after.status[1],0);assert.equal(record.after.lifecycle[6],0);
  assert.equal(record.after.details[8],0xffffffff);assert.equal(record.after.phase,0);assert.equal(record.after.status[19],1);
  assert.equal(replayFrames(record.replay).frames,record.after.status[4]+1);assert.equal(replayFrames(record.replay).complete,1);
 }
 compare(retired[0].after,retired[1].after,'retired local results');
 assert.deepEqual(replayFrames(retired[1].replay),replayFrames(retired[0].replay),'Lifecycle confirmed Replay differs from exact run');
 await writeFile(resolve(out,report.label+'.rpy'),Uint8Array.from(retired[1].replay));
 report.checks.push({kind:'retired-results',state:retired[1].after});
 report.confirmedReplayFrames=replayFrames(retired[1].replay).frames;report.final=retired[1].after;
 report.scope='Native stage completion and service-request ending/results lifecycle; story/staff script completion and live transport ACK retirement are separate acceptance cases.';
}
try{
 browser=await launchBrowser({...(process.env.EAGLER_CHROME_PATH?{executablePath:process.env.EAGLER_CHROME_PATH}:{}),
  args:[...(process.platform==='linux'&&process.getuid?.()===0?['--no-sandbox']:[]),'--enable-unsafe-swiftshader','--use-angle=swiftshader','--autoplay-policy=no-user-gesture-required']});
 const cases=lifecycleOnly?[{count:3,mode:2,scale:2}]:[...([2,3].flatMap(count=>[0,2].map(mode=>({count,mode,scale:1})))),{count:3,mode:2,scale:2}];
 for(const config of cases){const {count,mode,scale}=config,label=`${count}p-mode${mode}-scale${scale}${lifecycleOnly?'-lifecycle':''}`,trajectory=[];
  const exact=await browser.newPage(),late=await browser.newPage();await initialize(exact,count,1,scale);await initialize(late,count,mode,scale);
  const report={...config,label,oracle:'independent v7 pure-delay exact-input WASM',checks:[],trajectory};
  try{
   if(lifecycleOnly){
    await lifecycleAcceptance(exact,late,report);assert.deepEqual(errors,[]);report.status='PASS';reports.push(report);
    await writeFile(resolve(out,label+'.json'),JSON.stringify(report,null,2)+'\n');
    console.log('PASS '+label+' | confirmed stage 2, ending, results, immutable retired frontiers, completion Replay');continue;
   }
   await pairedExact(exact,late,0,240,trajectory);
   // Fresh, unused ANM images exercise a first CPU edit in discarded prediction
   // without altering the authored game. The fixture callback uses the real
   // image writer, journal, texture upload and framebuffer readback.
   const textureBefore=[];
   for(const [page,armed] of [[exact,0],[late,1]]){
    assert.equal(await page.evaluate(armed=>Module._th15_rollback_lab_texture_begin(armed),armed),1,label+' texture probe setup');
    textureBefore.push(await page.evaluate(()=>rollbackLab.textureSnapshot()));
   }
   for(const before of textureBefore){
    assert.deepEqual(before.gameOwned,[1,1],label+' texture pixels must belong to the game heap');
    assert.deepEqual(before.uploadedBytes,{edited:0,untouched:0},label+' prepared textures must already be cached');
    assert.equal(before.executions,0);assert.ok(before.images.every(image=>image.size===1024&&!image.allZero));
   }
   assert.deepEqual(textureBefore[1].images,textureBefore[0].images,label+' initial CPU and GPU texture bytes');
   // Eight speculative frames, then a second mismatch during bounded catchup.
   await exact.evaluate(()=>{for(let f=240;f<248;++f)rollbackLab.forward(f);});
   const retained=await late.evaluate(async()=>{for(let f=240;f<248;++f)rollbackLab.forward(f,1<<1);const capture=rollbackLab.status()[3],corrections=rollbackLab.status()[8];
    const textureEdited=await rollbackLab.textureSnapshot(true);
    rollbackLab.remote(240,1);rollbackLab.tick(240,false);let attempts=0;
    while(rollbackLab.status()[1]&&rollbackLab.status()[2]<242){rollbackLab.tick(rollbackLab.status()[2],false);if(++attempts>8)throw Error('First correction did not reach second predicted input');}
    const partial=rollbackLab.status();
    if(!partial[1]||partial[2]>=248)throw Error('Correction was not split across callbacks');
    for(let f=241;f<248;++f)rollbackLab.remote(f,1);const corrected=rollbackLab.drain(248);
    if(corrected.status[3]!==capture)throw Error('Correction recaptured local controls');return {corrections,partial,corrected,textureEdited};});
   assert.ok(retained.corrected.status[8]>=retained.corrections+2,'A new late mismatch did not restart correction');
   const edit=retained.textureEdited;
   assert.equal(edit.executions,1);assert.equal(edit.mutationFrame,240);assert.equal(edit.journalOpenAtMutation,true);assert.ok(edit.retainedAtMutation>0);
   assert.deepEqual(edit.uploadedBytes,{edited:1024,untouched:0},label+' first CPU edit must really upload to the GPU');
   for(const index of [0,2]){assert.equal(edit.images[index].allZero,true);assert.notEqual(edit.images[index].sha256,textureBefore[1].images[index].sha256);}
   for(const index of [1,3])assert.deepEqual(edit.images[index],textureBefore[1].images[index],label+' untouched texture during prediction');
   const textureAfter=[];for(const page of [exact,late])textureAfter.push(await page.evaluate(()=>rollbackLab.textureSnapshot()));
   for(let index=0;index<2;++index)assert.deepEqual(textureAfter[index].images,textureBefore[index].images,label+' restored CPU and GPU texture bytes');
   assert.deepEqual(textureAfter[1].images,textureAfter[0].images,label+' restored textures equal exact-input oracle');
   assert.equal(textureAfter[1].executions,1,label+' texture edit must not repeat during history');
   assert.deepEqual(textureAfter[0].uploadedBytes,{edited:0,untouched:0});
   assert.deepEqual(textureAfter[1].uploadedBytes,{edited:1024,untouched:0},label+' restore must refresh the first-edited texture and keep the untouched upload');
   report.checks.push({kind:'first-speculative-texture-edit',before:textureBefore,edited:edit,restored:textureAfter});
   compare(await exact.evaluate(()=>rollbackLab.snapshot(true)),await late.evaluate(()=>rollbackLab.snapshot(true)),label+' nested correction');
   for(const page of [exact,late])assert.equal(await page.evaluate(()=>Module._th15_rollback_lab_texture_end()),1,label+' texture probe cleanup');
   report.checks.push({kind:'nested-correction',...retained});
   await pairedExact(exact,late,248,300,trajectory);
   // Pause arrives as the first replayed frame after a future GPU frontier.
   for(const [page,withhold] of [[exact,0],[late,1<<(count-1)]])await page.evaluate(withhold=>{for(let f=300;f<306;++f)rollbackLab.forward(f,withhold);},withhold);
   await late.evaluate(()=>{for(let f=300;f<306;++f)rollbackLab.remote(f,rollbackLab.count-1);rollbackLab.drain(306);});
   const pauseExact=await exact.evaluate(()=>rollbackLab.snapshot(true)),pauseLate=await late.evaluate(()=>rollbackLab.snapshot(true));
   assert.ok(pauseExact.details[2]>0,'Pause input did not enter native pause');compare(pauseExact,pauseLate,label+' late pause capture');
   report.checks.push({kind:'late-pause',state:pauseLate});await late.screenshot({path:resolve(out,label+'-pause.png')});
   await pairedExact(exact,late,306,420,trajectory);
   // An unused title resource retires after the last complete gameplay Draw.
   // This forces the actual confirmed-unload full-resolution fallback path.
   for(const page of [exact,late])assert.equal(await page.evaluate(()=>Module._th15_rollback_lab_retire_title()),1);
   for(const [page,withhold] of [[exact,0],[late,1<<(count-1)]])await page.evaluate(withhold=>{for(let f=420;f<426;++f)rollbackLab.forward(f,withhold);},withhold);
   await late.evaluate(()=>{for(let f=420;f<426;++f)rollbackLab.remote(f,rollbackLab.count-1);rollbackLab.drain(426);});
   const retiredExact=await exact.evaluate(()=>rollbackLab.snapshot(true)),retiredLate=await late.evaluate(()=>rollbackLab.snapshot(true));
   assert.ok(retiredExact.details[2]>0,'Resource-retirement pause did not activate');compare(retiredExact,retiredLate,label+' retired-handle pause capture');
   report.checks.push({kind:'retired-resource-predecessor',state:retiredLate});await late.screenshot({path:resolve(out,label+'-retired-pause.png')});
   await pairedExact(exact,late,426,860,trajectory);
   // The rollback window stops at twelve unconfirmed frames. A waiting caller
   // cannot replace the already captured physical input or publish it in Replay.
   await exact.evaluate(()=>{for(let f=860;f<872;++f)rollbackLab.forward(f);});
   const waiting=await late.evaluate(()=>{for(let f=860;f<872;++f)rollbackLab.forward(f,1<<(rollbackLab.count-1));
    rollbackLab.tick(872);const before=rollbackLab.status();for(let i=0;i<5;++i)rollbackLab.tick(872,true,[2,2,150,80]);
    const after=rollbackLab.status();if(after[2]!==before[2]||after[3]!==before[3])throw Error('Bounded wait advanced or recaptured');
    const replay=rollbackLab.replay();if(new DataView(Uint8Array.from(replay).buffer).getUint32(32,true)!==860)throw Error('Replay leaked predicted suffix');
    for(let f=860;f<872;++f)rollbackLab.remote(f,rollbackLab.count-1);rollbackLab.drain(872);return {before,after};});
   assert.equal(waiting.after[5],12);compare(await exact.evaluate(()=>rollbackLab.snapshot(true)),await late.evaluate(()=>rollbackLab.snapshot(true)),label+' bounded recovery');
   await pairedExact(exact,late,872,873,trajectory);report.checks.push({kind:'twelve-frame-wait',...waiting});
   const expectedReplay=await exact.evaluate(()=>rollbackLab.replay()),actualReplay=await late.evaluate(()=>rollbackLab.replay());
   assert.deepEqual(replayFrames(actualReplay),replayFrames(expectedReplay),'Confirmed all-seat Replay differs from exact run');
   await writeFile(resolve(out,label+'.rpy'),Uint8Array.from(actualReplay));report.confirmedReplayFrames=replayFrames(actualReplay).frames;
   report.futureMatchingFrames=600;report.final=await late.evaluate(()=>rollbackLab.snapshot(true));
   assert.ok(report.final.status[11]>0&&report.final.status[9]>0);assert.deepEqual(errors,[]);
   report.status='PASS';reports.push(report);await writeFile(resolve(out,label+'.json'),JSON.stringify(report,null,2)+'\n');
   console.log('PASS '+label+' | nested correction, late pause, retired GPU predecessor, 600 future frames, 12-frame fence, confirmed Replay');
  }catch(error){
   await writeFile(resolve(out,label+'.failure.json'),JSON.stringify({error:String(error.stack||error),report,
    exact:await exact.evaluate(()=>rollbackLab.snapshot(false)).catch(String),late:await late.evaluate(()=>rollbackLab.snapshot(false)).catch(String)},null,2)+'\n');throw error;
  }finally{await exact.close();await late.close();}
 }
 await writeFile(resolve(out,'report.json'),JSON.stringify({status:'PASS',reports},null,2)+'\n');
}finally{await browser?.close();await new Promise(yes=>server.close(yes));}
