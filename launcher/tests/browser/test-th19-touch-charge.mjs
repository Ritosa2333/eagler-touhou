import assert from 'node:assert/strict';
import {writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {mkdirSync} from 'node:fs';
const option=(name,fallback)=>process.argv.find(arg=>arg.startsWith(`--${name}=`))?.slice(name.length+3)||fallback;
const base=new URL(option('url','http://127.0.0.1:8140/'));
const directory=resolve(option('output','artifacts/th19-touch-browser'));
mkdirSync(directory,{recursive:true});
const executablePath=process.env.TH19_BROWSER;
if(!executablePath)throw Error('Set TH19_BROWSER to Chrome or Edge.');
const modulePath=process.env.TH19_PUPPETEER_MODULE||resolve(import.meta.dirname,'../../node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js');
const {default:puppeteer}=await import(pathToFileURL(modulePath).href);
const browser=await puppeteer.launch({executablePath,headless:true,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding'],defaultViewport:{width:960,height:720},timeout:60000});
const errors=[],results=[];
writeFileSync(resolve(directory,'gameplay-report.json'),JSON.stringify({passed:false,status:'running'})+'\n');
let page;
try{
 page=await browser.newPage();
 await page.setViewport({width:960,height:600,deviceScaleFactor:1,isMobile:true,hasTouch:true,isLandscape:true});
 await page.setUserAgent('Mozilla/5.0 (Linux; Android 13; Mobile) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/140.0.0.0 Mobile Safari/537.36');
 await page.evaluateOnNewDocument(()=>localStorage.setItem('eagler-touch-help-seen-v8','1'));
 await page.evaluateOnNewDocument(()=>{window.__TH19_SMOKE_EVENTS=[];addEventListener("message",e=>{if(e.data?.protocol==="eagler-touhou/1"&&e.data.game==="th19")window.__TH19_SMOKE_EVENTS.push({event:e.data.event,ok:e.data.ok,error:e.data.error,request:e.data.request});});});
 page.on('pageerror',e=>errors.push(e.message));page.on('response',r=>{if(r.status()>=400)errors.push(`${r.status()} ${r.url()}`);});
 await page.goto(new URL('?game=th19',base).href,{waitUntil:'domcontentloaded',timeout:120000});
 async function until(check,label){const end=Date.now()+Number(option('timeout','120000'));while(Date.now()<end){if(await check())return;await new Promise(r=>setTimeout(r,100));}throw Error('Timeout '+label);}
 await until(()=>page.evaluate(()=>window.__eaglerBoot?.done===true&&!!document.querySelector('#launch')),'launcher boot');
 await until(()=>page.evaluate(()=>localStorage.getItem('eagler-touhou-first-use-notice-seen-v1')==='1'||document.querySelector('#firstUseNoticeDialog')?.open===true),'first-use notice');
 await page.evaluate(()=>{if(document.querySelector('#firstUseNoticeDialog')?.open)document.querySelector('#firstUseNoticeClose')?.click();});
 await until(()=>page.evaluate(()=>{if(document.querySelector('#firstUseNoticeDialog')?.open)document.querySelector('#firstUseNoticeClose')?.click();return document.querySelector('#firstUseNoticeDialog')?.open!==true;}),'first-use notice closed');
 await page.click('#launch');
 await until(async()=>{await page.evaluate(()=>document.querySelector('#decisionConfirm')?.click());return page.evaluate(()=>{const r=document.querySelector('#gameFrame')?.contentWindow?.__th19Runtime;return !!r&&r.status().title[0]>0;});},'native first frame');
 results.push({check:'Launcher online DATA and OGG installation, configure and first frame',passed:true});
 async function step(code,count=120){return page.evaluate(({code,count})=>{
  const win=document.querySelector('#gameFrame').contentWindow,{core,status}=win.__th19Runtime;core._th19_loop_pause(1);
  if(code){win.dispatchEvent(new win.KeyboardEvent('keydown',{code,key:code,bubbles:true}));core._th19_test_ticks(1);win.dispatchEvent(new win.KeyboardEvent('keyup',{code,key:code,bubbles:true}));}
  core._th19_test_ticks(count);return status().title;
 },{code,count});}
 await step(null,600);
 const difficulty=await step('KeyZ');assert.equal(difficulty[1],1);
 const selection=await step('KeyZ');assert.equal(selection[1],9);
 const dialogue=await step('KeyZ');assert.equal(dialogue[1],10);
 let battle;
 for(let i=0;i<12;i++){
  battle=await page.evaluate(()=>{const r=document.querySelector('#gameFrame').contentWindow.__th19Runtime;r.core._th19_test_story_pump();return r.status().title;});
  if(battle[1]===11)break;
 }
 assert.equal(battle[1],11,'normal menu and dialogue reach live gameplay');
 await step(null,180);
 const movement=await page.evaluate(()=>{
  const w=document.querySelector('#gameFrame').contentWindow,r=w.__th19Runtime,c=r.core;
  const get=()=>Array.from(c.HEAP32.subarray(c._th19_character_combat_status()/4,c._th19_character_combat_status()/4+17));
  const before=get();w.dispatchEvent(new w.KeyboardEvent('keydown',{code:'ArrowRight',key:'ArrowRight',bubbles:true}));c._th19_test_ticks(8);w.dispatchEvent(new w.KeyboardEvent('keyup',{code:'ArrowRight',key:'ArrowRight',bubbles:true}));return {before:before[14],after:get()[14]};
 });
 assert.ok(movement.after>movement.before,'keyboard movement reaches the native player');
 results.push({check:'Story difficulty, character selection, dialogue, battle and keyboard movement',passed:true,movement});
 await page.waitForSelector('#touchFunction',{visible:true});
 const labels=await page.evaluate(()=>({charge:document.querySelector('#touchFunction strong').textContent,chargeKey:document.querySelector('#touchFunction .touch-icon').textContent,bombKey:document.querySelector('#touchBomb .touch-icon').textContent,enabled:document.querySelector('#player').classList.contains('touch-enabled')}));
 assert.equal(labels.charge,'蓄力');assert.equal(labels.chargeKey,'X');assert.equal(labels.bombKey,'C');assert(labels.enabled);
 const client=await page.createCDPSession();
 const chargeBox=await page.$eval('#touchFunction',e=>{const r=e.getBoundingClientRect();return {x:r.x+r.width/2,y:r.y+r.height/2};});
 await client.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[{...chargeBox,id:1}]});
 await until(()=>page.evaluate(()=>document.querySelector('#touchFunction').getAttribute('aria-pressed')==='true'),'charge button pressed');
 const hold=await page.evaluate(()=>{const c=document.querySelector('#gameFrame').contentWindow.__th19Runtime.core;c._th19_test_ticks(90);const p=c._th19_charge_attack_status()/4;return {charge:c.HEAP32[p+3],held:c.HEAP32[c._th19_network_info()/4+27]};});
 assert.equal(hold.held,1);assert(hold.charge>0);await page.screenshot({path:resolve(directory,'charge-held.png')});
 await client.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});
 await until(()=>page.evaluate(()=>document.querySelector('#touchFunction').getAttribute('aria-pressed')==='false'),'charge button released');
 const release=await page.evaluate(()=>{const c=document.querySelector('#gameFrame').contentWindow.__th19Runtime.core;c._th19_test_ticks(1);const p=c._th19_charge_attack_status()/4;return {charge:c.HEAP32[p+3],lastRelease:c.HEAP32[p+4],held:c.HEAP32[c._th19_network_info()/4+27]};});assert.equal(release.held,0);assert(release.lastRelease>=1);
 const bombsBefore=await page.evaluate(()=>{const c=document.querySelector('#gameFrame').contentWindow.__th19Runtime.core;return c.HEAP32[c._th19_card_status()/4+10];});const bombBox=await page.$eval('#touchBomb',e=>{const r=e.getBoundingClientRect();return {x:r.x+r.width/2,y:r.y+r.height/2};});await client.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[{...bombBox,id:2}]});await client.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});
 const bombsAfter=await page.evaluate(()=>{const c=document.querySelector('#gameFrame').contentWindow.__th19Runtime.core;c._th19_test_ticks(2);return c.HEAP32[c._th19_card_status()/4+10];});assert.equal(bombsAfter,bombsBefore-1);
 results.push({check:'Visible X charge and C Bomb buttons; trusted touch holds and releases native charge',passed:true,labels,hold,release,bombsBefore,bombsAfter});
 const pause=await step('Escape',30);
 const paused=await page.evaluate(()=>{const c=document.querySelector('#gameFrame').contentWindow.__th19Runtime.core;return c.HEAP32[c._th19_title_status()/4+13];});
 assert.equal(paused,1,'native pause menu opens');
 results.push({check:'Native pause menu',passed:true});
 await page.screenshot({path:resolve(directory,'gameplay.png')});
 const diagnostics=await page.evaluate(()=>{const c=document.querySelector('#gameFrame').contentWindow.__th19Runtime.core;return {sfxVoices:c.eaglerSfxOutput?.voices.size||0,storageRevision:c._th19_storage_revision(),title:Array.from(c.HEAP32.subarray(c._th19_title_status()/4,c._th19_title_status()/4+19))};});
 assert.ok(diagnostics.sfxVoices>0,'shared immediate SFX output owns native sound voices');
 assert.deepEqual(errors,[]);
 writeFileSync(resolve(directory,'gameplay-report.json'),JSON.stringify({passed:true,results,diagnostics,errors},null,2));
 console.log(JSON.stringify({passed:true,results,diagnostics,errors}));
}catch(error){const diagnostics=await page?.evaluate(()=>({body:document.body.innerText,frame:document.querySelector('#gameFrame')?.src,events:window.__TH19_SMOKE_EVENTS})).catch(()=>null);writeFileSync(resolve(directory,'gameplay-report.json'),JSON.stringify({passed:false,results,errors,diagnostics,failure:error.stack},null,2)+'\n');throw error;}finally{await browser.close();}
