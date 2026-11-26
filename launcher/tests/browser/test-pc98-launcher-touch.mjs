import assert from 'node:assert/strict';
import {writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import puppeteer from 'puppeteer-core';
import {mkdirSync} from 'node:fs';
const option=(name,fallback)=>process.argv.find(arg=>arg.startsWith(`--${name}=`))?.slice(name.length+3)||fallback;
// Start a local site publishing the selected title and a relay; pass --url and --game.
const base=new URL(option('url','http://127.0.0.1:8130/'));
const game=option('game','th04');assert(['th01','th03','th04'].includes(game),'invalid game');
const pointer=option('pointer','pen');assert(['pen','touch'].includes(pointer),'invalid pointer');
const directory=resolve(option('output','artifacts/launcher-browser'));
mkdirSync(directory,{recursive:true});
const executablePath=await findChromiumExecutable();
const report={passed:false,stage:'starting',results:[],errors:[]};
const output=resolve(directory,'room-report.json');
const save=()=>writeFileSync(output,JSON.stringify(report,null,2)+'\n');save();
const browser=await puppeteer.launch({executablePath,headless:true,timeout:60000,defaultViewport:{width:1280,height:900},args:['--no-sandbox','--ignore-certificate-errors','--no-proxy-server','--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
const pages=[];
async function until(check,label,timeout=Number(option('timeout','120000'))){report.stage=label;save();let end=Date.now()+timeout;while(Date.now()<end){if(await check())return;await new Promise(r=>setTimeout(r,150));}throw Error('Timeout '+label);}
async function click(page,selector){await page.waitForSelector(selector,{visible:true});await page.click(selector);}
async function select(page,selector,value){await page.waitForSelector(selector);await page.select(selector,value);}
try{
 for(let side=0;side<2;side++){
  const context=await browser.createBrowserContext(),page=await context.newPage();pages.push(page);
  await page.setViewport({width:960,height:600,deviceScaleFactor:1,isMobile:pointer==='touch',hasTouch:pointer==='touch',isLandscape:true});
  await page.setUserAgent(pointer==='touch'?'Mozilla/5.0 (Linux; Android 13; Mobile) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/140.0.0.0 Mobile Safari/537.36':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/140.0.0.0 Safari/537.36');
  await page.evaluateOnNewDocument((game,pointer)=>{if(pointer==='pen')Object.defineProperty(navigator,'maxTouchPoints',{get:()=>0});localStorage.setItem('eagler-touch-help-seen-v8','1');localStorage.setItem('eagler-touhou-touch-options-v1',JSON.stringify({touchMovementMode:'touch-unlimited'}));for(const product of [game,game+'mp'])localStorage.setItem('eagler-touhou-game-options-v1-'+product,JSON.stringify({options:{touchEnabled:true,touchMovementMode:'touch-unlimited'}}));},game,pointer);
  page.on('pageerror',e=>report.errors.push({side,error:e.message}));
  page.on('response',r=>{if(r.status()>=400)report.errors.push({side,http:r.status(),url:r.url()});});
  await page.goto(new URL('lobby.html',base).href,{waitUntil:'domcontentloaded',timeout:120000});
  await until(()=>page.evaluate(()=>{const button=document.querySelector('#createButton');return !!button&&!button.disabled&&Object.keys(button).some(key=>key.startsWith('__reactProps$')&&typeof button[key]?.onClick==='function');}),'P'+side+' interactive directory');
  await page.evaluate(()=>document.querySelector('#firstUseNoticeCloseHint')?.click());

  await click(page,side===0?'#createButton':'#codeButton');
  await until(()=>page.evaluate(()=>document.querySelector('#roomDialog')?.open===true),'P'+side+' room form');
  await select(page,'#gameSelect',game+'mp');
  if(side===0){await select(page,'#difficultySelect','1');await select(page,'#visibilitySelect','private');}
  else await page.type('#roomCodeInput',report.room);
  await click(page,'#submitRoom');
  await page.waitForSelector('#mpRoomCode',{visible:true});
  await until(()=>page.evaluate(side=>document.querySelector(`[data-mp-seat-me="${side}"]`)?.hidden===false||document.querySelector(`[data-mp-seat-drop="${side}"] button`)?.disabled===false,side),'P'+side+' seat');
  await until(async()=>{
   if(await page.evaluate(side=>document.querySelector(`[data-mp-seat-me="${side}"]`)?.hidden===false,side))return true;
   const clickable=await page.evaluate(side=>{const button=document.querySelector(`[data-mp-seat-drop="${side}"] button`);return !!button&&!button.disabled&&button.getClientRects().length>0;},side);
   if(clickable)try{await click(page,`[data-mp-seat-drop="${side}"] button`);}catch{}
   return false;
  },'P'+side+' seated');
  report.room=await page.$eval('#mpRoomCode',e=>e.textContent.trim());save();

 }
 await until(async()=>{for(const page of pages){await page.bringToFront();await page.evaluate(()=>{if(document.querySelector('#decisionDialog')?.open)document.querySelector('#decisionConfirm')?.click();const button=document.querySelector('#mpReady');if(button&&!button.disabled&&button.getAttribute('aria-pressed')!=='true')button.click();});}return (await Promise.all(pages.map(page=>page.evaluate(()=>document.querySelector('#mpReady')?.getAttribute('aria-pressed')==='true')))).every(Boolean);},'both touch players prepared and ready');
 await until(()=>pages[0].evaluate(()=>document.querySelector('#mpStartGame')?.disabled===false),'host can start');
 await click(pages[0],'#mpStartGame');
 if(game==='th03'){
  for(const page of pages)await until(async()=>{await page.evaluate(()=>document.querySelector('#decisionConfirm')?.click());return page.evaluate(()=>document.querySelector('#gameFrame')?.contentWindow?.th03SyncState?.selecting===true);},'TH03 selection');
  for(const page of pages){await page.bringToFront();for(let i=0;i<3;i++){await page.keyboard.press('KeyZ');await new Promise(r=>setTimeout(r,500));}}
  for(const page of pages)await until(()=>page.evaluate(()=>document.querySelector('#gameFrame')?.contentWindow?.th03SyncState?.phase===1),'TH03 battle');
 }else{
  for(const page of pages)await until(async()=>{await page.evaluate(()=>document.querySelector('#decisionConfirm')?.click());return page.evaluate(()=>document.querySelector('#gameFrame')?.contentWindow?.__eaglerNetplayLanFrame>120);},game+' running',120000);
  if(game==='th01')for(const page of pages){await page.bringToFront();await page.keyboard.press('KeyZ');await new Promise(r=>setTimeout(r,1000));}
 }
 const snap=page=>page.evaluate(async game=>{
  const w=document.querySelector('#gameFrame').contentWindow;
  const h=game==='th01'?w.th01Net?.session?.game?.state():game==='th03'?(await w.eval("import('./managed-native-room.mjs')")).managedAPI.health():await (await w.eval("import('./netplay/room.js')")).managedAPI.sampleGame();
  const s=document.querySelector('#touchDirectSurface'),r=s.getBoundingClientRect(),x=r.left+r.width*.65,y=r.top+r.height*.55;
  const xPosition=game==='th01'?h?.players[1]?.left:game==='th03'?h?.players.find(p=>p.seat===1)?.x:h?.players[1]?.[0];
  return {xPosition,surface:{hidden:s.hidden,rect:{x:r.x,y:r.y,width:r.width,height:r.height},element:document.elementFromPoint(x,y)?.outerHTML.slice(0,250)}};
 },game);
 await pages[1].click('#fullscreenToggle');await new Promise(r=>setTimeout(r,1000));
 const before=await Promise.all(pages.map(snap));assert(Number.isFinite(before[1].xPosition),'P2 game position unavailable');
 assert.equal(before[1].surface.hidden,false,'touch input must remain available for a pen without reported touch points');
 assert.match(before[1].surface.element,/touch-direct-surface/,'the room must not cover the input surface');
 await pages[1].bringToFront();const client=await pages[1].createCDPSession(),r=before[1].surface.rect;
 if(pointer==='pen'){
  await client.send('Input.dispatchMouseEvent',{type:'mousePressed',x:r.x+r.width*.5,y:r.y+r.height*.5,button:'left',buttons:1,pointerType:'pen'});
  for(let i=1;i<=12;i++){await client.send('Input.dispatchMouseEvent',{type:'mouseMoved',x:r.x+r.width*(.5+i*.02),y:r.y+r.height*.5,button:'left',buttons:1,pointerType:'pen'});await new Promise(resolve=>setTimeout(resolve,70));}
  await client.send('Input.dispatchMouseEvent',{type:'mouseReleased',x:r.x+r.width*.74,y:r.y+r.height*.5,button:'left',buttons:0,pointerType:'pen'});
 }else{
  await client.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[{x:r.x+r.width*.5,y:r.y+r.height*.5,id:1}]});
  for(let i=1;i<=12;i++){await client.send('Input.dispatchTouchEvent',{type:'touchMove',touchPoints:[{x:r.x+r.width*(.5+i*.02),y:r.y+r.height*.5,id:1}]});await new Promise(resolve=>setTimeout(resolve,70));}
  await client.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});
 }
 await until(async()=>{const states=await Promise.all(pages.map(snap));return states.every(state=>Number.isFinite(state.xPosition)&&state.xPosition!==before[1].xPosition);},'both peers see P2 move');
 report.passed=true;report.stage='complete';save();console.log('PASS');
}catch(error){report.failure=error.stack;report.pages=await Promise.all(pages.map(async p=>({body:await p.evaluate(()=>document.body.innerText).catch(()=>'' )}))); save();throw error;}finally{await browser.close();}
