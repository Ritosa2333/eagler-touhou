import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
const browser = await puppeteer.launch({executablePath: await findChromiumExecutable(), headless: true, args: ['--no-sandbox']});
try {
  for (const game of ['th02','th04']) {
    const page = await browser.newPage();
    await page.goto(process.env.EAGLER_TEST_URL || 'http://127.0.0.1:8132/');
    const result = await page.evaluate(async game => {
      const {managedInput} = await import(`/ports/${game}/build-eagler/managed-input.mjs`);
      const input = managedInput({game,epoch:1,active:()=>true,options:()=>({touchEnabled:true})});
      let changes = 0; input.onChange = () => changes++;
      const key = (type,code) => window.dispatchEvent(new KeyboardEvent(type,{code,key:code,bubbles:true,cancelable:true}));
      key('keydown','ArrowLeft'); key('keyup','ArrowLeft'); key('keydown','ArrowRight');
      const reverse = input.sample();
      key('keyup','ArrowRight'); input.sample();
      key('keydown','ArrowUp'); key('keyup','ArrowUp'); key('keydown','ArrowRight');
      const axes = input.sample();
      key('keyup','ArrowRight'); input.sample();
      key('keydown','KeyZ'); input.sample();
      Object.defineProperty(document,'hidden',{configurable:true,value:true});
      document.dispatchEvent(new Event('visibilitychange'));
      const hidden = input.sample();
      Object.defineProperty(document,'hidden',{configurable:true,value:false});
      key('keydown','KeyZ'); input.sample(); window.dispatchEvent(new Event('pagehide'));
      const pagehide = input.sample();
      await Promise.resolve();
      const keyChanges = changes;
      input.touch({fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0});
      await Promise.resolve();
      const touchChanges = changes - keyChanges;
      return {reverse,axes,hidden,pagehide,keyChanges,touchChanges};
    },game);
    assert.equal(result.reverse & 12, 8, `${game} quick reverse must favor latest direction`);
    assert.equal(result.axes & 15, 9, `${game} independent axis tap must survive`);
    assert.equal(result.hidden & 32, 0, `${game} hidden clears held keys`);
    assert.equal(result.pagehide & 32, 0, `${game} pagehide clears held keys`);
    assert.ok(result.keyChanges > 0, `${game} keyboard changes notify sync`);
    assert.equal(result.touchChanges, 1, `${game} touch changes notify sync`);
    console.log('PASS',game,result); await page.close();
  }
} finally {await browser.close();}
