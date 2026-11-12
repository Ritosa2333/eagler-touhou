import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url)),puppeteer=require('puppeteer-core');
const base=process.env.PC98_LAUNCHER_URL||'http://127.0.0.1:18330/';
const out=resolve(import.meta.dirname,'../.cache/launcher-room');await mkdir(out,{recursive:true});
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
const pages=[],errors=[],events=[[],[]],touchQa=process.env.TH03_ROOM_TOUCH_QA==='1';
const sharedContext=process.env.PC98_SAME_BROWSER==='1'?await browser.createBrowserContext():null;
async function navigate(page,url){
  if(sharedContext)await page.bringToFront();
  await page.goto(url);await page.waitForFunction(()=>/lobby\.html/.test(location.pathname)?!document.documentElement.hasAttribute('data-lobby-boot'):window.__eaglerBoot?.done===true,{timeout:45000});
  await page.evaluate(()=>document.querySelector('#firstUseNoticeDialog')?.close());
}
async function open(seat,url){
  const context=sharedContext??await browser.createBrowserContext(),page=await context.newPage();pages.push(page);await page.setViewport({width:Number(process.env.TH03_TEST_WIDTH||1400),height:900});
  page.on('pageerror',e=>errors.push(e.message));
  if(touchQa)await page.evaluateOnNewDocument(()=>{Object.defineProperty(navigator,'maxTouchPoints',{value:5,configurable:true});localStorage.setItem('eagler-touhou-game-options-v1-th03',JSON.stringify({options:{touchEnabled:true,touchMovementMode:'touch',th03RapidFire:true}}));});
  await page.exposeFunction('recordRuntimeEvent',m=>events[seat].push(m));
  await page.evaluateOnNewDocument(()=>{addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')window.recordRuntimeEvent(e.data);});});
  await navigate(page,url);return page;
}
async function wait(check,label,ms=120000){const until=Date.now()+ms;let lastLog=0;while(Date.now()<until){
  for(const page of pages)await page.evaluate(()=>{document.querySelector('#decisionDialog[open] #decisionConfirm')?.click();document.querySelector('#touchHelpClose')?.click();});
  if(await check())return;
  if(Date.now()-lastLog>10000){console.log(label,await Promise.all(pages.map(page=>page.evaluate(()=>({status:document.querySelector('#mpRoomResourceLabel')?.textContent,ready:document.querySelector('#mpReady')?.disabled})))));lastLog=Date.now();}
  await new Promise(r=>setTimeout(r,200));
}throw Error(label+' timeout');}
try{
  for(const round of ['check','match-1','match-2']){
  for(const list of events)list.length=0;
  const host=pages[0]??await open(0,base+'lobby.html?test=th03');
  if(round!=='match-2'){
  if(round!=='check')await navigate(host,base+'lobby.html?test=th03');
  await host.waitForFunction(()=>document.querySelector('#createButton')?.disabled===false);await host.click('#createButton');
  await host.waitForSelector('#roomDialog[open]');await host.select('#gameSelect','th03mp');await host.click('#submitRoom');
  await host.waitForSelector('#mpRoomCode',{timeout:45000});const code=await host.$eval('#mpRoomCode',e=>e.textContent);
  console.log('Created through main lobby',code);
  const joinUrl=base+'?'+new URLSearchParams({game:'th03mp',mpRoom:code,lobbyAction:'join',test:'th03'});
  const guest=pages[1]??await open(1,joinUrl);if(round!=='check')await navigate(guest,joinUrl);
  await guest.waitForSelector('#mpSeatStage',{timeout:45000});
  for(const [seat,page] of pages.entries()){
    const join=await page.$('[data-mp-seat-drop="'+seat+'"] button');
    if(join&&await join.evaluate(e=>!e.closest('[hidden]')&&!e.disabled))await join.click();
  }
  }
  await wait(async()=>{const e=events.flat().filter(e=>e.event==='error');if(e.length)throw Error(JSON.stringify(e));return(await Promise.all(pages.map(page=>page.evaluate(()=>document.querySelector('#mpReady')?.disabled===false)))).every(Boolean);},'main room preparation');
  assert(await host.$eval('#mpRoomChallengeToggle',e=>!!e.closest('[hidden]')),'unsupported challenge rules must not be offered');
  if(round==='check'){
    const preview=host.frames().find(f=>/th03.html.*runtimeEpoch=/.test(f.url()));
    assert(preview,'room preview runtime is present');
    assert.equal(await preview.evaluate(async()=>{const api=(await import('./managed-native-solo.mjs')).managedAPI;api.unlock();return api.health().audio.state;}),'suspended','room preview remains silent after an unlock attempt');
    await host.bringToFront();
    await host.click('[data-mp-seat="0"] .mp-seat-edit');await host.waitForSelector('#mpCheckGame',{visible:true});await host.click('#mpCheckGame');
    await wait(async()=>{const failure=events[0].find(e=>e.event==='error');if(failure)throw Error(JSON.stringify(failure));return events[0].some(e=>e.event==='first-frame');},'main room real native preflight');
    const frame=host.frames().find(f=>/th03.html.*runtimeEpoch=/.test(f.url()));
    assert(await frame.evaluate(()=>!globalThis.__th03PeerTransport),'game check must not open gameplay transport');
    await host.screenshot({path:resolve(out,'preflight.png')});console.log('PASS main room check presents native title without gameplay transport');
  }else{
  if(round==='match-1')await host.$eval('#mpTh03DisableFocus',button=>button.click());
  for(const page of pages)await page.click('#mpReady');
  if(sharedContext)await host.bringToFront();
  await host.waitForFunction(()=>document.querySelector('#mpStartGame')?.disabled===false,{timeout:30000});
  await host.click('#mpStartGame');
  await wait(async()=>{const e=events.flat().filter(e=>e.event==='error');if(e.length)throw Error(JSON.stringify(e));return events.every(list=>list.some(e=>e.event==='first-frame'));},'main room native launch');
  const frames=pages.map(page=>page.frames().find(f=>/th03.html.*runtimeEpoch=/.test(f.url())));
  if(process.env.TH03_NETWORK_DELAY_MS)for(const frame of frames)await frame.evaluate(delay=>{
    const peer=[...globalThis.__th03PeerTransport.peers.values()][0],channel=peer.channel,send=channel.send.bind(channel);
    channel.send=text=>setTimeout(()=>{if(channel.readyState==='open')send(text);},delay);
  },Number(process.env.TH03_NETWORK_DELAY_MS));
  assert((await Promise.all(frames.map(frame=>frame.evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().scene)))).every(scene=>scene===3),'room launch starts at native character selection');
  for(const frame of frames)assert(await frame.evaluate(()=>!!document.querySelector('#canvas')&&!document.querySelector('.masthead,.mode-nav')),'managed room uses its own game surface');
  await wait(async()=>{const states=await Promise.all(frames.map(frame=>frame.evaluate(async()=>{const m=await import('./managed-native-room.mjs');return m.managedAPI.health();})));return states.every(s=>s.confirmed>=120&&s.frames>0);},'main room confirmed gameplay');
  const selectionStart=await Promise.all(frames.map(frame=>frame.evaluate(async()=>{const s=(await import('./managed-native-room.mjs')).managedAPI.health();return {frame:s.gameFrame,at:performance.now()};})));
  await new Promise(resolve=>setTimeout(resolve,3000));
  const selectionFps=await Promise.all(frames.map((frame,i)=>frame.evaluate(async start=>{const s=(await import('./managed-native-room.mjs')).managedAPI.health();return 1000*(s.gameFrame-start.frame)/(performance.now()-start.at);},selectionStart[i])));
  console.log('selection simulation fps',selectionFps);assert(selectionFps.every(fps=>fps>45),'character selection should advance near the native tick rate');
  await wait(async()=>(await Promise.all(pages.map(page=>page.evaluate(()=>{const e=document.querySelector('.room-workspace-preparation');return !e||e.hidden})))).every(Boolean),'main room loading cover must close',10000);
  for(const [seat,page] of pages.entries())await page.screenshot({path:resolve(out,'player-'+seat+'.png')});
  for(const frame of frames)assert(await frame.$('#pc98-game-fps'),'game FPS counter appears during multiplayer');
  assert.deepEqual(errors,[]);console.log('PASS main Launcher create/join/seat/preload/ready/start and native multiplayer');
  if(round==='match-1'){
    await host.bringToFront();
    await frames[0].waitForFunction(()=>__eaglerKeyboardInputBridge.active(),{timeout:5000});
    await host.keyboard.down('ArrowRight');
    await wait(()=>frames[0].evaluate(()=>__eaglerKeyboardInputBridge.keyDown('ArrowRight')),'mapped keyboard down',5000);
    await host.keyboard.up('ArrowRight');
    await wait(()=>frames[0].evaluate(()=>!__eaglerKeyboardInputBridge.keyDown('ArrowRight')),'mapped keyboard release',5000);
    await wait(async()=>(await Promise.all(frames.map(frame=>frame.evaluate(()=>(window.th03SyncState?.selecting===true))))).every(Boolean),'both players reach character select',30000);
    await new Promise(resolve=>setTimeout(resolve,2500));
    for(const [seat,page] of pages.entries()){
      if(await frames[seat].evaluate(()=>window.th03SyncState?.phase===1))continue;
      await page.bringToFront();
      await page.keyboard.down('KeyZ');
      await wait(()=>frames[seat].evaluate(()=>__eaglerKeyboardInputBridge.keyDown('KeyZ')),'seat '+seat+' shot bridge',5000);
      await new Promise(resolve=>setTimeout(resolve,50));await page.keyboard.up('KeyZ');
    }
    try { await wait(async()=>(await Promise.all(frames.map(frame=>frame.evaluate(()=>window.th03SyncState?.phase===1)))).every(Boolean),'both players select and enter battle',45000); }
    catch(error){console.log('native state',await Promise.all(frames.map(frame=>frame.evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health()))));throw error;}
    for(let i=0;i<20;i++){
      await new Promise(resolve=>setTimeout(resolve,50));
      for(const frame of frames){const state=await frame.evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health());
        assert(state.confirmedInput.length===2&&state.confirmedInput.every(value=>!(value&32)),'entry without Z must not shoot');}
    }
    console.log('PASS match entry does not retain selection fire');
    if(touchQa){
      if(!await host.evaluate(()=>!!document.fullscreenElement))await host.$eval('#fullscreenToggle',el=>el.click());
      await host.waitForFunction(()=>!!document.fullscreenElement,{timeout:10000});
      await host.evaluate(()=>document.exitFullscreen());
      await host.waitForFunction(()=>!document.fullscreenElement&&document.querySelector('#player')?.classList.contains('room-workspace'),{timeout:10000});
      await new Promise(resolve=>setTimeout(resolve,500));
      await host.screenshot({path:resolve(out,'touch-after-fullscreen.png')});
      const layout=await host.evaluate(()=>{const rect=id=>{const r=document.getElementById(id).getBoundingClientRect();return {left:r.left,top:r.top,right:r.right,bottom:r.bottom};};return {classes:document.getElementById('player').className,frame:rect('gameFrame'),surface:rect('touchDirectSurface'),fire:rect('touchFire'),bomb:rect('touchBomb'),escape:rect('touchEscape')};});
      assert.match(layout.classes,/\btouch-enabled\b/,'touch styles survive leaving fullscreen');
      for(const name of ['surface','fire','bomb','escape']){const r=layout[name],f=layout.frame;assert(r.left>=f.left-1&&r.top>=f.top-1&&r.right<=f.right+1&&r.bottom<=f.bottom+1,`${name} must remain inside the game after exiting fullscreen`);}
      if(!await host.$eval('#player',el=>el.classList.contains('room-workspace'))){await host.$eval('#fullscreenToggle',el=>el.click());await host.waitForFunction(()=>document.querySelector('#player')?.classList.contains('room-workspace'),{timeout:10000});}
      const ui=await host.evaluate(()=>{const player=document.querySelector('#player'),surface=document.querySelector('#touchDirectSurface'),fire=document.querySelector('#touchFire'),frame=document.querySelector('#gameFrame'),r=frame.getBoundingClientRect(),x=r.left+r.width*.7,y=r.top+r.height*.6;return {classes:player.className,options:localStorage.getItem('eagler-touhou-game-options-v1-th03'),active:player.classList.contains('th03-room-touch-active'),surface:getComputedStyle(surface).display,fire:getComputedStyle(fire.closest('.touch-hud')).display,pressed:fire.getAttribute('aria-pressed'),hit:document.elementFromPoint(x,y)?.id,rect:{x:r.left,y:r.top,w:r.width,h:r.height}};});
      console.log('room touch state',ui);assert(ui.active&&ui.surface!=='none'&&ui.fire!=='none','room touch controls must be visible');assert.equal(ui.pressed,'false','TH03 touch fire starts released');assert.equal(ui.hit,'touchDirectSurface','drag surface owns game area');
      const fire=await host.$('#touchFire'),fb=await fire.boundingBox(),cdp=await host.createCDPSession();
      const tap=async(x,y)=>{await cdp.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[{x,y,id:0}]});await cdp.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});};
      await tap(fb.x+fb.width/2,fb.y+fb.height/2);await wait(async()=>host.$eval('#touchFire',el=>el.getAttribute('aria-pressed')==='true'),'touch fire enabled',5000);
      await tap(fb.x+fb.width/2,fb.y+fb.height/2);await wait(async()=>host.$eval('#touchFire',el=>el.getAttribute('aria-pressed')==='false'),'touch fire disabled',5000);
      await new Promise(r=>setTimeout(r,500));for(const frame of frames){const state=await frame.evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health());assert(state.confirmedInput.every(n=>!(n&32)),'touch toggle release stops firing');}
      const x=ui.rect.x+ui.rect.w*.7,y=ui.rect.y+ui.rect.h*.6,before=await frames[0].evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().players.find(p=>p.seat===0)?.x);
      await cdp.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[{x,y,id:1}]});await cdp.send('Input.dispatchTouchEvent',{type:'touchMove',touchPoints:[{x:x+100,y,id:1}]});await new Promise(r=>setTimeout(r,400));await cdp.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});
      await wait(async()=>frames[0].evaluate(async old=>(await import('./managed-native-room.mjs')).managedAPI.health().players.find(p=>p.seat===0)?.x>old,before),'real room touch moves player',10000);await cdp.detach();console.log('PASS room touch surface and released fire');
    }
    await host.bringToFront();await host.keyboard.down('Space');
    await wait(async()=>frames[0].evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().localHitPoint),'local focus point appears',10000);
    await frames[0].evaluate(()=>__eaglerPC98Options().focusHitboxEnabled=false);
    await wait(async()=>frames[0].evaluate(async()=>!(await import('./managed-native-room.mjs')).managedAPI.health().localHitPoint),'local focus point hidden',10000);
    await frames[0].evaluate(()=>__eaglerPC98Options().alwaysHitbox=true);await host.keyboard.up('Space');
    await wait(async()=>frames[0].evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().localHitPoint),'local always point remains visible',10000);
    await frames[0].evaluate(()=>{__eaglerPC98Options().alwaysHitbox=false;__eaglerPC98Options().focusHitboxEnabled=true;});console.log('PASS room focus and local point switches');
    const touchChecks=await frames[0].evaluate(()=>{
      const input=__eaglerPC98Input,options=__eaglerPC98Options();options.touchEnabled=false;
      input.touch({fireEnabled:true,focusEnabled:true,bombSerial:1,escapeSerial:1,joystickX:1,joystickY:1});
      input.pointer({type:'down',id:90,x:.2,y:.5});input.pointer({type:'up',id:90,x:.2,y:.5});
      const disabled=input.sample({play:false});options.touchEnabled=true;options.touchFocusMode='two-finger';
      input.pointer({type:'down',id:90,x:.2,y:.5});input.pointer({type:'down',id:91,x:.3,y:.5});
      const focus=input.sample({play:true});input.cancelTouch();options.touchEnabled=false;return {disabled,focus};
    });
    assert.equal(touchChecks.disabled,0,'disabled touch must not inject buttons');
    assert.ok(touchChecks.focus&64,'second finger enables focus');
    console.log('PASS disabled touch and two-finger focus');
    for(const [seat,page] of pages.entries()){
      await page.bringToFront();
      const readX=()=>frames[seat].evaluate(async seat=>(await import('./managed-native-room.mjs')).managedAPI.health().players.find(p=>p.seat===seat)?.x,seat);
      const x=await readX();assert.notEqual(x,undefined,'native player is visible');
      await page.keyboard.down('ArrowLeft');
      await wait(async()=>{const next=await readX();return next!==undefined&&next<x;},'P'+(seat+1)+' native player moves',15000);
      await page.keyboard.up('ArrowLeft');
    }
    if(!touchQa){await frames[0].evaluate(()=>{__eaglerPC98Options().touchEnabled=true;const input=__eaglerPC98Input;input.pointer({type:'down',id:91,x:.2,y:.5});input.pointer({type:'move',id:91,x:.3,y:.5});});
    const touchStart=await frames[0].evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().players.find(p=>p.seat===0)?.x);
    await frames[0].evaluate(()=>__eaglerPC98Input.pointer({type:'move',id:91,x:.4,y:.5}));
    await wait(async()=>{const x=await frames[0].evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().players.find(p=>p.seat===0)?.x);return x>touchStart;},'touch drag moves P1',15000);
    await frames[0].evaluate(()=>{__eaglerPC98Input.pointer({type:'up',id:91,x:.4,y:.5});__eaglerPC98Input.cancelTouch();__eaglerPC98Options().touchEnabled=false;});
    console.log('PASS touch drag moves the native player');}
    for(const frame of frames)await frame.evaluate(()=>{
      const peer=[...globalThis.__th03PeerTransport.peers.values()][0],channel=peer.channel,send=channel.send.bind(channel);
      channel.send=text=>{const data=JSON.parse(text);if(data.type==='native-rollback-input'&&data.frame%17===0)setTimeout(()=>{if(channel.readyState==='open')send(text);},80);else send(text);};
    });
    for(const page of pages){await page.bringToFront();await page.keyboard.down('KeyZ');await new Promise(resolve=>setTimeout(resolve,500));await page.keyboard.up('KeyZ');}
    await wait(async()=>(await Promise.all(frames.map(frame=>frame.evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().rollbacks>0)))).every(Boolean),'both clients roll back delayed input',30000);
    console.log('PASS delayed input triggers rollback on both clients');
    const start=await Promise.all(frames.map(frame=>frame.evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().confirmed)));
    for(let i=0;i<10;i++){
      await wait(async()=>(await Promise.all(frames.map((frame,seat)=>frame.evaluate(async start=>{const s=(await import('./managed-native-room.mjs')).managedAPI.health();return s.confirmed>=start&&s.confirmedInput.every(value=>!(value&32));},start[seat]+i+1)))).every(Boolean),'released rapid fire remains released',15000);
    }
    console.log('PASS release stops rapid fire after rollback');
    if(touchQa)await frames[0].evaluate(()=>__eaglerPC98Options().touchEnabled=true);
    await host.bringToFront();await host.keyboard.down('Escape');await new Promise(r=>setTimeout(r,300));await host.keyboard.up('Escape');
    await wait(async()=>(await Promise.all(frames.map(frame=>frame.evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().paused)))).every(Boolean),'synchronized native pause',15000);
    assert((await Promise.all(frames.map(frame=>frame.evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().pause.owner)))).every(owner=>owner===0));
    await pages[1].bringToFront();await pages[1].keyboard.press('ArrowDown',{delay:250});await new Promise(r=>setTimeout(r,500));
    assert.equal(await frames[0].evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().pause.choice),0,'other player cannot control menu');
    await host.bringToFront();await host.keyboard.press('ArrowDown',{delay:250});
    await wait(async()=>(await Promise.all(frames.map(frame=>frame.evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().pause.choice)))).every(choice=>choice===1),'synchronized pause selection',10000);
    await host.screenshot({path:resolve(out,'native-pause-menu.png')});
    const box=await (await frames[0].$('#canvas')).boundingBox();assert(await frames[0].evaluate(()=>document.querySelector('#cover').hidden),'game cover is hidden');if(touchQa){const scale=Math.min(box.width/640,box.height/400),x=box.x+(box.width-640*scale)/2+160*scale,y=box.y+(box.height-400*scale)/2+120*scale,cdp=await host.createCDPSession();await cdp.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[{x,y,id:3}]});await cdp.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});await cdp.detach();}else await host.mouse.click(box.x+box.width*160/640,box.y+box.height*120/400);
    await wait(async()=>(await Promise.all(frames.map(frame=>frame.evaluate(async()=>!(await import('./managed-native-room.mjs')).managedAPI.health().paused)))).every(Boolean),'synchronized native resume',15000);
    console.log('PASS native pause ownership, selection and mouse resume');
    await pages[1].bringToFront();await pages[1].keyboard.press('Escape',{delay:250});
    await wait(async()=>(await Promise.all(frames.map(frame=>frame.evaluate(async()=>{const s=(await import('./managed-native-room.mjs')).managedAPI.health();return s.pause.owner===1&&s.pause.allowed;})))).every(Boolean),'guest pause for surrender',15000);
    await pages[1].keyboard.press('ArrowDown',{delay:250});await pages[1].keyboard.press('KeyZ',{delay:250});
    await wait(async()=>(await Promise.all(frames.map(frame=>frame.evaluate(async()=>{const s=(await import('./managed-native-room.mjs')).managedAPI.health();return !s.paused&&s.players.length<2;})))).every(Boolean),'round surrender settles',15000);
    console.log('PASS guest round surrender');
    await wait(async()=>(await Promise.all(frames.map(frame=>frame.evaluate(async()=>{const s=(await import('./managed-native-room.mjs')).managedAPI.health();return s.scene===9&&s.players.length===2;})))).every(Boolean),'next native round',30000);
    await pages[1].keyboard.press('Escape',{delay:250});
    await wait(async()=>(await Promise.all(frames.map(frame=>frame.evaluate(async()=>{const s=(await import('./managed-native-room.mjs')).managedAPI.health();return s.pause.owner===1&&s.pause.allowed;})))).every(Boolean),'guest pause for match surrender',15000);
    await frames[1].evaluate(()=>{__eaglerPC98Options().touchEnabled=true;__eaglerPC98Input.pointer({type:'down',id:92,x:480/640,y:212/400});});
    await wait(async()=>(await Promise.all(frames.map(frame=>frame.evaluate(async()=>{const s=(await import('./managed-native-room.mjs')).managedAPI.health();return !s.paused&&s.players.length<2;})))).every(Boolean),'touch match surrender settles',15000);
    await wait(async()=>(await Promise.all(frames.map(frame=>frame.evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().scene!==9)))).every(Boolean),'match result transition',30000);
    for(let i=0;i<25;i++){for(const page of pages){await page.bringToFront();await page.keyboard.press('KeyZ',{delay:120});}await new Promise(r=>setTimeout(r,500));if(await frames[0].evaluate(()=>[...__eaglerPC98Files.memory.keys()].some(k=>k.endsWith('.th03r'))))break;}
    await wait(async()=>frames[0].evaluate(()=>[...__eaglerPC98Files.memory.keys()].some(k=>k.endsWith('.th03r'))),'native replay persisted',15000);
    const results=events[0].filter(e=>e.event==='runtime-info'&&e.th03Match?.winner!==undefined);assert.equal(results.length,1,'one score update for completed match');assert.equal(results[0].th03Match.winner,0,'guest match surrender gives host the win');
    const lobby=await browser.newPage();await navigate(lobby,base+'lobby.html?test=th03');
    await lobby.waitForFunction(()=>[...document.querySelectorAll('.book-room-score-value')].some(e=>e.textContent.replace(/\s/g,'')==='1:0'),{timeout:15000});await lobby.close();
    console.log('PASS match surrender reports host win exactly once and lobby shows 1:0');
    for(const frame of frames){const replay=await frame.evaluate(async()=>{const path=[...__eaglerPC98Files.memory.keys()].find(k=>k.endsWith('.th03r'));if(!path)return null;const data=await (await import('./native-replay.mjs')).decodeReplay(__eaglerPC98Files.memory.get(path));return {path,rows:data.rows.length,native:data.native.length};});assert(replay?.rows>0&&replay.native>0,'native replay contains input stream and V14 document');}
    const replayPath=await frames[0].evaluate(()=>[...__eaglerPC98Files.memory.keys()].find(k=>k.endsWith('.th03r')));
    await frames[0].evaluate(async path=>{const api=(await import('./managed-native-room.mjs')).managedAPI;api.stop();__eaglerPC98Options().replayViewer=true;await api.start({options:__eaglerPC98Options(),music:'none',language:'ja'});const description=await api.playback({action:'inspect',path});if(!description.description.stages.length)throw Error('missing replay description');await api.playback({action:'open',path,stage:0});},replayPath);
    await wait(async()=>frames[0].evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().gameFrame>120),'native replay advances',15000);
    await frames[0].evaluate(async()=>{const api=(await import('./managed-native-room.mjs')).managedAPI;await api.playback({action:'pause'});await api.playback({action:'seek',value:30});});
    await wait(async()=>frames[0].evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().gameFrame===30),'native replay seeks',15000);
    await frames[0].evaluate(async()=>{const api=(await import('./managed-native-room.mjs')).managedAPI;const result=await api.playback({action:'rate',value:8});if(result.playback.rate!==8)throw Error('replay speed failed');await api.playback({action:'play'});});
    await wait(async()=>frames[0].evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().gameFrame>50),'native replay resumes after seek',15000);
    console.log('PASS native replay persisted, plays and seeks');
    console.log('PASS touch match surrender and native result');break;
  }
  }
  // Exercise the actual main room leave handler, including after a preflight.
  // Both contexts keep their Package Store for the next room and match.
  await Promise.all(pages.map(page=>page.$eval('#mpLeaveRoom',button=>button.click())));
  await wait(async()=>(await Promise.all(pages.map(page=>page.evaluate(()=>!document.querySelector('#mpRoomView')&&!document.querySelector('#gameFrame')?.src.includes('runtimeEpoch='))))).every(Boolean),'main leave retires Runtime and room',15000);
  console.log('PASS leave and release',round);
  }
}catch(error){for(const [seat,page] of pages.entries())await page.screenshot({path:resolve(out,'failure-'+seat+'.png')});throw error;}
finally{await writeFile(resolve(out,'result.json'),JSON.stringify({errors,events},null,2));await browser.close();}
