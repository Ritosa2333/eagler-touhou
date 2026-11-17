import assert from 'node:assert/strict';
import {writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {mkdirSync} from 'node:fs';
const option=(name,fallback)=>process.argv.find(arg=>arg.startsWith(`--${name}=`))?.slice(name.length+3)||fallback;
const difficulty=Number(option('difficulty','3'));
const base=new URL(option('url','http://127.0.0.1:8140/'));
const directory=resolve(option('output','artifacts/launcher-browser'));
mkdirSync(directory,{recursive:true});
const executablePath=process.env.TH19_BROWSER;
if(!executablePath)throw Error('Set TH19_BROWSER to Chrome or Edge.');
const modulePath=process.env.TH19_PUPPETEER_MODULE||resolve(import.meta.dirname,'../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js');
const {default:puppeteer}=await import(pathToFileURL(modulePath).href);
const report={passed:false,stage:'starting',results:[],errors:[]};
const output=resolve(directory,'room-report.json');
const save=()=>writeFileSync(output,JSON.stringify(report,null,2)+'\n');save();
const browser=await puppeteer.launch({executablePath,headless:true,timeout:60000,defaultViewport:{width:1280,height:900},args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
const pages=[];
async function until(check,label,timeout=Number(option('timeout','120000'))){report.stage=label;save();let end=Date.now()+timeout;while(Date.now()<end){if(await check())return;await new Promise(r=>setTimeout(r,150));}throw Error('Timeout '+label);}
async function click(page,selector){await page.waitForSelector(selector,{visible:true});await page.click(selector);}
async function select(page,selector,value){await page.waitForSelector(selector);await page.select(selector,value);}
try{
 for(let side=0;side<2;side++){
  const context=await browser.createBrowserContext(),page=await context.newPage();pages.push(page);
  await page.evaluateOnNewDocument(()=>{localStorage.setItem('eagler-touhou-beta-opt-in-v1','1');localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');});
  page.on('pageerror',e=>report.errors.push({side,error:e.message}));
  page.on('response',r=>{if(r.status()>=400)report.errors.push({side,http:r.status(),url:r.url()});});
  await page.goto(new URL('lobby.html',base).href,{waitUntil:'domcontentloaded',timeout:120000});
  await until(()=>page.evaluate(()=>{const button=document.querySelector('#createButton');return !!button&&!button.disabled&&Object.keys(button).some(key=>key.startsWith('__reactProps$')&&typeof button[key]?.onClick==='function');}),'P'+side+' interactive directory');
  await page.evaluate(()=>document.querySelector('#firstUseNoticeCloseHint')?.click());

  await click(page,side===0?'#createButton':'#codeButton');
  await until(()=>page.evaluate(()=>document.querySelector('#roomDialog')?.open===true),'P'+side+' room form');
  await select(page,'#gameSelect','th19mp');
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
 const rules=[0,2,3,3,3,1,1,9,9];
 for(const page of pages)await click(page,'#roomWorkspaceTab-game');
 await until(()=>pages[0].evaluate(()=>!!document.querySelector('#mpTh19Rule3')),'TH19 rule controls');
 for(const page of pages){const controls=await page.evaluate(()=>({life:document.querySelector('#mpTh19Rule3')?.value,removed:[0,4,5,6].every(index=>!document.querySelector('#mpTh19Rule'+index))}));assert.equal(controls.life,'4');assert.equal(controls.removed,true);}
 for(const index of [1,2,3,7,8]){await select(pages[0],'#mpTh19Rule'+index,String(rules[index]));await until(()=>pages[1].evaluate((index,value)=>document.querySelector('#mpTh19Rule'+index)?.value===String(value),index,rules[index]),'rule '+index+' synchronized');}
 await pages[0].evaluate(d=>document.querySelector('[data-mp-difficulty="'+d+'"]')?.click(),difficulty);
 report.rules=rules;
 for(let side=0;side<2;side++){
  const page=pages[side];
  await until(()=>page.evaluate(()=>{if(document.querySelector('#decisionDialog')?.open)document.querySelector('#decisionConfirm')?.click();return document.querySelector('#mpReady')?.disabled===false;}),'P'+side+' resources');
  await click(page,'#mpReady');
  await until(()=>page.evaluate(()=>document.querySelector('#mpReady')?.getAttribute('aria-pressed')==='true'),'P'+side+' ready');
 }
 await until(()=>pages[0].evaluate(()=>document.querySelector('#mpStartGame')?.disabled===false),'host can start');
 await click(pages[0],'#mpStartGame');
 for(const page of pages)await until(async()=>{
  await page.evaluate(()=>document.querySelector('#decisionConfirm')?.click());
  return page.evaluate(()=>document.querySelector('#gameFrame')?.contentWindow?.__th19Runtime?.netplay?.active===true);
 },'native RTC running');
 for(const page of pages)await page.evaluate(pressure=>{const core=document.querySelector('#gameFrame').contentWindow.__th19Runtime.core,step=core._th19_network_step;
  globalThis.spiritPressureSamples=[];
  core._th19_network_step=(...args)=>{
   if(pressure&&args[0]>=240&&args[0]%120===0){const p=core._th19_test_spirit_pressure(200)/4;globalThis.spiritPressureSamples.push({frame:args[0],state:Array.from(core.HEAP32.subarray(p,p+6)),heap:core.HEAPU8.length});}
   args[9]=0;return step(...args);
  };
 },option('spirit-pressure','0')==='1');
 const snapshot=page=>page.evaluate(()=>{const r=document.querySelector('#gameFrame').contentWindow.__th19Runtime;return {rank:Array.from(r.core.HEAP32.subarray(r.core._th19_test_gameplay_parity(0)/4,r.core._th19_test_gameplay_parity(0)/4+4)),active:r.netplay.active,frame:r.netplay.frame,route:r.netplay.socket.route,hashes:[...r.netplay.hashes],options:{side:r.core.eaglerOptions.netplayPlayer,count:r.core.eaglerOptions.netplayPlayerCount,difficulty:r.core.eaglerOptions.netplayDifficulty,rules:r.core.eaglerOptions.netplayTh19Rules},native:Array.from(r.core.HEAP32.subarray(r.core._th19_network_info()/4,r.core._th19_network_info()/4+31))};});
 await until(async()=> (await Promise.all(pages.map(snapshot))).every(s=>s.active&&s.frame>=Number(option('frames','360'))),'room-started frames');
 const states=await Promise.all(pages.map(snapshot));report.states=states;
 assert(states.every(s=>s.route==='rtc'));assert.deepEqual(states.map(s=>s.options.side),[0,1]);
 const remote=new Map(states[1].hashes),common=states[0].hashes.filter(([f])=>remote.has(f));assert(common.length>=2);for(const [f,h]of common)assert.equal(h,remote.get(f));
 for(const state of states){assert.deepEqual(state.rank,Array(4).fill([0,0,2,4][difficulty]));assert.deepEqual(state.options.rules,rules);assert.equal(state.options.difficulty,difficulty);assert.deepEqual(state.native.slice(8,12),Array(4).fill(difficulty));assert.deepEqual(state.native.slice(12,19),rules.slice(0,7));assert.deepEqual(state.native.slice(19,23),[0,3,4,4]);assert.deepEqual(state.native.slice(25,27),[60000,60000]);}
 if(option('spectator','0')==='1'){
  const context=await browser.createBrowserContext(),viewer=await context.newPage();pages.push(viewer);
  await viewer.evaluateOnNewDocument(()=>localStorage.setItem('eagler-touhou-beta-opt-in-v1','1'));
  viewer.on('pageerror',e=>report.errors.push({side:'spectator',error:e.message}));
  await viewer.goto(new URL('lobby.html',base).href,{waitUntil:'domcontentloaded',timeout:120000});
  await until(()=>viewer.evaluate(()=>!!document.querySelector('#codeButton')&&!document.querySelector('#codeButton').disabled),'spectator directory');
  await viewer.evaluate(()=>document.querySelector('#firstUseNoticeCloseHint')?.click());await click(viewer,'#codeButton');
  await until(()=>viewer.evaluate(()=>document.querySelector('#roomDialog')?.open===true),'spectator form');
  await select(viewer,'#gameSelect','th19mp');await viewer.type('#roomCodeInput',report.room);await click(viewer,'#submitRoom');
  await until(async()=>{await viewer.evaluate(()=>document.querySelector('#decisionConfirm')?.click());return viewer.evaluate(()=>{const n=document.querySelector('#gameFrame')?.contentWindow?.__th19Runtime?.netplay;return n?.spectator&&n.frame>0;});},'native spectator running',180000);
  await viewer.evaluate(()=>{const c=document.querySelector('#gameFrame').contentWindow.Module,step=c._th19_network_step;c._th19_network_step=(...args)=>{args[9]=0;return step(...args);};});
  await until(()=>viewer.evaluate(()=>document.querySelector('#cooperativeSpectatorPause')?.getClientRects().length>0&&document.querySelector('#netplayConnectionWindow')?.hidden===true),'Launcher spectator controls and gameplay readiness');
  await click(viewer,'#cooperativeSpectatorPause');await until(()=>viewer.evaluate(()=>document.querySelector('#gameFrame').contentWindow.__th19Runtime.netplay.spectatorPaused),'spectator paused');
  const retained=await viewer.evaluate(()=>document.querySelector('#gameFrame').contentWindow.__th19Runtime.netplay.frame);
  await click(viewer,'#cooperativeSpectatorLive');await until(()=>viewer.evaluate(retained=>{const n=document.querySelector('#gameFrame').contentWindow.__th19Runtime.netplay;return !n.spectatorPaused&&n.frame>retained;},retained),'spectator returns live');
  report.spectator=await viewer.evaluate(()=>{const n=document.querySelector('#gameFrame').contentWindow.__th19Runtime.netplay;return {frame:n.frame,received:n.receivedFrames,following:n.spectatorFollowing};});
  report.results.push({check:'Third Launcher joins live room, reconstructs history, shows timeline and operates Pause/Live',passed:true});
 }
 if(option('spirit-pressure','0')==='1'){
  report.spiritPressure=await Promise.all(pages.slice(0,2).map(page=>page.evaluate(()=>globalThis.spiritPressureSamples)));
  assert(report.spiritPressure.every(rows=>rows.length>=4));
 }
 assert.deepEqual(report.errors,[]);
 report.results.push({check:'Launcher create/join/seat/ready/start and native RTC state agreement',passed:true,hashes:common});report.passed=true;report.stage='complete';save();console.log(JSON.stringify(report));
}catch(error){report.failure=error.stack;report.pages=await Promise.all(pages.map(async p=>({url:p.url(),body:await p.evaluate(()=>document.body.innerText).catch(()=>''),native:await p.evaluate(()=>{const r=document.querySelector('#gameFrame')?.contentWindow?.__th19Runtime;return r?{active:r.netplay?.active,frame:r.netplay?.frame,status:r.status()}:null;}).catch(()=>null)})));save();throw error;}finally{await browser.close();}
