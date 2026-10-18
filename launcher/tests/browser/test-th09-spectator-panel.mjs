// Actual React directory, room preparation, players and late hosted spectator.
// Native state is read for assertions; all playback actions use visible controls.
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createReadStream} from 'node:fs';
import {copyFileSync,existsSync,linkSync,mkdirSync,readFileSync,statSync,unlinkSync,writeFileSync,constants as fsConstants} from 'node:fs';
import {dirname,join,resolve} from 'node:path';
import {chromium} from 'playwright';
import {browserTestUrl} from '../support/browser-target.mjs';
const origin=new URL(browserTestUrl()).origin;
const label=process.env.RUN_LABEL;if(!/^[\w-]+$/.test(label||''))throw Error('Unique RUN_LABEL required');
const runtimeDir=process.env.TH09_SPECTATOR_RUNTIME_DIR;if(!runtimeDir)throw Error('TH09_SPECTATOR_RUNTIME_DIR must name the multiplayer Runtime under test');
const testDataPath=process.env.TH09_SPECTATOR_DATA_FILE;if(!testDataPath)throw Error('TH09_SPECTATOR_DATA_FILE must name the verified TH09 raw-data fixture');
const resolvedTestDataPath=resolve(testDataPath);
async function sha256File(path){const hash=createHash('sha256');for await(const chunk of createReadStream(path))hash.update(chunk);return hash.digest('hex');}
const testDataBytes=statSync(resolvedTestDataPath).size,testDataSha256=await sha256File(resolvedTestDataPath);
assert.equal(testDataBytes,88180006,'TH09 spectator fixture matches the current Site Manifest byte count');
assert.equal(testDataSha256,'73a344a2cb7b1113831058010758f03d6d20e7710d889a95f249046fb8fa5ea0','TH09 spectator fixture matches the current Site Manifest SHA-256');
const runtimeFiles=JSON.parse(readFileSync(join(runtimeDir,'runtime-files.json'),'utf8'));
const focusedMode=process.env.TH09_SPECTATOR_FOCUSED==='1';
const forceWsRelay=process.env.TH09_SPECTATOR_FORCE_WS_RELAY==='1';
const runtimeSha256=path=>createHash('sha256').update(readFileSync(join(runtimeDir,path))).digest('hex');
const runtimeIdentity={directory:resolve(runtimeDir),manifestSha256:runtimeSha256('runtime-files.json'),
  wasmSha256:runtimeFiles.files['th09.wasm']?.sha256,sharedNetplaySha256:runtimeFiles.files['shared-netplay.mjs']?.sha256,
  spectatorTimelineSha256:runtimeFiles.files['spectator-timeline.mjs']?.sha256,version:JSON.parse(readFileSync(join(runtimeDir,'version.json'),'utf8'))};
const out=resolve(process.env.TH09_SPECTATOR_TEST_OUTPUT||'D:/workspace/eagler/worktrees/th09-managed-touch-4x/web/artifacts/spectator-panel-20261014',label);
if(existsSync(out))throw Error('Test output already exists');mkdirSync(out,{recursive:true});
const importFixturePath=resolve(process.env.TH09_SPECTATOR_IMPORT_TEMP_DIR||'D:/workspace/eagler/.codex-tmp/spectator-panel-20261018/import-fixtures',label,'th09.dat');
let importFixtureCreated=false,importFixtureMethod='';
const report={schema:'th09/spectator-panel/1',passed:false,scope:focusedMode?'Focused actual React/native spectator lifecycle: initial late join, chat, long pauses, seek/pause/live and two toolbar exit/re-entry cycles. Does not cover whole-match progression or match-end statistics.':'Actual local React room flow and release Runtime. Not physical-device or public-network performance.',runtimeIdentity,testData:{path:resolvedTestDataPath,bytes:testDataBytes,sha256:testDataSha256,importFilename:'th09.dat'},transportFixture:{playerRoute:forceWsRelay?'forced-supported-websocket-relay':'automatic'},stages:[],errors:[],checks:{},browsers:[]};
const browsers=[],actors=[];
function stage(name,details={}){report.stages.push({name,at:new Date().toISOString(),...details});console.log(JSON.stringify({stage:name,...details}));}
async function actor(role){const browser=await chromium.launch({headless:true,executablePath:process.env.TH09_TEST_CHROME||'C:/Users/Ritosa/AppData/Local/ms-playwright/chromium-1243/chrome-win64/chrome.exe',args:['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required']});browsers.push(browser);report.browsers.push(await browser.version());const page=await browser.newPage({viewport:{width:1280,height:900}});page.setDefaultTimeout(30000);
  const row={role,page,lobbySockets:[],runtimeResponses:{},runtimeResponseTasks:[],consoleErrors:[],requestFailures:[]};actors.push(row);page.on('pageerror',e=>report.errors.push({role,error:e.message}));
  page.on('console',message=>{if(message.type()==='error')row.consoleErrors.push(message.text().slice(0,1000));});
  page.on('requestfailed',request=>row.requestFailures.push({url:request.url(),failure:request.failure()?.errorText||'unknown'}));
  page.on('response',response=>{const task=(async()=>{try{const url=new URL(response.url());if(!url.pathname.includes('/build-eagler-multiplayer/'))return;
    const name=url.pathname.split('/').at(-1);if(!['shared-netplay.mjs','spectator-timeline.mjs','th09.wasm','version.json'].includes(name))return;
    row.runtimeResponses[name]={url:response.url(),status:response.status(),sha256:createHash('sha256').update(await response.body()).digest('hex')};}catch{}})();row.runtimeResponseTasks.push(task);});
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
  await page.addInitScript(({role,name,forceWsRelay})=>{
    if(forceWsRelay&&(role==='host'||role==='guest'))window.RTCPeerConnection=undefined;
    localStorage.setItem('eagler-touhou-mp-display-name-v1',name);
  },{role,name:({host:'甲测试昵称',guest:'乙测试昵称',viewer:'丙测试观众','current-viewer':'丁测试观众'})[role]||'外部测试用户',forceWsRelay});
  await page.addInitScript(()=>{
    const NativeWebSocket=window.WebSocket;if(!NativeWebSocket||window.__th09LobbyWireCaptureInstalled)return;
    window.__th09LobbyWireCaptureInstalled=true;window.__th09LobbyWire=[];
    window.WebSocket=new Proxy(NativeWebSocket,{construct(target,args,newTarget){
      const socket=Reflect.construct(target,args,newTarget);let url;try{url=new URL(String(args[0]),location.href);}catch{return socket;}
      if(url.searchParams.has('lobby')){
        const record={roomId:url.searchParams.get('room')||'',closed:false,snapshots:[]};window.__th09LobbyWire.push(record);
        socket.addEventListener('message',event=>{if(typeof event.data!=='string')return;try{const message=JSON.parse(event.data);
          const serial=Number(message.room?.startSerial??message.serial);if(Number.isSafeInteger(serial)){
            record.snapshots.push({type:message.type||null,startSerial:serial,phase:message.room?.phase||null,
              spectatorCount:Number(message.room?.spectatorCount??message.room?.spectators?.length??0),
              spectatorNames:Array.isArray(message.room?.spectators)?message.room.spectators.map(item=>item.name||''):[]});
            if(record.snapshots.length>32)record.snapshots.shift();
          }
        }catch{}});
        socket.addEventListener('close',()=>record.closed=true);
      }
      return socket;
    }});
  });
  await page.addInitScript(()=>{
    window.__quickChatPlayed=[];const audioPlay=Audio.prototype.play;
    Audio.prototype.play=function(...args){if(this.src.includes('/quick-chat/'))window.__quickChatPlayed.push(this.src);return audioPlay.apply(this,args);};
    const rows=[];window.__th09FrontendDebug=rows;let hooked;
    setInterval(()=>{const runtime=window.__th09Runtime,c=runtime?.core||window.Module;if(!c?._th09_network_info)return;
      if(c!==hooked){hooked=c;const pause=c._th09_loop_pause;c._th09_loop_pause=function(on){rows.push({pause:on,at:performance.now()});return pause.call(this,on);};}
      const words=(fn,n)=>{const at=fn()/4;return Array.from(c.HEAPU32.subarray(at,at+n));};
      if(rows.length<1000)rows.push({at:performance.now(),visibilityState:document.visibilityState,hidden:document.hidden,focused:document.hasFocus(),activeElement:document.activeElement?.id||null,network:words(c._th09_network_info,6),rollback:words(c._th09_rollback_info,8),active:runtime?.netplay?.active,transport:window.__eaglerNetplayTransport||null,path:window.__eaglerNetplayPath||null});
    },250);
  });
  page.on('websocket',s=>{const url=new URL(s.url());if(url.searchParams.has('lobby')){const socket={closed:false,roomId:url.searchParams.get('room')||'',snapshots:[]};row.lobbySockets.push(socket);s.on('close',()=>socket.closed=true);s.on('framereceived',payload=>{try{const text=typeof payload==='string'?payload:payload?.toString?.();const message=JSON.parse(text);const serial=Number(message.room?.startSerial??message.serial);if(Number.isSafeInteger(serial))socket.snapshots.push({type:message.type||null,startSerial:serial,phase:message.room?.phase||null});if(socket.snapshots.length>32)socket.snapshots.shift();}catch{}});}});
  await page.goto(origin+'/lobby.html?game=th09mp',{waitUntil:'networkidle'});if(await page.locator('#firstUseNoticeClose').isVisible())await page.locator('#firstUseNoticeClose').click();return row;}
