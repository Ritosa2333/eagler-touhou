// Actual React directory, room preparation, players and late hosted spectator.
// Native state is read for assertions; all playback actions use visible controls.
import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {chromium} from 'playwright';
import {browserTestUrl} from '../support/browser-target.mjs';
const origin=new URL(browserTestUrl()).origin;
const label=process.env.RUN_LABEL;if(!/^[\w-]+$/.test(label||''))throw Error('Unique RUN_LABEL required');
const out=resolve(process.env.TH09_SPECTATOR_TEST_OUTPUT||'D:/workspace/eagler/worktrees/th09-managed-touch-4x/th09_web/artifacts/spectator-continuity-20261014',label);
if(existsSync(out))throw Error('Test output already exists');mkdirSync(out,{recursive:true});
const report={schema:'th09/spectator-continuity/1',passed:false,scope:'Actual local React room flow and release Runtime. Not physical-device or public-network performance.',stages:[],errors:[],checks:{},browsers:[]};
const browsers=[],actors=[];
function stage(name,details={}){report.stages.push({name,at:new Date().toISOString(),...details});console.log(JSON.stringify({stage:name,...details}));}
async function actor(role){const browser=await chromium.launch({headless:true,executablePath:process.env.TH09_TEST_CHROME||'C:/Users/Ritosa/AppData/Local/ms-playwright/chromium-1243/chrome-win64/chrome.exe',args:['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required']});browsers.push(browser);report.browsers.push(await browser.version());const page=await browser.newPage({viewport:{width:1280,height:900}});page.setDefaultTimeout(30000);
  const row={role,page,lobbySockets:[]};actors.push(row);page.on('pageerror',e=>report.errors.push({role,error:e.message}));
  await page.addInitScript(()=>{
    const rows=[];window.__th09FrontendDebug=rows;let hooked;
    setInterval(()=>{const c=window.Module;if(!c?._th09_network_info)return;
      if(c!==hooked){hooked=c;const pause=c._th09_loop_pause;c._th09_loop_pause=function(on){rows.push({pause:on,at:performance.now()});return pause.call(this,on);};}
      const words=(fn,n)=>{const at=fn()/4;return Array.from(c.HEAPU32.subarray(at,at+n));};
      if(rows.length<1000)rows.push({at:performance.now(),hidden:document.hidden,network:words(c._th09_network_info,6),rollback:words(c._th09_rollback_info,8),active:window.__th09Runtime?.netplay?.active});
    },250);
  });
  page.on('websocket',s=>{if(new URL(s.url()).searchParams.has('lobby')){const socket={closed:false};row.lobbySockets.push(socket);s.on('close',()=>socket.closed=true);}});
  await page.goto(origin+'/lobby.html?game=th09mp',{waitUntil:'networkidle'});if(await page.locator('#firstUseNoticeClose').isVisible())await page.locator('#firstUseNoticeClose').click();return row;}
