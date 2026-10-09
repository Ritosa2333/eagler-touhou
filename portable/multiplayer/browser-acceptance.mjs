// Uses the production package and shared relay/BrowserPeerTransport.
// No synthetic remote input, native state mutation or executable oracle.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {spawn} from 'node:child_process';
import {readFileSync, writeFileSync, existsSync, mkdirSync} from 'node:fs';
import {resolve, extname} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createRequire} from 'node:module';
import {randomUUID,createHash} from 'node:crypto';

const root=resolve(import.meta.dirname,'../..');
const option=(name,fallback)=>{const i=process.argv.indexOf(name);return i<0?fallback:process.argv[i+1];};
const players=Number(option('--players','2')),route=option('--route','rtc');
const language=option('--language','ja');
assert.ok(['ja','lang_zh-hans'].includes(language));
const selections=option('--loadouts',Array.from({length:players},(_,seat)=>seat).join(',')).split(',').map(Number);
const frames=Number(option('--frames','600')),spectator=process.argv.includes('--spectator');
const replay=!process.argv.includes('--no-replay');
const restart=process.argv.includes('--restart');
const fullSlots=process.argv.includes('--full-replay-slots');
const disconnect=process.argv.includes('--disconnect');
const uiAudit=process.argv.includes('--ui-audit');
const verifyUi=process.argv.includes('--verify-ui');
assert.ok(!verifyUi||uiAudit,'UI verification requires current-run captures');
const automatic=process.argv.includes('--auto'),dropFirst=process.argv.includes('--drop-first-input');
const joinLag=Number(option('--spectator-lag-frames','0'));
assert.ok([2,3].includes(players));assert.ok(['rtc','relay'].includes(route));
assert.ok(selections.length===players&&selections.every(value=>Number.isInteger(value)&&value>=0&&value<6));
assert.ok(Number.isInteger(frames)&&frames>=180&&frames<=3600);
assert.ok(Number.isInteger(joinLag)&&joinLag>=0&&joinLag<frames);
assert.ok(!fullSlots||restart,'Full Replay slots are exercised at a real restart boundary');
const launcher=resolve(process.env.EAGLER_LAUNCHER_ROOT||resolve(root,'../../eagler-touhou'));
const runtime=resolve(process.env.TH11_MP_PACKAGE||resolve(root,'build-eagler-multiplayer'));
const archive=resolve(process.env.TH11_TEST_ARCHIVE||resolve(root,'../../th11-eagler/[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
const existingReplay=fullSlots?resolve(option('--existing-replay',resolve(root,'artifacts/multiplayer-browser/2p-rtc-auto.rpy'))):null;
const output=resolve(option('--output',resolve(root,'artifacts/multiplayer-browser',players+'p-'+route+(spectator?'-spectator':'')+(restart?'-restart':'')+(automatic?'-auto':'')+(dropFirst?'-repair':'')+(fullSlots?'-full-slots':'')+'.json')));
mkdirSync(resolve(output,'..'),{recursive:true});
for(const path of [archive,resolve(runtime,'runtime-files.json'),resolve(launcher,'server/netplay-relay.mjs')])
  assert.ok(existsSync(path),'Missing required fixture: '+path);
if(existingReplay)assert.ok(existsSync(existingReplay),'First run the same-build 2P RTC gate to produce an existing Replay: '+existingReplay);
const require=createRequire(resolve(launcher,'package.json'));
const {chromium}=require('playwright');
const {buildMultiplayerRuntimeOptions}=await import(pathToFileURL(resolve(launcher,'.cache/build/browser/assets/launcher/multiplayer-runtime-options.mjs')));
const {buildMultiplayerGameplayRelayUrl}=await import(pathToFileURL(resolve(launcher,'.cache/build/browser/assets/launcher/multiplayer-relay-url.mjs')));
const {PRODUCT_GAMES}=await import(pathToFileURL(resolve(launcher,'lib/contracts/product-catalog.mjs')));
const policy=PRODUCT_GAMES.th11.multiplayer;
assert.equal(policy.inputTiming.rollbackLimit,0);
const manifest=JSON.parse(readFileSync(resolve(runtime,'manifest.json')));
assert.equal(manifest.product,'th11mp');
const inventory=JSON.parse(readFileSync(resolve(runtime,'runtime-files.json')));
const html=readFileSync(resolve(import.meta.dirname,'browser-host.html'));
const report={passed:false,scope:'local Chromium production Runtime and shared relay',players,route,language,
  selections,spectator,replay,restart,disconnect,automatic,dropFirst,fullSlots,joinLag,frames,wasm:manifest.execution.sha256,errors:[],checkpoints:[]};
const bytesHash=bytes=>createHash('sha256').update(Uint8Array.from(bytes)).digest('hex');
const occupiedPaths=Array.from({length:99},(_,i)=>'replay/th11_'+String(i+1).padStart(2,'0')+'.rpy').concat('replay/th11_ud0000.rpy');
const pause=ms=>new Promise(accept=>setTimeout(accept,ms));
async function captureUi(page,name,{narrow=true}={}){
  if(!uiAudit)return;
  report.uiScreenshots??=[];
  const capture=async suffix=>{
    const file=output.replace(/\.json$/,'-ui-'+name+'-'+suffix+'.png');
    await page.evaluate(()=>new Promise(accept=>requestAnimationFrame(()=>requestAnimationFrame(accept))));
    await page.screenshot({path:file});
    const surface=await page.frameLocator('iframe').locator('body').evaluate(body=>({
      text:body.innerText,focus:body.ownerDocument.activeElement?.outerHTML,
      canvasBottom:body.querySelector('canvas')?.getBoundingClientRect().bottom,
      controls:[...body.querySelectorAll('#th11-multiplayer-menu,#th11-multiplayer-replay-controls,#th11-multiplayer-session-controls')]
        .map(node=>({id:node.id,width:node.clientWidth,scrollWidth:node.scrollWidth,height:node.clientHeight,scrollHeight:node.scrollHeight,top:node.getBoundingClientRect().top})),
    }));
    report.uiScreenshots.push({name,viewport:page.viewportSize(),file,surface});
    if(verifyUi)for(const control of surface.controls){
      assert.ok(control.scrollWidth<=control.width+1,name+' '+suffix+' must not clip or scroll horizontally');
      if(control.id==='th11-multiplayer-replay-controls')
        assert.ok(surface.canvasBottom<=control.top+1,'Replay controls must stay outside the native game frame');
    }
  };
  await capture('desktop');
  if(narrow){
    const viewport=page.viewportSize();
    const prior=await page.locator('iframe').getAttribute('style');
    await page.setViewportSize({width:390,height:844});
    await page.locator('iframe').evaluate(node=>node.style.cssText='width:100vw;height:100vh;display:block');
    await capture('portrait');
    await page.setViewportSize(viewport);
    await page.locator('iframe').evaluate((node,value)=>value===null?node.removeAttribute('style'):node.setAttribute('style',value),prior);
  }
}
let server,relay,browser;const pages=[],contexts=[];
let relayLog='';
try {
  relay=spawn(process.execPath,[resolve(launcher,'server/netplay-relay.mjs')],{
    cwd:launcher,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',
      EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',
      EAGLER_NETPLAY_RELAY_DROP_FIRST_INPUT_PER_EDGE:dropFirst?'1':'0'},
    windowsHide:true,stdio:['ignore','pipe','pipe'],
  });
  relay.stdout.on('data',bytes=>relayLog+=bytes);relay.stderr.on('data',bytes=>relayLog+=bytes);
  const relayDeadline=Date.now()+15000;
  while(!/listening ws:\/\/127\.0\.0\.1:\d+/.test(relayLog)){
    if(relay.exitCode!==null||Date.now()>relayDeadline)throw Error('Shared relay startup failed: '+relayLog);
    await pause(25);
  }
  const relayUrl=relayLog.match(/listening (ws:\/\/127\.0\.0\.1:\d+)/)[1];
  const mime={'.mjs':'text/javascript','.js':'text/javascript','.html':'text/html',
    '.json':'application/json','.wasm':'application/wasm','.css':'text/css'};
  server=createServer((req,res)=>{
    const path=new URL(req.url,'http://localhost').pathname;
    res.setHeader('Cache-Control','no-store');
    if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
    let file;
    if(path==='/data/th11')file=archive;
    else if(path.startsWith('/runtime/')){
      const name=path.slice('/runtime/'.length);
      if(name==='runtime-files.json'||Object.hasOwn(inventory.files,name))file=resolve(runtime,name);
    }
    if(!file||!existsSync(file)){res.writeHead(404);res.end('No test resource');return;}
    res.setHeader('Content-Type',mime[extname(file)]||'application/octet-stream');
    res.end(readFileSync(file));
  });
  await new Promise((accept,reject)=>{server.once('error',reject);server.listen(0,'127.0.0.1',accept);});
  const base='http://127.0.0.1:'+server.address().port;
  browser=await chromium.launch({headless:true,args:['--enable-unsafe-swiftshader',
    '--disable-background-timer-throttling','--disable-backgrounding-occluded-windows',
    '--disable-renderer-backgrounding']});
  const room='th11mp-'+randomUUID().replaceAll('-','').slice(0,12);
  const gameUrl=relayUrl+'/?room='+room+'&run=1';
  async function newPage(label){
    const context=await browser.newContext({viewport:{width:660,height:510},serviceWorkers:'block'});
    contexts.push(context);
    if(route==='relay')await context.addInitScript("Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true})");
    const page=await context.newPage();pages.push(page);
    page.on('pageerror',error=>report.errors.push({label,error:String(error)}));
    await page.goto(base);await page.evaluate(language=>{host.language=language;host.open();},language);
    await page.waitForFunction(()=>host.ready(),{},{timeout:120000});
    return page;
  }
  const identities=[];
  for(let seat=0;seat<players+Number(spectator);++seat){
    const page=await newPage('seat-'+seat),id='th11_acceptance_'+seat+'_'+randomUUID().slice(0,8);
    if(seat===0&&fullSlots){
      // Import a real same-build Replay before launch, through the existing
      // Host validator. Runtime file writes are correctly forbidden in play.
      const bytes=Array.from(readFileSync(existingReplay));
      await page.evaluate(async({paths,bytes})=>{
        for(const path of paths)await host.request('write',{path,bytes});
      },{paths:occupiedPaths,bytes});
      report.fullSlotBaseline={files:occupiedPaths.length,sha256:bytesHash(bytes)};
    }
    identities.push(id);
    await page.evaluate(([url,id,seat,count,selection])=>host.lobby(url,id,seat,count,selection),
      [gameUrl,id,seat,players,selections[seat]??0]);
  }
  for(const page of pages.slice(0,players))await page.evaluate(()=>host.readyLobby());
  await pages[0].evaluate(automatic=>host.startLobby(automatic),automatic);
  const loadouts=selections.map(selection=>policy.loadouts[selection]);
  for(let seat=0;seat<pages.length;++seat){
    const watcher=seat===players;
    if(watcher)await Promise.all(pages.slice(0,players).map((page,index)=>page.evaluate(async code=>{
      await host.key('KeyZ',true);await host.key(code,true);
    },index%2?'ArrowRight':'ArrowLeft')));
    if(watcher&&joinLag){
      const until=Date.now()+120000;
      while(true){
        const state=await pages[0].evaluate(()=>{host.pump();return host.snapshot();});
        assert.ok(!state.error&&!state.shellError,JSON.stringify(state));
        if(state.net[3]>=joinLag)break;
        if(Date.now()>until)throw Error('Live stream did not reach spectator admission target');
        await pause(25);
      }
    }
    const options=buildMultiplayerRuntimeOptions({
      url:buildMultiplayerGameplayRelayUrl(relayUrl,{product:'th11mp',roomCode:room.slice(7),runId:1,
        role:watcher?{spectator:identities[seat]}:{player:seat},memberId:'member_'+identities[seat]}),
      player:watcher?null:seat,playerCount:players,seed:1234,difficulty:1,
      challengeMode:false,inputDelay:automatic?0:2,inputDelayAuto:automatic,predictionReserve:2,
      adonisMode:1,predictionLimit:0,spectator:watcher,spectatorId:watcher?identities[seat]:'',
      spectatorCount:Number(spectator),iceServers:[],loadouts,
    },policy);
    await pages[seat].evaluate(async({options,frames,restart,watcher})=>{
      host.observe(restart&&watcher?0:frames,restart&&!watcher?1:0);
      await host.configure(options);await host.launch();
    },{options,frames,restart,watcher});
    if(seat===0)await captureUi(pages[seat],'connection');
    if(!watcher){
      await pages[seat].evaluate(()=>host.key('KeyZ',true));
      await pages[seat].evaluate(([code,down])=>host.key(code,down),[seat%2?'ArrowRight':'ArrowLeft',true]);
    }
  }
  // A watcher attempts controls through the same Host ABI as a player. Its
  // received confirmed stream, peer worlds and stage phase must remain intact.
  if(spectator){
    await pages[players].evaluate(async()=>{
      // Escape intentionally returns a viewer to the room; exercise only
      // gameplay inputs here, including a forged restart request.
      await host.key('KeyR',true);await host.key('KeyX',true);
      await host.key('ArrowUp',true);await host.key('KeyZ',true);
    });
  }
  const deadline=Date.now()+180000;let checkpoints,started=false,pauseSent=false,restartSent=false,restartInputsSent=false,lastProgress=0,pauseCaptureFrame=null;
  while(true){
    checkpoints=await Promise.all(pages.map(page=>page.evaluate(()=>{host.pump();return host.snapshot();})));
    if(Date.now()-lastProgress>5000){
      lastProgress=Date.now();writeFileSync(output.replace(/\.json$/,'.progress.json'),JSON.stringify({pauseSent,restartSent,checkpoints},null,2)+'\n');
    }
    for(const state of checkpoints)assert.ok(!state.error&&!state.shellError&&!state.events.length,JSON.stringify(state));
    assert.equal(report.errors.length,0,JSON.stringify(report.errors));
    if(!started&&checkpoints.slice(0,players).every(state=>state.calibration[1]===5)){
      started=true;report.startup=checkpoints;
      for(const state of checkpoints.slice(0,players)){
        assert.equal(state.calibration[2],129);
        assert.ok(state.calibration[3]>=96);
        assert.equal(state.calibration[10],0);
        // AdonisStartup keeps measured B in word 9. Manual D is an explicit
        // override and must not be equated with that measured estimate.
        assert.ok(state.calibration[9]>=1);
        if(automatic){
          assert.equal(state.calibration[8],state.calibration[9]);
          assert.ok(state.calibration[8]>=1&&state.calibration[8]<=9);
        }else assert.equal(state.calibration[8],2);
        assert.equal(state.net[7],state.calibration[8]);
        assert.deepEqual(state.calibration.slice(8,11),checkpoints[0].calibration.slice(8,11));
        assert.equal(state.route,route);
      }
      console.log('TH11 '+players+'P '+route+': measured startup committed, D='+checkpoints[0].calibration[8]+' P=0');
      await Promise.all(pages.slice(0,players).map((page,seat)=>page.evaluate(async code=>{
        await host.key('KeyZ',true);await host.key(code,true);
      },seat%2?'ArrowRight':'ArrowLeft')));
    }
    if(restart){
      const live=checkpoints.slice(0,players);
      if(!restartInputsSent&&live.every(state=>state.net[12]===1&&state.calibration[1]===5)){
        restartInputsSent=true;
        await Promise.all(pages.slice(0,players).map((page,seat)=>page.evaluate(async code=>{
          await host.key('KeyZ',true);await host.key(code,true);
        },seat%2?'ArrowRight':'ArrowLeft')));
      }
      if(!pauseSent&&live.every(state=>state.net[12]===0&&state.net[3]>=120)){
        pauseSent=true;await pages[0].evaluate(()=>host.key('Escape',true));
        console.log('TH11: P1 pause requested at '+live[0].net[3]);
      }
      if(pauseSent&&!restartSent&&live.every(state=>state.game[3]===2)){
        if(uiAudit){
          pauseCaptureFrame??=Math.max(...live.map(state=>state.net[3]))+18;
          if(live.some(state=>state.net[3]<pauseCaptureFrame)){await pause(25);continue;}
          await captureUi(pages[0],'pause');
        }
        restartSent=true;report.paused=checkpoints;
        await pages[0].evaluate(async()=>{await host.key('Escape',false);await host.key('KeyR',true);});
        console.log('TH11: synchronized pause observed, P1 R requested at '+live[0].net[3]);
      }
      if(live.every(state=>state.net[12]===1&&state.net[3]>=frames)&&
          (!spectator||checkpoints[players].net[13]===1))break;
    }else if(checkpoints.every(state=>state.net[3]>=frames))break;
    if(Date.now()>deadline)throw Error('Gameplay failed to reach target: '+JSON.stringify(checkpoints));
    await pause(50);
  }
  report.last=checkpoints;
  const histories=await Promise.all(pages.map(page=>page.evaluate(()=>[...host.snapshots])));
  const maps=histories.slice(0,restart?players:histories.length).map(entries=>new Map(entries));
  const common=[...maps[0].keys()].filter(frame=>frame>=60&&maps.every(map=>map.has(frame)));
  assert.ok(common.length>=10,'Insufficient same-frame browser evidence: '+common.length);
  for(const frame of common){
    const states=maps.map(map=>map.get(frame)),hash=states[0].game[1];
    for(const state of states){
      assert.equal(state.net[8],0,'Prediction must remain disabled');
      assert.equal(state.game[1],hash,'Same-frame native owner mismatch at '+frame);
      assert.equal(state.game[4],frame);
    }
  }
  report.sameFrameComparisons=common.length;
  {
    const observations=[...maps[0].values()];
    report.movedSeats=[];
    for(let seat=0;seat<players;++seat){
      if(new Set(observations.map(state=>state.game[8+seat*16+9])).size>1)
        report.movedSeats.push(seat);
    }
    assert.equal(report.movedSeats.length,players,'Every live seat must actually exercise movement');
  }
  report.checkpoints=common.filter((_,i)=>i===0||i===Math.floor(common.length/2)||i===common.length-1)
    .map(frame=>({frame,states:maps.map(map=>map.get(frame))}));
  let reference=maps[0];
  if(restart){
    assert.ok(restartSent,'P1 must request a synchronized restart');
    assert.ok(restartInputsSent,'New generation must exercise real controls');
    for(const state of checkpoints.slice(0,players)){
      assert.equal(state.net[12],1);assert.equal(state.calibration[1],5);
      assert.equal(state.calibration[2],129,'Each generation must remeasure its real input path');
    }
    reference=new Map(await pages[0].evaluate(()=>[...(host.generations.get(0)||[])]));
    if(spectator){
      const watcher=new Map(histories[players]);
      const overlap=[...watcher.keys()].filter(frame=>frame>=60&&reference.has(frame));
      assert.ok(overlap.length>=10);
      for(const frame of overlap)assert.equal(watcher.get(frame).game[1],reference.get(frame).game[1]);
      assert.equal(checkpoints[players].net[13],1,'R terminates the admitted spectator stream normally');
      report.spectatorTerminalComparisons=overlap.length;
    }
  }
  for(let seat=0;seat<players;++seat){
    assert.equal(checkpoints[seat].game[8+seat*16],1);
    assert.equal(checkpoints[seat].game[8+seat*16+11],selections[seat]);
  }
  if(spectator){
    assert.equal(checkpoints[players].net[10],1);
    assert.equal(checkpoints[players].net[15],0,'Spectator must never send player packets');
  }
  console.log('TH11 '+players+'P '+route+': '+common.length+' same-frame world comparisons passed');
  await pages[0].screenshot({path:output.replace(/\.json$/,'.png')});
  await captureUi(pages[0],'gameplay');
  if(restart&&spectator)await captureUi(pages[players],'spectator-end');
  const retiredReplayPath=fullSlots?'replay/th11_ud0001.rpy':'replay/th11_01.rpy';
  const bytes=restart?(await pages[0].evaluate(path=>host.request('read',{path}),retiredReplayPath)).bytes:
    await pages[0].evaluate(()=>host.exportReplay());
  report.replayBytes=bytes.length;assert.ok(bytes.length>128);
  writeFileSync(output.replace(/\.json$/,'.rpy'),Uint8Array.from(bytes));
  await pages[0].evaluate(()=>host.request('sync'));
  const files=await pages[0].evaluate(()=>host.request('list'));
  assert.ok(files.files.some(file=>/^replay\/th11_\d{2}\.rpy$/.test(file.path)));
  assert.ok(files.files.every(file=>/^replay\/th11_(?:\d{2}|ud[a-z0-9]{4})\.rpy$/.test(file.path)));
  report.files=files.files;
  if(fullSlots){
    const existing=await pages[0].evaluate(async paths=>{
      const result=[];for(const path of paths)result.push({path,bytes:(await host.request('read',{path})).bytes});return result;
    },occupiedPaths);
    for(const file of existing)assert.equal(bytesHash(file.bytes),report.fullSlotBaseline.sha256,
      'Automatic Replay storage must preserve '+file.path);
    const oldHash=bytesHash(bytes);
    assert.equal(bytesHash((await pages[0].evaluate(path=>host.request('read',{path}),retiredReplayPath)).bytes),oldHash);
    await pages[0].evaluate(()=>host.request('sync'));
    const again=await pages[0].evaluate(()=>host.request('list'));
    assert.deepEqual(again.files.map(file=>file.path).sort(),files.files.map(file=>file.path).sort(),
      'Repeated flush must reuse the current generation automatic path');
    assert.equal(bytesHash((await pages[0].evaluate(path=>host.request('read',{path}),retiredReplayPath)).bytes),oldHash);
    report.fullSlots={preservedFiles:existing.length,retiredReplayPath,retiredReplaySha256:oldHash,repeatedFlushReusedPath:true};
    console.log('TH11: 99 numbered and 1 imported Replay preserved; overflow Replay and repeated flush passed');
  }
  if(replay){
    if(uiAudit){
      const empty=await newPage('empty-playback');
      await empty.evaluate(async()=>{await host.configure({replayViewer:true});await host.launch();});
      await captureUi(empty,'replay-empty');
    }
    const page=await newPage('playback');
    await page.evaluate(async({bytes,frames,restart})=>{
      await host.configure({replayViewer:true});
      await host.request('write',{path:'replay/th11_01.rpy',bytes});
      host.observe(restart?0:frames);await host.launch();
    },{bytes,frames,restart});
    const frame=page.frameLocator('iframe');
    await captureUi(page,'replay-list');
    if(verifyUi){
      const selected=frame.locator('#th11-multiplayer-menu').getByRole('button',{name:/th11_01\.rpy/});
      assert.equal(await selected.evaluate(node=>node===node.ownerDocument.activeElement),true,'Replay launch must retain menu focus');
      await page.keyboard.press('ArrowDown');
      assert.equal(await frame.locator('#th11-multiplayer-menu').getByRole('button',{name:/^(Back|返回)$/}).evaluate(node=>node===node.ownerDocument.activeElement),true);
      await page.keyboard.press('ArrowUp');await page.keyboard.press('KeyZ');
    }else await frame.locator('#th11-multiplayer-menu').getByRole('button',{name:/th11_01\.rpy/}).click();
    const replayDeadline=Date.now()+180000;
    while(true){
      const state=await page.evaluate(()=>host.snapshot());
      assert.ok(!state.error&&!state.shellError&&!state.events.length,JSON.stringify(state));
      if(restart?state.net[13]===1:state.net[3]>=frames){report.playback=state;break;}
      if(Date.now()>replayDeadline)throw Error('MP playback timeout: '+JSON.stringify(state));
      await pause(50);
    }
    const playback=new Map(await page.evaluate(()=>[...host.snapshots]));
    const replayFrames=[...playback.keys()].filter(frame=>frame>=60&&reference.has(frame));
    assert.ok(replayFrames.length>=10);
    for(const frame of replayFrames)assert.equal(playback.get(frame).game[1],reference.get(frame).game[1],
      'MP replay diverged at logical frame '+frame);
    report.replayComparisons=replayFrames.length;
    assert.equal(report.playback.net[11],1);
    console.log('TH11 '+players+'P '+route+': '+replayFrames.length+' replay frame comparisons passed');
    // Seek through the real Runtime control. Its handler rebuilds from frame
    // zero and uses the ordinary native begin/update/draw path, never a state
    // snapshot. Clear observations so pre-seek data cannot satisfy this gate.
    const controls=frame.locator('#th11-multiplayer-replay-controls');
    const pauseControl=controls.locator('[data-action="pause"]');
    if((await pauseControl.getAttribute('aria-pressed'))==='false'||(await pauseControl.textContent())==='Pause')await pauseControl.click();
    const seekFrame=replayFrames[Math.floor(replayFrames.length/3)];
    await page.evaluate(()=>host.snapshots.clear());
    await controls.locator('input[type="range"]').evaluate((input,value)=>{
      input.value=String(value);input.dispatchEvent(new Event('change',{bubbles:true}));
    },seekFrame);
    const seekDeadline=Date.now()+30000;
    let sought;
    while(true){
      sought=await page.evaluate(()=>host.snapshot());
      assert.ok(!sought.error&&!sought.shellError&&!sought.events.length,JSON.stringify(sought));
      if(sought.net[3]===seekFrame)break;
      if(Date.now()>seekDeadline)throw Error('MP Replay seek timeout: '+JSON.stringify(sought));
      await pause(25);
    }
    assert.equal(sought.game[1],reference.get(seekFrame).game[1],'Replay seek world mismatch');
    assert.equal(sought.game[4],seekFrame);
    assert.ok((await page.evaluate(()=>[...host.snapshots.keys()])).some(value=>value<seekFrame),
      'Backward seek must reconstruct earlier frames');
    await pause(120);
    assert.equal((await page.evaluate(()=>host.snapshot())).net[3],seekFrame,'Replay remains paused after seek');
    report.seek={frame:seekFrame,nativeHash:sought.game[1],matched:true,reconstructedFromStart:true};
    if(verifyUi){
      const range=controls.locator('input[type="range"]');
      await pauseControl.focus();await page.keyboard.press('Tab');
      assert.equal(await controls.locator('select').evaluate(node=>node===node.ownerDocument.activeElement),true,'Replay toolbar must allow Tab navigation');
      await page.keyboard.press('Tab');
      assert.equal(await range.evaluate(node=>node===node.ownerDocument.activeElement),true);
      const prior=Number(await range.inputValue());
      await page.keyboard.press('ArrowRight');
      await page.waitForFunction(value=>host.snapshot().net[3]===value,prior+1,{timeout:30000});
      assert.equal(Number(await range.inputValue()),prior+1,'Replay range must accept native keyboard changes');
      report.uiKeyboard={menuNavigation:true,menuPlay:true,tabNavigation:true,rangeSeek:true};
    }
    await captureUi(page,'replay-seek');
    if(verifyUi){
      await page.keyboard.press('Escape');
      const selected=frame.locator('#th11-multiplayer-menu').getByRole('button',{name:/th11_01\.rpy/});
      assert.equal(await selected.evaluate(node=>node===node.ownerDocument.activeElement),true,'Returning to the Replay list must restore the selected file');
      report.uiKeyboard.menuReturn=true;
    }
    console.log('TH11 '+players+'P '+route+': backward Replay seek matched frame '+seekFrame);
  }
  assert.equal(report.errors.length,0,JSON.stringify(report.errors));
  if(disconnect){
    const before=await pages[0].evaluate(()=>host.snapshot());
    // Close the other real browser context; do not fabricate a transport flag
    // or invoke native failure entry points. The Runtime must surface it.
    await contexts[1].close();
    await pages[0].waitForFunction(()=>{
      const state=host.snapshot();
      return state.error&&state.events.some(value=>value.event==='error');
    },{},{timeout:30000});
    const failed=await pages[0].evaluate(()=>host.snapshot());
    assert.ok(failed.error,'A live peer disconnect must be visible');
    assert.equal(failed.net[3],before.net[3],'Disconnected world must stay frozen');
    await pages[0].frameLocator('iframe').getByRole('heading',{name:/^(Multiplayer stopped|多人运行已停止)$/}).waitFor();
    report.disconnect={error:failed.error,frame:failed.net[3],visible:true};
    await captureUi(pages[0],'disconnect');
    if(verifyUi){
      await pages[0].frameLocator('iframe').getByRole('button',{name:/^(Back to room|返回房间)$/}).click();
      await pages[0].waitForFunction(()=>host.events.some(event=>event.event==='exit'),{},{timeout:30000});
      const room=await pages[0].evaluate(()=>({open:host.lobbyState.socket.readyState===WebSocket.OPEN,
        member:host.lobbyState.room.seats.some(seat=>seat?.clientId===host.lobbyState.id)}));
      assert.deepEqual(room,{open:true,member:true},'Runtime exit must preserve the Launcher-owned room connection and membership');
      report.disconnect.returnedWithRoomMembership=true;
    }
    console.log('TH11 '+players+'P '+route+': actual peer disconnect reported visibly');
  }
  report.passed=true;
} catch(error) {
  report.failure=error.stack||String(error);
  report.last=await Promise.all(pages.map(page=>page.evaluate(()=>host.snapshot()).catch(error=>({error:String(error)}))));
  throw error;
} finally {
  report.relayLog=relayLog;
  writeFileSync(output,JSON.stringify(report,null,2)+'\n');
  await browser?.close();
  if(relay&&relay.exitCode===null)relay.kill();
  if(server)await new Promise(accept=>server.close(accept));
}
console.log('PASS '+output);
