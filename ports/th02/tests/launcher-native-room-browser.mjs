import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';

const require=createRequire(new URL('../../../launcher/package.json',import.meta.url));
const puppeteer=require('puppeteer-core');
const base=process.env.PC98_LAUNCHER_URL||'http://127.0.0.1:18326/';
const seats=Number(process.env.TH02_TEST_SEATS??2);
const rollback=process.env.TH02_TEST_ROLLBACK==='1', rapidFire=process.env.TH02_TEST_RAPID_FIRE!=='0';
const renderer=rollback?'DOSBox-X / PC-98':'SoEW Native';
assert([2,3].includes(seats));
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,protocolTimeout:20000,
  args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
const pages=[];
const sleep=ms=>new Promise(r=>setTimeout(r,ms));
async function open(url){
  const page=await (await browser.createBrowserContext()).newPage(),events=[];
  pages.push({page,events});
  page.on('pageerror',error=>events.push({event:'error',message:error.message}));
  await page.setViewport({width:1400,height:900});
  await page.exposeFunction('recordRuntimeEvent',m=>events.push(m));
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
      await page.evaluate(()=>document.querySelector('#decisionDialog[open] #decisionConfirm')?.click());
    }
    if(await check())return;
    await sleep(150);
  }
  throw Error(`${label}: timeout`);
}
const runtime=page=>page.frames().find(f=>/th02\.html.*runtimeEpoch=/.test(f.url()));
const received=(page,predicate)=>pages.find(p=>p.page===page).events.some(predicate);
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
  await wait('host can set rollback',()=>enabled(host,'#mpRollbackToggle'));
  assert.equal(await host.$eval('#mpTh02RapidFire',e=>e.getAttribute('aria-checked')),'true');
  if(!rapidFire)await host.click('#mpTh02RapidFire');
  if(rollback)assert.equal(await host.$eval('#mpRollbackToggle',e=>e.getAttribute('aria-checked')),'true');
  else if(await host.$eval('#mpRollbackToggle',e=>e.getAttribute('aria-checked')==='true'))await host.click('#mpRollbackToggle');
  await wait('room rules and seats ready',async()=>
    (await Promise.all(players.map(async page=>await enabled(page,'#mpReady')&&
      await page.$eval('#mpRollbackToggle',(e,v)=>e.getAttribute('aria-checked')===String(v),rollback)&&
      await page.$eval('#mpTh02RapidFire',(e,v)=>e.getAttribute('aria-checked')===String(v),rapidFire)))).every(Boolean));
  if(!rapidFire)for(const page of players)assert(await page.$('.room-workspace-meta .room-challenge-on'));
  for(const page of players)await page.click('#mpReady');
  await wait('start match',()=>enabled(host,'#mpStartGame'));
  await host.click('#mpStartGame');
  await wait('all players draw a frame',()=>players.every(page=>received(page,m=>m.renderer===renderer)&&received(page,m=>m.event==='first-frame')),
    rollback?120000:45000);
  for(const page of players)assert(await runtime(page).$eval('#game canvas',e=>e.width>0&&e.height>0&&e.getBoundingClientRect().width>0));
  await wait('all players continue past the first state check',async()=>
    (await Promise.all(players.map(page=>runtime(page).evaluate(()=>globalThis.__eaglerNetplayLanConfirmed>=130)))).every(Boolean));
  for(const page of players)assert.equal(await runtime(page).evaluate(()=>globalThis.__eaglerPC98Options().netplayRapidFire),rapidFire);
  console.log('PASS Eagler create, join, seat, ready, '+renderer+' '+seats+'P launch, rapid fire '+(rapidFire?'on':'off'));

  if(!rollback){
    const observer=await open(roomUrl);
    await wait('join ongoing match as spectator',()=>enabled(observer,'#mpSpectatorPreviewJoin'));
    await observer.click('#mpSpectatorPreviewJoin');
    await wait('running native spectator',()=>received(observer,m=>m.renderer==='SoEW Native')&&received(observer,m=>m.event==='first-frame')&&playback(observer)?.position>=120);
    assert.equal(await runtime(observer).evaluate(()=>globalThis.__eaglerPC98Options().netplayRapidFire),rapidFire);
    const pixels=await runtime(observer).$eval('canvas',e=>{
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
    console.log('PASS Eagler late join, native spectator pixels, pause, seek and return live');
  }
} catch(error){
  for(const {page,events} of pages)console.log(await page.evaluate(()=>({url:location.href,status:document.body.innerText.slice(-1700)})),events.slice(-6));
  throw error;
} finally {await browser.close();}