async function enter(row,mode,code){const p=row.page;await p.locator(mode==='create'?'#createButton':'#codeButton').click();await p.locator('#roomDialog[open]').waitFor();await p.locator('#gameSelect').selectOption('th09mp');if(code)await p.locator('#roomCodeInput').fill(code);await p.locator('#roomForm button[type=submit]').click();await p.locator('#mpRoomCode').waitFor();return (await p.locator('#mpRoomCode').innerText()).trim();}
async function ready(row,side){const p=row.page;await p.locator('#mpRoomConnection').waitFor();if(await p.locator(`[data-mp-seat-me="${side}"]`).evaluate(n=>n.hidden))await p.locator(`[data-mp-seat-drop="${side}"] button`).click();await p.locator('#mpReady').click();await p.waitForFunction(()=>document.querySelector('#mpReady')?.getAttribute('aria-pressed')==='true',{},{timeout:120000});}
async function runtime(row){return row.page.frameLocator('#gameFrame').locator('body').evaluate(()=>{const r=window.__th09Runtime;if(!r?.core||!r.netplay?.prepared)return null;const c=r.core;const at=c._th09_rollback_info()/4;const player=Array.from(c.HEAPU32.subarray(at,at+8));const pointer=c._th09_spectator_playback_info()/4;return {player,viewer:Array.from(c.HEAPU32.subarray(pointer,pointer+11)),spectator:r.netplay.spectator,active:r.netplay.active,run:Number(new URL(r.netplay.options.netplayUrl).searchParams.get('run')),pendingMatches:r.netplay.spectatorTape?.begins||0,wireFrame:r.netplay.spectatorTape?.frame||0,title:r.status().title,session:r.status().session};});}
async function waitRuntime(row,test,label,timeout=120000){const until=Date.now()+timeout;while(Date.now()<until){try{const s=await runtime(row);if(s&&test(s))return s;}catch{}await row.page.waitForTimeout(100);}throw Error('Runtime wait failed: '+label+'; '+(await row.page.locator('body').innerText()).slice(-1200));}
try{
  const host=await actor('host');const code=await enter(host,'create');stage('created-room',{code});
  const guest=await actor('guest');await enter(guest,'join',code);stage('joined-room');
  await ready(host,0);stage('host-ready');await ready(guest,1);stage('guest-ready');
  await host.page.locator('#mpStartGame').click();
  await waitRuntime(host,s=>s.active&&s.player[1]>=180,'host running');await waitRuntime(guest,s=>s.active&&s.player[1]>=180,'guest running');
  stage('players-running');
  for(const row of [host,guest]){await row.page.frameLocator('#gameFrame').locator('#canvas').focus();await row.page.keyboard.down('z');}
  const viewer=await actor('viewer');await enter(viewer,'join',code);await viewer.page.locator('#mpSpectatorToggle').click();await viewer.page.locator('#mpSpectatorJoin').click();
  const first=await waitRuntime(viewer,s=>s.spectator&&s.viewer[2]>s.viewer[1]+100,'late viewer');
  stage('late-viewer-running',{state:first.viewer});report.checks.lateJoin=true;
  const frame=viewer.page.frameLocator('#gameFrame');
  await frame.locator('#spectatorPause').click();const paused=await waitRuntime(viewer,s=>s.viewer[5]===1,'pause');
  const arriving=await waitRuntime(viewer,s=>s.viewer[2]>=paused.viewer[2]+40,'receive while paused');assert.equal(arriving.viewer[1],paused.viewer[1]);report.checks.pauseKeepsReceiving=true;
  await frame.locator('body').evaluate(()=>{const c=window.__th09Runtime.core,prior=c.onGameFrame;window.__spectatorTestFrames=[];c.onGameFrame=function(...args){const at=c._th09_spectator_playback_info()/4;const s=Array.from(c.HEAPU32.subarray(at,at+11));if(window.__spectatorTestFrames.length<5000)window.__spectatorTestFrames.push(s);return prior?.apply(this,args);};});
  await frame.locator('#spectatorSeek').focus();await frame.locator('#spectatorSeek').press('End');
  const seeking=await waitRuntime(viewer,s=>s.viewer[6]===1,'manual fast forward');const target=seeking.viewer[4];
  await waitRuntime(viewer,s=>s.viewer[1]>=target&&!s.viewer[6],'stop at selected target');
  const trace=await frame.locator('body').evaluate(()=>window.__spectatorTestFrames);assert.ok(trace.some(s=>s[1]===target),'exact target was reached without skipped input');report.checks.manualTargetAndStop=true;
  const beforeBack=await runtime(viewer);await frame.locator('#spectatorSeek').press('Home');const afterBack=await runtime(viewer);assert.ok(afterBack.viewer[1]>=beforeBack.viewer[1]);report.checks.noBackwardSeek=true;
  await frame.locator('#spectatorLive').click();const live=await waitRuntime(viewer,s=>s.viewer[7]===1&&!s.viewer[6]&&s.viewer[2]-s.viewer[1]>60&&s.viewer[2]-s.viewer[1]<=110,'delayed live');report.checks.liveDelay=live.viewer[2]-live.viewer[1];
  async function unobstructedTimeline() {
    const timeline = await frame.locator('#spectatorTimeline').boundingBox();
    const diagnostics = await viewer.page.locator('#runtimeDiagnostics').boundingBox();
    assert.ok(timeline && diagnostics && diagnostics.y + diagnostics.height <= timeline.y,
      'diagnostics must stay above the playback controls');
  }
  await unobstructedTimeline();await viewer.page.screenshot({path:resolve(out,'timeline-desktop.png')});stage('playback-controls-pass',{target,delay:report.checks.liveDelay});
  await viewer.page.setViewportSize({width:390,height:844});await unobstructedTimeline();await viewer.page.screenshot({path:resolve(out,'timeline-mobile-layout.png')});await viewer.page.setViewportSize({width:1280,height:900});
  report.checks.unobstructedTimeline = true;
  await frame.locator('#spectatorPause').click();
  const held=await waitRuntime(viewer,s=>s.viewer[5]===1,'hold old match');
  await host.page.frameLocator('#gameFrame').locator('body').evaluate(()=>{
    const c=window.__th09Runtime.core,prior=c.onNetworkSpectatorEnd;
    c.onNetworkSpectatorEnd=function(frame,...args){const at=c._th09_confirmed_frame_hash(frame-1)/4;
      window.__oldEndHash={frame,record:c.HEAPU32[at],hash:c.HEAPU32[at+1]};return prior?.call(this,frame,...args);};
  });
  const witness=await actor('current-viewer');await enter(witness,'join',code);
  await witness.page.locator('#mpSpectatorToggle').click();await witness.page.locator('#mpSpectatorJoin').click();
  await waitRuntime(witness,s=>s.spectator&&s.viewer[2]>s.viewer[1]+90,'independent live viewer');
  await witness.page.frameLocator('#gameFrame').locator('#spectatorLive').click();
  let boundary=false,lastProgress=0,completed;const until=Date.now()+360000;
  while(Date.now()<until){
    const h=await runtime(host),g=await runtime(guest),old=await runtime(viewer);let current;try{current=await runtime(witness);}catch{}
    assert.equal(old.viewer[1],held.viewer[1]);assert.equal(old.viewer[5],1);
    assert.equal(old.viewer[8],held.viewer[8],'player round changes cannot move a paused viewer');
    if(current?.viewer[8]>0){boundary=true;report.checks.roundBoundary={start:current.viewer[8],round:current.viewer[9]};}
    if(h?.session[0]===3&&g?.session[0]===3){completed=h;break;}
    if(Date.now()-lastProgress>20000){lastProgress=Date.now();stage('natural-match-progress',{held:old.viewer[1],current:current?.viewer,player:h?.player});}
    await host.page.waitForTimeout(500);
  }
  assert.ok(boundary);assert.ok(completed);report.checks.pausedAcrossRounds=true;
  const ended=await waitRuntime(viewer,s=>s.viewer[10]===1,'old confirmed end arrived while paused');
  const endHash=await host.page.frameLocator('#gameFrame').locator('body').evaluate(()=>window.__oldEndHash);
  assert.equal(endHash.record,endHash.frame-1);assert.equal(ended.viewer[2],endHash.frame);
  await waitRuntime(witness,s=>s.viewer[10]===1&&s.viewer[1]>=s.viewer[2],'viewer waits at completed match');
  assert.equal(await witness.page.locator('#player').getAttribute('aria-hidden'),'false');
  report.checks.waitsInPlayerAtMatchEnd=true;
  await host.page.frameLocator('#gameFrame').locator('body').evaluate(()=>{
    const c=window.__th09Runtime.core,oldFrame=c.onNetworkSpectatorFrame,oldBegin=c.onNetworkSpectatorBegin;window.__matchHashes={};
    c.onNetworkSpectatorBegin=function(frame,...args){window.__newMatchStart=frame;return oldBegin?.call(this,frame,...args);};
    c.onNetworkSpectatorFrame=function(frame,...args){const at=c._th09_confirmed_frame_hash(frame)/4;
      if(c.HEAPU32[at]===frame)window.__matchHashes[frame+1]=c.HEAPU32[at+1];return oldFrame?.call(this,frame,...args);};
  });
  for(const row of [host,guest]){await row.page.frameLocator('#gameFrame').locator('#canvas').focus();await row.page.keyboard.up('z');}
  await host.page.waitForTimeout(100);for(const row of [host,guest])await row.page.keyboard.down('z');
  await waitRuntime(host,s=>s.run===1&&s.session[0]===1&&s.session[1]<500&&s.player[0]>completed.player[0],'same-connection Retry');
  const start=await host.page.frameLocator('#gameFrame').locator('body').evaluate(()=>window.__newMatchStart);
  const queued=await waitRuntime(viewer,s=>s.pendingMatches===1&&s.wireFrame>start+90,'new match queued behind paused old match');
  assert.equal(queued.viewer[1],held.viewer[1]);assert.equal(queued.viewer[2],endHash.frame);
  assert.equal(queued.viewer[5],1);assert.equal(queued.viewer[8],held.viewer[8]);
  assert.match(await frame.locator('#spectatorPlaybackStatus').innerText(),/1 局待播放/);
  assert.ok([host,guest,viewer,witness].every(r=>r.lobbySockets.length===1&&!r.lobbySockets[0].closed));
  report.checks.oldMatchPreservedAfterRetry={played:queued.viewer[1],oldEnd:endHash.frame,newStart:start};
  await viewer.page.screenshot({path:resolve(out,'old-match-retained.png')});stage('old-match-retained',{start,oldEnd:endHash.frame,held:held.viewer[1]});
  // The unpaused observer advances without leaving the player.
  const late=witness;
  await waitRuntime(late,s=>s.spectator&&s.viewer[8]===start,'automatic next match');
  assert.equal(await late.page.locator('#player').getAttribute('aria-hidden'),'false');
  report.checks.autoNextMatchWithoutRoom=true;
  await late.page.frameLocator('#gameFrame').locator('#spectatorPause').click();report.checks.latestMatchHistory=true;
  await frame.locator('body').evaluate((node,details)=>{
    const c=window.__th09Runtime.core,prior=c.onGameFrame;window.__oldTail=null;window.__pauseOnNext=true;
    c.onGameFrame=function(...args){const at=c._th09_spectator_playback_info()/4;
      if(c.HEAPU32[at+1]===details.end&&c.HEAPU32[at+10])window.__oldTail={frame:details.end,hash:c._th09_network_hash()>>>0};
      if(window.__pauseOnNext&&c.HEAPU32[at+8]===details.start){window.__pauseOnNext=false;
        if(!c.HEAPU32[at+5])document.querySelector('#spectatorPause').click();}
      return prior?.apply(this,args);};
  },{end:endHash.frame,start});
  await frame.locator('#spectatorSeek').press('End');
  await waitRuntime(viewer,s=>s.viewer[8]===start&&s.viewer[5]===1,'finish every old frame before next match',180000);
  const oldTail=await frame.locator('body').evaluate(()=>window.__oldTail);
  assert.deepEqual(oldTail,{frame:endHash.frame,hash:endHash.hash});report.checks.oldTailHash=oldTail;
  const target2=start+600;await waitRuntime(late,s=>s.viewer[3]>target2,'second-match target available');
  for(const row of [viewer,late]){
    const f=row.page.frameLocator('#gameFrame');if(!(await runtime(row)).viewer[5])await f.locator('#spectatorPause').click();
    await f.locator('body').evaluate((node,target)=>{const c=window.__th09Runtime.core,prior=c.onGameFrame;window.__pauseAt=target;
      c.onGameFrame=function(...args){const at=c._th09_spectator_playback_info()/4;if(c.HEAPU32[at+1]===window.__pauseAt){window.__pauseAt=-1;document.querySelector('#spectatorPause').click();}return prior?.apply(this,args);};},target2);
    await f.locator('#spectatorSeek').evaluate((node,value)=>{node.value=String(value);node.dispatchEvent(new Event('input',{bubbles:true}));node.dispatchEvent(new Event('change',{bubbles:true}));},target2);
    await waitRuntime(row,s=>s.viewer[1]===target2&&s.viewer[5]===1,'same-frame pause');
  }
  const hashes=[];for(const row of [viewer,late])hashes.push(await row.page.frameLocator('#gameFrame').locator('body').evaluate(()=>window.__th09Runtime.core._th09_network_hash()>>>0));
  const expected=await host.page.frameLocator('#gameFrame').locator('body').evaluate((node,target)=>window.__matchHashes[target],target2);
  report.checks.sameFrameHash={frame:target2,host:expected,viewers:hashes};assert.deepEqual(hashes,[expected,expected]);
  await viewer.page.screenshot({path:resolve(out,'second-match.png')});stage('continuity-and-state-pass',{oldTail,target2,hashes});
  assert.equal(report.errors.length,0);report.passed=true;
}catch(error){report.error=error.stack;for(const row of actors){try{await row.page.screenshot({path:resolve(out,row.role+'-failure.png')});report[row.role+'Text']=(await row.page.locator('body').innerText()).slice(-1600);}catch{}}throw error;}
finally{writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n',{flag:'wx'});for(const browser of browsers)await browser.close();console.log(JSON.stringify({passed:report.passed,output:out,checks:report.checks}));}
