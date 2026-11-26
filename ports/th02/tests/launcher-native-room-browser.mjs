import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';
import {enableTouch,hitboxPoints,startTouchFire,touchButton,touchGame} from './browser-touch.mjs';

const require=createRequire(new URL('../../../launcher/package.json',import.meta.url));
const puppeteer=require('puppeteer-core');
const base=process.env.PC98_LAUNCHER_URL||'http://127.0.0.1:18326/';
const seats=Number(process.env.TH02_TEST_SEATS??2);
const challenge=process.env.TH02_TEST_CHALLENGE==='1',rapidFire=process.env.TH02_TEST_RAPID_FIRE!=='0';
const akyu=process.env.TH02_TEST_AKYU==='1';
const touch=process.env.TH02_TEST_TOUCH==='1';
const renderer='SoEW Native';
assert([2,3].includes(seats));
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,protocolTimeout:60000,
  args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
const pages=[];
const sleep=ms=>new Promise(r=>setTimeout(r,ms));
async function open(url){
  const page=await (await browser.createBrowserContext()).newPage(),events=[],diagnostics=[],tracks=new Set();
  pages.push({page,events,diagnostics,tracks});
  page.on('request',request=>{const name=new URL(request.url()).pathname.split('/').at(-1);if(/^\d{2}\.ogg$/.test(name))tracks.add(name);});
  page.on('pageerror',error=>events.push({event:'error',message:error.message}));
  page.on('console',entry=>{if(entry.type()==='error')diagnostics.push(entry.text());});
  page.on('requestfailed',request=>diagnostics.push(`${request.url()}: ${request.failure()?.errorText}`));
  page.on('response',response=>{if(response.status()>=400)diagnostics.push(`${response.status()} ${response.url()}`);});
  await page.setViewport({width:1400,height:900,hasTouch:touch});
  if(touch)await enableTouch(page,{mode:challenge?'touch':'touch-unlimited',music:akyu?'ogg-akyu':undefined});
  await page.exposeFunction('recordRuntimeEvent',m=>events.push(m));
  if(akyu&&!touch)await page.evaluateOnNewDocument(()=>{
    if(window===window.top)localStorage.setItem('eagler-touhou-game-options-v1-th02',JSON.stringify({music:'ogg-akyu',musicPreferenceExplicit:true}));
  });
  await page.evaluateOnNewDocument(()=>addEventListener('message',e=>{
    if(e.data?.protocol==='eagler-touhou/1'&&e.data.event)window.recordRuntimeEvent(e.data);
  }));
  await page.goto(url);
  await page.waitForFunction(()=>location.pathname.endsWith('lobby.html')
    ? !document.documentElement.hasAttribute('data-lobby-boot') : window.__eaglerBoot?.done===true,{timeout:45000});
  await page.evaluate(()=>document.querySelector('#firstUseNoticeDialog')?.close());
  return page;
}
async function wait(label,check,ms=45000){
  const deadline=Date.now()+ms;
  while(Date.now()<deadline){
    for(const {page,events} of pages){
      const error=events.find(m=>m.event==='error');
      if(error)throw Error(`${label}: ${JSON.stringify(error)}`);
      await page.evaluate(()=>{
        document.querySelector('#decisionDialog[open] #decisionConfirm')?.click();
        document.querySelector('#touchHelpClose')?.click();
      });
    }
    if(await check())return;
    await sleep(150);
  }
  throw Error(`${label}: timeout`);
}
const runtime=page=>page.frames().find(f=>/th02\.html.*runtimeEpoch=/.test(f.url()));
const received=(page,predicate)=>pages.find(p=>p.page===page).events.some(predicate);
const audioHealth=page=>pages.find(p=>p.page===page).events.findLast(m=>m.event==='audio-health');
const gameCanvas='#game canvas';
const playback=page=>pages.find(p=>p.page===page).events.filter(m=>m.spectatorPlayback).at(-1)?.spectatorPlayback;
const enabled=(page,selector)=>page.evaluate(sel=>{const e=document.querySelector(sel);return e&&!e.disabled&&!e.closest('[hidden]');},selector);