async function enter(row,mode,code){
  const p=row.page;
  if(mode==='list'){
    const room=p.locator(`.lobby-room-row[data-key="th09mp-${code}"]`);await room.waitFor();
    assert.equal(await room.locator('.lobby-room-state').getAttribute('data-state'),'playing');
    const button=room.locator('.lobby-join');assert.equal(await button.isEnabled(),true);
    assert.match(await button.innerText(),/Spectate|加入旁观|加入房间/);
    report.checks.playingDirectoryAdmissionLabel=await button.innerText();
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
async function ensureImportFixture(){
  if(importFixtureCreated)return importFixturePath;
  mkdirSync(dirname(importFixturePath),{recursive:true});
  if(existsSync(importFixturePath))throw Error('Import fixture path already exists');
  try{linkSync(resolvedTestDataPath,importFixturePath);importFixtureMethod='hard-link';}
  catch(error){
    if(!['EXDEV','EPERM','EACCES','ENOTSUP','EOPNOTSUPP'].includes(error?.code))throw error;
    copyFileSync(resolvedTestDataPath,importFixturePath,fsConstants.COPYFILE_EXCL);importFixtureMethod='copy';
  }
  importFixtureCreated=true;
  assert.equal(statSync(importFixturePath).size,testDataBytes,'staged .dat file keeps the Site Manifest byte count');
  assert.equal(await sha256File(importFixturePath),testDataSha256,'staged .dat file keeps the Site Manifest SHA-256');
  report.testData.stagedImportPath=importFixturePath;report.testData.staging=importFixtureMethod;
  return importFixturePath;
}
async function importLocalGameData(row){
  const p=row.page,dialog=p.locator('#gameDataImportWindow');
  if(!(await dialog.isVisible())){
    await p.locator('#mpSettingsRoomDrawerToggle').click();
    await p.locator('#mpSettingsRoomDrawer:not([hidden])').waitFor();
    await p.locator('#mpGamePackageImport').click();
    await dialog.waitFor({state:'visible'});
  }
  await p.locator('#gameDataImportInput').setInputFiles(await ensureImportFixture());
  const importStartedAt=Date.now(),importSamples=[];let importStarted=false,lastImportState=null;
  while(Date.now()-importStartedAt<120000){
    try{lastImportState=await p.evaluate(()=>{
      const node=id=>document.querySelector(`#${id}`),window=node('gameDataImportWindow'),busy=node('gameDataImportBusy');
      const toast=node('toast'),progress=node('mpRoomResourceProgress');
      return {dialogExists:!!window,dialogHidden:window?.hidden??null,ariaBusy:window?.getAttribute('aria-busy')||null,
        reason:node('gameDataImportReason')?.textContent?.trim()||'',busyHidden:busy?.hidden??null,
        busyText:node('gameDataImportBusyText')?.textContent?.trim()||'',dialogText:window?.innerText?.trim()?.slice(0,1200)||'',
        toastText:toast?.innerText?.trim()?.slice(0,600)||'',resourceStatus:progress?.dataset.status||null};
    });}catch(error){lastImportState={inspectionError:String(error)};}
    const elapsedMs=Date.now()-importStartedAt;
    if(lastImportState.dialogHidden===true)break;
    const importing=lastImportState.ariaBusy==='true'||lastImportState.busyHidden===false;
    if(importing)importStarted=true;
    if(importSamples.length===0||elapsedMs-importSamples.at(-1).elapsedMs>=5000||importing!==importSamples.at(-1).importing){
      importSamples.push({elapsedMs,importing,...lastImportState});
      stage('game-data-import-state',{role:row.role,elapsedMs,importing,...lastImportState});
    }
    if(elapsedMs>=2000&&lastImportState.dialogHidden===false&&!importing&&
      (importStarted||elapsedMs>=5000||lastImportState.toastText)){
      report.checks.packageImportDiagnostics={...report.checks.packageImportDiagnostics,[row.role]:{samples:importSamples,last:lastImportState}};
      throw Error(`TH09 ${row.role} data import stopped before closing; state=${JSON.stringify(lastImportState)}`);
    }
    await p.waitForTimeout(1000);
  }
  report.checks.packageImportDiagnostics={...report.checks.packageImportDiagnostics,[row.role]:{samples:importSamples,last:lastImportState}};
  if(lastImportState?.dialogHidden!==true)
    throw Error(`TH09 ${row.role} data import timed out after 120s; state=${JSON.stringify(lastImportState)}; samples=${JSON.stringify(importSamples)}`);
  const drawer=p.locator('#mpSettingsRoomDrawer');
  if(await drawer.isVisible()){
    await p.locator('#mpGamePackageImport').focus();await p.keyboard.press('Escape');
    await drawer.waitFor({state:'hidden'});
  }
  const progress=p.locator('#mpRoomResourceProgress'),status=await progress.getAttribute('data-status');
  let resumedStatus=status,resumeOutcome='unchanged';
  if(status==='cancelled'||status==='failed'){
    await p.locator('#mpRoomResourceRetry').click();
    const outcome=await p.waitForFunction(()=>{
      const status=document.querySelector('#mpRoomResourceProgress')?.dataset.status;
      if(status==='ready')return 'resources-ready';
      try{if(document.querySelector('#gameFrame')?.contentWindow?.__th09Runtime?.core)return 'native-runtime-created';}catch{}
      return false;
    },{},{timeout:120000});
    resumeOutcome=await outcome.jsonValue();await outcome.dispose();
    resumedStatus=await progress.getAttribute('data-status');
  }
  stage('official-local-data-imported',{role:row.role,bytes:testDataBytes,sha256:testDataSha256,resourceStatusBeforeRetry:status,resourceStatusAfterRetry:resumedStatus,resumeOutcome});
}
async function ready(row,side){const p=row.page;await p.locator('#mpRoomConnection').waitFor();if(await p.locator(`[data-mp-seat-me="${side}"]`).evaluate(n=>n.hidden))await p.locator(`[data-mp-seat-drop="${side}"] button`).click();await p.locator('#mpReady').click();await p.waitForFunction(()=>document.querySelector('#mpReady')?.getAttribute('aria-pressed')==='true',{},{timeout:120000});}
async function nativeSnapshot(row){return row.page.frameLocator('#gameFrame').locator('body').evaluate(()=>{const r=window.__th09Runtime,c=r?.core;if(!r||!c)return {runtime:false,body:document.body.innerText.slice(-1000)};const words=(fn,n)=>{try{const at=fn()/4;return Array.from(c.HEAP32.subarray(at,at+n));}catch{return null;}};let status=null;try{status=r.status?.()||null;}catch{}let peerError=null;try{const at=c._th09_peer_error?.()||0;if(at){const bytes=c.HEAPU8.subarray(at,at+256),end=bytes.indexOf(0);peerError=new TextDecoder().decode(bytes.subarray(0,end<0?bytes.length:end));}}catch{}return {runtime:true,prepared:r.netplay?.prepared??false,active:r.netplay?.active??false,connected:r.netplay?.connected??false,netplayError:r.netplay?.startupError||r.netplay?.error||null,timelineExperiment:r.netplay?.timelineExperiment??null,timelineAttached:!!r.netplay?.timeline,coreSpectatorControl:typeof c._th09_spectator_control,transport:globalThis.__eaglerNetplayTransport||null,path:globalThis.__eaglerNetplayPath||null,rtcPaths:globalThis.__eaglerNetplayRtcPaths||null,visibilityState:document.visibilityState,hidden:document.hidden,focused:document.hasFocus(),frameTimelineIds:['spectatorTimeline','spectatorToggle','spectatorDetails','spectatorSeek','spectatorPause','spectatorLive'].map(id=>[id,!!document.getElementById(id)]),title:status?.title||null,session:status?.session||null,titleStatus:status?.title||null,selection:c._th09_network_selection_info?words(c._th09_network_selection_info,16):null,network:c._th09_network_info?words(c._th09_network_info,6):null,player:c._th09_rollback_info?words(c._th09_rollback_info,8):null,peerState:c._th09_peer_state?.()??null,peerError,body:document.body.innerText.slice(-1000)};});}
async function browserDiagnostics(row){return row.page.evaluate(()=>{const inspect=win=>{if(!win)return null;try{return {visibilityState:win.document.visibilityState,hidden:win.document.hidden,focused:win.document.hasFocus(),debugRows:Array.isArray(win.__th09FrontendDebug)?win.__th09FrontendDebug.slice(-48):null};}catch(error){return {error:String(error)};}};let frame=null;try{frame=document.querySelector('#gameFrame')?.contentWindow||null;}catch{}return {outer:inspect(window),frame:inspect(frame)};});}
async function timelineMountSnapshot(row){return row.page.evaluate(()=>{
  const iframe=document.querySelector('#gameFrame'),frameDocument=iframe?.contentDocument,netplay=frameDocument?.defaultView?.__th09Runtime?.netplay;
  const ids=['spectatorTimeline','spectatorToggle','spectatorDetails','spectatorSeek','spectatorPause','spectatorLive'];
  const inspect=document=>{if(!document)return null;const panel=document.querySelector('#spectatorTimeline'),toggle=document.querySelector('#spectatorToggle');
    return {bodyClass:document.body?.className||'',ids:Object.fromEntries(ids.map(id=>[id,!!document.getElementById(id)])),
      panel:panel?{collapsed:panel.dataset.collapsed,display:getComputedStyle(panel).display,visibility:getComputedStyle(panel).visibility}:null,
      toggle:toggle?{display:getComputedStyle(toggle).display,visibility:getComputedStyle(toggle).visibility}:null};};
  return {netplay:{spectator:netplay?.spectator??null,timelineExperiment:netplay?.timelineExperiment??null,timelineAttached:!!netplay?.timeline},
    outer:inspect(document),frame:inspect(frameDocument)};
});}
async function confirmNativePrompt(row){const prompt=row.page.locator('#decisionDialog[open]:not(.closing)');if(!(await prompt.isVisible().catch(()=>false)))return false;const text=(await prompt.innerText().catch(()=>'' )).slice(0,500);const confirm=prompt.locator('#decisionConfirm');if(!(await confirm.isVisible().catch(()=>false)))return false;await confirm.click();stage('native-decision-confirmed',{role:row.role,text});return true;}
async function waitNativeMenu(row,screen,label,timeout=60000){const started=Date.now(),deadline=started+timeout;let last=null,reported20=false;while(Date.now()<deadline){await confirmNativePrompt(row);try{last=await nativeSnapshot(row);const title=last.titleStatus,selection=last.selection;if(last.runtime&&selection?.[1]&&title?.[2]===screen&&title?.[3]===1&&title?.[5]>=10)return last;}catch{}
  if(!reported20&&Date.now()-started>=20000){reported20=true;const snapshot={role:row.role,label,elapsedMs:Date.now()-started,native:await nativeSnapshot(row).catch(error=>({error:error.message})),outerBody:(await row.page.locator('body').innerText().catch(()=>'' )).slice(-1400),consoleErrors:row.consoleErrors.slice(-12),requestFailures:row.requestFailures.slice(-12)};stage('native-startup-20s-diagnostic',snapshot);}
  await row.page.waitForTimeout(100);
 }throw Error('Native menu wait failed: '+label+'; '+JSON.stringify({last,consoleErrors:row.consoleErrors.slice(-12),requestFailures:row.requestFailures.slice(-12),body:(await row.page.locator('body').innerText()).slice(-1400)}));}
async function pressNativeKey(row,key){await row.page.frameLocator('#gameFrame').locator('#canvas').focus();await row.page.keyboard.press(key,{delay:100});await confirmNativePrompt(row);await row.page.waitForTimeout(200);}
async function startNativeMatch(host,guest){
  const [hostMenu7,guestMenu7]=await Promise.all([waitNativeMenu(host,7,'host title menu'),waitNativeMenu(guest,7,'guest title menu')]);
  stage('native-title-menu-ready',{host:{title:hostMenu7.titleStatus,selection:hostMenu7.selection},guest:{title:guestMenu7.titleStatus,selection:guestMenu7.selection}});
  await pressNativeKey(host,'KeyZ');await waitNativeMenu(host,8,'host character menu');await waitNativeMenu(guest,8,'guest character menu');
  await pressNativeKey(host,'KeyZ');await pressNativeKey(guest,'KeyZ');await waitNativeMenu(host,16,'host final launch menu');await waitNativeMenu(guest,16,'guest final launch menu');
  const selections={host:await nativeSnapshot(host),guest:await nativeSnapshot(guest)};stage('native-players-selected',{host:{title:selections.host.titleStatus,selection:selections.host.selection},guest:{title:selections.guest.titleStatus,selection:selections.guest.selection}});
  await pressNativeKey(host,'KeyZ');
  const hostRun=await waitRuntime(host,s=>s.active&&s.player[1]>=180,'host running');const guestRun=await waitRuntime(guest,s=>s.active&&s.player[1]>=180,'guest running');
  return {host:hostRun,guest:guestRun,selections};
}
async function runtime(row){return row.page.frameLocator('#gameFrame').locator('body').evaluate(()=>{const r=window.__th09Runtime;if(!r?.core||!r.netplay)return null;const c=r.core,transport=r.netplay.startupTransport;const at=c._th09_rollback_info()/4;const player=Array.from(c.HEAPU32.subarray(at,at+8));const pointer=c._th09_spectator_playback_info()/4;const selectionAt=c._th09_network_selection_info()/4;return {player,selection:Array.from(c.HEAP32.subarray(selectionAt,selectionAt+16)),viewer:Array.from(c.HEAPU32.subarray(pointer,pointer+11)),spectator:r.netplay.spectator,active:r.netplay.active,connected:r.netplay.connected,prepared:r.netplay.prepared,build:r.netplay.build,peerState:c._th09_peer_state(),transport:globalThis.__eaglerNetplayTransport||null,path:globalThis.__eaglerNetplayPath||null,rtcPaths:globalThis.__eaglerNetplayRtcPaths||null,relayReadyState:transport?.relay?.readyState??null,sourcePollTimerActive:r.netplay.timer!=null,sourceReceivePumpInstalled:typeof transport?.onReceive==='function',visibilityState:document.visibilityState,hidden:document.hidden,focused:document.hasFocus(),run:Number(new URL(r.netplay.options.netplayUrl).searchParams.get('run')),pendingMatches:r.netplay.spectatorTape?.begins||0,streamStarted:r.netplay.spectatorTape?.started||false,wireFrame:r.netplay.spectatorTape?.frame||0,title:r.status().title,session:r.status().session};});}
async function waitRuntime(row,test,label,timeout=120000){const until=Date.now()+timeout;while(Date.now()<until){await confirmNativePrompt(row);try{const s=await runtime(row);if(s&&test(s))return s;}catch{}await row.page.waitForTimeout(100);}throw Error('Runtime wait failed: '+label+'; '+JSON.stringify({native:await nativeSnapshot(row).catch(error=>({error:error.message})),consoleErrors:row.consoleErrors.slice(-12),requestFailures:row.requestFailures.slice(-12),body:(await row.page.locator('body').innerText()).slice(-1400)}));}
async function runtimeInstanceId(row){return row.page.evaluate(()=>{const runtime=document.querySelector('#gameFrame')?.contentWindow?.__th09Runtime;if(!runtime)return null;window.__th09SpectatorRuntimeIds??=new WeakMap();window.__th09SpectatorRuntimeNext??=1;let id=window.__th09SpectatorRuntimeIds.get(runtime);if(!id){id=window.__th09SpectatorRuntimeNext++;window.__th09SpectatorRuntimeIds.set(runtime,id);}return id;});}
async function latestLobbyStart(row,roomCode){const roomId=`th09mp-${roomCode}`;const socket=[...row.lobbySockets].reverse().find(item=>item.roomId===roomId&&!item.closed);assert.ok(socket,`same-room lobby WebSocket remains open for ${roomId}`);const snapshot=await row.page.evaluate(id=>{const records=window.__th09LobbyWire||[];for(let i=records.length-1;i>=0;i--){const entry=records[i];if(entry.roomId===id&&!entry.closed){for(let j=entry.snapshots.length-1;j>=0;j--)if(Number.isSafeInteger(entry.snapshots[j].startSerial))return entry.snapshots[j];}}return null;},roomId);assert.ok(snapshot,`same-room lobby WebSocket exposes startSerial for ${roomId}`);return {roomId,...snapshot};}
async function waitRuntimeClosed(row,label){const until=Date.now()+30000;while(Date.now()<until){if(await runtimeInstanceId(row)===null)return;await row.page.waitForTimeout(100);}throw Error(`Toolbar exit did not retire the native Runtime (${label}); ${JSON.stringify({url:row.page.url(),native:await nativeSnapshot(row).catch(error=>({error:error.message})),body:(await row.page.locator('body').innerText()).slice(-1200)})}`);}
async function waitFreshSpectatorProgress(row,{instance,run,startSerial,roomCode,baseline},label,timeout=30000){const until=Date.now()+timeout;let last=null;while(Date.now()<until){const currentInstance=await runtimeInstanceId(row);if(currentInstance!==instance)throw Error(`${label} Runtime epoch changed while checking playback progress`);last=await runtime(row);if(last&&last.spectator&&last.active&&last.connected&&last.run===run&&last.streamStarted&&last.wireFrame>0){assert.equal((await latestLobbyStart(row,roomCode)).startSerial,startSerial,`${label} stays in the original room startSerial`);if(last.viewer[1]>=baseline.viewer[1]+10)return last;}await row.page.waitForTimeout(100);}throw Error(`${label} fresh Runtime playback cursor did not advance by 10 frames; ${JSON.stringify({baseline:{position:baseline.viewer?.[1],titleFrame:baseline.title?.[0]},last:last&&{position:last.viewer?.[1],titleFrame:last.title?.[0],edge:last.viewer?.[2],wireFrame:last.wireFrame},instance,run,startSerial})}`);}
async function leaveAndReenterSpectator(row,roomCode,iteration){
  const p=row.page,before=await runtime(row),oldInstance=await runtimeInstanceId(row);
  assert.ok(before?.spectator&&before.active&&before.connected,`cycle ${iteration} starts from the actual active spectator Runtime`);
  const run=before.run,roomStart=await latestLobbyStart(row,roomCode);assert.equal(run,roomStart.startSerial,`cycle ${iteration} native run matches the room startSerial`);
  await p.locator('#playbackExit').click();await waitRuntimeClosed(row,`cycle ${iteration}`);
  const retiredFrameUrl=await p.locator('#gameFrame').evaluate(frame=>frame.contentWindow.location.href);assert.equal(retiredFrameUrl,'about:blank',`cycle ${iteration} retires the old Runtime into about:blank`);
  await p.waitForTimeout(1200);assert.equal(await runtimeInstanceId(row),null,'a deliberate toolbar exit does not auto-restart the same run');
  assert.equal((await p.locator('#mpRoomCode').innerText()).trim(),roomCode,'toolbar exit returns to the same room');
  stage('spectator-toolbar-exited',{iteration,run,position:before.viewer[1],edge:before.viewer[2],wireFrame:before.wireFrame,runtimeInstance:oldInstance});
  const rail=p.locator('.mp-spectator-rail-head');await rail.click();
  const panel=p.locator('#mpRoomPanel[data-panel="spectators"]');await panel.waitFor({state:'visible'});
  const join=p.locator('#mpSpectatorJoin');await join.waitFor({state:'visible'});
  const roster=p.locator('#mpSpectatorList .mp-spectator-entry.mine');
  if(await roster.count()){
    await join.click();await p.waitForFunction(()=>!document.querySelector('#mpSpectatorList .mp-spectator-entry.mine'),{},{timeout:15000});
  }
  assert.equal(await roster.count(),0,`cycle ${iteration} retired the old spectator admission`);
  const afterLeave=await latestLobbyStart(row,roomCode);assert.equal(afterLeave.startSerial,roomStart.startSerial);assert.equal(afterLeave.spectatorCount,0,`cycle ${iteration} server roster has retired the old spectator`);
  await join.click();
  const current=await waitRuntime(row,s=>s.spectator&&s.active&&s.connected&&s.run===run&&s.streamStarted&&s.wireFrame>0,`same-run spectator re-entry ${iteration}`);
  const newInstance=await runtimeInstanceId(row);assert.notEqual(newInstance,oldInstance,`cycle ${iteration} starts a new Runtime epoch`);
  const reenteredRoomStart=await latestLobbyStart(row,roomCode);assert.equal(reenteredRoomStart.startSerial,roomStart.startSerial,`cycle ${iteration} re-enters the same room startSerial`);
  assert.equal(current.build,runtimeIdentity.version.build,'re-entry uses the same frozen multiplayer package');
  const admission=await p.waitForFunction(({roomId,startSerial})=>{const entries=(window.__th09LobbyWire||[]).filter(entry=>entry.roomId===roomId&&!entry.closed);for(let i=entries.length-1;i>=0;i--){const snapshots=entries[i].snapshots;for(let j=snapshots.length-1;j>=0;j--){const snapshot=snapshots[j];if(snapshot.startSerial===startSerial&&snapshot.spectatorCount>0)return snapshot;}}return false;},{roomId:roomStart.roomId,startSerial:roomStart.startSerial},{timeout:15000});
  const admittedRoster=await admission.jsonValue();await admission.dispose();
  await p.waitForFunction(()=>!!document.querySelector('#mpSpectatorList .mp-spectator-entry.mine'),{},{timeout:15000});
  assert.equal(await roster.count(),1,`cycle ${iteration} has a fresh server-confirmed spectator roster admission`);
  const freshBaseline=await runtime(row);assert.equal(await runtimeInstanceId(row),newInstance,`cycle ${iteration} progress baseline belongs to the fresh Runtime`);
  assert.ok(freshBaseline?.spectator&&freshBaseline.active&&freshBaseline.connected&&freshBaseline.run===run&&freshBaseline.streamStarted&&freshBaseline.wireFrame>0);
  const progressed=await waitFreshSpectatorProgress(row,{instance:newInstance,run,startSerial:roomStart.startSerial,roomCode,baseline:freshBaseline},`cycle ${iteration}`);
  stage('spectator-same-run-reentered',{iteration,run,startSerial:roomStart.startSerial,serverRoster:admittedRoster.spectatorCount,position:progressed.viewer[1],edge:progressed.viewer[2],wireFrame:progressed.wireFrame,titleFrame:progressed.title?.[0],runtimeInstance:newInstance});
  report.checks.spectatorReentry??=[];report.checks.spectatorReentry.push({iteration,roomId:roomStart.roomId,startSerial:roomStart.startSerial,run,serverRoster:admittedRoster.spectatorCount,fromPosition:before.viewer[1],freshBaselinePosition:freshBaseline.viewer[1],progressedPosition:progressed.viewer[1],freshBaselineTitleFrame:freshBaseline.title?.[0],progressedTitleFrame:progressed.title?.[0],fromWireFrame:before.wireFrame,toWireFrame:progressed.wireFrame,oldRuntimeInstance:oldInstance,newRuntimeInstance:newInstance});
  const panelClose=p.locator('#mpRoomPanelClose');if(await panelClose.isVisible())await panelClose.click();await p.locator('#mpRoomPanel').waitFor({state:'hidden'});
  const afterClose=await waitRuntime(row,s=>s.spectator&&s.active&&s.connected&&s.run===run&&s.streamStarted,'spectator after room panel close');
  assert.equal(await runtimeInstanceId(row),newInstance,'closing the room panel keeps the current Runtime epoch');
  return afterClose;
}
async function assertRuntimeIdentity(rows){
  const expected={sharedNetplay:runtimeIdentity.sharedNetplaySha256,spectatorTimeline:runtimeIdentity.spectatorTimelineSha256,
    wasm:runtimeIdentity.wasmSha256,version:runtimeSha256('version.json')};
  for(const row of rows){await Promise.all(row.runtimeResponseTasks);const actual=row.runtimeResponses;
    for(const [name,key] of [['shared-netplay.mjs','sharedNetplay'],['spectator-timeline.mjs','spectatorTimeline'],['th09.wasm','wasm'],['version.json','version']])
      assert.equal(actual[name]?.sha256,expected[key],`${row.role} loaded the frozen TH09 multiplayer ${name}`);
  }
}
try{
  const host=await actor('host');const code=await enter(host,'create');stage('created-room',{code});await importLocalGameData(host);
  const guest=await actor('guest');await enter(guest,'join',code);stage('joined-room');await importLocalGameData(guest);
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
  const started=await startNativeMatch(host,guest);report.checks.nativeSelection={host:started.selections.host.selection,guest:started.selections.guest.selection};
  stage('players-running');
  for(const row of [host,guest]){await row.page.frameLocator('#gameFrame').locator('#canvas').focus();await row.page.keyboard.down('z');}
  const playerTransport=await Promise.all([host,guest].map(runtime));
  report.checks.playerTransport=playerTransport.map((state,index)=>({role:[host,guest][index].role,transport:state?.transport||null,path:state?.path||null,rtcPaths:state?.rtcPaths||null,visibilityState:state?.visibilityState||null,hidden:state?.hidden??null,focused:state?.focused??null}));
  if(forceWsRelay)assert.ok(playerTransport.every(state=>state?.transport==='relay'),'forced fallback uses the existing WebSocket relay route on both player Runtimes');
  stage('player-transport-route',report.checks.playerTransport);
  const viewer=await actor('viewer');await enter(viewer,'list',code);await importLocalGameData(viewer);
  const first=await waitRuntime(viewer,s=>s.spectator&&s.viewer[2]>s.viewer[1]+100,'late viewer');
  stage('late-viewer-running',{state:first.viewer});report.checks.lateJoin=true;
  await assertRuntimeIdentity([host,guest,viewer]);
  assert.equal(first.build,runtimeIdentity.version.build,'viewer Runtime matches the frozen multiplayer package');
  report.checks.runtimeIdentity={version:first.build,wasm:runtimeIdentity.wasmSha256,manifest:runtimeIdentity.manifestSha256,
    loaded:Object.fromEntries([host,guest,viewer].map(row=>[row.role,row.runtimeResponses]))};
  const initialBefore=await runtime(viewer);await viewer.page.waitForTimeout(1000);const initialAfter=await runtime(viewer);
  const initialDelta=initialAfter.viewer[1]-initialBefore.viewer[1];
  assert.ok(initialDelta>=30&&initialDelta<=90,`initial playback should run near 1x, observed ${initialDelta} frames/s`);
  assert.equal(initialAfter.viewer[6],0,'initial playback must not enter catch-up seek');
  report.checks.initialPlaybackNormalSpeed={framesPerSecond:initialDelta,position:initialAfter.viewer[1],received:initialAfter.viewer[2]};
  stage('initial-playback-normal-speed',report.checks.initialPlaybackNormalSpeed);
  const frame=viewer.page.frameLocator('#gameFrame');
  report.checks.timelineMount=await timelineMountSnapshot(viewer);stage('spectator-timeline-mount',report.checks.timelineMount);
  assert.ok(report.checks.timelineMount.outer?.ids?.spectatorToggle,
    `spectator playback panel was not mounted: ${JSON.stringify(report.checks.timelineMount)}`);
  assert.equal(await viewer.page.locator('#spectatorDetails').isVisible(),false);
  await viewer.page.locator('#spectatorToggle').click();assert.equal(await viewer.page.locator('#spectatorDetails').isVisible(),true);
  await viewer.page.locator('#spectatorScore').filter({hasText:'<0 : 0>'}).waitFor();
  assert.deepEqual(await viewer.page.locator('.spectator-score-player').allTextContents(),['P1(甲)','P2(乙)']);
  report.checks.playbackNicknameInitials=true;
  assert.equal(await viewer.page.locator('.th09-match-hud').count(),0);
  assert.equal(await viewer.page.locator('#spectatorClock').count(),0);
  assert.match(await viewer.page.locator('#spectatorMatchDuration').innerText(),/已进行 \d+ 分钟/);
  await viewer.page.evaluate(async () => {await document.fonts.ready;});
  assert.match(await viewer.page.locator('#spectatorTimeline').evaluate(node=>getComputedStyle(node).fontFamily),/ET Yatra/);
  assert.equal(await viewer.page.locator('#spectatorTimeline').evaluate(node=>node.ownerDocument===document),true);
  assert.equal(await viewer.page.locator('#spectatorUiFonts').count(),0,'Host-owned timeline uses launcher fonts directly; no cross-document font copy is needed');
  assert.equal(await viewer.page.locator('#spectatorNextRound').count(),0);
  await viewer.page.evaluate(async()=>{
    await Promise.all([...document.querySelectorAll('#spectatorTimeline img')].map(img=>img.decode()));
  });
  assert.ok(await viewer.page.locator('#spectatorTimeline img').evaluateAll(nodes=>nodes.every(img=>img.complete&&img.naturalWidth>0)));
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

  await viewer.page.locator('#spectatorPause').click();const paused=await waitRuntime(viewer,s=>s.viewer[5]===1,'pause');
  await viewer.page.waitForTimeout(16000);
  const arriving=await runtime(viewer);assert.ok(arriving.viewer[2]>paused.viewer[2],'source continues to deliver frames during a long local pause');
  assert.equal(arriving.viewer[1],paused.viewer[1],'local pause keeps the native cursor fixed');
  assert.equal(arriving.viewer[5],1);assert.equal(arriving.connected,true);
  report.checks.pauseKeepsReceiving=true;report.checks.longLocalPause={held:arriving.viewer[1],receivedBefore:paused.viewer[2],receivedAfter:arriving.viewer[2]};
  await frame.locator('body').evaluate(()=>{const c=window.__th09Runtime.core,prior=c.onGameFrame;window.__spectatorTestFrames=[];c.onGameFrame=function(...args){const at=c._th09_spectator_playback_info()/4;const s=Array.from(c.HEAPU32.subarray(at,at+11));if(window.__spectatorTestFrames.length<5000)window.__spectatorTestFrames.push(s);return prior?.apply(this,args);};});
  const range=viewer.page.locator('#spectatorSeek'),rangeBefore=Number(await range.inputValue());
  await viewer.page.locator('#spectatorSeek').focus();await viewer.page.locator('#spectatorSeek').press('End');
  const seeking=await waitRuntime(viewer,s=>s.viewer[6]===1,'manual fast forward');const target=seeking.viewer[4],seekProgress=Number(await range.inputValue());
  assert.ok(seekProgress>rangeBefore,`timeline cursor advances during seek (${rangeBefore} -> ${seekProgress})`);
  assert.ok(seekProgress<Number(await range.getAttribute('max')),'timeline cursor follows progress instead of staying pinned to target');
  await viewer.page.locator('#spectatorPause').click();
  const pausedSeek=await waitRuntime(viewer,s=>s.viewer[5]===1&&s.viewer[6]===1,'pause during fixed-target seek');
  const rangeAtPause=Number(await range.inputValue());await viewer.page.waitForTimeout(1200);
  const heldSeek=await runtime(viewer),rangeHeld=Number(await range.inputValue());
  assert.equal(heldSeek.viewer[1],pausedSeek.viewer[1],'pause aborts seek progress at the current native frame');
  assert.equal(heldSeek.viewer[6],1,'native retains the fixed seek target for resume');assert.equal(rangeHeld,rangeAtPause,'visible cursor holds with native progress');
  await viewer.page.locator('#spectatorPause').click();
  const resumedSeek=await waitRuntime(viewer,s=>s.viewer[1]>=target&&!s.viewer[6],'resume paused seek to its fixed target');
  const trace=await frame.locator('body').evaluate(()=>window.__spectatorTestFrames);assert.ok(trace.some(s=>s[1]===target),'exact target was reached without skipped input');
  report.checks.manualTargetAndStop=true;report.checks.seekPauseAbort={pausedAt:pausedSeek.viewer[1],heldAt:heldSeek.viewer[1],target:resumedSeek.viewer[1],rangeAtPause:rangeAtPause};
  const beforeBack=await runtime(viewer);await viewer.page.locator('#spectatorSeek').press('Home');
  const afterBack=await waitRuntime(viewer,s=>s.viewer[1]<beforeBack.viewer[1],'rewind retained match history');
  assert.equal(afterBack.active,true);report.checks.backwardSeek=true;
  const liveTraceStart=await frame.locator('body').evaluate(()=>window.__spectatorTestFrames.length);
  const beforeLive=await runtime(viewer);await viewer.page.locator('#spectatorLive').click();
  const liveSeeking=await waitRuntime(viewer,s=>s.viewer[7]===1,'enter following live');const liveTarget=liveSeeking.viewer[4];
  assert.ok(liveTarget>beforeLive.viewer[1],'live chooses the current confirmed edge as a fixed catch-up target');
  const live=await waitRuntime(viewer,s=>s.viewer[7]===1&&!s.viewer[6]&&s.viewer[1]>=liveTarget,'finish catch-up at the live edge');
  const liveTrace=(await frame.locator('body').evaluate(()=>window.__spectatorTestFrames)).slice(liveTraceStart);
  const liveTargetSample=liveTrace.find(s=>s[1]===liveTarget),liveFollowingSample=liveTrace.find(s=>s[1]>=liveTarget&&s[6]===0&&s[7]===1);
  const targetOvershoot=live.viewer[1]-liveTarget;
  report.checks.liveCatchup={target:liveTarget,positionAtPoll:live.viewer[1],pollOvershootFrames:targetOvershoot,
    edgeAtPoll:live.viewer[2],
    nativeTargetSample:liveTargetSample||null,nativeFollowingSample:liveFollowingSample||null,
    following:live.viewer[7],seeking:live.viewer[6],delay:live.viewer[2]-live.viewer[1]};
  assert.ok(liveTargetSample,'native frame trace records the exact fixed live target');
  assert.ok(liveFollowingSample,'native frame trace records seek cleared while following after the fixed target');
  const caughtDelay=live.viewer[2]-live.viewer[1];assert.ok(caughtDelay>=60,`live cursor keeps the confirmed safety buffer (${caughtDelay})`);
  assert.ok(live.viewer[1]<=live.viewer[2],'live playback does not move beyond the confirmed edge');
  await viewer.page.waitForTimeout(1600);const liveAfter=await runtime(viewer),liveRate=(liveAfter.viewer[1]-live.viewer[1])/1.6;
  assert.ok(liveRate>=30&&liveRate<=90,`following runs at normal speed after catch-up, observed ${liveRate} frames/s`);
  assert.equal(liveAfter.viewer[7],1);report.checks.liveDelay=caughtDelay;
  report.checks.liveCatchup={...report.checks.liveCatchup,position:live.viewer[1],targetOvershoot,
    nativeTargetVisited:true,seekClearedWhileFollowing:true,delay:caughtDelay,followingFramesPerSecond:liveRate};
  async function unobstructedTimeline() {
    const timeline = await viewer.page.locator('#spectatorTimeline').boundingBox();
    const canvas = await frame.locator('#canvas').boundingBox();
    const viewport = await viewer.page.locator('#gameFrame').boundingBox();
    const overlaps=(a,b)=>a.x<b.x+b.width&&a.x+a.width>b.x&&a.y<b.y+b.height&&a.y+a.height>b.y;
    assert.ok(timeline && canvas && viewport && Math.abs(canvas.y + canvas.height / 2 - viewport.y - viewport.height / 2) < 2,
      'playback controls must leave the canvas centered');
    const chat=await viewer.page.locator('.mp-quick-chat-prompt').boundingBox();
    assert.ok(chat&&chat.x<=20,'quick chat stays anchored at the bottom-left');
    assert.ok(!overlaps(chat,timeline),`quick chat and playback panel do not overlap: ${JSON.stringify({chat,timeline})}`);
    const picker=viewer.page.locator('.mp-quick-chat-picker:not([hidden])');
    if(await picker.count()){const box=await picker.boundingBox();assert.ok(box.y+box.height<=chat.y,'picker opens upward');assert.ok(!overlaps(box,timeline),'quick-chat picker does not overlap playback panel');}
    const diagnostic=viewer.page.locator('#runtimeDiagnostics');
    if(await diagnostic.isVisible()){
      const box=await diagnostic.boundingBox();
      assert.ok(box&&!overlaps(box,timeline),`diagnostics do not overlap the playback panel: ${JSON.stringify({diagnostic:box,timeline})}`);
    }
  }
  await unobstructedTimeline();await viewer.page.screenshot({path:resolve(out,'timeline-desktop.png')});
  await viewer.page.locator('#spectatorTimeline').screenshot({path:resolve(out,'playback-panel-desktop.png')});
  stage('playback-controls-pass',{target,delay:report.checks.liveDelay});
  await viewer.page.setViewportSize({width:390,height:844});await unobstructedTimeline();await viewer.page.screenshot({path:resolve(out,'timeline-mobile-layout.png')});
  await viewer.page.locator('#spectatorTimeline').screenshot({path:resolve(out,'playback-panel-mobile.png')});await viewer.page.setViewportSize({width:1280,height:900});
  report.checks.centeredCanvasAndUnobstructedChat = true;
  await viewer.page.locator('#spectatorToggle').click();await unobstructedTimeline();
  await viewer.page.screenshot({path:resolve(out,'timeline-collapsed.png')});
  await viewer.page.locator('#spectatorToggle').click();report.checks.collapseAndExpand=true;
  if(focusedMode){
    await leaveAndReenterSpectator(viewer,code,1);
    await leaveAndReenterSpectator(viewer,code,2);
    stage('focused-spectator-lifecycle-pass',{reentries:report.checks.spectatorReentry});
  }else{
  await viewer.page.locator('#spectatorPause').click();
  let held=await waitRuntime(viewer,s=>s.viewer[5]===1,'hold old match');
  await host.page.frameLocator('#gameFrame').locator('body').evaluate(()=>{
    const c=window.__th09Runtime.core,prior=c.onNetworkSpectatorEnd;
    c.onNetworkSpectatorEnd=function(frame,winner,...args){const at=c._th09_confirmed_frame_hash(frame-1)/4;
      window.__oldEndHash={frame,winner,record:c.HEAPU32[at],hash:c.HEAPU32[at+1]};return prior?.call(this,frame,winner,...args);};
    const round=c.onNetworkSpectatorRound;window.__roundHashes={};
    c.onNetworkSpectatorRound=function(frame,index){const at=c._th09_confirmed_frame_hash(frame-1)/4;
      window.__roundHashes[frame]=c.HEAPU32[at+1];return round?.call(this,frame,index);};
  });
  const witness=await actor('current-viewer');await enter(witness,'list',code);await importLocalGameData(witness);
  await waitRuntime(witness,s=>s.spectator&&s.viewer[2]>s.viewer[1]+90,'independent live viewer');
  await witness.page.locator('#spectatorToggle').click();
  await witness.page.locator('#spectatorLive').click();
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
          if(c.HEAPU32[at+1]===window.__pauseAtRound){window.__pauseAtRound=-1;parent.document.querySelector('#spectatorPause').click();}
          return prior?.apply(this,args);};},point);
      const markBox=await marker.first().boundingBox();await viewer.page.mouse.click(markBox.x+markBox.width/2,markBox.y+markBox.height/2);
      held=await waitRuntime(viewer,s=>s.viewer[1]===point&&s.viewer[5]===1,'next round stops at actual reset');
      assert.equal(held.viewer[8],point,'annotation is the real reset, not the previous round victory');
      const actual=await frame.locator('body').evaluate(()=>window.__th09Runtime.core._th09_network_hash()>>>0);
      const expected=await host.page.frameLocator('#gameFrame').locator('body').evaluate((node,f)=>window.__roundHashes[f],point);
      assert.equal(actual,expected);roundSeeks++;
      await viewer.page.screenshot({path:resolve(out,`round-marker-${roundSeeks}.png`)});
      await viewer.page.locator('#spectatorTimeline').screenshot({path:resolve(out,`playback-marker-${roundSeeks}.png`)});
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
  assert.match(await viewer.page.locator('#spectatorPlaybackStatus').innerText(),/1 局待播放/);
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
  await late.page.locator('#spectatorPause').click();report.checks.latestMatchHistory=true;
  await frame.locator('body').evaluate((node,details)=>{
    const c=window.__th09Runtime.core,prior=c.onGameFrame;window.__oldTail=null;window.__pauseOnNext=true;
    c.onGameFrame=function(...args){const at=c._th09_spectator_playback_info()/4;
      if(c.HEAPU32[at+1]===details.end&&c.HEAPU32[at+10])window.__oldTail={frame:details.end,hash:c._th09_network_hash()>>>0};
      if(window.__pauseOnNext&&c.HEAPU32[at+8]===details.start){window.__pauseOnNext=false;
        if(!c.HEAPU32[at+5])parent.document.querySelector('#spectatorPause').click();}
      return prior?.apply(this,args);};
  },{end:endHash.frame,start});
  await viewer.page.locator('#spectatorSeek').press('End');
  await waitRuntime(viewer,s=>s.viewer[8]===start&&s.viewer[5]===1,'finish every old frame before next match',180000);
  const oldTail=await frame.locator('body').evaluate(()=>window.__oldTail);
  assert.deepEqual(oldTail,{frame:endHash.frame,hash:endHash.hash});report.checks.oldTailHash=oldTail;
  const target2=start+600;await waitRuntime(late,s=>s.viewer[3]>target2,'second-match target available');
  for(const row of [viewer,late]){
    const f=row.page.frameLocator('#gameFrame');if(!(await runtime(row)).viewer[5])await row.page.locator('#spectatorPause').click();
    await f.locator('body').evaluate((node,target)=>{const c=window.__th09Runtime.core,prior=c.onGameFrame;window.__pauseAt=target;
      c.onGameFrame=function(...args){const at=c._th09_spectator_playback_info()/4;if(c.HEAPU32[at+1]===window.__pauseAt){window.__pauseAt=-1;parent.document.querySelector('#spectatorPause').click();}return prior?.apply(this,args);};},target2);
    await row.page.locator('#spectatorSeek').evaluate((node,value)=>{node.value=String(value);node.dispatchEvent(new Event('input',{bubbles:true}));node.dispatchEvent(new Event('change',{bubbles:true}));},target2);
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
  await viewer.page.locator('#spectatorLive').click();
  const jumped=await waitRuntime(viewer,s=>s.viewer[8]===latestStart&&s.viewer[7]===1&&s.viewer[5]===0,'live jumps to newest match');
  assert.ok(jumped.viewer[1]>=latestStart&&latestStart>beforeJump.viewer[2]);
  const recovered=await waitRuntime(viewer,s=>!s.viewer[6]&&s.viewer[2]-s.viewer[1]>60&&s.viewer[2]-s.viewer[1]<=110,'live delay restored after jump');
  report.checks.skipOldMatchToLive={from:beforeJump.viewer[1],latestStart,delay:recovered.viewer[2]-recovered.viewer[1]};
  stage('live-jump-pass',report.checks.skipOldMatchToLive);
  }
  if(focusedMode){
    // Test an actually silent but healthy source transport last. Pausing only
    // the game loop leaves shared-netplay's independent confirmed-input poll
    // running, which is correctly protected by the player watchdog. This
    // fixture suspends each source loop and that test polling timer while
    // keeping both real relay sockets open; it does not test source recovery.
    const sourceBeforeIdle=await Promise.all([host,guest].map(runtime));
    const suspended=await Promise.all([host,guest].map(row=>row.page.frameLocator('#gameFrame').locator('body').evaluate(()=>{
      const runtime=window.__th09Runtime,netplay=runtime?.netplay,core=runtime?.core;
      const transport=netplay?.startupTransport||globalThis.__eaglerPeerTransport;
      const relay=transport?.relay;
      if(!core||!netplay?.active||!netplay.connected||!transport||transport.route!=='relay'||!relay||relay.readyState!==WebSocket.OPEN)
        throw Error('source-idle fixture requires an active native player and an open WebSocket relay');
      const timer=netplay.timer;
      if(timer===null||timer===undefined)throw Error('source-idle fixture expected the source polling timer to be active');
      core._th09_loop_pause(1);
      clearTimeout(timer);netplay.timer=null;
      const hadReceive=typeof transport.onReceive==='function';transport.onReceive=null;
      return {playerFrame:core.HEAPU32[core._th09_rollback_info()/4+1],route:transport.route,
        relayReadyState:relay.readyState,timerSuspended:true,receivePumpSuppressed:hadReceive,
        visibilityState:document.visibilityState,hidden:document.hidden,focused:document.hasFocus()};
    })));
    assert.deepEqual(suspended.map(s=>s.route),['relay','relay']);
    stage('focused-source-idle-held',{sources:suspended});
    await viewer.page.waitForTimeout(250);
    let sourceHeld=await Promise.all([host,guest].map(runtime));
    await viewer.page.waitForTimeout(250);
    const sourceSettled=await Promise.all([host,guest].map(runtime));
    assert.deepEqual(sourceSettled.map(s=>s.player[1]),sourceHeld.map(s=>s.player[1]),'source native frames settle after loop and polling hold');
    sourceHeld=sourceSettled;
    for(const state of sourceHeld){assert.equal(state.active,true);assert.equal(state.connected,true);assert.equal(state.peerState,1);assert.equal(state.relayReadyState,1);assert.equal(state.sourcePollTimerActive,false);assert.equal(state.sourceReceivePumpInstalled,false);}
    const viewerBeforeQuiet=await runtime(viewer);
    // Let in-flight confirmed history drain before measuring the silent span.
    let quiet=await runtime(viewer),quietSince=Date.now(),settled=false;
    const settleDeadline=Date.now()+8000;
    while(Date.now()<settleDeadline){
      await viewer.page.waitForTimeout(250);const current=await runtime(viewer);
      if(current.wireFrame!==quiet.wireFrame||current.viewer[2]!==quiet.viewer[2]){quiet=current;quietSince=Date.now();}
      else if(Date.now()-quietSince>=1200){settled=true;break;}
    }
    assert.equal(settled,true,'spectator confirmed-history edge settles after source polling is held');
    const quietStart=await runtime(viewer);
    assert.equal(quietStart.connected,true);assert.equal(quietStart.peerState,1);
    await viewer.page.waitForTimeout(16000);
    const viewerAfterQuiet=await runtime(viewer),sourcesAfterQuiet=await Promise.all([host,guest].map(runtime));
    assert.deepEqual(sourcesAfterQuiet.map(s=>s.player[1]),sourceHeld.map(s=>s.player[1]),'source native frames remain fixed throughout the silent interval');
    for(const state of sourcesAfterQuiet){assert.equal(state.active,true);assert.equal(state.connected,true);assert.equal(state.peerState,1);assert.equal(state.relayReadyState,1,'source WebSocket relay remains open');assert.equal(state.sourcePollTimerActive,false);assert.equal(state.sourceReceivePumpInstalled,false);}
    assert.equal(viewerAfterQuiet.connected,true,'healthy viewer remains connected during 16 seconds without source frames');
    assert.equal(viewerAfterQuiet.peerState,1,'source silence does not close the viewer transport');
    assert.equal(viewerAfterQuiet.wireFrame,quietStart.wireFrame,'no new confirmed spectator frames arrive during the silent interval');
    assert.equal(viewerAfterQuiet.viewer[2],quietStart.viewer[2],'spectator history edge remains fixed during the silent interval');
    report.checks.longSourcePause={sourceFramesBeforeHold:sourceBeforeIdle.map(s=>s.player[1]),sourceFramesAtHold:suspended.map(s=>s.playerFrame),sourceFrames:sourceHeld.map(s=>s.player[1]),sourceTransport:suspended,
      viewerBeforeDrain:{position:viewerBeforeQuiet.viewer[1],edge:viewerBeforeQuiet.viewer[2],wireFrame:viewerBeforeQuiet.wireFrame},
      viewerAtQuietStart:{position:quietStart.viewer[1],edge:quietStart.viewer[2],wireFrame:quietStart.wireFrame,received:quietStart.viewer[2]},
      viewerAfter16s:{position:viewerAfterQuiet.viewer[1],edge:viewerAfterQuiet.viewer[2],wireFrame:viewerAfterQuiet.wireFrame,received:viewerAfterQuiet.viewer[2],peerState:viewerAfterQuiet.peerState},
      durationMs:16000,fixture:'both native source loops and source polling timers held; actual relay sockets stayed open; no source resume asserted'};
    stage('focused-viewer-survived-source-idle',{durationMs:16000,viewer:report.checks.longSourcePause.viewerAfter16s});
  }
  assert.equal(report.errors.length,0);report.passed=true;
}catch(error){report.error=error.stack;for(const row of actors){try{report[row.role+'Runtime']=await runtime(row);report[row.role+'Native']=await nativeSnapshot(row);report[row.role+'BrowserDiagnostics']=await browserDiagnostics(row);report[row.role+'ConsoleErrors']=row.consoleErrors.slice(-20);report[row.role+'RequestFailures']=row.requestFailures.slice(-20);await row.page.screenshot({path:resolve(out,row.role+'-failure.png')});report[row.role+'Text']=(await row.page.locator('body').innerText()).slice(-1600);}catch{}}throw error;}
finally{for(const browser of browsers){try{await browser.close();}catch(error){report.errors.push({cleanup:'browser',error:String(error)});}}if(importFixtureCreated){try{unlinkSync(importFixturePath);report.testData.stagedFileRemoved=true;}catch(error){report.errors.push({cleanup:'import-fixture',error:String(error)});}}if(report.errors.length&&report.passed)report.passed=false;writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n',{flag:'wx'});console.log(JSON.stringify({passed:report.passed,output:out,checks:report.checks}));}
