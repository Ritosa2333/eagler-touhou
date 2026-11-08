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
      let focus = 0, releasedFocus = 0, back = 0;
      if (game !== 'th01') {
        input.clear(); options.touchFocusMode = 'two-finger';
        input.pointer({type:'down',id:1,x:.5,y:.5}); input.pointer({type:'down',id:2,x:.6,y:.5});
        focus = input.sample({play:true});
        input.pointer({type:'up',id:2,x:.6,y:.5}); releasedFocus = input.sample({play:true});
        input.clear(); input.pointer({type:'down',id:1,x:.5,y:.5}); input.pointer({type:'down',id:2,x:.6,y:.5});
        input.pointer({type:'up',id:2,x:.6,y:.5}); input.pointer({type:'up',id:1,x:.5,y:.5}); back = menu();
      } else {
        input.clear(); input.pointer({type:'down',id:1,x:.5,y:.5}); input.pointer({type:'down',id:2,x:.6,y:.5});
        input.pointer({type:'up',id:2,x:.6,y:.5}); input.pointer({type:'up',id:1,x:.5,y:.5}); back = menu();
      }
      let quickSwipe,releaseAfterSample,controls;
      if(game==='th02'||game==='th04'){
        input.clear();input.touch({fireEnabled:true,focusEnabled:true,bombSerial:1,escapeSerial:1,joystickX:20000,joystickY:-20000});
        controls={first:input.sample({play:true}),held:input.sample({play:true})};
        input.clear();pointer('down',.5,.5);pointer('move',.6,.5);pointer('up',.6,.5);
        quickSwipe=Array.from({length:5},()=>input.motion());
        input.clear();pointer('down',.5,.5);pointer('move',.6,.5);input.motion();pointer('up',.6,.5);
        releaseAfterSample=Array.from({length:5},()=>input.motion());
      }
      let slide;
      if (game === 'th01') {
        const touch = focusEnabled => input.touch({fireEnabled:false,focusEnabled,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0});
        input.clear(); touch(true);
        const localHeld = input.localSample(), netHeld = input.nativeSample(), compatHeld = input.sample();
        touch(false);
        const localReleased = input.localSample(), netReleased = input.nativeSample(), compatReleased = input.sample();
        touch(true); touch(false);
        const localTap = input.localSample(), netTap = input.nativeSample(), compatTap = input.sample();
        slide = {localHeld,netHeld,compatHeld,localReleased,netReleased,compatReleased,localTap,netTap,compatTap};
      }
      return {idle,tap,down,gameplayTap,movement,focus,releasedFocus,back,slide,quickSwipe,releaseAfterSample,controls};
    }, game);
    assert.equal(result.idle & (game === 'th01' ? 4 : game === 'th03' ? 512 : 256), 0, `${game}: menu must not auto-confirm`);
    assert.equal(result.tap & (game === 'th01' ? 64 : game === 'th03' ? 512 : 256), game === 'th01' ? 64 : game === 'th03' ? 512 : 256, `${game}: tap confirms`);
    assert.equal(result.down & (game === 'th01' ? 32 : 2), game === 'th01' ? 32 : 2, `${game}: swipe moves down`);
    assert.equal(result.gameplayTap & (game === 'th01' ? 64 : game === 'th03' ? 512 : 256), game === 'th03' ? 512 : 0, `${game}: gameplay tap does not confirm a menu`);
    assert.ok(game === 'th01' ? result.movement & 2 : result.movement > 0, `${game}: direct movement reaches the game`);
    if(result.quickSwipe){
      assert.equal(result.controls.first&249,249,`${game}: touch shot, bomb, focus, pause and joystick are delivered`);
      assert.equal(result.controls.held&249,105,`${game}: bomb and pause are one-shot while held controls persist`);
      assert.ok(result.quickSwipe[0].active&&result.quickSwipe[0].begin&&result.quickSwipe[0].x>0,`${game}: short swipe survives release`);
      assert.ok(result.quickSwipe.slice(1,4).every(m=>m.active&&m.x===0&&!m.begin),`${game}: held target does not repeat the delta`);
      assert.equal(result.quickSwipe[4].active,false,`${game}: short swipe releases after consumption`);
      assert.ok(result.releaseAfterSample.slice(0,4).every(m=>m.active&&m.x===0),`${game}: sampled swipe remains active until native consumption`);
      assert.equal(result.releaseAfterSample[4].active,false,`${game}: sampled swipe releases`);
    }
    if (game !== 'th01') {
      assert.equal(result.focus & 64, 64, `${game}: second finger holds focus`);
      assert.equal(result.releasedFocus & 64, 0, `${game}: releasing second finger releases focus`);
      assert.equal(result.back & (game === 'th03' ? 640 : 384), 128, `${game}: two-finger menu tap goes back without confirming`);
    } else assert.equal(result.back & 192, 128, `${game}: two-finger menu tap goes back without confirming`);
    if (game === 'th01') {
      for (const key of ['localHeld','netHeld','localTap','netTap']) assert.equal(result.slide[key] & 8, 8, key);
      for (const key of ['compatHeld','compatTap']) assert.equal(result.slide[key] & 16, 16, key);
      for (const key of ['localReleased','netReleased']) assert.equal(result.slide[key] & 8, 0, key);
      assert.equal(result.slide.compatReleased & 16, 0);
    }
    console.log('PASS', game, result);
  }
} finally { await browser.close(); }
