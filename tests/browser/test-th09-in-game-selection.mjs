// Actual React room -> managed Release Runtime -> native menus -> natural
// result -> changed characters. Viewer controls use the mounted Runtime UI.
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {browserTestUrl} from '../support/browser-target.mjs';
const {chromium}=createRequire(process.env.BROWSER_TEST_PACKAGE||new URL('../../package.json',import.meta.url))('playwright');
const origin=new URL(browserTestUrl()).origin,label=process.env.RUN_LABEL;
if(!/^[\w-]+$/.test(label||''))throw Error('Unique RUN_LABEL required');
const out=resolve(process.env.TH09_SELECTION_TEST_OUTPUT||'.cache/tests/th09-in-game-selection',label);mkdirSync(out,{recursive:true});
const report={frontendBranch:'main',frontendSource:process.cwd(),passed:false,scope:'Actual local React, managed Release WASM and shared room, natural match completion. Not physical devices/public network.',stages:[],errors:[]};
const browser=await chromium.launch({headless:true,executablePath:process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE,args:['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required']});
const actors=[];const stage=(name,fields={})=>{report.stages.push({name,...fields});console.log(JSON.stringify({name,...fields}));};
async function actor(role){const context=await browser.newContext({viewport:{width:1280,height:900},serviceWorkers:'block'}),page=await context.newPage();
  const row={role,page,sockets:[]};actors.push(row);page.on('pageerror',e=>report.errors.push({role,error:e.stack}));
  page.on('websocket',ws=>{if(new URL(ws.url()).searchParams.has('lobby')){const s={closed:false};row.sockets.push(s);ws.on('close',()=>s.closed=true);}});
  if(process.env.TH09_SELECTION_RUNTIME_PATH) await page.route('**/site-manifest.json',async route=>{
    const response=await route.fetch(),manifest=await response.json();
    manifest.games.th09.runtime=process.env.TH09_SELECTION_RUNTIME_PATH;
    manifest.games.th09.multiplayerRuntime=process.env.TH09_SELECTION_RUNTIME_PATH;
    await route.fulfill({response,json:manifest});
  });
  await page.goto(origin+'/lobby.html?game=th09mp');
  if(await page.locator('#firstUseNoticeClose').isVisible())await page.locator('#firstUseNoticeClose').click();return row;}
