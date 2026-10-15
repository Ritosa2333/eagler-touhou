// Actual React directory, room preparation, players and late hosted spectator.
// Native state is read for assertions; all playback actions use visible controls.
import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {chromium} from 'playwright';
import {browserTestUrl} from '../support/browser-target.mjs';
const origin=new URL(browserTestUrl()).origin;
const label=process.env.RUN_LABEL;if(!/^[\w-]+$/.test(label||''))throw Error('Unique RUN_LABEL required');
const out=resolve(process.env.TH09_SPECTATOR_TEST_OUTPUT||'D:/workspace/eagler/worktrees/th09-managed-touch-4x/th09_web/artifacts/spectator-panel-20261014',label);
if(existsSync(out))throw Error('Test output already exists');mkdirSync(out,{recursive:true});
const report={schema:'th09/spectator-panel/1',passed:false,scope:'Actual local React room flow and release Runtime. Not physical-device or public-network performance.',stages:[],errors:[],checks:{},browsers:[]};
const browsers=[],actors=[];
function stage(name,details={}){report.stages.push({name,at:new Date().toISOString(),...details});console.log(JSON.stringify({stage:name,...details}));}
async function actor(role){const browser=await chromium.launch({headless:true,executablePath:process.env.TH09_TEST_CHROME||'C:/Users/Ritosa/AppData/Local/ms-playwright/chromium-1243/chrome-win64/chrome.exe',args:['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required']});browsers.push(browser);report.browsers.push(await browser.version());const page=await browser.newPage({viewport:{width:1280,height:900}});page.setDefaultTimeout(30000);
  const row={role,page,lobbySockets:[]};actors.push(row);page.on('pageerror',e=>report.errors.push({role,error:e.message}));
  const delayMs = Number(process.env.TH09_TEST_DELAY_OGG_MS || 0);
  if (role === 'guest' && delayMs > 0) {
    let delayed = false;
    await page.route(/\/games\/th09\/music\/ogg\/th09_01\.ogg(?:\?|$)/, async route => {
      if (!delayed) {
        delayed = true;
        stage('guest-ogg-delayed', {delayMs});
        await new Promise(resolve => setTimeout(resolve, delayMs));
        const host = actors.find(actor => actor.role === 'host');
        report.checks.slowOggBarrier = await runtime(host);
        assert.equal(report.checks.slowOggBarrier?.active ?? false, false, 'loading peer cannot start simulation');
      }
      await route.continue();
    });
  }
  await page.addInitScript(name=>localStorage.setItem('eagler-touhou-mp-display-name-v1',name),
    ({host:'甲测试昵称',guest:'乙测试昵称',viewer:'丙测试观众','current-viewer':'丁测试观众'})[role] || '外部测试用户');
  await page.addInitScript(()=>{
    window.__quickChatPlayed=[];const audioPlay=Audio.prototype.play;
    Audio.prototype.play=function(...args){if(this.src.includes('/quick-chat/'))window.__quickChatPlayed.push(this.src);return audioPlay.apply(this,args);};
    const rows=[];window.__th09FrontendDebug=rows;let hooked;
    setInterval(()=>{const c=window.Module;if(!c?._th09_network_info)return;
      if(c!==hooked){hooked=c;const pause=c._th09_loop_pause;c._th09_loop_pause=function(on){rows.push({pause:on,at:performance.now()});return pause.call(this,on);};}
      const words=(fn,n)=>{const at=fn()/4;return Array.from(c.HEAPU32.subarray(at,at+n));};
      if(rows.length<1000)rows.push({at:performance.now(),hidden:document.hidden,network:words(c._th09_network_info,6),rollback:words(c._th09_rollback_info,8),active:window.__th09Runtime?.netplay?.active});
    },250);
  });
  page.on('websocket',s=>{if(new URL(s.url()).searchParams.has('lobby')){const socket={closed:false};row.lobbySockets.push(socket);s.on('close',()=>socket.closed=true);}});
  await page.goto(origin+'/lobby.html?game=th09mp',{waitUntil:'networkidle'});if(await page.locator('#firstUseNoticeClose').isVisible())await page.locator('#firstUseNoticeClose').click();return row;}
