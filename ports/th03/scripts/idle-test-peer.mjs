import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';

// Local development helper: join a waiting TH03 room as a second runtime.
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url));
const puppeteer=require('puppeteer-core');
const base=process.env.PC98_LAUNCHER_URL||'http://127.0.0.1:18310/';
const relay=process.env.PC98_TEST_RELAY||'ws://127.0.0.1:18311/';
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
const context=await browser.createBrowserContext(),page=await context.newPage();
await page.setViewport({width:1280,height:800});
let active=null,busy=false,selected=false;
async function join(code){
  active=code;selected=false;
  const url=base+'?'+new URLSearchParams({game:'th03mp',mpRoom:code,lobbyAction:'join'});
  await page.goto(url);await page.waitForSelector('#mpSeatStage',{timeout:60000});
  console.log('joined',code);
}
async function tick(){
  if(busy)return;busy=true;
  try{
    if(active){
      const seat=await page.$('[data-mp-seat-drop="1"] button');
      if(seat&&await seat.evaluate(e=>!e.closest('[hidden]')&&!e.disabled))await seat.click();
      const ready=await page.$('#mpReady');
      if(ready&&await ready.evaluate(e=>!e.disabled&&e.getAttribute('aria-pressed')==='false'))await ready.click();
      const frame=page.frames().find(f=>/th03.html.*runtimeEpoch=/.test(f.url()));
      if(frame&&!selected){
        const state=await frame.evaluate(()=>window.th03SyncState||null).catch(()=>null);
        if(state&&state.frame>180&&state.phase===0){
          await page.bringToFront();await page.$eval('#gameFrame',e=>e.focus());
          await page.keyboard.down('KeyZ');await new Promise(resolve=>setTimeout(resolve,500));await page.keyboard.up('KeyZ');
          selected=true;console.log('character selected',active);
        }
      }
      if(!await page.$('#mpSeatStage')){active=null;selected=false;}
    }
  }catch(error){console.error(error.message);active=null;selected=false;}
  finally{busy=false;}
}
const directory=new WebSocket(new URL('?directory=1&member=th03_idle_test_peer',relay));
directory.onopen=()=>directory.send(JSON.stringify({type:'refresh',product:'th03mp'}));
directory.onmessage=event=>{
  const data=JSON.parse(event.data);
  if(active||busy||data.type!=='directory')return;
  const room=data.rooms?.find(r=>r.product==='th03mp'&&r.phase==='lobby'&&r.players===1&&r.capacity===2);
  if(room){busy=true;join(room.code).catch(error=>{console.error(error.message);active=null;}).finally(()=>busy=false);}
};
directory.onerror=error=>console.error('directory',error.message);
setInterval(()=>{if(directory.readyState===WebSocket.OPEN)directory.send(JSON.stringify({type:'refresh',product:'th03mp'}));void tick();},1000);
for(const signal of ['SIGINT','SIGTERM'])process.on(signal,async()=>{directory.close();await browser.close();process.exit();});