async function enter(row,code){const p=row.page;await p.locator(code?'#codeButton':'#createButton').click();await p.locator('#roomDialog[open]').waitFor();await p.locator('#gameSelect').selectOption('th09mp');if(code)await p.locator('#roomCodeInput').fill(code);await p.locator('#roomForm button[type=submit]').click();await p.locator('#mpRoomCode').waitFor();return (await p.locator('#mpRoomCode').innerText()).trim();}
async function ready(row,side){const p=row.page;if(await p.locator(`[data-mp-seat-me="${side}"]`).evaluate(n=>n.hidden))await p.locator(`[data-mp-seat-drop="${side}"] button`).click();await p.locator('#mpReady').click();await p.waitForFunction(()=>document.querySelector('#mpReady')?.getAttribute('aria-pressed')==='true',{},{timeout:120000});}
function nativeFrame(row){return row.page.frameLocator('#gameFrame');}
async function read(row){return nativeFrame(row).locator('body').evaluate(()=>{
  const r=window.__th09Runtime;if(!r?.netplay?.prepared)return null;const c=r.core,words=(fn,n)=>{const at=fn()/4;return Array.from(c.HEAP32.subarray(at,at+n));};
  return {active:r.netplay.active,selection:words(c._th09_network_selection_info,16),title:r.status().title,session:r.status().session,player:words(c._th09_rollback_info,8),viewer:words(c._th09_spectator_playback_info,11),pending:r.netplay.spectatorTape.begins,start:r.netplay.matchStart,hash:c._th09_network_hash()>>>0,build:r.netplay.build,route:window.__eaglerNetplayTransport,error:document.querySelector('#error')?.textContent};
});}
async function until(row,predicate,name,timeout=40000){const end=Date.now()+timeout;let last;while(Date.now()<end){const confirm=row.page.locator('#decisionDialog[open]:not(.closing) #decisionConfirm');if(await confirm.isVisible())await confirm.click();try{last=await read(row);if(last?.error)throw Error(last.error);if(last&&predicate(last))return last;}catch(e){if(last?.error)throw e;}await row.page.waitForTimeout(100);}throw Error(name+' '+JSON.stringify(last)+' '+(await row.page.locator('body').innerText()).slice(-1200));}
async function key(row,name){await nativeFrame(row).locator('#canvas').focus();await row.page.keyboard.press(name,{delay:100});await row.page.waitForTimeout(200);}
async function menu(screen){for(const row of actors.slice(0,2))await until(row,s=>s.selection[1]&&s.title[2]===screen&&s.title[3]===1&&s.title[5]>=10,'menu '+screen);}
try{
  const host=await actor('host'),code=await enter(host);const guest=await actor('guest');await enter(guest,code);const viewer=await actor('viewer');await enter(viewer,code);
  await viewer.page.locator('#mpSpectatorToggle').click();await viewer.page.locator('#mpSpectatorJoin').click();
  assert.equal(await host.page.locator('#mpRoomDifficultyBadge').innerText(),'游戏内选角');
  assert.equal(await host.page.locator('#mpLoadoutPrevSeat, #mpLoadoutNextSeat').count(),0);
  await ready(host,0);await ready(guest,1);await host.page.locator('#mpStartGame').click();await menu(7);
  stage('managed-native-selection',{code,host:await read(host)});
  await until(viewer,s=>s.active&&s.viewer[2]===0,'spectator waits for selection');
  await key(host,'KeyZ');await menu(8);const initial=await read(host);await key(host,'ArrowRight');const left=await read(host);assert.equal(left.selection[4],initial.selection[4]);assert.notEqual(left.selection[3],initial.selection[3]);
  await key(guest,'ArrowRight');const both=await read(host);assert.equal(both.selection[3],left.selection[3]);assert.notEqual(both.selection[4],left.selection[4]);
  for(const row of [host,guest]){await nativeFrame(row).locator('#canvas').focus();await row.page.keyboard.down('ShiftLeft');await row.page.keyboard.down('ArrowLeft');}
  await host.page.waitForTimeout(4000);for(const row of [host,guest]){await row.page.keyboard.up('ArrowLeft');await row.page.keyboard.up('ShiftLeft');}
  await key(host,'KeyZ');assert.deepEqual((await read(host)).selection.slice(5,7),[1,0]);await key(guest,'KeyZ');await menu(16);await host.page.screenshot({path:resolve(out,'both-chosen.png')});await key(host,'KeyZ');
  const first=await until(host,s=>!s.selection[1]&&s.session[1]>=100,'first gameplay');assert.deepEqual(first.selection.slice(8,10),[1,2]);assert.deepEqual((await read(guest)).selection.slice(8,10),[1,2]);
  await until(viewer,s=>s.viewer[1]>first.start+10,'first viewer');await viewer.page.locator('#spectatorToggle').click();await viewer.page.locator('#spectatorPause').click();const held=await read(viewer);
  for(const row of [host,guest]){await nativeFrame(row).locator('#canvas').focus();await row.page.keyboard.down('KeyZ');await row.page.keyboard.down('ControlLeft');}
  stage('first-match-with-paused-viewer',{first,held});
  await until(host,s=>s.session[0]===3,'natural match completion',240000);for(const row of [host,guest]){await row.page.keyboard.up('KeyZ');await row.page.keyboard.up('ControlLeft');}
  await host.page.screenshot({path:resolve(out,'result.png')});await host.page.waitForTimeout(500);await key(host,'ArrowDown');await key(host,'KeyZ');await menu(7);
  assert.equal((await read(viewer)).viewer[1],held.viewer[1]);await key(host,'KeyZ');await menu(8);await key(host,'ArrowRight');await key(guest,'ArrowRight');await key(host,'KeyZ');await key(guest,'KeyZ');await menu(16);await key(host,'KeyZ');
  const second=await until(host,s=>!s.selection[1]&&s.start>first.start&&s.session[1]>120,'changed-character match');assert.deepEqual(second.selection.slice(8,10),[2,3]);assert.deepEqual((await read(guest)).selection.slice(8,10),[2,3]);
  const queued=await until(viewer,s=>s.pending===1,'next match retained');assert.equal(queued.viewer[1],held.viewer[1]);
  await viewer.page.locator('#spectatorLive').click();const watching=await until(viewer,s=>s.viewer[8]===second.start&&s.viewer[1]>second.start+30,'viewer switches character worlds');assert.deepEqual(watching.selection.slice(8,10),[2,3]);
  const late=await actor('late-viewer');await enter(late,code);await late.page.locator('#mpSpectatorToggle').click();await late.page.locator('#mpSpectatorJoin').click();const lateState=await until(late,s=>s.viewer[8]===second.start&&s.viewer[1]>second.start+30,'late viewer receives new config',120000);assert.deepEqual(lateState.selection.slice(8,10),[2,3]);
  assert.ok(actors.every(row=>row.sockets.length===1&&!row.sockets[0].closed));assert.equal(report.errors.length,0);
  await viewer.page.screenshot({path:resolve(out,'new-characters-viewer.png')});stage('managed-reselection-and-spectators-pass',{second,watching,late:lateState});report.passed=true;
}catch(error){report.failure=error.stack;for(const row of actors){report[row.role]=await read(row).catch(()=>null);report[row.role+'Text']=(await row.page.locator('body').innerText()).slice(-1600);await row.page.screenshot({path:resolve(out,row.role+'-failure.png')}).catch(()=>{});}throw error;}
finally{writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2));await browser.close();console.log(JSON.stringify({passed:report.passed,output:out}));}