try {
  const host=await open(new URL('lobby.html?test=th02',base).href);
  await wait('create room',()=>enabled(host,'#createButton'));
  await host.click('#createButton');
  await host.waitForSelector('#roomDialog[open]');
  await host.select('#gameSelect','th02mp');
  await host.click('#submitRoom');
  await host.waitForSelector('#mpRoomCode',{timeout:45000});
  const code=await host.$eval('#mpRoomCode',e=>e.textContent);
  const roomUrl=new URL('?'+new URLSearchParams({game:'th02mp',mpRoom:code,lobbyAction:'join',test:'th02'}),base).href;
  await host.click('#roomWorkspaceTab-game');
  if(seats===3){
    await host.select('#mpRoomPlayerCount','3');
    await host.waitForSelector('[data-mp-seat-drop="2"]');
  }
  const players=[host];
  while(players.length<seats)players.push(await open(roomUrl));
  for(const [seat,page] of players.entries()){
    await page.waitForSelector('#mpSeatStage',{timeout:45000});
    const join=`[data-mp-seat-drop="${seat}"] button`;
    if(await enabled(page,join))await page.click(join);
  }
  await wait('host can set room rules',()=>enabled(host,'#mpTh02RapidFire'));
  assert.equal(await host.$eval('#mpTh02RapidFire',e=>e.getAttribute('aria-checked')),'true');
  if(!rapidFire)await host.click('#mpTh02RapidFire');
  assert.equal(await enabled(host,'#mpRollbackToggle'),false);
  if(challenge)await host.click('#mpRoomChallengeToggle');
  await wait('room rules and seats ready',async()=>
    (await Promise.all(players.map(async page=>await enabled(page,'#mpReady')&&
      await page.$eval('#mpRoomChallengeToggle',(e,v)=>e.getAttribute('aria-checked')===String(v),challenge)&&
      await page.$eval('#mpTh02RapidFire',(e,v)=>e.getAttribute('aria-checked')===String(v),rapidFire)))).every(Boolean));
  if(!rapidFire)for(const page of players)assert(await page.$('.room-workspace-meta .room-challenge-on'));
  for(const page of players)await page.click('#mpReady');
  await wait('start match',()=>enabled(host,'#mpStartGame'));
  assert(pages.every(p=>p.tracks.size===0),'room preload must not download recordings');
  await host.click('#mpStartGame');
  await wait('all players draw a frame',()=>players.every(page=>received(page,m=>m.renderer===renderer)&&received(page,m=>m.event==='first-frame')));
  for(const page of players)assert(await runtime(page).$eval('#game canvas',e=>e.width>0&&e.height>0&&e.getBoundingClientRect().width>0));
  await wait('all players continue past the first state check',async()=>
    (await Promise.all(players.map(page=>runtime(page).evaluate(()=>globalThis.__eaglerNetplayLanConfirmed>=130)))).every(Boolean));
  for(const page of players){
    const options=await runtime(page).evaluate(()=>globalThis.__eaglerPC98Options());
    if(akyu)assert.equal(options.musicSource,'akyu',JSON.stringify(await page.evaluate(()=>localStorage.getItem('eagler-touhou-game-options-v1-th02'))));
    assert.equal(options.netplayRapidFire,rapidFire);
    assert.equal(options.netplayChallengeMode===true,challenge);
  }
  console.log('PASS Eagler create, join, seat, ready, '+renderer+' '+seats+'P launch, rapid fire '+(rapidFire?'on':'off')+', challenge '+(challenge?'on':'off'));

  const guest=players.at(-1);
  if(touch)await runtime(guest).$eval(gameCanvas,el=>el.focus());
  else await runtime(guest).click(gameCanvas);
  assert(await runtime(guest).$eval(gameCanvas,el=>document.activeElement===el&&getComputedStyle(el).outlineStyle==='none'));
  if(touch){
    const read=page=>runtime(page).evaluate(hitboxPoints);
    const hostStart=await read(host),guestStart=await read(guest);
    assert.equal(hostStart.length,1);
    assert.equal(guestStart.length,1);
    assert.notEqual(hostStart[0].x,guestStart[0].x);
    await touchGame(guest,-70,0);
    await wait('guest drag updates player position',async()=>{
      const p=await read(guest);
      return p.length===1&&p[0].x<guestStart[0].x-10;
    },10000);
    assert.deepEqual(await read(host),hostStart,'Guest marker must not appear or displace the host marker');
    console.log('PASS TH02 room touch moves the guest; each seat sees just its own marker');
  }
  if(akyu){
    await wait('Akyu song plays',()=>players.every(page=>audioHealth(page)?.playing));
    assert.deepEqual(pages.map(p=>Array.from(p.tracks)),players.map(()=>['02.ogg']));
    if(touch)await startTouchFire(guest);
    else await guest.keyboard.press('KeyZ',{delay:160});
    await wait('Akyu and native SFX playing',()=>players.every(page=>audioHealth(page)?.playing&&audioHealth(page).sfx>0));
    console.log('PASS Akyu downloads one song at launch; native SFX still play');
  }
  if(touch)await touchButton(guest,'#touchEscape');
  else await guest.keyboard.press('Escape',{delay:160});
  await wait('guest pauses everyone',()=>players.every(page=>audioHealth(page)?.paused===true),10000);
  const pausedPixels=await runtime(host).$eval(gameCanvas,el=>el.toDataURL());
  const pausedFrame=await runtime(host).evaluate(()=>globalThis.__eaglerNetplayLanConfirmed);
  await wait('paused players still synchronize',async()=>await runtime(host).evaluate(()=>globalThis.__eaglerNetplayLanConfirmed)>pausedFrame+10);
  assert.equal(await runtime(host).$eval(gameCanvas,el=>el.toDataURL()),pausedPixels,'paused gameplay pixels must not advance');
  if(touch)await touchButton(host,'#touchEscape');
  else {
    await runtime(host).click(gameCanvas);
    await host.keyboard.press('Escape',{delay:160});
  }
  await wait('a different player resumes everyone',()=>players.every(page=>audioHealth(page)?.paused===false),10000);
  console.log('PASS guest Esc pauses all seats, host Esc resumes, canvas retains focus without white outline');

  {
    const observer=await open(roomUrl);
    await wait('join ongoing match as spectator',()=>enabled(observer,'#mpSpectatorPreviewJoin'));
    await observer.click('#mpSpectatorPreviewJoin');
    await wait('running spectator',()=>received(observer,m=>m.renderer===renderer)&&received(observer,m=>m.event==='first-frame')&&playback(observer)?.position>=120);
    assert.equal(await runtime(observer).evaluate(()=>globalThis.__eaglerPC98Options().netplayRapidFire),rapidFire);
    const pixels=await runtime(observer).$eval('canvas:last-of-type',e=>{
      const data=e.getContext('2d').getImageData(0,0,e.width,e.height).data;
      return data.some((c,i)=>i%4!==3&&c>0);
    });
    assert(pixels,'spectator renders game pixels');
    await observer.click('.spectator-pause-button');
    await wait('spectator paused',()=>playback(observer)?.paused&&!playback(observer).seeking);
    const paused=playback(observer).position;
    await wait('spectator receives while paused',()=>playback(observer)?.received>paused+15);
    assert.equal(playback(observer).position,paused);
    await observer.$eval('.spectator-timeline input[type=range]',el=>{
      el.value='30';el.dispatchEvent(new Event('input',{bubbles:true}));el.dispatchEvent(new Event('change',{bubbles:true}));
    });
    await wait('spectator seeks backwards',()=>playback(observer)?.position===30&&!playback(observer).seeking);
    await observer.click('#cooperativeSpectatorLive');
    await wait('spectator returns to live',()=>playback(observer)?.following&&playback(observer).position>paused);
    console.log('PASS Eagler late join, '+renderer+' spectator pixels, pause, seek and return live');
  }
} catch(error){
  for(const {page,events,diagnostics} of pages)console.log(page.url(),events.slice(-6),diagnostics);
  throw error;
} finally {await browser.close();}
