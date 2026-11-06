import test from 'node:test';
import assert from 'node:assert/strict';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {applicationServer,applicationPage,inspectApplication} from './application-fixture.mjs';
import {report} from '../cpp/helpers.mjs';

test('Practice displays its stage rows and starts the selected stage in the full application',{timeout:240000},async()=>{
 const server=await applicationServer(),observations=[],errors=[];let browser;
 try{
  browser=await launchBrowser();const page=await applicationPage(browser,server,2);page.on('pageerror',e=>errors.push(String(e)));
  await page.evaluate(()=>{fixture._application_unlock();choose(2,5);choose(0,6);choose(0,7);choose(1,8);settle(20);});
  const menu=await inspectApplication(page,'practice-stage-select-fixed',observations);assert.equal(menu.state[8],8);
  await page.evaluate(()=>{fixture._application_title_select(5);step(1);waitFor(()=>state()[0]===1);settle(90);});
  const game=await inspectApplication(page,'practice-stage-six-fixed',observations);assert.equal(game.state[2],6);assert.equal(game.state[12]&0x30,0x10);
  assert.deepEqual(errors,[]);report('sdl-application-practice-fixed',{passed:true,observations,scope:'Actual scale-2 SDL3/WebGL2 application: Stage Practice selection rendered and selected sixth stage entered. Native text, colors and scores are independently checked by title-stage-draw.'});
 }finally{await browser?.close();await server.close();}
});
