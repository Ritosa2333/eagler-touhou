import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';

// Local development helper: join a waiting room as a second runtime.
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url));
const puppeteer=require('puppeteer-core');
const base=process.env.PC98_LAUNCHER_URL||'http://127.0.0.1:18310/';
const relay=process.env.PC98_TEST_RELAY||'ws://127.0.0.1:18311/';
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
const context=await browser.createBrowserContext(),page=await context.newPage();
await page.setViewport({width:1280,height:800});
let active=null,busy=false,selected=false,missing=0;
async function join(room){
  active={code:room.code,product:room.product};selected=false;
  const game=room.product.replace(/mp$/,'');
  const url=base+'?'+new URLSearchParams({game:room.product,mpRoom:room.code,lobbyAction:'join',test:game});
  await page.goto(url);await page.waitForSelector('#mpSeatStage',{timeout:60000});
  await page.evaluate(()=>document.querySelector('#firstUseNoticeDialog')?.close());
  await page.waitForFunction(()=>!document.documentElement.hasAttribute('data-lobby-boot')&&window.__eaglerBoot?.done===true,{timeout:60000});
  console.log('joined',room.product,room.code);
}
async function tick(){
  if(busy)return;busy=true;
  try{
    if(active){
      await page.evaluate(()=>{document.querySelector('#decisionDialog[open] #decisionConfirm')?.click();document.querySelector('#touchHelpClose')?.click();});
      const seat=await page.$('[data-mp-seat-drop="1"] button');
      if(seat&&await seat.evaluate(e=>!e.closest('[hidden]')&&!e.disabled))await seat.click();
      const ready=await page.$('#mpReady');
      if(ready&&await ready.evaluate(e=>!e.disabled&&e.getAttribute('aria-pressed')==='false'))await ready.click();
      const frame=active.product==='th03mp'&&page.frames().find(f=>/th03.html.*runtimeEpoch=/.test(f.url()));
      if(frame&&!selected){
        const state=await frame.evaluate(()=>window.th03SyncState||null).catch(()=>null);
        if(state&&state.frame>180&&state.phase===0){
          await page.bringToFront();await page.$eval('#gameFrame',e=>e.focus());
          await page.keyboard.down('KeyZ');await new Promise(resolve=>setTimeout(resolve,500));await page.keyboard.up('KeyZ');
          selected=true;console.log('character selected',active.code);
        }
      }
      if(page.url().includes('lobby.html')&&!await page.$('#mpRoomCode')){active=null;selected=false;}
    }
  }catch(error){console.error(error.message);active=null;selected=false;}
  finally{busy=false;}
}
const directory=new WebSocket(new URL('?directory=1&member=pc98_idle_test_peer',relay));
directory.onopen=()=>directory.send(JSON.stringify({type:'refresh'}));
directory.onmessage=event=>{
  const data=JSON.parse(event.data);
  if(data.type!=='directory')return;
  if(active){
    missing=data.rooms?.some(r=>r.product===active.product&&r.code===active.code&&r.seats?.[0])?0:missing+1;
    if(missing>=3){active=null;selected=false;missing=0;}
  }
  if(active||busy)return;
  const room=data.rooms?.find(r=>/^th\d+mp$/.test(r.product)&&r.phase==='lobby'&&r.players===1&&r.capacity===2&&r.seats?.[0]?.online&&!r.seats?.[1]);
  if(room){busy=true;join(room).catch(error=>{console.error(error.message);active=null;}).finally(()=>busy=false);}
};
directory.onerror=error=>console.error('directory',error.message);
setInterval(()=>{if(directory.readyState===WebSocket.OPEN)directory.send(JSON.stringify({type:'refresh'}));void tick();},1000);
for(const signal of ['SIGINT','SIGTERM'])process.on(signal,async()=>{directory.close();await browser.close();process.exit();});
