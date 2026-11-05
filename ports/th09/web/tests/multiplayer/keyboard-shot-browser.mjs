// Release ABI, production browser keyboard owner and SharedNetplay transport.
// Two independent Chromium processes; injected INPUT delay is observed before
// delivery so retransmissions cannot be mistaken for fresh captures.
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {existsSync,mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createRequire} from 'node:module';
import {presentationServer} from '../../scripts/presentation-server.mjs';

const root=resolve(import.meta.dirname,'../..');
const {chromium}=createRequire(process.env.BROWSER_TEST_PACKAGE||resolve(root,'package.json'))('playwright');
const label=process.env.RUN_LABEL;if(!/^[\w-]+$/.test(label||''))throw Error('Unique RUN_LABEL required');
const buildDir=resolve(process.env.PC_BUILD||resolve(root,'artifacts/sdl-release'));
const inputDelay=Number(process.env.TH09_TEST_INPUT_DELAY||0);assert.ok(Number.isInteger(inputDelay)&&inputDelay>=0&&inputDelay<=9);
const out=resolve(root,'artifacts/multiplayer-tests',label);if(existsSync(out))throw Error('Retained run already exists');mkdirSync(out,{recursive:true});
const report={passed:false,scope:'Two local Chromium processes, Release game worlds, production keyboard ownership and RTC/relay with injected 80/250 ms per-direction INPUT delay. Not main-site deployment, WAN or physical input-to-photon acceptance.',build:JSON.parse(readFileSync(resolve(buildDir,'build.json'))).sha256,scenarios:[],errors:[]};
const ownerUrl='data:text/javascript;base64,'+Buffer.from(readFileSync(resolve(root,'sdl-runtime/directory-keyboard.mjs'))).toString('base64');
const host=await presentationServer(0,{release:true,artifactDirectory:buildDir});
const relayPort=await new Promise(r=>{const s=net.createServer();s.listen(0,'127.0.0.1',()=>{const p=s.address().port;s.close(()=>r(p));});});
const relay=spawn(process.execPath,[resolve(process.env.TH09_SELECTION_RELAY||resolve(root,'../../eagler-touhou/services/netplay/netplay-relay.mjs'))],{windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(relayPort),EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
let relayLog='';relay.stderr.on('data',b=>relayLog+=b);await new Promise((ok,fail)=>{relay.stdout.on('data',b=>{relayLog+=b;if(relayLog.includes('netplay relay listening'))ok();});relay.on('error',fail);relay.on('exit',code=>fail(Error('relay '+code+' '+relayLog)));});
const browsers=[],pages=[];
async function state(page){return page.evaluate(()=>{
  const words=(fn,n)=>Array.from(core.HEAPU32.subarray(fn()/4,fn()/4+n));
  return {rollback:words(core._th09_rollback_info,8),pacing:words(core._th09_network_pacing_info,6),session:words(core._th09_session_status,8),touch:words(core._th09_touch_state,4),route:window.__eaglerNetplayTransport,closed:window.closedMessages,errors:window.testErrors};
});}
async function until(page,test,name,timeout=60000){const end=Date.now()+timeout;while(Date.now()<end){const s=await state(page);assert.equal(s.closed.length,0,JSON.stringify(s));if(test(s))return s;await page.waitForTimeout(50);}throw Error(name+' timeout '+JSON.stringify(await state(page)));}
async function actor(index){
  const browser=await chromium.launch({headless:true,executablePath:process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE,args:['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required']});browsers.push(browser);
  const page=await browser.newPage({viewport:{width:900,height:700}});pages.push(page);page.on('pageerror',e=>report.errors.push({index,error:e.stack}));
  await page.goto(host.url);await page.evaluate(async({index,relayPort,ownerUrl})=>{
    await openProbe(0,1,2,0,true);
    const {scanCodes}=await import('/app/keyboard.mjs');const {createBrowserKeyboard}=await import(ownerUrl);
    window.keyboardOwner=createBrowserKeyboard({accept:code=>!!scanCodes[code],send:(code,down)=>core._th09_key(scanCodes[code],+down)});
    core.resetBrowserKeyboard=()=>keyboardOwner.clear();
    window.audit={events:[],wire:[],duplicates:0,mutations:[],seen:new Map()};window.inputDelayMs=0;
    for(const type of ['keydown','keyup'])addEventListener(type,e=>{audit.events.push({t:performance.now(),code:e.code,down:type==='keydown',trusted:e.isTrusted,repeat:e.repeat});if(keyboardOwner.event(e,type==='keydown'))e.preventDefault();},true);
    addEventListener('blur',()=>{keyboardOwner.clear();core._th09_keys_clear();});
    function observe(data,lane){
      let b=data instanceof ArrayBuffer?new Uint8Array(data):ArrayBuffer.isView(data)?new Uint8Array(data.buffer,data.byteOffset,data.byteLength):null;
      if(!b)return false;if(b[0]===0xe7)b=b.subarray(2);
      if(b.length<44||b[0]!==69||b[2]!==78||b[3]!==80||b[5]!==1)return false;
      const d=new DataView(b.buffer,b.byteOffset,b.byteLength),first=d.getUint32(32,true),count=b[42];if(b.length!==44+count*12)return false;
      for(let i=0;i<count;++i){const frame=first+i,entry=Array.from(b.subarray(44+i*12,56+i*12)).join(',');
        if(audit.seen.has(frame)){++audit.duplicates;if(audit.seen.get(frame)!==entry)audit.mutations.push(frame);continue;}
        audit.seen.set(frame,entry);audit.wire.push({t:performance.now(),frame,buttons:d.getUint16(44+i*12,true),lane});
      }return true;
    }
    for(const [klass,lane] of [[WebSocket,'ws'],[RTCDataChannel,'rtc']]){const send=klass.prototype.send;klass.prototype.send=function(data){const input=observe(data,lane),ms=input?window.inputDelayMs:0;if(ms){const copy=data.slice?data.slice(0):new Uint8Array(data).slice();setTimeout(()=>{if(this.readyState===1||this.readyState==='open')send.call(this,copy);},ms);return;}return send.call(this,data);};}
    window.closedMessages=[];window.hashes={};
    window.lobby=new WebSocket(`ws://127.0.0.1:${relayPort}/?room=th09mp-0778&lobby=shotpeer${index}&member=shotmember${index}`);
    lobby.onmessage=e=>{const m=JSON.parse(e.data);if(m.type==='error')throw Error(m.error);window.room=m.room||window.room;window.roomMessage=m;};
  },{index,relayPort,ownerUrl});await page.waitForFunction(()=>window.room);return page;
}
async function connect(index){await pages[index].evaluate(async({index,relayPort,inputDelay})=>{
  const {SharedNetplay}=await import('/app/shared-netplay.mjs');
  const options={netplayUrl:`ws://127.0.0.1:${relayPort}/?room=th09mp-0778&run=1&players=2&member=shotmember${index}&player=${index}`,netplayPlayer:index,netplayPlayerCount:2,netplaySeed:12345,netplayDifficulty:0,netplayLoadouts:[{character:0},{character:1}],netplayInGameSelection:false,netplayIceServers:[],netplayAdonisMode:0,netplayInputDelay:inputDelay,netplayInputDelayAuto:false};
  core.eaglerOptions=options;window.play=new SharedNetplay(core,{onStatus:()=>{},onClose:s=>closedMessages.push(s),onResult:()=>{}});
  core.onNetworkSpectatorBegin=(...args)=>play.beginMatch(...args);core.onNetworkSpectatorEnd=(...args)=>play.endMatch(...args);core.onNetworkSpectatorFrame=(...args)=>play.publish(...args);core.onNetworkSpectatorRound=(...args)=>play.roundStart(...args);core.onNetworkResult=()=>play.result();
  let previous=0;core.onGameFrame=()=>{const at=core._th09_rollback_info()/4,confirmed=core.HEAPU32[at+1];for(let f=Math.max(previous,confirmed-32);f<confirmed;++f){const p=core._th09_confirmed_frame_hash(f)/4;if(core.HEAPU32[p]===f)hashes[f+1]=core.HEAPU32[p+1];}previous=confirmed;play.frame();};
  core._th09_loop_start();core._th09_loop_pause(1);await play.connect(options);
},{index,relayPort,inputDelay});}
async function scenario(name,delay,cpu){
  for(const p of pages)await p.evaluate(ms=>{inputDelayMs=ms;audit.events=[];audit.wire=[];},delay);
  const cdp=await pages[0].context().newCDPSession(pages[0]);await cdp.send('Emulation.setCPUThrottlingRate',{rate:cpu});await pages[0].waitForTimeout(1200);
  await pages[0].evaluate(()=>{audit.events=[];audit.wire=[];});
  // These complete strokes are guaranteed to fall between captures, whatever
  // the display phase. No logical tick or simulation input is directly edited.
  for(let i=0;i<8;++i){await pages[0].evaluate(()=>{dispatchEvent(new KeyboardEvent('keydown',{code:'KeyZ',key:'z',cancelable:true}));dispatchEvent(new KeyboardEvent('keyup',{code:'KeyZ',key:'z',cancelable:true}));});await pages[0].waitForTimeout(160);}
  for(const ms of [8,16,32,80])for(let i=0;i<5;++i){await pages[0].keyboard.press('KeyZ',{delay:ms});await pages[0].waitForTimeout(160);}
  await pages[0].waitForTimeout(600);
  const raw=await pages[0].evaluate(()=>({events:audit.events,wire:audit.wire,duplicates:audit.duplicates,mutations:audit.mutations}));writeFileSync(resolve(out,name+'-raw.json'),JSON.stringify(raw,null,2));
  const pairs=[];let down;
  for(const e of raw.events.filter(e=>e.code==='KeyZ')){if(e.down&&!e.repeat)down=e;else if(!e.down&&down){pairs.push({down:down.t,up:e.t,trusted:down.trusted});down=null;}}
  const trials=pairs.map((p,i)=>{const limit=pairs[i+1]?.down??Infinity,frames=raw.wire.filter(w=>w.t>=p.down&&w.t<limit),first=frames[0],shot=frames.find(w=>w.buttons&1);return {...p,width:p.up-p.down,captured:!!shot,firstFreshFrameFires:!!(first?.buttons&1),latency:shot?shot.t-p.down:null};});
  assert.equal(trials.length,28);assert.ok(trials.every(t=>t.captured),name+' lost stroke '+JSON.stringify(trials));assert.ok(trials.every(t=>t.firstFreshFrameFires),name+' extra input frame '+JSON.stringify(trials));assert.deepEqual(raw.mutations,[],'sent input remains immutable');
  const times=trials.map(t=>t.latency).sort((a,b)=>a-b),states=await Promise.all(pages.map(state));
  assert.ok(states.every(s=>s.session[0]===1&&s.touch[0]===1),'must remain in active gameplay');
  const frame=Math.min(...states.map(s=>s.rollback[1]))-30,hashes=await Promise.all(pages.map(p=>p.evaluate(f=>window.hashes[f],frame)));assert.ok(Number.isInteger(hashes[0]));assert.equal(hashes[0],hashes[1],'confirmed game state agrees');
  const result={name,delayPerDirectionMs:delay,cpuSlowdown:cpu,trials,captured:trials.filter(t=>t.captured).length,firstFreshFrameFires:trials.filter(t=>t.firstFreshFrameFires).length,medianMs:times[Math.floor(times.length*.5)],p95Ms:times[Math.ceil(times.length*.95)-1],confirmedFrame:frame,hash:hashes[0],states};report.scenarios.push(result);console.log(JSON.stringify({name,captured:result.captured,firstFreshFrameFires:result.firstFreshFrameFires,p95Ms:result.p95Ms,corrections:states.map(s=>s.rollback[2])}));
  await cdp.send('Emulation.setCPUThrottlingRate',{rate:1});await cdp.detach();
}
async function movementScenario(name,delay,cpu){
  for(const p of pages)await p.evaluate(ms=>{inputDelayMs=ms;audit.events=[];audit.wire=[];},delay);
  const cdp=await pages[0].context().newCDPSession(pages[0]);await cdp.send('Emulation.setCPUThrottlingRate',{rate:cpu});await pages[0].waitForTimeout(500);
  await pages[0].evaluate(()=>{audit.events=[];audit.wire=[];});
  const directions={ArrowLeft:64,ArrowUp:16,ArrowRight:128,ArrowDown:32};
  for(const code of Object.keys(directions))for(let i=0;i<2;++i){await pages[0].evaluate(code=>{dispatchEvent(new KeyboardEvent('keydown',{code,cancelable:true}));dispatchEvent(new KeyboardEvent('keyup',{code,cancelable:true}));},code);await pages[0].waitForTimeout(160);}
  for(const code of Object.keys(directions)){await pages[0].keyboard.press(code,{delay:8});await pages[0].waitForTimeout(160);}
  const raw=await pages[0].evaluate(()=>({events:audit.events,wire:audit.wire,mutations:audit.mutations}));
  const pairs=[];let down;for(const e of raw.events.filter(e=>directions[e.code])){if(e.down&&!e.repeat)down=e;else if(!e.down&&down){pairs.push({code:e.code,down:down.t,up:e.t,trusted:down.trusted});down=null;}}
  const trials=pairs.map((p,i)=>{const frames=raw.wire.filter(w=>w.t>=p.down&&w.t<(pairs[i+1]?.down??Infinity)),mask=directions[p.code];return {...p,mask,captured:!!(frames[0]?.buttons&mask),activeFrames:frames.filter(w=>w.buttons&mask).length};});
  assert.equal(trials.length,12);assert.ok(trials.every(t=>t.captured),name+' lost direction tap '+JSON.stringify(trials));assert.ok(trials.filter(t=>!t.trusted).every(t=>t.activeFrames===1),'unseen direction strokes produce exactly one capture');
  const releases=[];
  for(const code of Object.keys(directions)){
    await pages[0].keyboard.down(code);await pages[0].waitForTimeout(100);
    await pages[0].keyboard.up(code);await pages[0].waitForTimeout(180);
    releases.push(await pages[0].evaluate(({code,mask})=>{const up=audit.events.findLast(e=>e.code===code&&!e.down),first=audit.wire.find(w=>w.t>=up.t);return {code,up:up.t,first,stopped:!!first&&!(first.buttons&mask)};},{code,mask:directions[code]}));
  }
  assert.ok(releases.every(r=>r.stopped),name+' added a release tail '+JSON.stringify(releases));
  await pages[0].waitForTimeout(600);const states=await Promise.all(pages.map(state));assert.ok(states.every(s=>s.session[0]===1&&s.touch[0]===1),'movement check remains in active gameplay');
  const frame=Math.min(...states.map(s=>s.rollback[1]))-30,hashes=await Promise.all(pages.map(p=>p.evaluate(f=>window.hashes[f],frame)));assert.ok(Number.isInteger(hashes[0]));assert.equal(hashes[0],hashes[1],'movement confirmed game state agrees');
  assert.deepEqual(raw.mutations,[]);assert.deepEqual(await pages[0].evaluate(()=>audit.mutations),[]);
  const result={name,delayPerDirectionMs:delay,cpuSlowdown:cpu,trials,releases,confirmedFrame:frame,hash:hashes[0]};report.scenarios.push(result);writeFileSync(resolve(out,name+'-raw.json'),JSON.stringify({raw,releases},null,2));console.log(JSON.stringify({name,directionTaps:trials.length,captured:trials.filter(t=>t.captured).length,immediateReleases:releases.filter(t=>t.stopped).length}));
  await cdp.send('Emulation.setCPUThrottlingRate',{rate:1});await cdp.detach();
}
try{
  await actor(0);await actor(1);
  for(let i=0;i<2;++i)await pages[i].evaluate(i=>lobby.send(JSON.stringify({type:'take-seat',seat:i,loadout:i})),i);
  await pages[0].waitForFunction(()=>room.seats.slice(0,2).every(Boolean));for(const p of pages)await p.evaluate(()=>lobby.send(JSON.stringify({type:'set-ready',ready:true})));
  await pages[0].waitForFunction(()=>room.seats.slice(0,2).every(s=>s.ready));await pages[0].evaluate(inputDelay=>lobby.send(JSON.stringify({type:'start',adonisMode:0,inputDelay,inputDelayAuto:false})),inputDelay);await pages[0].waitForFunction(()=>roomMessage.type==='start');
  assert.equal(await pages[0].evaluate(()=>room.inputDelay),inputDelay,'authoritative relay retains room delay');
  await connect(0);await connect(1);for(const p of pages)await until(p,s=>s.rollback[6]&&s.session[1]>30,'connected');
  for(const p of pages){const native=await p.evaluate(()=>{const at=core._th09_adonis_info()/4;return Array.from(core.HEAPU32.subarray(at,at+2));});assert.deepEqual(native,[0,inputDelay],'native rollback uses selected D');}report.inputDelay=inputDelay;
  for(const p of pages)await p.keyboard.down('ControlLeft');for(const p of pages)await until(p,s=>s.touch[0]===1,'dialogue complete');for(const p of pages)await p.keyboard.up('ControlLeft');
  await pages[1].evaluate(()=>{let left=false;window.movement=setInterval(()=>{dispatchEvent(new KeyboardEvent('keyup',{code:left?'ArrowLeft':'ArrowRight'}));left=!left;dispatchEvent(new KeyboardEvent('keydown',{code:left?'ArrowLeft':'ArrowRight',cancelable:true}));},180);});
  if(process.env.TH09_TEST_ONLY_MOVEMENT!=='1'){await scenario('normal',0,1);await scenario('delay80-cpu4',80,4);await scenario('delay250-cpu4',250,4);}
  await movementScenario('directions-normal',0,1);await movementScenario('directions-delay80-cpu4',80,4);await movementScenario('directions-delay250-cpu4',250,4);
  // Lifecycle cancellation must discard a pending tap, rather than emitting it
  // after blur/pause. This uses the same clear boundary as the managed shell.
  await pages[0].evaluate(()=>{inputDelayMs=0;audit.wire=[];for(const code of ['KeyZ','ArrowLeft']){dispatchEvent(new KeyboardEvent('keydown',{code}));dispatchEvent(new KeyboardEvent('keyup',{code}));}keyboardOwner.clear();core._th09_keys_clear();});
  await pages[0].waitForTimeout(500);assert.equal(await pages[0].evaluate(()=>audit.wire.some(w=>w.buttons&0xf1)),false,'clear discards pending shot and direction taps');
  await pages[0].screenshot({path:resolve(out,'active-match.png')});assert.equal(report.errors.length,0);report.passed=true;
}catch(e){report.failure=e.stack;report.states=await Promise.all(pages.map(p=>state(p).catch(e=>({error:String(e)}))));throw e;}
finally{report.relayLog=relayLog;writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2));await Promise.all(browsers.map(b=>b.close()));host.netplay.close();await new Promise(r=>host.server.close(r));relay.kill();console.log(JSON.stringify({passed:report.passed,output:out}));}
