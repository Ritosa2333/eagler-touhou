import {verifyOriginalOpening,verifyInGameReplaySave} from './coop-browser-original.mjs';
import {configurePalettes,configureFreezeMotivation} from './coop-browser-palettes.mjs';
import {pinnedAudio,instrumentAudio,readAudio,audioLane,liveLoop} from './coop-browser-audio.mjs';
import {verifyBrowserResult} from './coop-browser-results.mjs';
// Exercise the actual authored Launcher; not an alternative room/game UI. Always owns its temporary processes/profile.
import assert from 'node:assert/strict';import {createServer} from 'node:net';import {once} from 'node:events';import {spawn} from 'node:child_process';import {createRequire} from 'node:module';import {mkdtemp,readFile,writeFile,mkdir,unlink} from 'node:fs/promises';import {join,resolve} from 'node:path';import {pathToFileURL} from 'node:url';import {createHash,randomUUID} from 'node:crypto';import {setTimeout as sleep} from 'node:timers/promises';
const [staged,programs,relayRoot,baseManifest,privatePackage,exe,root,lease]=process.argv.slice(2).map(x=>resolve(x));await mkdir(root,{recursive:true});const work=await mkdtemp(join(root,'session-')),token=randomUUID(),launcher=join(staged,'launcher'),logs=new Map(),children=[],errors=[];const browsers=[],pages=[],wire=[],boot=Date.now();let activePage;
const puppeteer=createRequire(join(launcher,'package.json'))('puppeteer-core');const {assembleTh128LocalHost}=await import(pathToFileURL(join(staged,'ports/th128-mp/tools/assemble-local-host.mjs')));const projection=JSON.parse(await readFile(join(programs,'projection.json'))),sources=JSON.parse(await readFile(join(staged,'coop-source.json')));assert.equal(sources.generation,projection.multiplayerGeneration);
const until=async(what,fn,timeout=22000)=>{const deadline=performance.now()+timeout;while(performance.now()<deadline){const v=await fn();if(v)return v;for(const p of children)if(p.child.exitCode!==null)throw Error(`${p.name} exited: ${logs.get(p.name)}`);await sleep(100);}throw Error('Timeout: '+what);};
function run(name,args,cwd,extra){const child=spawn(process.execPath,args,{cwd,env:{...process.env,...extra},stdio:['ignore','pipe','pipe'],windowsHide:true});children.push({name,child});logs.set(name,'');for(const ch of [child.stdout,child.stderr])ch.on('data',d=>logs.set(name,(logs.get(name)+d).slice(-65536)));return child;}
const reserve=async()=>{const s=createServer();s.listen(0,'127.0.0.1');await once(s,'listening');const port=s.address().port;await new Promise(r=>s.close(r));return port;};
async function inspect(page,label){const detail=await page.evaluate(()=>({href:location.href,title:document.title,body:document.body?.innerText.slice(0,4200),buttons:[...document.querySelectorAll('button,[role=button],input,select')].filter(el=>el.getBoundingClientRect().width||el.type==='file').slice(0,120).map(el=>[el.id,el.getAttribute('aria-label')??el.title??'',(el.textContent??'').trim().slice(0,45),el.type,!!el.disabled]),frames:[...document.querySelectorAll('iframe')].map(f=>({src:f.getAttribute('src'),hidden:f.hidden}))}));await writeFile(join(work,label+'.json'),JSON.stringify(detail,null,2));return detail;}
await writeFile(lease,token,{flag:'wx'});
try{const audio=process.env.TH128_TEST_AUDIO==='1'?await pinnedAudio(privatePackage):null;
 run('relay',[join(relayRoot,'services/relay/src/netplay-relay.mjs')],relayRoot,{EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_STUN_URLS:''});const relay='ws://127.0.0.1:'+(await until('Relay',()=>/listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(logs.get('relay'))))[1];const httpPort=await reserve(),hostRoot=join(work,'host');
 const verified=await assembleTh128LocalHost({host:baseManifest,normal:join(programs,'normal'),multiplayer:join(programs,'multiplayer'),multiplayerGeneration:sources.generation,output:hostRoot,relay});const base=`http://127.0.0.1:${httpPort}`;
 run('launcher',[join(launcher,'scripts/serve.mjs'),String(httpPort)],launcher,{EAGLER_TOUHOU_HOST:'127.0.0.1',EAGLER_TOUHOU_PORT:String(httpPort),EAGLER_WORKSPACE_ROOT:hostRoot,EAGLER_REACT_BUILD_DIRECTORY:join(launcher,'.cache/build/ui-rewrite'),EAGLER_DEVELOPMENT_SITE_MANIFEST:verified.siteManifest,EAGLER_TOUHOU_NETPLAY_RELAY:relay,EAGLER_ENABLE_THCRAP:'0',...(process.env.TH128_QA_ARTWORK?{EAGLER_TOUHOU_ARTWORK_DIR:process.env.TH128_QA_ARTWORK}:{})});
 await until('Launcher',()=>logs.get('launcher').includes(base));
 if(process.env.TH128_QA_ARTWORK){const cover=await fetch(base+'/assets/th128.jpg');assert.equal(cover.status,200);assert.match(cover.headers.get('Content-Type'),/^image\/jpeg/);}
 const advertised=await fetch(base+'/site-manifest.json').then(r=>r.json());assert.equal(advertised.profile,sources.profile);assert.ok(advertised.games.th128.multiplayerRuntime.includes(sources.generation));const doc=await fetch(new URL(advertised.games.th128.multiplayerRuntime,base));assert.equal(doc.status,200);assert.equal(doc.headers.get('Cross-Origin-Embedder-Policy'),'require-corp','an isolated author Launcher cannot navigate an unisolated game iframe');assert.equal(doc.headers.get('Cross-Origin-Resource-Policy'),'same-origin');const entryURL=new URL(advertised.games.th128.multiplayerRuntime,base),programInfo=await fetch(new URL('runtime/program.json',entryURL)).then(r=>r.json());const wasmTransfer=await fetch(new URL(programInfo.files.coopWasm.path,entryURL),{method:'HEAD',headers:{'Accept-Encoding':'br, gzip'}});assert.equal(wasmTransfer.headers.get('Content-Encoding'),null,'local late spectators must not wait for synchronous program compression');assert.equal(Number(wasmTransfer.headers.get('Content-Length')),programInfo.files.coopWasm.bytes);
 async function importAt(seat){
 const browser=await puppeteer.launch({executablePath:exe,userDataDir:join(work,'profile-'+seat),headless:true,pipe:true,timeout:22000,args:['--no-proxy-server','--host-resolver-rules=MAP * ~NOTFOUND, EXCLUDE 127.0.0.1, EXCLUDE localhost, EXCLUDE *.local','--allow-loopback-in-peer-connection',...(process.env.TH128_TEST_HARDWARE==='1'?['--ignore-gpu-blocklist','--use-angle=d3d11']:['--enable-unsafe-swiftshader','--use-angle=swiftshader']),'--enable-webgl','--disable-background-networking','--disable-sync','--disable-component-update','--disable-background-timer-throttling','--disable-renderer-backgrounding','--no-first-run']});
 browsers.push(browser);const page=await browser.newPage();pages.push(page);activePage=page;await page.setViewport({width:1250,height:860});page.on('pageerror',error=>errors.push({type:'page',seat,message:error?.stack??String(error)}));page.on('console',msg=>{if(['error','warn','warning'].includes(msg.type()))errors.push({type:'console',seat,message:msg.text().slice(0,1000)});});page.on('requestfailed',r=>errors.push({type:'failed',seat,message:r.method()+' '+r.url()+' '+r.failure()?.errorText}));
 if(audio)await page.evaluateOnNewDocument(instrumentAudio);
 const cdp=await page.createCDPSession();await cdp.send('Page.enable');await cdp.send('Network.enable');await page.evaluateOnNewDocument(()=>{window.__coopTestLifecycle=[];addEventListener('message',({data:d})=>{if(d?.protocol==='eagler-touhou/1'&&window.__coopTestLifecycle.length<90&&(d.event||d.command||d.ok===false))window.__coopTestLifecycle.push([Math.round(performance.now()),d.event??d.command??null,d.ok,d.error?.message]);});});
 for(const [event,dir] of [['Network.webSocketFrameReceived','in'],['Network.webSocketFrameSent','out']])cdp.on(event,({response:f})=>{if(f.opcode!==1||wire.length>2500)return;try{const x=JSON.parse(f.payloadData);if(!['spectate','spectator-start','lobby-state','lobby-welcome','set-ready','start','error','resource-progress','resources-changed','set-transport','spectator-session','return-room'].includes(x.type))return;const r=x.room;wire.push([Date.now()-boot,seat,dir,x.type,r?.phase,r?.spectatorTimeline,x.status,x.stage,x.percent,x.error,x.serial]);}catch{}});await cdp.send('Audits.enable');cdp.on('Audits.issueAdded',({issue})=>{if(issue.code==='BlockedByResponseIssue'||issue.code==='CorsIssue')errors.push({type:'audit',seat,message:JSON.stringify(issue).slice(0,2200)});});cdp.on('Network.loadingFailed',e=>{if(e.type==='Document'||e.blockedReason)errors.push({type:'blocked',seat,message:JSON.stringify([e.type,e.errorText,e.blockedReason,e.corsErrorStatus]).slice(0,1200)});});cdp.on('Network.responseReceivedExtraInfo',({statusCode,headers})=>{if(headers['content-type']?.includes('text/html')||headers['Content-Type']?.includes('text/html'))errors.push({type:'htmlHeaders',seat,message:JSON.stringify([statusCode,Object.fromEntries(Object.entries(headers).filter(([k])=>/cross-origin|x-frame|content-type|content-security/i.test(k)))]).slice(0,1000)});});cdp.on('Page.frameNavigated',e=>{if(e.frame.parentId||e.frame.unreachableUrl)errors.push({type:'childNav',seat,message:JSON.stringify([e.frame.url,e.frame.unreachableUrl,e.frame.securityOrigin]).slice(0,1400)});});cdp.on('Network.responseReceived',({type,response})=>{if(type==='Document')errors.push({type:'document',seat,message:JSON.stringify([response.status,response.url,response.fromServiceWorker,response.mimeType]).slice(0,1400)});});page.on('response',r=>{if(r.status()>=400&&r.url().startsWith(base))errors.push({type:'response',message:r.status()+' '+new URL(r.url()).pathname});});
 await page.goto(base+'/?game=th128mp',{waitUntil:'domcontentloaded'});await page.waitForSelector('#gameDataImportInput',{timeout:18000});await inspect(page,'01-launcher-P'+seat);const notice=await page.waitForSelector('#firstUseNoticeCloseHint',{visible:true,timeout:5500}).catch(()=>null);if(notice){await notice.click();await page.waitForFunction(()=>!document.querySelector('#firstUseNoticeCloseHint')?.closest('dialog')?.open,{timeout:5000});}await page.waitForFunction(()=>!document.querySelector('dialog[open]'),{timeout:5000});await page.evaluate(()=>document.querySelector('#mpGamePackageImport').click());await page.waitForSelector('#gameDataImportWindow:not([hidden])',{visible:true,timeout:8000});const upl=await page.$('#gameDataImportInput');await upl.uploadFile(privatePackage);
 await until('import acknowledgement',async()=>{const s=await page.evaluate(()=>({progress:document.querySelector('#gameDataImportBusy')?.textContent?.trim(),reason:document.querySelector('#gameDataImportReason')?.textContent?.trim(),body:document.body.innerText}));return /导入成功|导入完成|已导入|游戏已就绪|安装成功|安装完成|imported|Import complete/i.test(s.body);},75000).catch(async e=>{await inspect(page,'02-import-pending-P'+seat);throw e;});
 await inspect(page,'02-import-P'+seat);return page;
 }
 const seats=Number(process.env.TH128_TEST_SEATS??1);assert.ok([1,2,3].includes(seats));
 const hostPage=await importAt(0);if(seats===1){console.log(JSON.stringify({work,profile:advertised.profile,generation:sources.generation,imported:true}));}else{
  const code=process.env.TH128_TEST_ROOM_CODE??'7402';assert.match(code,/^\d{4}$/);
  const rank=seats===3?4:1,challenge=seats===3||process.env.TH128_TEST_CHALLENGE==='1';
  function invite(i){const u=new URL(base);u.search=new URLSearchParams({game:'th128mp',mpRoom:code,fromLobby:'1',lobbyAction:i?'join':'create',lobbyPlayers:seats,lobbyDifficulty:rank,lobbyVisibility:'private'});return u.href;}
  async function joined(page,i){activePage=page;await page.goto(invite(i),{waitUntil:'domcontentloaded'});await page.waitForFunction(n=>document.querySelector('#mpSeatStage')?.dataset.playerCount===String(n),{timeout:30000},seats);await inspect(page,'03-room-P'+i);
   // A creator may already own P1. A visible empty-seat button is the author's real seat action.
   const drop=`[data-mp-seat-drop="${i}"]`;await page.waitForFunction(sel=>{const d=document.querySelector(sel);return d&&(d.hidden||!d.querySelector('button')?.disabled);},{timeout:12000},drop);
   await page.evaluate(sel=>{const d=document.querySelector(sel);if(!d.hidden)d.querySelector('button').click();},drop);
   await page.waitForFunction(sel=>document.querySelector(sel)?.hidden,{timeout:12000},drop);
  }
  await joined(hostPage,0);if(process.env.TH128_DIAG_PREP==='1'){await sleep(12000);await inspect(hostPage,'host-prep');throw Error('Diagnostic stop after the actual first-seat prepare event');}for(let i=1;i<seats;i++)await joined(await importAt(i),i);
  const watcher=process.env.TH128_TEST_VIEWER==='1'?await importAt(seats):null;const playingPages=pages.slice(0,seats);
  if(challenge){await hostPage.evaluate(()=>document.querySelector('#roomWorkspaceTab-game')?.click());await hostPage.waitForSelector('#mpRoomChallengeToggle',{timeout:6000});await hostPage.evaluate(()=>{const x=document.querySelector('#mpRoomChallengeToggle');if(x.getAttribute('aria-checked')!=='true'&&!x.checked)x.click();});}
  if(process.env.TH128_TEST_PALETTE)await configurePalettes({pages:playingPages,enabled:process.env.TH128_TEST_PALETTE==='on',until,work});
  if(process.env.TH128_TEST_FREEZE_MULTIPLIER)await configureFreezeMotivation({pages:playingPages,multiplier:Number(process.env.TH128_TEST_FREEZE_MULTIPLIER),until,work});
  const child=async p=>{const f=p.frames().find(f=>f.url().includes(`/multiplayer/${sources.generation}/th128.html`));if(!f)return null;try{const status=await f.evaluate(()=>({scene:document.querySelector('#game')?.dataset.scene,frame:Number(document.querySelector('#game')?.dataset.frame??-1),oldHud:!!document.querySelector('#coop-hud,#coop-pause'),menu:document.querySelector('#game')?.dataset.pauseMenu,confirm:document.querySelector('#game')?.dataset.pauseConfirm,resultMenu:document.querySelector('#game')?.dataset.resultMenu,helpPage:document.querySelector('#game')?.dataset.helpPage,roomMusic:document.querySelector('#game')?.dataset.roomMusic,textVisible:!document.querySelector('#stage-text')?.hidden,replayVisible:!!document.querySelector('#replay-menu:not([hidden])'),saveVisible:!!document.querySelector('#save-replay')?.getClientRects().length,epoch:Number(document.querySelector('#game')?.dataset.epoch??0),paused:document.querySelector('#game')?.dataset.sharedPause==='true',error:document.querySelector('#status')?.innerText??''}));return {f,status};}catch{return null;}};
  const title=await until('native title preview',async()=>{const t=await child(hostPage);return t?.status.scene==='title'&&t.status.frame>=72?t:null;},50000);
  assert.equal(title.status.replayVisible,false);assert.equal(title.status.saveVisible,false);
  const firstCanvas=await title.f.$('#game'),snap=await firstCanvas.screenshot({path:join(work,'title-a.png')});
  const later=await until('visible title ticks after readiness',async()=>{const t=await child(hostPage);return t?.status.scene==='title'&&t.status.frame>=title.status.frame+16?t:null;},12000);
  const next=await (await later.f.$('#game')).screenshot({path:join(work,'title-b.png')});assert.notEqual(createHash('sha256').update(snap).digest('hex'),createHash('sha256').update(next).digest('hex'),'title canvas pixels must animate');
  await writeFile(join(work,'title-animation.json'),JSON.stringify({visibleFrames:[title.status.frame,later.status.frame],pickerHidden:true,pixelsChanged:true}));
  // Wait for each actual iframe before sending Ready; room prep can revoke an early click.
  for(let i=0;i<seats;i++){
    const p=pages[i];activePage=p;await p.bringToFront();
    await until('seat native title '+i,async()=>{const t=await child(p);return t?.status.scene==='title'&&t.status.frame>=72;},65000);
    await until('Relay confirmed ready '+i,async()=>p.evaluate(()=>{
      const r=document.querySelector('#mpReady');if(!r||r.disabled)return false;
      if(/取消|Unready|Cancel/i.test(r.innerText))return true;
      if(performance.now()-(window.__coopTestReadyClick??-5000)>1500){window.__coopTestReadyClick=performance.now();r.click();}
      return false;
    }),65000);await inspect(p,'04-ready-P'+i);
  }
  await hostPage.bringToFront();
  const preview=await child(hostPage);await (await preview.f.$('#game')).click({offset:{x:90,y:90}});
  const track=await until('author title music after the actual click',async()=>{const x=await child(hostPage);return x?.status.scene==='title'&&x.status.roomMusic==='playing'?x.status:null;},30000);
  await writeFile(join(work,'room-music.json'),JSON.stringify(track));
  const proof={};let titleSource=null;
  if(audio){const sample=await readAudio(preview.f);titleSource=liveLoop(sample,audio.title);
    assert.ok(titleSource,'preparing room must actually start the author Main song (07)');assert.equal(liveLoop(sample,audio.firstStage),undefined);
    proof.titleSong07=titleSource.digest;}

  activePage=hostPage;await hostPage.waitForFunction(()=>!document.querySelector('#mpStartGame')?.disabled,{timeout:60000});await hostPage.evaluate(()=>document.querySelector('#mpStartGame').click());

  const seen=await Promise.all(playingPages.map(p=>until('shared native room render',async()=>{const x=await child(p);return x?.status.scene==='match'&&x.status.frame>=30?x.status:null;},70000)));
  for(const st of seen){assert.equal(st.oldHud,false);assert.equal(st.replayVisible,false);assert.equal(st.saveVisible,false);}await writeFile(join(work,'playing-result.json'),JSON.stringify({seats,rank,generation:sources.generation,seen},null,2));for(const [i,p] of playingPages.entries()){await inspect(p,'05-playing-P'+i);await p.screenshot({path:join(work,'05-playing-P'+i+'.png')});}
  const hostFrame=(await child(hostPage)).f;
  if(process.env.TH128_TEST_ORIGINAL_OPENING==='1') await verifyOriginalOpening({pages:playingPages,child,until,rank,work});
  if(audio){
    const sample=await readAudio(hostFrame);assert.ok(sample.stopped.some(s=>s.id===titleSource.id),'preview source stops at launch');
    const stage=liveLoop(sample,seats===3?null:audio.firstStage);assert.ok(stage,'authored first-stage BGM starts on confirmed play');
    assert.notEqual(stage.digest,audio.title);proof.firstStage=stage;
  }
  if(process.env.TH128_TEST_CONTROLS==='1'){
   const command=async(i,key)=>{activePage=pages[i];const game=await child(activePage);await game.f.focus('#game');await activePage.keyboard.press(key);};
   const common=async fn=>{const list=await Promise.all(playingPages.map(async p=>(await child(p))?.status));return list.every(s=>s&&fn(s))?list:null;};
   await command(seats-1,'Escape');const held=await until('all author peers really paused',()=>common(s=>s.paused));await sleep(350);
   for(const [i,p] of playingPages.entries()){const q=await child(p);assert.equal(q.status.menu,'resume');assert.equal(q.status.saveVisible,false);await (await q.f.$('#game')).screenshot({path:join(work,`pause-P${i+1}.png`)});}
   const same=await common(s=>s.paused);assert.deepEqual(same.map(s=>s.frame),held.map(s=>s.frame));assert.equal(new Set(held.map(s=>s.frame)).size,1);
   let frozenClock=null;
   if(audio){
     const frozen=await until('game AudioContext suspended at the confirmed pause',async()=>{
       const actual=await readAudio(hostFrame),lane=audioLane(actual,proof.firstStage);return lane?.state==='suspended'?actual:null;
     });frozenClock=audioLane(frozen,proof.firstStage).time;
     assert.ok(frozen.started.some(s=>s.digest===audio.menu[14]&&s.context!==proof.firstStage.context),'opening menu cue uses the original sample');
     const from=Math.max(...frozen.started.map(s=>s.id));
     await command(0,'ArrowDown');await command(0,'ArrowUp');
     const moved=await until('original selection SE plays on the live menu lane during pause',async()=>{
       const actual=await readAudio(hostFrame);return actual.started.find(s=>s.id>from&&s.digest===audio.menu[10])?actual:null;
     });
     const selected=moved.started.find(s=>s.id>from&&s.digest===audio.menu[10]);
     assert.equal(audioLane(moved,selected).state,'running');assert.notEqual(selected.context,proof.firstStage.context);
     assert.equal(audioLane(moved,proof.firstStage).time,frozenClock);assert.equal(liveLoop(moved,proof.firstStage.digest)?.id,proof.firstStage.id);
     proof.pause={world:frozenClock,menuCue:selected};
   }
   await command(0,'Escape');await until('another participant resumes confirmed play',()=>common(s=>!s.paused&&s.frame>held[0].frame+3));
   if(audio){
     const resumed=await until('same source continues from its frozen clock',async()=>{
       const actual=await readAudio(hostFrame),lane=audioLane(actual,proof.firstStage);return lane?.state==='running'&&lane.time>frozenClock?actual:null;
     });
     assert.equal(liveLoop(resumed,proof.firstStage.digest)?.id,proof.firstStage.id);
     assert.ok(resumed.started.some(s=>s.digest===audio.menu[7]&&s.context!==proof.firstStage.context&&s.id>proof.pause.menuCue.id));
     proof.resumedWithoutRestart=true;
   }

   await command(0,'Escape');await until('second collective pause',()=>common(s=>s.paused));await command(seats-1,'r');await until('restart confirmation',async()=>{const p=await child(pages[seats-1]);return p?.status.menu==='restart'&&p.status.confirm==='no';});await command(seats-1,'ArrowUp');await command(seats-1,'Enter');const replay=await until('joint restart on all actual canvases',()=>common(s=>s.epoch===2&&!s.paused&&s.frame>4),35000);
   assert.ok(replay.every(s=>!s.saveVisible&&!s.replayVisible));
   await writeFile(join(work,'shared-controls.json'),JSON.stringify({seats,pausedFrame:held[0].frame,epochs:replay.map(s=>s.epoch),fresh:replay.map(s=>s.frame)},null,2));
  }
  if(process.env.TH128_TEST_WIPE==='1'){
    const terminal=()=>until('authored Game Over menu for both seats',async()=>{const x=await Promise.all(playingPages.map(p=>child(p)));return x.every(r=>r?.status.resultMenu==='team-over')?x:null;},150000);
    const team=await terminal();assert.equal(new Set(team.map(r=>r.status.frame)).size,1);assert.ok(team.every(r=>!r.status.saveVisible&&!r.status.paused));
    await (await team[0].f.$('#game')).screenshot({path:join(work,'team-game-over.png')});
    const source=playingPages[0],game=team[0].f;
    if(audio){
      const over=await until('team over closes stage music',async()=>{
        const actual=await readAudio(game);return actual.stopped.some(s=>s.id===proof.firstStage.id)&&audioLane(actual,proof.firstStage).state==='suspended'?actual:null;
      });assert.equal(liveLoop(over,proof.firstStage.digest),undefined);proof.teamOverStopsSource=true;
    }

    await game.focus('#game');for(const k of ['Escape','ArrowDown','ArrowDown','Enter','Enter'])await source.keyboard.press(k);
    await until('authored manual from Game Over',async()=>{const x=await child(source);return x?.status.helpPage==='0';});
    if(audio){const moved=await readAudio(game);assert.ok(moved.started.some(s=>s.digest===audio.menu[10]&&s.context!==proof.firstStage.context));proof.gameOverMenuSound=true;}

    await source.keyboard.press('Escape');await source.keyboard.press('Escape');
    await until('Game Over after closing the manual',async()=>{const x=await child(source);return x?.status.resultMenu==='team-over'&&x.status.helpPage===''&&x.status.menu==='manual';});
    await source.keyboard.press('ArrowUp');await source.keyboard.press('Enter');
    await verifyInGameReplaySave({page:source,frame:game,work,until,gameOver:true});
    await source.keyboard.press('ArrowDown');await source.keyboard.press('ArrowDown');await source.keyboard.press('Enter');
    await until('fresh run follows completed team game over',async()=>{const x=await Promise.all(playingPages.map(p=>child(p)));return x.every(r=>r?.status.epoch===2&&r.status.frame>4&&!r.status.resultMenu);},35000);
    await writeFile(join(work,'native-game-over.json'),JSON.stringify({seats,frame:team[0].status.frame,savedSlot:2,manual:true,freshRun:true}));
  }
  if(audio)await writeFile(join(work,'native-audio-proof.json'),JSON.stringify(proof,null,2));
  const postrun=process.env.TH128_TEST_RESULTS==='1'?await verifyBrowserResult({watcher,players:playingPages,seats,invite,child,until,work,inspect}):null;
  console.log(JSON.stringify({work,realAuthorLauncher:true,seats,rank,seen,postrun,errors}));
 }
}catch(error){await writeFile(join(work,'failure.txt'),String(error?.stack??error));for(const [i,p] of pages.entries()){const detail=await p.evaluate(async()=>{const regs=(await navigator.serviceWorker.getRegistrations()).map(r=>[r.scope,r.active?.scriptURL]);const url=performance.getEntriesByType('resource').map(e=>e.name).find(x=>x.includes('/multiplayer/')&&x.includes('/th128.html?'));let child=null;if(url)try{const r=await fetch(url,{cache:'no-store'});child={status:r.status,headers:[...r.headers],body:(await r.text()).slice(0,350)};}catch(e){child=String(e);}return {isolated:crossOriginIsolated,regs,url,child,lifecycle:window.__coopTestLifecycle};}).catch(()=>null);await writeFile(join(work,'diagnostic-P'+i+'.json'),JSON.stringify(detail,null,2));await inspect(p,'failed-P'+i).catch(()=>{});await writeFile(join(work,'failed-child-P'+i+'.json'),JSON.stringify(await Promise.all(p.frames().map(async f=>({url:f.url(),text:await f.evaluate(()=>document.body?.innerText.slice(0,1200)).catch(()=>null)}))))).catch(()=>{});}if(activePage){await inspect(activePage,'failed-ui').catch(()=>{});await activePage.screenshot({path:join(work,'failed-ui.png')}).catch(()=>{});}throw error;}
finally{for(const b of browsers)await b.close();for(const {child} of children.reverse())if(child.exitCode===null&&child.signalCode===null){const exit=once(child,'exit');child.kill();await exit;}for(const [k,v] of logs)await writeFile(join(work,k+'.log'),v);await writeFile(join(work,'browser-errors.json'),JSON.stringify(errors,null,2));await writeFile(join(work,'protocol-traffic.json'),JSON.stringify(wire));if(await readFile(lease,'utf8')===token)await unlink(lease);}