async function enter(row,mode,code){
  const p=row.page;
  if(mode==='list'){
    const room=p.locator(`.lobby-room-row[data-key="th09mp-${code}"]`);await room.waitFor();
    assert.equal(await room.locator('.lobby-room-state').getAttribute('data-state'),'playing');
    const button=room.locator('.lobby-join');assert.equal(await button.isEnabled(),true);
    assert.match(await button.innerText(),/Spectate|加入旁观/);
    assert.deepEqual(await room.locator('.th09-duel-avatar').allTextContents(),['甲','乙']);
    assert.deepEqual(await room.locator('.th09-duel-name').allTextContents(),['P1','P2']);
    report.checks.originalNicknameAvatars=true;
    await p.screenshot({path:resolve(out,row.role+'-directory-entry.png')});await button.click();
    await p.locator('#mpRoomCode').waitFor();assert.equal((await p.locator('#mpRoomCode').innerText()).trim(),code);
    report.checks.playingListEntries=(report.checks.playingListEntries||0)+1;
    stage('playing-directory-row-entered',{role:row.role,code});return code;
  }
  await p.locator(mode==='create'?'#createButton':'#codeButton').click();await p.locator('#roomDialog[open]').waitFor();
  await p.locator('#gameSelect').selectOption('th09mp');if(code)await p.locator('#roomCodeInput').fill(code);
  await p.locator('#roomForm button[type=submit]').click();await p.locator('#mpRoomCode').waitFor();return (await p.locator('#mpRoomCode').innerText()).trim();
}
async function ready(row,side){const p=row.page;await p.locator('#mpRoomConnection').waitFor();if(await p.locator(`[data-mp-seat-me="${side}"]`).evaluate(n=>n.hidden))await p.locator(`[data-mp-seat-drop="${side}"] button`).click();await p.locator('#mpReady').click();await p.waitForFunction(()=>document.querySelector('#mpReady')?.getAttribute('aria-pressed')==='true',{},{timeout:120000});}
async function runtime(row){return row.page.frameLocator('#gameFrame').locator('body').evaluate(()=>{const r=window.__th09Runtime;if(!r?.core||!r.netplay?.prepared)return null;const c=r.core;const at=c._th09_rollback_info()/4;const player=Array.from(c.HEAPU32.subarray(at,at+8));const pointer=c._th09_spectator_playback_info()/4;return {player,viewer:Array.from(c.HEAPU32.subarray(pointer,pointer+11)),spectator:r.netplay.spectator,active:r.netplay.active,run:Number(new URL(r.netplay.options.netplayUrl).searchParams.get('run')),pendingMatches:r.netplay.spectatorTape?.begins||0,wireFrame:r.netplay.spectatorTape?.frame||0,title:r.status().title,session:r.status().session};});}
async function waitRuntime(row,test,label,timeout=120000){const until=Date.now()+timeout;while(Date.now()<until){try{const s=await runtime(row);if(s&&test(s))return s;}catch{}await row.page.waitForTimeout(100);}throw Error('Runtime wait failed: '+label+'; '+(await row.page.locator('body').innerText()).slice(-1200));}
try{
  const host=await actor('host');const code=await enter(host,'create');stage('created-room',{code});
  const guest=await actor('guest');await enter(guest,'join',code);stage('joined-room');
  for(const row of [host,guest]){
    await row.page.locator('[data-mp-seat-glyph="0"]').filter({hasText:'甲'}).waitFor();
    await row.page.locator('[data-mp-seat-glyph="1"]').filter({hasText:'乙'}).waitFor();
  }
  assert.equal(await host.page.locator('#mpRoomView .mp-quick-chat').count(),0);
  assert.equal(await host.page.locator('.mp-quick-chat-prompt').isVisible(),false);
  assert.equal(await host.page.locator('#mpRoomChat').count(),0);
  report.checks.noRoomQuickChat=true;
  await ready(host,0);stage('host-ready');await ready(guest,1);stage('guest-ready');
  await host.page.locator('#mpStartGame').click();
  await waitRuntime(host,s=>s.active&&s.player[1]>=180,'host running');await waitRuntime(guest,s=>s.active&&s.player[1]>=180,'guest running');
  stage('players-running');
  for(const row of [host,guest]){await row.page.frameLocator('#gameFrame').locator('#canvas').focus();await row.page.keyboard.down('z');}
  const viewer=await actor('viewer');await enter(viewer,'list',code);
  const first=await waitRuntime(viewer,s=>s.spectator&&s.viewer[2]>s.viewer[1]+100,'late viewer');
  stage('late-viewer-running',{state:first.viewer});report.checks.lateJoin=true;
  const frame=viewer.page.frameLocator('#gameFrame');
  assert.equal(await frame.locator('#spectatorDetails').isVisible(),false);
  await frame.locator('#spectatorToggle').click();assert.equal(await frame.locator('#spectatorDetails').isVisible(),true);
  await frame.locator('#spectatorScore').filter({hasText:'<0 : 0>'}).waitFor();
  assert.deepEqual(await frame.locator('.spectator-score-player').allTextContents(),['P1(甲)','P2(乙)']);
  report.checks.playbackNicknameInitials=true;
  assert.equal(await viewer.page.locator('.th09-match-hud').count(),0);
  assert.equal(await frame.locator('#spectatorClock').count(),0);
  assert.match(await frame.locator('#spectatorMatchDuration').innerText(),/已进行 \d+ 分钟/);
  await frame.locator('body').evaluate(async () => {await document.fonts.ready;});
  assert.match(await frame.locator('#spectatorTimeline').evaluate(node=>getComputedStyle(node).fontFamily),/ET Yatra/);
  assert.equal(await frame.locator('#spectatorUiFonts').count(),1);
  assert.equal(await frame.locator('#spectatorNextRound').count(),0);
  await frame.locator('body').evaluate(async()=>{
    await Promise.all([...document.querySelectorAll('#spectatorTimeline img')].map(img=>img.decode()));
  });
  assert.ok(await frame.locator('#spectatorTimeline img').evaluateAll(nodes=>nodes.every(img=>img.complete&&img.naturalWidth>0)));
  assert.ok(await viewer.page.locator('.th09-chat-icon').evaluate(img=>img.complete&&img.naturalWidth>0));
  report.checks.playbackAndChatIcons=true;
  report.checks.reusesLauncherFonts=true;
  report.checks.scoreReplacesPlaybackHeader=true;
  const voiceCounts=await Promise.all([host,guest].map(row=>row.page.evaluate(()=>window.__quickChatPlayed.length)));
  await viewer.page.locator('.mp-quick-chat-prompt').click();await viewer.page.locator('[data-phrase="thanks"]').click();
  for(const row of [host,guest])await row.page.waitForFunction(()=>document.querySelector('.mp-quick-chat-log')?.innerText.includes('观众'));
  assert.deepEqual(await Promise.all([host,guest].map(row=>row.page.evaluate(()=>window.__quickChatPlayed.length))),voiceCounts);
  report.checks.spectatorTextWithoutPlayerVoice=true;
  const guestVoiceBefore=await guest.page.evaluate(()=>window.__quickChatPlayed.length);
  await host.page.locator('.mp-quick-chat-prompt').click();await host.page.locator('[data-phrase="1"]').click();
  await guest.page.waitForFunction(count=>window.__quickChatPlayed.length>count,guestVoiceBefore);
  report.checks.playerVoicePreserved=true;
  await frame.locator('#spectatorPause').click();const paused=await waitRuntime(viewer,s=>s.viewer[5]===1,'pause');
  const arriving=await waitRuntime(viewer,s=>s.viewer[2]>=paused.viewer[2]+40,'receive while paused');assert.equal(arriving.viewer[1],paused.viewer[1]);report.checks.pauseKeepsReceiving=true;
  await frame.locator('body').evaluate(()=>{const c=window.__th09Runtime.core,prior=c.onGameFrame;window.__spectatorTestFrames=[];c.onGameFrame=function(...args){const at=c._th09_spectator_playback_info()/4;const s=Array.from(c.HEAPU32.subarray(at,at+11));if(window.__spectatorTestFrames.length<5000)window.__spectatorTestFrames.push(s);return prior?.apply(this,args);};});
  await frame.locator('#spectatorSeek').focus();await frame.locator('#spectatorSeek').press('End');
  const seeking=await waitRuntime(viewer,s=>s.viewer[6]===1,'manual fast forward');const target=seeking.viewer[4];
  await waitRuntime(viewer,s=>s.viewer[1]>=target&&!s.viewer[6],'stop at selected target');
  const trace=await frame.locator('body').evaluate(()=>window.__spectatorTestFrames);assert.ok(trace.some(s=>s[1]===target),'exact target was reached without skipped input');report.checks.manualTargetAndStop=true;
  const beforeBack=await runtime(viewer);await frame.locator('#spectatorSeek').press('Home');
  const afterBack=await waitRuntime(viewer,s=>s.viewer[1]<beforeBack.viewer[1],'rewind retained match history');
  assert.equal(afterBack.active,true);report.checks.backwardSeek=true;
  await frame.locator('#spectatorLive').click();const live=await waitRuntime(viewer,s=>s.viewer[7]===1&&!s.viewer[6]&&s.viewer[2]-s.viewer[1]>60&&s.viewer[2]-s.viewer[1]<=110,'delayed live');report.checks.liveDelay=live.viewer[2]-live.viewer[1];
  async function unobstructedTimeline() {
    const timeline = await frame.locator('#spectatorTimeline').boundingBox();
    const canvas = await frame.locator('#canvas').boundingBox();
    const viewport = await viewer.page.locator('#gameFrame').boundingBox();
    assert.ok(timeline && canvas && viewport && Math.abs(canvas.y + canvas.height / 2 - viewport.y - viewport.height / 2) < 2,
      'playback controls must leave the canvas centered');
    const chat=await viewer.page.locator('.mp-quick-chat-prompt').boundingBox();
    assert.ok(chat&&chat.x<=20&&chat.y+chat.height<=timeline.y,'bottom-left chat must sit above the playback panel');
    const picker=viewer.page.locator('.mp-quick-chat-picker:not([hidden])');
    if(await picker.count()){const box=await picker.boundingBox();assert.ok(box.y+box.height<=chat.y,'picker opens upward');}
    const diagnostic=viewer.page.locator('#runtimeDiagnostics');
    if(await diagnostic.isVisible()){
      const box=await diagnostic.boundingBox();
      assert.ok(box&&box.y+box.height<=timeline.y,'diagnostics must also clear the playback panel');
    }
  }
  await unobstructedTimeline();await viewer.page.screenshot({path:resolve(out,'timeline-desktop.png')});
  await frame.locator('#spectatorTimeline').screenshot({path:resolve(out,'playback-panel-desktop.png')});
  stage('playback-controls-pass',{target,delay:report.checks.liveDelay});
  await viewer.page.setViewportSize({width:390,height:844});await unobstructedTimeline();await viewer.page.screenshot({path:resolve(out,'timeline-mobile-layout.png')});
  await frame.locator('#spectatorTimeline').screenshot({path:resolve(out,'playback-panel-mobile.png')});await viewer.page.setViewportSize({width:1280,height:900});
  report.checks.centeredCanvasAndUnobstructedChat = true;
  await frame.locator('#spectatorToggle').click();await unobstructedTimeline();
  await viewer.page.screenshot({path:resolve(out,'timeline-collapsed.png')});
  await frame.locator('#spectatorToggle').click();report.checks.collapseAndExpand=true;
  await frame.locator('#spectatorPause').click();
  let held=await waitRuntime(viewer,s=>s.viewer[5]===1,'hold old match');
  await host.page.frameLocator('#gameFrame').locator('body').evaluate(()=>{
    const c=window.__th09Runtime.core,prior=c.onNetworkSpectatorEnd;
    c.onNetworkSpectatorEnd=function(frame,winner,...args){const at=c._th09_confirmed_frame_hash(frame-1)/4;
      window.__oldEndHash={frame,winner,record:c.HEAPU32[at],hash:c.HEAPU32[at+1]};return prior?.call(this,frame,winner,...args);};
    const round=c.onNetworkSpectatorRound;window.__roundHashes={};
    c.onNetworkSpectatorRound=function(frame,index){const at=c._th09_confirmed_frame_hash(frame-1)/4;
      window.__roundHashes[frame]=c.HEAPU32[at+1];return round?.call(this,frame,index);};
  });
  const witness=await actor('current-viewer');await enter(witness,'list',code);
  await waitRuntime(witness,s=>s.spectator&&s.viewer[2]>s.viewer[1]+90,'independent live viewer');
  await witness.page.frameLocator('#gameFrame').locator('#spectatorToggle').click();
  await witness.page.frameLocator('#gameFrame').locator('#spectatorLive').click();
  let boundary=false,lastProgress=0,completed,roundSeeks=0;const until=Date.now()+360000;
  while(Date.now()<until){
    const h=await runtime(host),g=await runtime(guest),old=await runtime(viewer);let current;try{current=await runtime(witness);}catch{}
    assert.equal(old.viewer[1],held.viewer[1]);assert.equal(old.viewer[5],1);
    assert.equal(old.viewer[8],held.viewer[8],'player round changes cannot move a paused viewer');
    if(current?.viewer[8]>0){boundary=true;report.checks.roundBoundary={start:current.viewer[8],round:current.viewer[9]};}
      const marker=frame.locator('.spectator-round-mark[data-next="true"]:not([hidden]):not(:disabled)');
    if(roundSeeks<2&&await marker.count()){
      const point=Number(await marker.first().getAttribute('data-frame'));
      assert.match(await marker.first().innerText(),/^\d+$/,'round ticks have no ambiguous next-round label');
      await frame.locator('body').evaluate((node,target)=>{const c=window.__th09Runtime.core,prior=c.onGameFrame;window.__pauseAtRound=target;
        c.onGameFrame=function(...args){const at=c._th09_spectator_playback_info()/4;
          if(c.HEAPU32[at+1]===window.__pauseAtRound){window.__pauseAtRound=-1;document.querySelector('#spectatorPause').click();}
          return prior?.apply(this,args);};},point);
      const markBox=await marker.first().boundingBox();await viewer.page.mouse.click(markBox.x+markBox.width/2,markBox.y+markBox.height/2);
      held=await waitRuntime(viewer,s=>s.viewer[1]===point&&s.viewer[5]===1,'next round stops at actual reset');
      assert.equal(held.viewer[8],point,'annotation is the real reset, not the previous round victory');
      const actual=await frame.locator('body').evaluate(()=>window.__th09Runtime.core._th09_network_hash()>>>0);
      const expected=await host.page.frameLocator('#gameFrame').locator('body').evaluate((node,f)=>window.__roundHashes[f],point);
      assert.equal(actual,expected);roundSeeks++;
      await viewer.page.screenshot({path:resolve(out,`round-marker-${roundSeeks}.png`)});
      await frame.locator('#spectatorTimeline').screenshot({path:resolve(out,`playback-marker-${roundSeeks}.png`)});
      report.checks.roundMarkers={seeks:roundSeeks,frame:point,hash:actual};
    }
    if(h?.session[0]===3&&g?.session[0]===3){completed=h;break;}
    if(Date.now()-lastProgress>20000){lastProgress=Date.now();stage('natural-match-progress',{held:old.viewer[1],current:current?.viewer,player:h?.player});}
    await host.page.waitForTimeout(500);
  }
  assert.ok(boundary);assert.ok(completed);assert.ok(roundSeeks>=1,'at least one real next-round annotation was exercised');report.checks.pausedAcrossRounds=true;
  const ended=await waitRuntime(viewer,s=>s.viewer[10]===1,'old confirmed end arrived while paused');
  const endHash=await host.page.frameLocator('#gameFrame').locator('body').evaluate(()=>window.__oldEndHash);
  assert.equal(endHash.record,endHash.frame-1);assert.equal(ended.viewer[2],endHash.frame);
  await waitRuntime(witness,s=>s.viewer[10]===1&&s.viewer[1]>=s.viewer[2],'viewer waits at completed match');
  assert.equal(await witness.page.locator('#player').getAttribute('aria-hidden'),'false');
  report.checks.waitsInPlayerAtMatchEnd=true;
  const expectedStats=[0,1].map(index=>({wins:index===endHash.winner?1:0,losses:index===endHash.winner?0:1,minutes:Math.floor(endHash.frame/3600)}));
  assert.equal(await witness.page.locator('.mp-seat-match-stats').count(),0);
  report.checks.noRoomSeatStatistics=true;
  const outside=await browsers[3].newPage({viewport:{width:1280,height:900}});
  await outside.goto(origin+'/lobby.html?game=th09mp',{waitUntil:'networkidle'});
  if(await outside.locator('#firstUseNoticeClose').isVisible())await outside.locator('#firstUseNoticeClose').click();
  const publicRow=outside.locator(`.lobby-room-row[data-key="th09mp-${code}"]`);
  await publicRow.waitFor();
  const publicStats={score:await publicRow.locator('.th09-room-score').innerText(),duration:await publicRow.locator('.th09-room-duration').innerText()};
  assert.equal(publicStats.score,`${expectedStats[0].wins} : ${expectedStats[1].wins}`);
  assert.match(publicStats.duration,new RegExp(`已进行 ${expectedStats[0].minutes} 分钟`));
  report.checks.publicStats=publicStats;await publicRow.screenshot({path:resolve(out,'directory-card-desktop.png')});
  await outside.setViewportSize({width:390,height:844});
  const mobileBox=await publicRow.boundingBox();
  assert.ok(mobileBox.width<=390&&mobileBox.height<=180,'mobile card stays compact');
  const cardScore=await publicRow.locator('.th09-room-score').boundingBox();
  const cardNames=await publicRow.locator('.th09-duel-name').evaluateAll(nodes=>nodes.map(n=>{const b=n.getBoundingClientRect();return {left:b.left,right:b.right};}));
  assert.ok(cardNames[0].right<cardScore.x&&cardNames[1].left>cardScore.x+cardScore.width,'names cannot overlap the score');
  const count=await publicRow.locator('.th09-room-spectators').boundingBox();
  assert.ok(count.x>mobileBox.x+mobileBox.width/2&&count.y>mobileBox.y+mobileBox.height/2,'spectator count at bottom-right');
  await publicRow.screenshot({path:resolve(out,'directory-card-mobile.png')});report.checks.mobileCardLayout=true;
  await outside.close();stage('whole-match-statistics-pass',{winner:endHash.winner,stats:publicStats});
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
  const clockPage=await browsers[3].newPage();await clockPage.goto(origin+'/lobby.html?game=th09mp',{waitUntil:'networkidle'});
  if(await clockPage.locator('#firstUseNoticeClose').isVisible())await clockPage.locator('#firstUseNoticeClose').click();
  const clockRow=clockPage.locator(`.lobby-room-row[data-key="th09mp-${code}"]`);
  await clockRow.waitFor();
  const resetClock={score:await clockRow.locator('.th09-room-score').innerText(),duration:await clockRow.locator('.th09-room-duration').innerText()};
  assert.equal(resetClock.score,publicStats.score);assert.equal(resetClock.duration,'已进行 0 分钟');
  report.checks.currentMatchClockResets=resetClock;
  await clockPage.screenshot({path:resolve(out,'new-match-clock-reset.png')});await clockPage.close();
  await viewer.page.screenshot({path:resolve(out,'old-match-retained.png')});stage('old-match-retained',{start,oldEnd:endHash.frame,held:held.viewer[1]});
  // An unpaused observer advances to the next match in the same player.
  const late=witness;
  await waitRuntime(late,s=>s.spectator&&s.viewer[8]===start,'automatic next match in same player');
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
  const afterRetry=await browsers[3].newPage();await afterRetry.goto(origin+'/lobby.html?game=th09mp',{waitUntil:'networkidle'});
  const retryRow=afterRetry.locator(`.lobby-room-row[data-key="th09mp-${code}"]`);
  await retryRow.waitFor();
  const retryStats={score:await retryRow.locator('.th09-room-score').innerText(),duration:await retryRow.locator('.th09-room-duration').innerText()};
  assert.equal(retryStats.score,publicStats.score);
  report.checks.scoreSurvivesRetry=retryStats;await afterRetry.close();
  // Keep both viewers paused in match two until match three exists, then
  // verify the real live button bypasses the remaining match-two input.
  await waitRuntime(host,s=>s.session[0]===3,'second whole match completes',360000);
  for(const row of [host,guest]){await row.page.frameLocator('#gameFrame').locator('#canvas').focus();await row.page.keyboard.up('z');}
  await host.page.waitForTimeout(100);for(const row of [host,guest])await row.page.keyboard.down('z');
  await waitRuntime(viewer,s=>s.pendingMatches>=1&&s.wireFrame>s.viewer[2]+100,'third match buffered');
  const latestStart=await host.page.frameLocator('#gameFrame').locator('body').evaluate(()=>window.__newMatchStart);
  const beforeJump=await runtime(viewer);
  await frame.locator('#spectatorLive').click();
  const jumped=await waitRuntime(viewer,s=>s.viewer[8]===latestStart&&s.viewer[7]===1&&s.viewer[5]===0,'live jumps to newest match');
  assert.ok(jumped.viewer[1]>=latestStart&&latestStart>beforeJump.viewer[2]);
  const recovered=await waitRuntime(viewer,s=>!s.viewer[6]&&s.viewer[2]-s.viewer[1]>60&&s.viewer[2]-s.viewer[1]<=110,'live delay restored after jump');
  report.checks.skipOldMatchToLive={from:beforeJump.viewer[1],latestStart,delay:recovered.viewer[2]-recovered.viewer[1]};
  stage('live-jump-pass',report.checks.skipOldMatchToLive);
  assert.equal(report.errors.length,0);report.passed=true;
}catch(error){report.error=error.stack;for(const row of actors){try{await row.page.screenshot({path:resolve(out,row.role+'-failure.png')});report[row.role+'Text']=(await row.page.locator('body').innerText()).slice(-1600);}catch{}}throw error;}
finally{writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n',{flag:'wx'});for(const browser of browsers)await browser.close();console.log(JSON.stringify({passed:report.passed,output:out,checks:report.checks}));}
