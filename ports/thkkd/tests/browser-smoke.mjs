import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
const root=fileURLToPath(new URL('../',import.meta.url)),output=root+'build/browser-smoke/';mkdirSync(output,{recursive:true});
const browser=await puppeteer.launch({executablePath:process.env.CHROME_PATH||'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,args:['--autoplay-policy=no-user-gesture-required','--no-first-run','--disable-features=Translate'],defaultViewport:{width:960,height:720}});
const report={browser:await browser.version(),errors:[],console:[]};
try{
 const page=await browser.newPage();page.on('pageerror',error=>report.errors.push(error.stack||error.message));page.on('console',message=>{if(message.type()==='error')report.console.push(message.text());});
 await page.goto('http://127.0.0.1:8173/runtime/thkkd.html?runtimeEpoch=1&standalone=1&data=../build/web-content/thkkd.data',{waitUntil:'load',timeout:60000});
 await page.waitForFunction(()=>globalThis.__thkkdRuntime?.status().first||document.querySelector('#error').textContent,{timeout:60000});
 await new Promise(r=>setTimeout(r,2000));
 report.state=await page.evaluate(()=>({runtime:globalThis.__thkkdRuntime?.status(),error:document.querySelector('#error').textContent,loading:document.querySelector('#loading').textContent,frames:globalThis.__thkkdRuntime?.platform.presented,scene:globalThis.__thkkdRuntime?.core._thkkd_word(0xa8c608)}));
 await page.screenshot({path:output+'title.png'});writeFileSync(output+'result.json',JSON.stringify(report,null,2));console.log(JSON.stringify(report));
 if(report.state.runtime?.first){
  report.transitions=[];
  for(let i=0;i<4;i++){
   await page.keyboard.down('z');await new Promise(r=>setTimeout(r,65));await page.keyboard.up('z');await new Promise(r=>setTimeout(r,450));
   report.transitions.push(await page.evaluate(()=>({scene:__thkkdRuntime.core._thkkd_word(0xa78e2c),error:document.querySelector('#error').textContent})));
   await page.screenshot({path:output+`start-${i}.png`});
  }
  await page.keyboard.down('z');await new Promise(r=>setTimeout(r,1000));await page.keyboard.up('z');await page.screenshot({path:output+'game.png'});
  report.game=await page.evaluate(()=>({scene:__thkkdRuntime.core._thkkd_word(0xa8ce58),error:document.querySelector('#error').textContent,frames:__thkkdRuntime.platform.presented,debug3d:__thkkdRuntime.platform.debug3d,videos:[...__thkkdRuntime.platform.graphs.values()].filter(g=>g.video).map(g=>({src:g.video.src,ready:g.video.readyState,paused:g.video.paused,time:g.video.currentTime}))}));
  await page.keyboard.down('Escape');await new Promise(r=>setTimeout(r,65));await page.keyboard.up('Escape');await new Promise(r=>setTimeout(r,350));await page.screenshot({path:output+'pause.png'});
  await page.evaluate(()=>__thkkdRuntime.stop('test'));
  await page.evaluate(()=>__thkkdRuntime.command({command:'launch'}));await new Promise(r=>setTimeout(r,600));
  report.restart=await page.evaluate(()=>({scene:__thkkdRuntime.core._thkkd_word(0xa78e2c),status:__thkkdRuntime.status(),error:document.querySelector('#error').textContent}));
  writeFileSync(output+'result.json',JSON.stringify(report,null,2));console.log(JSON.stringify({transitions:report.transitions,game:report.game,restart:report.restart}));
  if(report.game.error||report.restart.error)process.exitCode=1;
 }
 if(report.errors.length||report.state.error||!report.state.runtime?.first)process.exitCode=1;
}finally{await browser.close();}
