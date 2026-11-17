import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../tools/browser/launch.mjs';

const root=resolve(import.meta.dirname,'../../..'),port=Number(process.env.TH05_TEST_PORT??8145);
const font=process.env.TH05_TEST_FONT??resolve(root,'dependencies/unifont-15.1.05/unifont-15.1.05.otf');
const server=spawn(process.execPath,['launcher/scripts/serve.mjs',String(port)],{cwd:root,windowsHide:true,env:{...process.env,
 EAGLER_DEVELOPMENT_GAMES:'th05',EAGLER_TH05_LANGUAGE_PACK_DIR:resolve(root,'ports/th05/artifacts/languages'),
 EAGLER_DEVELOPMENT_VANILLA_FONT:font,EAGLER_TOUHOU_ARTWORK_DIR:resolve(root,'ports/th05/artifacts/content/artwork')},stdio:['ignore','pipe','pipe']});
let serverLog='';for(const stream of [server.stdout,server.stderr])stream.on('data',data=>serverLog+=data);
let browser;
try{
 await new Promise((accept,reject)=>{const timeout=setTimeout(()=>reject(Error(serverLog||'Launcher server startup timed out')),20000);const check=data=>{if(String(data).includes('eagler-touhou:')){clearTimeout(timeout);accept();}};server.stdout.on('data',check);server.on('error',reject);server.on('exit',code=>reject(Error('Launcher server exited '+code+' '+serverLog)));});
 browser=await launchBrowser();const page=await browser.newPage({viewport:{width:1280,height:900}}),errors=[],report={cases:[]};
 const build=JSON.parse(await readFile(resolve(root,'ports/th05/artifacts/runtime/build.json'),'utf8'));report.runtime={wasmSha256:build.sha256,builtAt:build.builtAt,diagnostic:build.diagnostic};
 page.on('pageerror',error=>errors.push(error.message));
 await page.addInitScript(()=>localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1'));
 for(const [music,language,enabled] of [['ogg-akyu','lang_zh-hans',true],['ogg-full','ja',false]]){
  console.log('Launcher case:',music,language,enabled);
  await page.goto(`http://127.0.0.1:${port}/`);
  if(await page.locator('#firstUseNoticeClose').isVisible())await page.locator('#firstUseNoticeClose').click();
  await page.waitForTimeout(400);await page.locator('a[data-game="th05"]').click();await page.waitForTimeout(400);
  async function select(id,value){await page.locator('#'+id).evaluate((element,value)=>{element.value=value;element.dispatchEvent(new Event('change',{bubbles:true}));},value);}
  await select('languageSelect',language);
  if(!await page.locator('#advancedOptions').evaluate(element=>element.open))await page.locator('#advancedOptions > summary').click();
  await page.locator('#musicOption .mizuki-select-trigger').click();await page.locator(`[role="option"][data-value="${music}"]:visible`).click();
  const toggle=page.locator('#thpracToggle');if((await toggle.getAttribute('aria-checked')==='true')!==enabled)await toggle.click();
  await page.locator('#launch').click();
  await page.waitForFunction(()=>{const iframe=document.querySelector('#gameFrame');return iframe?.contentWindow?.Module?._th05_ticks()>10;},null,{timeout:45000});
  const frame=page.frames().find(frame=>frame.url().includes('th05.html'));assert.ok(frame);
  await frame.waitForFunction(()=>Module.eaglerAudio.health().playing,null,{timeout:20000});
  const native=await frame.evaluate(()=>({enabled:Module._th05_practice_enabled(),codepage:Module._th05_codepage(),ticks:Module._th05_ticks(),audio:Module.eaglerAudio.health()}));
  assert.equal(native.enabled,+enabled);assert.equal(native.codepage,language==='ja'?932:936);assert.equal(native.audio.mode,'ogg');assert.equal(native.audio.musicSource,music==='ogg-akyu'?'akyu':'original');assert.equal(native.audio.decodeMode,music==='ogg-full'?'full':'stream');assert.ok(native.audio.sfx);
  report.cases.push({music,language,enabled,native});
  await page.screenshot({path:resolve(root,`ports/th05/artifacts/launcher-${enabled?'practice-akyu':'vanilla-original'}.png`)});
 }
 assert.deepEqual(errors,[]);report.errors=errors;
 await writeFile(resolve(root,'ports/th05/artifacts/launcher-validation.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report,null,2));
}catch(error){console.error(serverLog);throw error;}
finally{await browser?.close();server.kill();}
