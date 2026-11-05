import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const browser = await puppeteer.launch({executablePath: await findChromiumExecutable(), headless: true, args: ['--no-sandbox']});
try {
  const page = await browser.newPage();
  await page.goto(process.env.EAGLER_TEST_URL || 'http://127.0.0.1:18310/');
  for (const game of ['th01', 'th02', 'th03', 'th04']) {
    const result = await page.evaluate(async game => {
      const {managedInput} = await import(`/ports/${game}/build-eagler/managed-input.mjs`);
      const options = {touchEnabled:true, touchSensitivity:150, touchMovementMode:'touch-unlimited'};
      const input = managedInput({game, epoch:1, active:()=>true, options:()=>options});
      const pointer = (type,x,y) => input.pointer({type,id:1,x,y});
      const menu = () => game === 'th01' ? input.nativeSample({menu:true}) : input.sample({play:false});
      input.touch({fireEnabled:true,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0});
      const idle = menu();
      pointer('down',.5,.5); pointer('up',.5,.5); const tap = menu();
      pointer('down',.5,.5); pointer('move',.5,.6); pointer('up',.5,.6); const down = menu();
      pointer('down',.5,.5); pointer('up',.5,.5);
      const gameplayTap = game === 'th01' ? input.nativeSample({menu:false}) : input.sample({play:true});
      pointer('down',.5,.5); pointer('move',.6,.5);
      const movement = game === 'th01' ? input.localSample() : input.motion().x;
      return {idle,tap,down,gameplayTap,movement};
    }, game);
    assert.equal(result.idle & (game === 'th01' ? 4 : 256), 0, `${game}: menu must not auto-confirm`);
    assert.equal(result.tap & (game === 'th01' ? 64 : 256), game === 'th01' ? 64 : 256, `${game}: tap confirms`);
    assert.equal(result.down & (game === 'th01' ? 32 : 2), game === 'th01' ? 32 : 2, `${game}: swipe moves down`);
    assert.equal(result.gameplayTap & (game === 'th01' ? 64 : 256), 0, `${game}: gameplay tap does not confirm a menu`);
    assert.ok(game === 'th01' ? result.movement & 2 : result.movement > 0, `${game}: direct movement reaches the game`);
    console.log('PASS', game, result);
  }
} finally { await browser.close(); }
