// UI regression with native state stubbed; real network coverage lives in test_sync_game.cjs.
const assert=require('node:assert/strict');
const {readFileSync,existsSync}=require('node:fs');
const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
(async()=>{
  const browser=await chromium.launch({headless:true,executablePath:[chromium.executablePath(),'C:/Program Files/Google/Chrome/Application/chrome.exe','C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe'].find(existsSync)});
  try{
    const page=await browser.newPage();const errors=[];page.on('pageerror',e=>errors.push(e.message));
    await page.route('**/app.js',route=>route.fulfill({contentType:'text/javascript',body:readFileSync('web/app.js','utf8')+`
      emulator={state:'running',module:{},pause(){this.state='paused';},run(){this.state='running';}};
      nativePause={update:()=>({phase:1,generation:1}),result(){},markers:()=>[],setFocus(){},setTouch(){},request(command){window.lastConcession=command;return true;}};
      activeLang='cn';$('cover').hidden=true;
      window.pauseTest={player,open:openPause,seat:()=>pauseSeat};
    `}));
    await page.goto((process.env.TH03_URL||'http://127.0.0.1:9874')+'/local.html');
    await page.waitForFunction(()=>window.pauseTest);
    await page.evaluate(()=>pauseTest.player.enter({native:false}));
    await page.locator('#canvas').focus();await page.keyboard.press('Escape');
    assert.equal(await page.evaluate(()=>pauseTest.seat()),0);
    assert.equal(await page.locator('[data-choice=match]').textContent(),'背水阵');
    await page.keyboard.press('ArrowDown');
    // Lose the selected button, click the shade, then continue via keyboard.
    await page.evaluate(()=>document.activeElement.blur());
    await page.locator('#pause-shade').click({position:{x:5,y:5}});
    assert.equal(await page.locator('[data-choice=round]').evaluate(n=>n===document.activeElement),true);
    await page.keyboard.press('ArrowUp');await page.keyboard.press('Space');
    assert.equal(await page.evaluate(()=>pauseTest.seat()),null);
    await page.evaluate(()=>pauseTest.open(1));
    await page.keyboard.press('Escape');assert.equal(await page.evaluate(()=>pauseTest.seat()),1,'P1 must not dismiss P2 pause');
    await page.keyboard.press('KeyS');await page.keyboard.press('KeyW');
    await page.evaluate(()=>document.activeElement.blur());
    await page.keyboard.press('Enter');assert.equal(await page.evaluate(()=>pauseTest.seat()),null);
    await page.evaluate(()=>pauseTest.open(0));
    await page.locator('[data-choice=round]').click();
    assert.equal(await page.evaluate(()=>window.lastConcession),1);
    // A browser-owned exit may swallow Esc entirely: fullscreenchange must pause.
    await page.evaluate(()=>pauseTest.player.enter());
    await page.waitForFunction(()=>!!document.fullscreenElement);
    await page.evaluate(()=>document.exitFullscreen());
    await page.waitForFunction(()=>pauseTest.seat()===0);
    assert(await page.locator('#screen').evaluate(n=>n.classList.contains('immersive')));
    await page.keyboard.press('Enter');assert.equal(await page.evaluate(()=>pauseTest.seat()),null);
    await page.evaluate(()=>pauseTest.player.exit());
    assert.equal(await page.evaluate(()=>pauseTest.seat()),null,'Explicit return to page must not pause');
    assert.equal(await page.locator('#screen').evaluate(n=>n.classList.contains('immersive')),false);
    // The fullscreen icon retains DOM focus: Esc must still reach pause controls.
    await page.locator('.touch-full-open').click();
    await page.waitForFunction(()=>!!document.fullscreenElement);
    await page.keyboard.press('Escape');
    await page.waitForFunction(()=>pauseTest.seat()===0);
    await page.keyboard.press('Enter');
    assert.equal(await page.evaluate(()=>pauseTest.seat()),null);
    await page.locator('.touch-full-open').click();
    await page.waitForFunction(()=>!document.fullscreenElement);
    assert.equal(await page.evaluate(()=>pauseTest.seat()),null);
    assert.deepEqual(errors,[]);
    console.log('PASS: pause keyboard/click, focus loss, ownership, CN label, native fullscreen exit fallback');
  }finally{await browser.close();}
})().catch(e=>{console.error(e);process.exit(1);});
