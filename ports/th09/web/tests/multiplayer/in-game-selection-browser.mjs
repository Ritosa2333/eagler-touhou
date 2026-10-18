// Real Release worlds, browser keyboard input, shared room relay and read-only
// native observations. No forced winner, invincibility or gameplay shortcut.
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
const buildDir=resolve(process.env.PC_BUILD||resolve(root,'artifacts/in-game-selection-release-20261015'));
const out=resolve(root,'artifacts/multiplayer-tests',label);if(existsSync(out))throw Error('Retained run already exists');mkdirSync(out,{recursive:true});
const fallback=process.argv.includes('--relay'),mode=Number(process.env.ADONIS_MODE||0),selectionOnly=process.argv.includes('--selection-only'),packetDelay=Number(process.env.MENU_PACKET_DELAY_MS||0);
const report={passed:false,scope:selectionOnly?'Local Chromium selection cadence, independent character inputs and shared difficulty/stage controls under delayed packets. Not full-match/public-network/device acceptance.':'Local Chromium, real Release worlds and shared transport. Natural match completion. Not public-network/device acceptance.',build:JSON.parse(readFileSync(resolve(buildDir,'build.json'))).sha256,fallback,mode,stages:[],errors:[]};
const stage=(name,details={})=>{const row={name,at:new Date().toISOString(),...details};report.stages.push(row);console.log(JSON.stringify(row));};
const relayPort=await new Promise(r=>{const s=net.createServer();s.listen(0,'127.0.0.1',()=>{const p=s.address().port;s.close(()=>r(p));});});
const relay=spawn(process.execPath,[resolve(process.env.TH09_SELECTION_RELAY||resolve(root,'../../eagler-touhou/services/netplay/netplay-relay.mjs'))],{windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(relayPort),EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_TH09_SPECTATOR_TIMELINE:'1'},stdio:['ignore','pipe','pipe']});
let relayLog='';relay.stderr.on('data',b=>relayLog+=b);await new Promise((ok,fail)=>{relay.stdout.on('data',b=>{relayLog+=b;if(relayLog.includes('netplay relay listening'))ok();});relay.on('error',fail);relay.on('exit',code=>fail(Error('relay '+code+' '+relayLog)));});
const host=await presentationServer(0,{release:true,artifactDirectory:buildDir});
const browser=await chromium.launch({headless:true,executablePath:process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE,args:['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required']});
const pages=[];
async function actor(index){
  const page=await browser.newPage({viewport:{width:900,height:700}});pages.push(page);
  page.on('pageerror',e=>report.errors.push({index,error:e.stack}));
  await page.goto(host.url);await page.evaluate(async({index,relayPort,fallback,packetDelay})=>{
    await openProbe(0,1,2,3,true);
    if(packetDelay&&index<2){const send=WebSocket.prototype.send;WebSocket.prototype.send=function(data){if(typeof data==='string')return send.call(this,data);const copy=data.slice?data.slice(0):data;setTimeout(()=>{if(this.readyState===1)send.call(this,copy);},packetDelay);};}
if(fallback)window.RTCPeerConnection=undefined;
    const {scanCodes}=await import('/app/keyboard.mjs');core.resetBrowserKeyboard=()=>{};
    addEventListener('keydown',e=>{if(scanCodes[e.code]){e.preventDefault();core._th09_key(scanCodes[e.code],1);}});
    addEventListener('keyup',e=>{if(scanCodes[e.code]){e.preventDefault();core._th09_key(scanCodes[e.code],0);}});
    window.notices=[];window.closeEvents=[];window.begins=[];window.ends=[];window.hashes={};
    window.lobby=new WebSocket(`ws://127.0.0.1:${relayPort}/?room=th09mp-0777&lobby=selectionpeer${index}&member=selectionmember${index}`);
    lobby.onmessage=e=>{const m=JSON.parse(e.data);if(m.type==='error')throw Error(m.error);window.room=m.room||window.room;window.roomMessage=m;};
  },{index,relayPort,fallback,packetDelay});
  await page.waitForFunction(()=>window.room);return page;
}
async function state(page){return page.evaluate(()=>{
  const words=(fn,n)=>{const at=fn()/4;return Array.from(core.HEAP32.subarray(at,at+n));};
  const p=core._th09_error(),a=core.HEAPU8,e=a.indexOf(0,p);
  return {selection:words(core._th09_network_selection_info,16),title:words(core._th09_title_status,8),session:words(core._th09_session_status,8),rollback:words(core._th09_rollback_info,8),viewer:words(core._th09_spectator_playback_info,11),hash:core._th09_network_hash()>>>0,error:new TextDecoder().decode(a.subarray(p,e)),closed:closeEvents,notices:notices.slice(-4),begins,ends,active:window.play?.active,route:window.__eaglerNetplayTransport};
});}
async function waitFor(page,predicate,name,timeout=30000){const until=Date.now()+timeout;while(Date.now()<until){const s=await state(page);if(s.error||s.closed.length)throw Error(name+': '+JSON.stringify(s));if(predicate(s))return s;await page.waitForTimeout(50);}throw Error(name+' timeout '+JSON.stringify(await state(page)));}
async function press(index,key){await pages[index].keyboard.press(key,{delay:100});await pages[index].waitForTimeout(180);}
async function connect(index){await pages[index].evaluate(async({index,relayPort,mode})=>{
  const {SharedNetplay}=await import('/app/shared-netplay.mjs');const spectator=index>=2;
  const options={netplayUrl:`ws://127.0.0.1:${relayPort}/?room=th09mp-0777&run=1&players=2&member=selectionmember${index}&${spectator?'spectator=selectionpeer'+index:'player='+index}`,netplayPlayer:spectator?0:index,netplayPlayerCount:2,netplaySeed:12345,netplayDifficulty:3,netplayLoadouts:[{character:0},{character:1}],netplayInGameSelection:true,netplaySpectator:spectator,netplaySpectatorId:spectator?'selectionpeer'+index:'',netplaySpectatorCount:2,netplaySpectatorTimeline:true,netplayIceServers:[],netplayAdonisMode:mode,netplayInputDelay:mode?2:0,netplayInputDelayAuto:false};
  core.eaglerOptions=options;window.play=new SharedNetplay(core,{onStatus:s=>notices.push(s),onClose:s=>closeEvents.push(s),onResult:()=>{}});
  const readHash=frame=>{const at=core._th09_confirmed_frame_hash(frame)/4;if(core.HEAPU32[at]===frame)hashes[frame+1]=core.HEAPU32[at+1];};
  core.onNetworkSpectatorBegin=(frame,bytes)=>{begins.push({frame,bytes:bytes.length});play.beginMatch(frame,bytes);};
  core.onNetworkSpectatorEnd=(frame,winner)=>{ends.push({frame,winner});play.endMatch(frame,winner);};
  core.onNetworkSpectatorRound=(...args)=>play.roundStart(...args);
  core.onNetworkSpectatorFrame=(frame,...args)=>{readHash(frame);play.publish(frame,...args);};
  core.onNetworkResult=()=>play.result();
  core.onGameFrame=()=>{if(!spectator){const at=core._th09_rollback_info()/4;const confirmed=core.HEAPU32[at+1];for(let f=Math.max(0,confirmed-9);f<confirmed;f++)readHash(f);}
    if(spectator&&window.pauseAt!==undefined&&core._th09_spectator_frame()===window.pauseAt){core._th09_spectator_control(0,0);window.pauseAt=undefined;}
    play.frame();};
  core._th09_loop_start();core._th09_loop_pause(1);await play.connect(options);
},{index,relayPort,mode});}
async function menu(screen){for(const page of pages.slice(0,2))await waitFor(page,s=>s.selection[1]===1&&s.title[2]===screen&&s.title[3]===1&&s.title[5]>=10,'menu '+screen);}
try{selectionRun:{
  await actor(0);await actor(1);await actor(2);
  for(let i=0;i<2;i++)await pages[i].evaluate(i=>lobby.send(JSON.stringify({type:'take-seat',seat:i,loadout:i})),i);
  await pages[2].evaluate(()=>lobby.send(JSON.stringify({type:'spectate'})));
  await pages[0].waitForFunction(()=>room.seats.slice(0,2).every(Boolean)&&room.spectatorCount===1);
  for(let i=0;i<2;i++)await pages[i].evaluate(()=>lobby.send(JSON.stringify({type:'set-ready',ready:true})));
  await pages[0].waitForFunction(()=>room.seats.slice(0,2).every(s=>s.ready));
  await pages[0].evaluate(mode=>lobby.send(JSON.stringify({type:'start',adonisMode:mode,inputDelay:mode?2:0,inputDelayAuto:false})),mode);
  await pages[0].waitForFunction(()=>roomMessage.type==='start');
  await connect(2);await connect(0);await connect(1);await menu(7);stage('connected-selection');
  // Waiting for people is valid; it must not trigger the gameplay-stream timeout.
  const startFrames=(await state(pages[0])).title[0],startAt=Date.now();await pages[0].waitForTimeout(selectionOnly?3000:16000);
  if(selectionOnly){const rate=((await state(pages[0])).title[0]-startFrames)*1000/(Date.now()-startAt);assert.ok(rate>48,'menu rate '+rate);stage('menu-cadence',{fps:rate,packetDelay});}
  assert.equal((await state(pages[2])).closed.length,0);
  await press(1,'ArrowDown');assert.equal((await state(pages[0])).title[4],2,'P2 can move shared difficulty cursor');await press(1,'ArrowUp');
  await press(0,'ArrowDown');await press(0,'KeyZ');await menu(8);
  const before=await state(pages[0]);await press(0,'ArrowRight');const left=await state(pages[0]);
  assert.notEqual(left.selection[3],before.selection[3]);assert.equal(left.selection[4],before.selection[4]);
  await press(1,'ArrowRight');const right=await state(pages[0]);assert.equal(right.selection[3],left.selection[3]);assert.notEqual(right.selection[4],left.selection[4]);
  for(const page of pages.slice(0,2)){await page.keyboard.down('ShiftLeft');await page.keyboard.down('ArrowLeft');}
  await pages[0].waitForTimeout(3500);
  for(const page of pages.slice(0,2)){await page.keyboard.up('ArrowLeft');await page.keyboard.up('ShiftLeft');}
  await pages[0].waitForTimeout(200);await press(0,'KeyZ');
  let selected=await state(pages[0]);assert.deepEqual(selected.selection.slice(5,7),[1,0]);
  await press(1,'KeyZ');await menu(16);await pages[0].screenshot({path:resolve(out,'first-stage-selection.png')});
  const hostStage=(await state(pages[0])).title[4];await press(0,'ArrowDown');assert.notEqual((await state(pages[0])).title[4],hostStage,'P1 can move shared stage cursor');
  const priorStage=(await state(pages[0])).title[4];await press(1,'ArrowDown');
  assert.notEqual((await state(pages[0])).title[4],priorStage,'P2 can move shared stage cursor');
  if(selectionOnly){
    const states=await Promise.all(pages.slice(0,2).map(state));const confirmed=Math.min(...states.map(s=>s.rollback[1]))-20;
    const hashes=await Promise.all(pages.slice(0,2).map(p=>p.evaluate(f=>window.hashes[f],confirmed)));
    assert.ok(Number.isInteger(hashes[0]));assert.equal(hashes[0],hashes[1],'confirmed menu hashes match');
    assert.ok(states.every(s=>s.rollback[2]>0),'delayed input exercised real menu rollback');
    stage('menu-rollback-and-confirmed-hash',{confirmed,hash:hashes[0],corrections:states.map(s=>s.rollback[2])});
  }
  await press(1,'KeyZ');
  const first=await waitFor(pages[0],s=>!s.selection[1]&&s.session[1]>=120,'first match');
  const peer=await state(pages[1]);assert.deepEqual(first.selection.slice(7,15),peer.selection.slice(7,15));assert.deepEqual(first.selection.slice(8,10),[1,2]);
  assert.equal(first.route,fallback?'relay':'rtc');
  assert.deepEqual(first.selection.slice(10,12),[1,1]);
  await waitFor(pages[2],s=>s.viewer[1]>first.begins[0].frame+20,'first spectator');
  if(selectionOnly){stage('both-stage-selection-and-launch');break selectionRun;}
  await pages[2].locator('#spectatorToggle').click();await pages[2].locator('#spectatorPause').click();const held=await state(pages[2]);
  for(const page of pages.slice(0,2)){await page.keyboard.down('KeyZ');await page.keyboard.down('ControlLeft');}
  stage('both-selected-and-playing',{characters:first.selection.slice(8,10),health:first.selection.slice(10,12),route:first.route,pausedViewer:held.viewer});
  const completed=await waitFor(pages[0],s=>s.session[0]===3&&s.ends.length===1,'natural match completion',240000);
  for(const page of pages.slice(0,2)){await page.keyboard.up('KeyZ');await page.keyboard.up('ControlLeft');}
  stage('natural-match-complete',{end:completed.ends[0],frame:completed.rollback[0]});await pages[0].screenshot({path:resolve(out,'first-result.png')});
  await pages[0].waitForTimeout(500);await press(0,'ArrowDown');await press(0,'KeyZ');await menu(7);
  assert.equal((await state(pages[2])).viewer[1],held.viewer[1]);
  await press(0,'ArrowDown');await press(0,'KeyZ');await menu(8);await press(0,'ArrowRight');await press(1,'ArrowRight');
  await press(0,'Shift+KeyZ');await press(1,'KeyZ');await menu(16);await press(0,'ArrowDown');await press(0,'KeyZ');
  const second=await waitFor(pages[0],s=>s.begins.length===2&&!s.selection[1]&&s.session[1]>120,'second match');
  assert.deepEqual(second.selection.slice(8,10),[2,3]);assert.equal((await state(pages[2])).viewer[1],held.viewer[1]);
  assert.equal(second.selection[7],3);assert.deepEqual(second.selection.slice(12,14),[1,0]);assert.equal(second.selection[14],-2);
  if(mode===1)assert.deepEqual(second.rollback.slice(2,4),[0,0]);
  await pages[2].waitForFunction(()=>play.spectatorTape.begins===1);
  stage('reselected-without-leaving',{characters:second.selection.slice(8,10),starts:second.begins,viewer:await state(pages[2])});
  const end=completed.ends[0].frame;
  await pages[2].evaluate(end=>{window.pauseAt=end;core._th09_spectator_control(1,end);},end);
  const oldTail=await waitFor(pages[2],s=>s.viewer[1]===end&&s.viewer[5]===1,'old match tail',120000);
  const expected=await pages[0].evaluate(end=>hashes[end],end);assert.equal(oldTail.hash,expected,'paused old world completes exactly before replacing characters');
  await pages[2].locator('#spectatorLive').click();
  await waitFor(pages[2],s=>s.viewer[8]===second.begins[1].frame&&s.viewer[1]>second.begins[1].frame+60,'viewer follows new characters');
  const target=(await state(pages[2])).viewer[1]+120;
  await pages[2].evaluate(target=>{window.pauseAt=target;core._th09_spectator_control(1,target);},target);
  const viewer=await waitFor(pages[2],s=>s.viewer[1]===target&&s.viewer[5]===1,'same-frame new world');
  const expected2=await pages[0].evaluate(target=>hashes[target],target);assert.equal(viewer.hash,expected2,'spectator new character world matches confirmed host state');
  await actor(3);await pages[3].evaluate(()=>lobby.send(JSON.stringify({type:'spectate'})));await pages[3].waitForFunction(()=>roomMessage.type==='spectator-start');await connect(3);
  const late=await waitFor(pages[3],s=>s.viewer[8]===second.begins[1].frame&&s.viewer[1]>second.begins[1].frame+30,'late spectator starts new match');
  assert.deepEqual(late.selection.slice(8,10),[2,3]);
  await pages[2].screenshot({path:resolve(out,'spectator-second-match.png')});
  stage('spectator-continuity-and-late-join',{oldTail:{frame:end,hash:expected},newMatch:{frame:target,hash:expected2},late:late.viewer});
  if(process.argv.includes('--retry-late')){
    for(const page of pages.slice(0,2)){await page.keyboard.down('KeyZ');await page.keyboard.down('ControlLeft');}
    await waitFor(pages[0],s=>s.session[0]===3&&s.ends.length===2,'second natural completion',240000);
    for(const page of pages.slice(0,2)){await page.keyboard.up('KeyZ');await page.keyboard.up('ControlLeft');}
    await pages[0].waitForTimeout(500);await press(0,'KeyZ');
    const third=await waitFor(pages[0],s=>s.begins.length===3&&s.session[0]===1&&s.session[1]>120,'native Retry');
    await actor(4);await pages[4].evaluate(()=>lobby.send(JSON.stringify({type:'spectate'})));await pages[4].waitForFunction(()=>roomMessage.type==='spectator-start');await connect(4);
    const retryViewer=await waitFor(pages[4],s=>s.viewer[8]===third.begins[2].frame&&s.viewer[1]>third.begins[2].frame+30,'late viewer after Retry');
    assert.deepEqual(retryViewer.selection.slice(7,15),third.selection.slice(7,15));
    stage('late-retry-initializer',{start:third.begins[2].frame,configuration:retryViewer.selection.slice(7,15)});
  }
  }assert.equal(report.errors.length,0);report.passed=true;
}catch(error){report.failure=error.stack;report.states=await Promise.all(pages.map(p=>state(p).catch(e=>({error:String(e)}))));for(let i=0;i<pages.length;i++)await pages[i].screenshot({path:resolve(out,`failure-${i}.png`)}).catch(()=>{});throw error;}
finally{report.relayLog=relayLog;writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2));await browser.close();host.netplay.close();await new Promise(r=>host.server.close(r));relay.kill();console.log(JSON.stringify({passed:report.passed,output:out}));}
