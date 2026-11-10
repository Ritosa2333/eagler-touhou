import assert from 'node:assert/strict';
import {readFile,writeFile} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import {launchBrowser} from '../../tools/browser/launch.mjs';

const ops=import.meta.dirname,staged=process.env.TH05_PREFLIGHT==='1',files=resolve(ops,'payload-v4/site-files'),origin='https://test.touhou.vip';
const vanilla=process.env.TH05_PROFILE==='vanilla',label=(staged?'preflight':'public')+(vanilla?'-vanilla':'');
const browser=await launchBrowser(),page=await browser.newPage({viewport:{width:1400,height:1000},serviceWorkers:'block'}),errors=[];
page.on('pageerror',error=>errors.push(error.message));
const mime={'.html':'text/html','.js':'application/javascript','.mjs':'application/javascript','.wasm':'application/wasm','.json':'application/json','.css':'text/css','.zip':'application/zip','.ogg':'audio/ogg','.webp':'image/webp','.svg':'image/svg+xml','.png':'image/png','.woff2':'font/woff2'};
try{
 await page.addInitScript(()=>{localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');window.__th05Events=[];addEventListener('message',event=>{if(event.data?.game==='th05'&&event.data?.event)window.__th05Events.push(event.data);});});
 if(staged)await page.route(origin+'/**',async route=>{const path=decodeURIComponent(new URL(route.request().url()).pathname);try{const bytes=await readFile(resolve(files,path==='/'?'index.html':path.slice(1)));await route.fulfill({status:200,body:bytes,contentType:mime[extname(path)]??(path==='/'?'text/html':'application/octet-stream')});}catch(error){if(error.code!=='ENOENT'&&error.code!=='EISDIR')throw error;await route.continue();}});
 await page.goto(origin+'/',{waitUntil:'domcontentloaded',timeout:60000});
 await page.waitForSelector('[data-shelf=singleplayer] a[data-game=th05]',{timeout:45000});
 await page.waitForSelector('[data-shelf=multiplayer] a[data-product=th05-standalone]',{timeout:45000});
 const cards=await page.locator('.game-shelf a[data-game]').evaluateAll(nodes=>nodes.map(node=>({game:node.dataset.game,product:node.dataset.product??null,shelf:node.closest('[data-shelf]')?.dataset.shelf,href:node.href,hidden:node.hidden})));
 assert.equal(cards.filter(c=>c.game==='th05'&&c.shelf==='singleplayer').length,1);
 assert.equal(cards.filter(c=>c.game==='th05'&&c.shelf==='classics').length,0);
 assert.equal(cards.find(c=>c.product==='th05-standalone')?.href,origin+'/th05');
 for(const game of ['th01','th02','th03','th04','th13','th17'])assert.ok(cards.some(c=>c.game===game&&!c.hidden),'Missing '+game);
 await page.locator('[data-shelf=singleplayer] a[data-game=th05]').click();
 await page.waitForTimeout(500);
 if(!await page.locator('#advancedOptions > summary').isVisible())await page.locator('[data-shelf=singleplayer] a[data-game=th05]').click();
 const select=async(id,value)=>page.locator('#'+id).evaluate((node,value)=>{node.value=value;node.dispatchEvent(new Event('change',{bubbles:true}));},value);
 await select('languageSelect',vanilla?'ja':'lang_zh-hans');
 if(!await page.locator('#advancedOptions').evaluate(node=>node.open))await page.locator('#advancedOptions > summary').click();
 await page.locator('#musicOption .mizuki-select-trigger').click();await page.locator('[role=option][data-value='+(vanilla?'ogg-full':'ogg-akyu')+']:visible').click();
 if((await page.locator('#thpracToggle').getAttribute('aria-checked')==='true')===vanilla)await page.locator('#thpracToggle').click();
 await page.screenshot({path:resolve(ops,label+'-settings.png')});
 await page.locator('#launch').click();
 await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.Module?._th05_ticks()>10,null,{timeout:90000});
 const frame=page.frames().find(frame=>frame.url().includes('/runtime/th05/'));assert.ok(frame);
 await frame.locator('#canvas').click({position:{x:15,y:15}});
 await frame.waitForFunction(()=>Module.eaglerAudio.health().playing,null,{timeout:45000});
 const native=await frame.evaluate(()=>({enabled:Module._th05_practice_enabled(),codepage:Module._th05_codepage(),scene:Module._th05_scene(),ticks:Module._th05_ticks(),font:Module.FS.readlink('/data/gothic.ttf'),audio:Module.eaglerAudio.health()}));
 assert.equal(native.enabled,vanilla?0:1);assert.equal(native.codepage,vanilla?932:936);assert.equal(native.font,vanilla?'/msgothic.ttc':'/unifont.otf');assert.equal(native.audio.mode,'ogg');assert.equal(native.audio.musicSource,vanilla?'original':'akyu');assert.equal(native.audio.path,vanilla?'/music/op-m86-opus64-single-loop-v2.ogg':'/music/akyu-01.ogg');assert.equal(native.audio.sfx.backend,'uth05win-original-se');
 const runtimeErrors=await page.evaluate(()=>window.__th05Events.filter(event=>event.event==='error'||event.event==='music-error'));assert.deepEqual(runtimeErrors,[]);assert.deepEqual(errors,[]);
 await page.screenshot({path:resolve(ops,label+'-game.png')});
 const result={passed:true,staged,vanilla,origin,cards,native,runtimeErrors,errors};await writeFile(resolve(ops,label+'-browser.json'),JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify(result));
}catch(error){await page.screenshot({path:resolve(ops,label+'-failure.png')});console.error((await page.locator('body').innerText()).slice(-4000));console.error(errors);throw error;}
finally{await browser.close();}
