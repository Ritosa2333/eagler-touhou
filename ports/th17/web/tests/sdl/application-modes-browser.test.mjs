import test from 'node:test';import assert from 'node:assert/strict';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {applicationServer,applicationPage,inspectApplication} from './application-fixture.mjs';
import {report,root,sha} from '../cpp/helpers.mjs';import {readFile} from 'node:fs/promises';import {resolve} from 'node:path';

test('Application preserves native config and composes practice, spell practice, ending and result destinations',{timeout:900000},async()=>{
 const server=await applicationServer();let browser;const observations=[],errors=[];
 try{browser=await launchBrowser();const page=await applicationPage(browser,server);page.on('pageerror',e=>errors.push(String(e)));
  await page.evaluate(()=>{choose(0,5);choose(3,6);choose(0,7);fixture._application_title_select(0);step(1);waitFor(()=>state()[0]===1);settle(45);});
  const config=await page.evaluate(()=>Array.from(fixture.FS.readFile('/save/th17.cfg')));assert.equal(config[0x1e],1,'Lunatic selection must preserve native display mode');
  await page.evaluate(()=>{fixture._application_close();initialize();fixture._application_unlock();choose(2,5);});assert.equal((await page.evaluate(()=>state()))[10],1,'Native default difficulty after restart');
  await page.evaluate(()=>{choose(0,6);choose(2,7);choose(2,8);fixture._application_keyboard_key(51,1);fixture._application_title_select(5);step(1);fixture._application_keyboard_key(51,0);waitFor(()=>state()[0]===1);settle(120);fixture._application_test_protection(60000);});
  const practice=await inspectApplication(page,'practice-six',observations);assert.equal(practice.state[2],6);assert.equal(practice.state[9],2,'Practice number 3 means two extra lives');assert.equal(practice.state[12]&0x30,0x10);
  await page.evaluate(()=>{fixture._application_complete_stage();waitFor(()=>pause()[1]===15||pause()[1]===6);});await inspectApplication(page,'practice-results',observations);
  await page.evaluate(()=>{if(pause()[1]===15){fixture._application_name_select(90);step(1);}waitFor(()=>pause()[1]===6);step(0x10000);waitFor(()=>state()[0]===0);ready(8);for(const screen of[7,6,5,1]){step(0x102);ready(screen);}choose(3,18);choose(0,19);choose(0,20);fixture._application_title_select(0);step(1);waitFor(()=>state()[0]===1);settle(120);fixture._application_test_protection(60000);});
  const spell=await inspectApplication(page,'spell-practice',observations);assert.equal(spell.state[12]&0x30,0x20);assert.equal(spell.state[9],0);assert.equal(spell.state[10],0);
  await page.evaluate(()=>{fixture._application_complete_stage();waitFor(()=>pause()[1]===15||pause()[1]===6);if(pause()[1]===15){fixture._application_name_select(90);step(1);}waitFor(()=>pause()[1]===6);step(0x10000);waitFor(()=>state()[0]===0);ready(20);step(0x102);ready(19);step(0x102);ready(18);step(0x102);ready(1);settle(30);});await inspectApplication(page,'practice-return',observations);
  // Previews belong to the Trophy gallery. Calling the isolated preview probe
  // directly from Main leaves impossible main-menu animations in screenshots.
  await page.evaluate(()=>{choose(5,10);fixture._application_title_select(2);step(1);waitFor(()=>state()[8]===23&&state()[9]===1);settle(30);});
  for(const index of[0,8,13,15,9,12]){
   await page.evaluate(index=>{if(!fixture._application_test_ending(index))throw Error('Ending preview failed '+index);},index);
   let frames=0;for(;frames<18000;frames+=120){const active=await page.evaluate(()=>{for(let i=0;i<120;i++){const e=ending();if(e[0]!==1)return false;step(i%30===0?1:0,0,0x200);}return ending()[0]===1;});if(!active)break;if(frames===120)await inspectApplication(page,'ending-'+index,observations);}
   assert.ok(frames<18000,'Ending preview completes '+index);observations.push({kind:'ending-preview-complete',index,frames});await page.evaluate(()=>settle(30));assert.equal(await page.evaluate(()=>decode(fixture._application_music())),'th17_01.wav');
  }
  await page.evaluate(()=>{step(0x102);ready(10);step(0x102);ready(1);settle(30);});
  // Drive real scene boundaries after each stage-clear request. Combat itself
  // is checked by the native replay suite; this fixture forces only completion.
  await page.evaluate(()=>{choose(0,5);choose(0,6);choose(0,7);fixture._application_title_select(0);step(1);waitFor(()=>state()[0]===1);settle(90);});
  for(let stage=1;stage<=6;stage++){
   assert.equal((await page.evaluate(()=>state()))[2],stage);await page.evaluate(()=>{fixture._application_test_protection(60000);if(!fixture._application_complete_stage())throw Error('Stage completion request failed');});
   if(stage<6){await page.evaluate(stage=>{waitFor(()=>state()[2]===stage+1,480);waitFor(()=>state()[5]>=90,480);},stage);await inspectApplication(page,'transition-'+stage+'-'+(stage+1),observations);}
  }
  await page.evaluate(()=>waitFor(()=>ending()[0]===1,480));await inspectApplication(page,'normal-ending',observations);
  let frames=0;for(;frames<24000;frames+=120){const active=await page.evaluate(()=>{for(let i=0;i<120;i++){if(ending()[0]!==1)return false;step(i%15===0?1:0,0,0x200);}return ending()[0]===1;});if(!active)break;}
  assert.ok(frames<24000,'Normal ending and staff complete');await page.evaluate(()=>ready(15));await inspectApplication(page,'normal-ranking',observations);
  await page.evaluate(()=>{step(0x40);for(let i=0;i<6;i++)step(0x20);step(1);ready(16);settle(20);step(1);settle(20);});await inspectApplication(page,'completed-replay-name',observations);
  await page.evaluate(()=>{step(0x40);for(let i=0;i<6;i++)step(0x20);step(1);ready(16);step(0x102);ready(1);settle(60);fixture._application_close();initialize();settle(40);});await inspectApplication(page,'completed-restarted',observations);
  const files=await page.evaluate(()=>fixture.FS.readdir('/save/replay'));assert.ok(files.includes('th17_01.rpy'));assert.deepEqual(errors,[]);
  report('sdl-application-modes',{passed:true,completeGame:false,wasmSha256:sha(await readFile(resolve(root,'artifacts/sdl-application/th17-application.wasm'))),observations,files,scope:'Actual full C++ application: native config roundtrip after Lunatic, numbered Stage Practice lives, Spell Practice, result/name/menu return, ending/staff resource and text composition, all six stage scene boundaries, normal ending/results and replay save followed by reload. Completion requests are explicit development probes, not claims of unassisted full-game native replay parity. Software browser rendering; physical phones remain separate.'});
 }finally{await browser?.close();await server.close();}
});
