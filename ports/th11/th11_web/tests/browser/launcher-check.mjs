import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {releaseServer} from '../../../th09_web/scripts/release-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url)),out=resolve(root,'artifacts/sdl3/browser/launcher');mkdirSync(out,{recursive:true});
const run=await releaseServer({root:resolve(root,process.env.TH11_LAUNCHER_SITE||'artifacts/architecture-preview/site'),port:0}),browser=await launchBrowser(),context=await browser.newContext({viewport:{width:390,height:844},isMobile:true,hasTouch:true,deviceScaleFactor:1}),page=await context.newPage(),errors=[],result={physicalDevice:false};
page.setDefaultTimeout(120000);page.on('pageerror',e=>errors.push(e.stack));
const frame=()=>page.frames().find(f=>f.url().includes('/runtime/th11/th11.html'));
async function launch(){await page.locator('#launch').click();await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.Module?._th11_frame()>100,null,{timeout:120000});if(await page.locator('#touchHelpClose').isVisible())await page.locator('#touchHelpClose').click();}
const key=async code=>{await page.keyboard.press(code,{delay:90});await page.waitForTimeout(450);};
try{
 await page.goto(run.url);await page.locator('button[data-game=th11]').waitFor();await page.waitForTimeout(500);if(await page.locator('#changelogConfirm').isVisible())await page.locator('#changelogConfirm').click();await page.locator('button[data-game=th11]').click();await page.screenshot({path:resolve(out,'home.png')});
 await launch();result.audio=await frame().evaluate(()=>({state:Module.SDL3.audioContext.state,shared:Module.SDL3.audioContext===parent.__touhouAudioContext}));assert.deepEqual(result.audio,{state:'running',shared:true});await page.screenshot({path:resolve(out,'title.png')});
 await frame().evaluate(()=>{parent.__exitingModule=Module;});
 await key('KeyX');await key('KeyZ');await page.waitForFunction(()=>!document.querySelector('#player').classList.contains('open'),null,{timeout:15000});result.titleExit=true;console.log('title exit passed');
 result.audioReleased=await page.evaluate(()=>{const c=window.__exitingModule,p=c._th11_audio_statistics();const out={ready:c.HEAPU32[p>>>2],playback:c.SDL3.audio_playback!==undefined,sharedState:window.__touhouAudioContext.state};delete window.__exitingModule;return out;});assert.deepEqual(result.audioReleased,{ready:0,playback:false,sharedState:'running'});
 await page.locator('button[data-game=th11]').click();await launch();result.relaunch=true;
 for(let i=0;i<4;++i)await key('KeyZ');await frame().waitForFunction(()=>Module._th11_phase()===1);await frame().waitForFunction(()=>Module._th11_frame()>80);
 const cdp=await context.newCDPSession(page),surface=page.locator('#touchDirectSurface'),direct=await surface.isVisible()?surface:frame().locator('canvas'),box=await direct.boundingBox(),x=box.x+box.width*.5,y=box.y+box.height*.65;
 await cdp.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[{x,y,id:7}]});await cdp.send('Input.dispatchTouchEvent',{type:'touchMove',touchPoints:[{x:x+22,y:y-11,id:7}]});await page.waitForTimeout(500);await cdp.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});
 const recording=await frame().evaluate(()=>{const c=Module,p=c._malloc(9);c.HEAPU8.set(new TextEncoder().encode('TOUCH\0'),p);const ok=c._th11_save_replay(1,p);c._free(p);if(!ok)throw Error('touch replay save failed');const path=c.FS.readdir('/save/replay').find(n=>n.startsWith('th11_01.'));return {path,bytes:Array.from(c.FS.readFile('/save/replay/'+path))};});assert.equal(recording.path,'th11_01.rpyx','actual touch reaches continuous-motion recorder');writeFileSync(resolve(out,recording.path),Buffer.from(recording.bytes));result.continuousTouch=true;
 await page.screenshot({path:resolve(out,'battle-portrait.png')});await page.evaluate(()=>document.exitFullscreen?.().catch(()=>{}));await page.setViewportSize({width:844,height:390});await page.screenshot({path:resolve(out,'battle-landscape.png')});
 await page.locator('#touchEscape').click();await frame().waitForFunction(()=>Module._th11_phase()===2);const before=await frame().evaluate(()=>Module._th11_frame());await page.waitForTimeout(350);assert.equal(await frame().evaluate(()=>Module._th11_frame()),before);await page.screenshot({path:resolve(out,'pause.png')});result.pauseFreezesBattle=true;
 await page.locator('#touchEscape').click();await frame().waitForFunction(()=>Module._th11_phase()===1);result.resume=true;
 await frame().evaluate(()=>{if(!Module._th11_return_title())throw Error('title return failed');});await frame().waitForFunction(()=>Module._th11_frame()>90);await key('KeyX');await key('KeyZ');await page.waitForFunction(()=>!document.querySelector('#player').classList.contains('open'));
 await page.locator('button[data-game=th11]').click();await page.locator('#replayFileTool [data-action=manage-replay]').click();await page.waitForFunction(()=>document.querySelector('#replayList').textContent.includes('th11_01.rpyx'));
 const original=readFileSync(resolve(root,'reference/replays/th11_ud0189.rpy')),chooser=page.waitForEvent('filechooser');await page.locator('#replayDialog [data-action=import-replay]').click();await(await chooser).setFiles({name:'th11_ud0189.rpy',mimeType:'application/octet-stream',buffer:original});await page.waitForFunction(()=>document.querySelector('#replaySummary').textContent.includes('2'));result.replayImport=true;
 const stored=await frame().evaluate(()=>Module.FS.readdir('/save/replay').filter(n=>n.endsWith('.rpy')).map(n=>({name:n,bytes:Array.from(Module.FS.readFile('/save/replay/'+n))})));assert.ok(stored.some(x=>Buffer.from(x.bytes).equals(original)));await page.screenshot({path:resolve(out,'replay-manager.png')});await page.locator('#replayDialog [data-replay-close]').last().click();
 const download=page.waitForEvent('download');await page.locator('#replayFileTool [data-action=export-replay]').click();const exported=await download;assert.ok(readFileSync(await exported.path()).length>original.length);result.replayExport=true;
 assert.deepEqual(errors,[]);result.passed=true;console.log(JSON.stringify(result));
}catch(e){result.error=e.stack;result.errors=errors;result.body=await page.locator('body').innerText().catch(()=>null);await page.screenshot({path:resolve(out,'failure.png')}).catch(()=>{});console.error(e);process.exitCode=1;}
finally{writeFileSync(resolve(out,'report.json'),JSON.stringify({...result,build:run.manifest.version},null,2));await browser.close();run.netplay.close();await new Promise(r=>run.server.close(r));}

