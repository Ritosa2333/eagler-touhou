// Uses the production package and shared relay/BrowserPeerTransport.
// No synthetic remote input, native simulation mutation or executable oracle.
// Negative Replay file fixtures and the offline seek ABI are labeled below.
// The real Launcher owns connection/calibration/error/return UI. This minimal
// protocol host checks Runtime signals only; frontend acceptance lives in
// eagler-touhou/tests/browser/test-th11mp-launcher.py.
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
function foreignReplayFixture(value){
  // Keep the recorded inputs and chapters intact. Recompute the published
  // TH11 gameplay ABI and common corruption checksum for one changed build
  // word, so rejection proves the Runtime fingerprint gate, not bad bytes.
  const bytes=Uint8Array.from(value),view=new DataView(bytes.buffer);
  const word=offset=>view.getUint32(offset,true);
  assert.equal(word(24),128);assert.equal(word(40),0x4d313154);
  view.setUint32(148,word(148)^0x80000000,true);
  const setup=Array.from({length:22},(_,i)=>word(80+i*4));
  const abiWords=[0x54483131,word(48),setup[1],setup[3],
    setup[8]*3+setup[9],setup[10]*3+setup[11],setup[12]*3+setup[13],
    setup[21],setup[15],setup[7],word(52),setup[16],...setup.slice(17,21)];
  let abi=2166136261;
  for(const n of abiWords)for(let shift=0;shift<32;shift+=8)
    abi=Math.imul(abi^((n>>>shift)&255),16777619)>>>0;
  view.setUint32(16,abi||1,true);
  let checksum=2166136261;
  for(let i=0;i<bytes.length;++i)if(i<36||i>=40)
    checksum=Math.imul(checksum^bytes[i],16777619)>>>0;
  view.setUint32(36,checksum,true);return Array.from(bytes);
}
const occupiedPaths=Array.from({length:99},(_,i)=>'replay/th11_'+String(i+1).padStart(2,'0')+'.rpy').concat('replay/th11_ud0000.rpy');
const pause=ms=>new Promise(accept=>setTimeout(accept,ms));
async function nativeKey(page,code){
  // Real browser keyboard events reach the existing Runtime input adapter.
  // The game's ANM/menu state machine remains the navigation authority.
  await page.frameLocator('iframe').locator('canvas').focus();
  await page.keyboard.press(code,{delay:90});
}
async function waitNative(page,predicate,label,timeout=30000){
  const deadline=Date.now()+timeout;
  while(true){
    const state=await page.evaluate(()=>host.snapshot());
    assert.ok(!state.error&&!state.shellError&&!state.events.length,JSON.stringify(state));
    if(predicate(state))return state;
    if(Date.now()>deadline)throw Error(label+': '+JSON.stringify(state));
    await pause(40);
  }
}
const nativeList=state=>state.replayUi?.[0]===1&&state.replayUi[1]===1&&state.replayUi[5]===11&&state.replayUi[6]===2;
const nativeStages=state=>state.replayUi?.[1]===1&&state.replayUi[5]===11&&state.replayUi[6]===4;
async function startNativeReplay(page){
  // The original stage menu ignores confirmation during its entrance. Retry
  // a released key only while that same menu is active; never inject gameplay.
  const deadline=Date.now()+30000;
  while(true){
    const state=await page.evaluate(()=>host.snapshot());
    assert.ok(!state.error&&!state.shellError&&!state.events.length,JSON.stringify(state));
    if(state.replayUi?.[1]===2)return state;
    if(nativeStages(state))await nativeKey(page,'KeyZ');
    if(Date.now()>deadline)throw Error('Native Replay stage did not start: '+JSON.stringify(state));
    await pause(260);
  }
}
async function captureUi(page,name,{narrow=true}={}){
  if(!uiAudit)return;
  report.uiScreenshots??=[];
  const capture=async suffix=>{
    const file=output.replace(/\.json$/,'-ui-'+name+'-'+suffix+'.png');
    await page.evaluate(()=>new Promise(accept=>requestAnimationFrame(()=>requestAnimationFrame(accept))));
    await page.screenshot({path:file});
    let canvasFile;
    if(/^replay-(?:empty|list|page-26|stages)$/.test(name)){
      canvasFile=file.replace(/\.png$/,'-canvas.png');
      await page.frameLocator('iframe').locator('canvas').screenshot({path:canvasFile});
    }
    const surface=await page.frameLocator('iframe').locator('body').evaluate(body=>{
      const doc=body.ownerDocument,win=doc.defaultView,canvas=body.querySelector('canvas');
      const bounds=canvas?.getBoundingClientRect();
      const css=[...doc.styleSheets].flatMap(sheet=>{try{return [...sheet.cssRules].map(rule=>rule.cssText);}catch{return [];}}).join('\n');
      return {text:body.innerText,focus:doc.activeElement?.outerHTML,
        canvas:bounds?{width:bounds.width,height:bounds.height,top:bounds.top,left:bounds.left,bottom:bounds.bottom}:null,
        nativeViewport:{width:win.innerWidth,height:win.innerHeight},
        legacyReplayNodes:[...body.querySelectorAll('#th11-multiplayer-menu,#th11-multiplayer-replay-controls,.th11-mp-replay-controls')].map(node=>node.id||node.className),
        legacyPlaybackClass:body.classList.contains('th11-mp-playback'),
        legacyControlsHeight:win.getComputedStyle(doc.documentElement).getPropertyValue('--th11-mp-controls-height').trim(),
        legacyCanvasRules:/th11-mp-playback|--th11-mp-controls-height|\.th11-mp-replay-controls/.test(css),
        legacyRuntimePanels:[...body.querySelectorAll('#th11-multiplayer-session-controls,#th11-multiplayer-error')]
          .map(node=>node.id),
      };
    });
    const native=await page.evaluate(()=>host.snapshot().replayUi);
    report.uiScreenshots.push({name,viewport:page.viewportSize(),file,canvasFile,surface,nativeReplayUi:native});
    if(verifyUi){
      assert.deepEqual(surface.legacyReplayNodes,[],'Replay lists and playback controls must remain native');
      assert.equal(surface.legacyPlaybackClass,false,'No playback class may shrink the native canvas');
      assert.equal(surface.legacyControlsHeight,'','No toolbar height may reserve native game space');
      assert.equal(surface.legacyCanvasRules,false,'The packaged styles must not contain the retired player layout');
      const expectedWidth=Math.min(surface.nativeViewport.width,surface.nativeViewport.height*4/3,960);
      assert.ok(surface.canvas&&Math.abs(surface.canvas.width-expectedWidth)<=1,'Native canvas must retain the available 4:3 game frame');
      assert.ok(Math.abs(surface.canvas.width/surface.canvas.height-4/3)<.01,'Native canvas aspect ratio must stay 4:3');
      assert.deepEqual(surface.legacyRuntimePanels,[],
        'Connection/error/return controls belong to the real Launcher, never this Runtime');
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
    if(seat===0)await captureUi(pages[seat],'startup-game-frame');
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
    const replayData=Uint8Array.from(bytes),header=new DataView(replayData.buffer);
    const recordedFrames=header.getUint32(32,true),chapterCount=header.getUint32(28,true);
    const firstStage=header.getUint32(40+header.getUint32(24,true),true);
    // onGameFrame observes after FrameCadence's at-most-four-tick batch.
    // Leave that exact margin so the normal cadence cannot cross EOF before
    // this harness stops it and captures the final authoritative world.
    const replayTarget=Math.min(frames,recordedFrames-4);
    assert.ok(replayTarget>=75,'Replay must contain enough confirmed gameplay before EOF');
    assert.ok(chapterCount>=1&&firstStage>=1&&firstStage<=7);
    report.nativeReplay={recordedFrames,chapterCount,firstStage,replayTarget,
      inputPath:'browser keyboard to native title/Replay menu',
      stageSelection:'existing recorded chapter; no synthetic chapter or stage-state shortcut'};
    if(uiAudit){
      const empty=await newPage('empty-playback');
      await empty.evaluate(async()=>{host.observe(0);await host.configure({replayViewer:true});await host.launch();});
      await waitNative(empty,nativeList,'Empty native Replay directory did not appear');
      await captureUi(empty,'replay-empty');
      if(verifyUi){
        await nativeKey(empty,'KeyZ');
        assert.ok(nativeList(await empty.evaluate(()=>host.snapshot())),'An empty native slot must not start playback');
      }
      await nativeKey(empty,'Escape');
      await empty.waitForFunction(()=>host.events.some(event=>event.event==='exit'),{},{timeout:30000});
    }
    const page=await newPage('playback');
    const negativeFixtures=verifyUi?{foreign:foreignReplayFixture(bytes),corrupt:bytes.slice(0,-1)}:null;
    await page.evaluate(async({bytes,replayTarget,verifyUi})=>{
      await host.configure({replayViewer:true});
      await host.request('write',{path:'replay/th11_01.rpy',bytes});
      // The second native page uses the exact same real recording. No forged
      // chapter or world snapshot is needed to prove slot 26 is reachable.
      if(verifyUi)await host.request('write',{path:'replay/th11_26.rpy',bytes});
      host.observe(replayTarget);
    },{bytes,replayTarget,verifyUi});
    if(negativeFixtures){
      const fixtures=await page.evaluate(async({foreign,corrupt})=>{
        const result={foreignStructurallyValid:host.validateReplayFixture(foreign),
          corruptStructurallyValid:host.validateReplayFixture(corrupt),imports:[]};
        for(const [path,bytes] of [['replay/th11_02.rpy',foreign],['replay/th11_03.rpy',corrupt]]){
          try{await host.request('write',{path,bytes});result.imports.push({path,accepted:true});}
          catch(error){result.imports.push({path,accepted:false,error:String(error)});}
          host.seedRejectedReplayFixture(path,bytes);
        }
        return result;
      },negativeFixtures);
      assert.equal(fixtures.foreignStructurallyValid,true,'The foreign fixture must pass the native codec before build identity filtering');
      assert.equal(fixtures.corruptStructurallyValid,false);
      assert.ok(fixtures.imports.every(result=>!result.accepted),'Host imports must reject foreign and corrupt Replays');
      report.nativeReplay.negativeFiles={...fixtures,
        source:'isolated prelaunch file fixtures for native scan, not a user import success'};
    }
    await page.evaluate(()=>host.launch());
    await waitNative(page,nativeList,'Native Replay directory did not appear');
    await captureUi(page,'replay-list');
    if(verifyUi){
      for(const row of [1,2]){
        await nativeKey(page,'ArrowDown');
        await waitNative(page,state=>nativeList(state)&&state.replayUi[7]===row,'Native Replay row navigation failed');
        await nativeKey(page,'KeyZ');
        await pause(160);
        assert.ok(nativeList(await page.evaluate(()=>host.snapshot())),
          (row===1?'Foreign-build':'Corrupt')+' stored Replay must not become a playable native menu entry');
      }
      report.nativeReplay.negativeFiles.nativeEntriesRejected=true;
      await nativeKey(page,'ArrowUp');await nativeKey(page,'ArrowUp');
      await waitNative(page,state=>nativeList(state)&&state.replayUi[7]===0,'Native Replay cursor did not return to row 1');
      await nativeKey(page,'ArrowRight');
      const second=await waitNative(page,state=>nativeList(state)&&state.replayUi[8]===1&&state.replayUi[7]===0,
        'Native Replay second page did not expose slot 26');
      report.nativeReplay.secondPage={slot:26,index:25,ui:second.replayUi,sameBytesSha256:bytesHash(bytes)};
      await captureUi(page,'replay-page-26');
    }
    const selectedIndex=verifyUi?25:0;
    await nativeKey(page,'KeyZ');
    const stages=await waitNative(page,nativeStages,'Original Replay stage menu did not appear');
    assert.equal(stages.replayUi[8],Math.floor(selectedIndex/25));
    assert.equal(stages.replayUi[7],firstStage-1,'Only a genuinely recorded starting stage may be selected');
    await captureUi(page,'replay-stages');
    await startNativeReplay(page);
    // These physical game controls are deliberately ignored by the read-only
    // Replay. Same-frame native hashes below detect any accidental injection.
    await page.evaluate(async()=>{await host.key('ArrowUp',true);await host.key('KeyX',true);});
    report.playback=await waitNative(page,state=>state.replayUi?.[1]===2&&state.net[3]>=replayTarget,
      'MP playback failed to reach the pre-EOF observation boundary',180000);
    assert.ok(report.playback.net[3]<recordedFrames,'Normal playback must stop before automatic EOF clears the world');
    assert.equal(report.playback.replayUi[9],selectedIndex,'The selected numbered slot must reach native playback');
    await page.evaluate(async()=>{await host.key('ArrowUp',false);await host.key('KeyX',false);});
    const playback=new Map(await page.evaluate(()=>[...host.snapshots]));
    const replayFrames=[...playback.keys()].filter(frame=>frame>=60&&reference.has(frame));
    assert.ok(replayFrames.length>=10);
    for(const frame of replayFrames)assert.equal(playback.get(frame).game[1],reference.get(frame).game[1],
      'MP replay diverged at logical frame '+frame);
    report.replayComparisons=replayFrames.length;
    assert.equal(report.playback.net[11],1);
    report.nativeReplay.physicalGameplayInputIgnored=true;
    console.log('TH11 '+players+'P '+route+': '+replayFrames.length+' replay frame comparisons passed');
    await captureUi(page,'replay-playback');

    // Explicit diagnostic invocation of the retained native offline seek ABI.
    // There is no user-facing timeline. The observation harness stops the
    // native loop at the target; this is not a Replay Pause UI claim.
    const seekFrame=replayFrames[Math.floor(replayFrames.length/3)];
    const seekStart=await page.evaluate(frame=>host.seekReplayDiagnostic(frame),seekFrame);
    assert.ok(seekStart.before>seekFrame,'The diagnostic seek must actually go backwards');
    assert.equal(seekStart.initial,0,'Backward seek must rebuild the native Replay from frame zero');
    const sought=await waitNative(page,state=>state.replayUi?.[1]===2&&state.net[3]===seekFrame,
      'Native diagnostic Replay seek did not stop at its target');
    assert.equal(sought.game[1],reference.get(seekFrame).game[1],'Replay seek world mismatch');
    assert.equal(sought.game[4],seekFrame);
    assert.ok((await page.evaluate(()=>[...host.snapshots.keys()])).some(value=>value<seekFrame),
      'Backward seek must expose its reconstructed earlier frames');
    await pause(120);
    assert.equal((await page.evaluate(()=>host.snapshot())).net[3],seekFrame,
      'The acceptance observer must stop exactly at the diagnostic seek target');
    report.seek={frame:seekFrame,nativeHash:sought.game[1],matched:true,reconstructedFromStart:true,
      via:'diagnostic th11_mp_replay_seek ABI',stoppedByAcceptanceObserver:true,userFacingSlider:false};
    await captureUi(page,'replay-diagnostic-seek');

    await page.evaluate(()=>host.resume(0));
    await nativeKey(page,'Escape');
    const returned=await waitNative(page,nativeList,'Physical Esc did not return to the original Replay directory');
    assert.equal(returned.replayUi[8],Math.floor(selectedIndex/25));
    assert.equal(returned.replayUi[7],selectedIndex%25,'Replay directory must remember the selected native file');
    report.nativeReplay.escapeReturn={selectedIndex,ui:returned.replayUi};
    await captureUi(page,'replay-return');
    // Start that same recorded chapter again and allow its real input tape to
    // end. No seek-to-EOF, forged completion flag or synthesized menu input.
    await page.evaluate(()=>{host.snapshots.clear();host.generations.clear();host.replayUiStates.length=0;host.observe(0);});
    await nativeKey(page,'KeyZ');
    await waitNative(page,nativeStages,'Replay stage menu did not reopen');
    await startNativeReplay(page);
    const ended=await waitNative(page,nativeList,'Native Replay EOF did not return to the directory',180000);
    assert.equal(ended.replayUi[8],Math.floor(selectedIndex/25));
    assert.equal(ended.replayUi[7],selectedIndex%25);
    assert.equal(await page.evaluate(()=>host.events.some(event=>event.event==='exit')),false,
      'Replay EOF returns to the native directory before the user exits the Runtime');
    report.nativeReplay.eof={automaticDirectoryReturn:true,recordedFrames,ui:ended.replayUi,
      transitions:await page.evaluate(()=>host.replayUiStates)};
    await captureUi(page,'replay-eof');
    await nativeKey(page,'Escape');
    await page.waitForFunction(()=>host.events.some(event=>event.event==='exit'),{},{timeout:30000});
    report.nativeReplay.launcherReturn=await page.evaluate(()=>host.events.find(event=>event.event==='exit'));
    if(verifyUi)report.uiKeyboard={native:true,menuNavigation:true,pageNavigation:true,menuPlay:true,
      stageSelection:true,menuReturn:true,automaticEofReturn:true,launcherReturn:true};
    console.log('TH11 '+players+'P '+route+': native Replay directory, stage selection, Esc, EOF and Launcher return passed; diagnostic backward seek matched '+seekFrame);
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
    assert.ok(failed.error,'A live peer disconnect must reach the standard Runtime error event');
    assert.equal(failed.net[3],before.net[3],'Disconnected world must stay frozen');
    report.disconnect={error:failed.error,frame:failed.net[3],standardErrorEvent:true,
      frontendUiOwner:'eagler-touhou/tests/browser/test-th11mp-launcher.py'};
    await captureUi(pages[0],'disconnect');
    assert.equal(await pages[0].frameLocator('iframe').locator(
      '#th11-multiplayer-session-controls,#th11-multiplayer-error').count(),0,
      'The Runtime must not recreate the frontend connection/return panel');
    console.log('TH11 '+players+'P '+route+': actual peer disconnect emitted a standard error; frontend return is verified by the real Launcher gate');
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
