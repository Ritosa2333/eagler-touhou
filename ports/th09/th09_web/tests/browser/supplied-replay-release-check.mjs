import assert from 'node:assert/strict';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {releaseServer} from '../../scripts/release-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const release=resolve(process.env.TH09_OUTPUT||'artifacts/sdl-release'),output=resolve(release,'verification');mkdirSync(output,{recursive:true});
const file=resolve(process.env.TH09_REPLAY_FILE||'../[th09] 东方花映塚 (日文版)/replay/th9_udyt22.rpy');
const run=await releaseServer({root:resolve(release,'site'),port:0}),browser=await launchBrowser({args:['--enable-unsafe-swiftshader']}),page=await browser.newPage(),errors=[],result={replaySha256:createHash('sha256').update(readFileSync(file)).digest('hex'),version:JSON.parse(readFileSync(resolve(release,'site/manifest.json'),'utf8')).version};
page.on('pageerror',e=>errors.push(e.stack));
const game=()=>page.frames().find(f=>f.url().includes('/runtime/th09/th09.html'));
try {
 await page.goto(run.url);await page.locator('button[data-game=th09]').waitFor();if(await page.locator('#changelogConfirm').isVisible())await page.locator('#changelogConfirm').click();await page.locator('button[data-game=th09]').click();
 await page.locator('#replayFileTool [data-action=manage-replay]').click();const chooser=page.waitForEvent('filechooser');await page.locator('#replayDialog [data-action=import-replay]').click();await(await chooser).setFiles(file);await page.waitForFunction(()=>document.querySelector('#replayList').textContent.includes('th9_ud'));
 await page.locator('#replayDialog [data-replay-close]').last().click();await page.locator('#launch').click();await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.__th09Runtime,null,{timeout:120000});
 await game().evaluate(()=>{const c=Module;c._th09_loop_pause(1);const tick=n=>{for(let i=0;i<n;i++)if(!c._th09_game_tick(0))throw Error('menu tick')};const key=s=>{c._th09_key(s,1);tick(1);c._th09_key(s,0);tick(2)};tick(35);key(208);key(208);key(44);tick(20);key(205);key(44);tick(10);key(44);});
 result.initial=await game().evaluate(()=>__th09Runtime.status());assert.equal(result.initial.session[6],1,'native replay selected through menu');
 result.playback=await game().evaluate(async()=>{const c=Module,stages=[],changes=[];let previousStage=-1,ticks=0;for(;ticks<90000;++ticks){if(!c._th09_game_tick(0))throw Error('Replay tick failed '+ticks);const s=__th09Runtime.status();if(s.session[5]!==previousStage){previousStage=s.session[5];stages.push(previousStage);changes.push({tick:ticks,session:s.session});}if(s.session[0]!==1){c._th09_game_draw();return {ticks:ticks+1,stages,changes,final:s};}if(ticks%240===239)await new Promise(requestAnimationFrame);}throw Error('Replay did not finish');});
 assert.deepEqual(result.playback.stages,[0,1,2,3,4,5,6,7,8]);assert.equal(result.playback.final.session[0],5);assert.equal(result.playback.final.session[1],71637);assert.equal(result.playback.final.session[4],0);assert.deepEqual(errors,[]);result.errors=errors;result.passed=true;await page.screenshot({path:resolve(output,'supplied-replay-complete.png')});console.log(JSON.stringify(result));
}catch(e){result.error=e.stack;result.errors=errors;console.error(e);process.exitCode=1;await page.screenshot({path:resolve(output,'supplied-replay-failure.png')}).catch(()=>{});}finally{writeFileSync(resolve(output,'supplied-replay-release.json'),JSON.stringify(result,null,2));await browser.close();run.netplay.close();await new Promise(r=>run.server.close(r));}
