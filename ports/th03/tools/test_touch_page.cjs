const assert = require('node:assert/strict');
const {mkdirSync, existsSync, readFileSync} = require('node:fs');
const {resolve} = require('node:path');
const {chromium} = require(process.env.PLAYWRIGHT_PATH || 'C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');

(async () => {
  const executablePath = [chromium.executablePath(), 'C:/Program Files/Google/Chrome/Application/chrome.exe',
    'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe'].find(existsSync);
  const browser = await chromium.launch({headless: true, executablePath});
  const base = process.env.TH03_URL || 'http://127.0.0.1:9869';
  const output = resolve('reports/touch-check'); mkdirSync(output, {recursive: true});
  const errors = [];
  try {
    for (const [name, viewport] of Object.entries({desktop:{width:1280,height:900},portrait:{width:390,height:844},landscape:{width:844,height:390}})) {
      const context = await browser.newContext({viewport, hasTouch:true});
      const page = await context.newPage(); page.on('pageerror', e => errors.push(e.message));
      await page.goto(base + '/local.html');
      await page.locator('.touch-help-open').click();
      await page.locator('[data-unlimited]').check();
      await page.locator('[data-always-point]').check();
      assert.equal(await page.locator('[data-touch]').getAttribute('aria-pressed'), 'true');
      assert(await page.locator('[data-unlimited]').isChecked());
      const bounds = await page.locator('.touch-layout-editor').boundingBox();
      assert(bounds.x >= 0 && bounds.y >= 0 && bounds.x + bounds.width <= viewport.width + 1);
      assert(bounds.height <= viewport.height);
      assert(await page.evaluate(() => document.documentElement.scrollWidth <= innerWidth));
      await page.screenshot({path:resolve(output, name+'-settings.png'),fullPage:true});
      await page.locator('[data-save]').click();
      await page.reload(); await page.locator('.touch-help-open').click();
      assert(await page.locator('[data-unlimited]').isChecked());
      assert(await page.locator('[data-always-point]').isChecked());
      await page.locator('[data-touch]').click();
      assert.equal(await page.locator('[data-touch]').getAttribute('aria-pressed'), 'false');
      await page.locator('[data-save]').click();
      assert(!await page.locator('[data-layout-control=fire]').isVisible());
      await page.reload(); await page.locator('.touch-help-open').click();
      assert.equal(await page.locator('[data-touch]').getAttribute('aria-pressed'), 'false');
      await page.locator('[data-touch]').click(); await page.locator('[data-save]').click();
      await page.locator('#fullscreen').click();
      await page.waitForFunction(()=>document.getElementById('screen').classList.contains('immersive'));
      await page.locator('.touch-help-open').click();
      const fullBounds=await page.locator('.touch-layout-editor').boundingBox();
      assert(fullBounds.x>=0&&fullBounds.y>=0&&fullBounds.x+fullBounds.width<=viewport.width+1
        &&fullBounds.y+fullBounds.height<=viewport.height+1);
      await page.screenshot({path:resolve(output,name+'-fullscreen.png')});
      await page.locator('[data-window]').click();await page.locator('[data-save]').click();
      // Exercise the production player module without booting the DOS game.
      await page.route('**/app.js', route => route.fulfill({contentType:'text/javascript',body:
        "import {mountPlayer} from './player-ui.js'; window.testPlayer=mountPlayer(document.getElementById('screen'),{solo:true}); testPlayer.setActive(true);testPlayer.setContext({key:'round:1',play:true});"}));
      await page.reload(); await page.waitForFunction(() => !!window.testPlayer);
      await page.evaluate(()=>testPlayer.enter({native:false}));
      await page.evaluate(() => new Promise(resolve => requestAnimationFrame(() => requestAnimationFrame(resolve))));
      const rapid = await page.evaluate(async () => {
        const values=[];for(let i=0;i<12;i++){values.push(testPlayer.sample()&32);testPlayer.consume();await new Promise(r=>setTimeout(r,20));}return values;
      });
      assert(rapid.every(v=>v===0), 'idle touch must not fire');
      const shot = await page.locator('[data-layout-control=fire]').boundingBox();
      await page.mouse.move(shot.x+shot.width/2,shot.y+shot.height/2); await page.mouse.down();
      const charge = await page.evaluate(async () => {
        const values=[];for(let i=0;i<10;i++){values.push(testPlayer.sample()&32);testPlayer.consume();await new Promise(r=>setTimeout(r,20));}return values;
      });
      assert(charge.every(v=>v===32), 'holding shot must keep native charge down');
      await page.mouse.up();
      assert.equal(await page.evaluate(() => testPlayer.sample()&32), 0, 'release must send native keyup');
      await page.waitForTimeout(90);
      const resumed = await page.evaluate(() => testPlayer.sample()&32);
      assert.equal(resumed, 0, 'release must not enable automatic fire');
      const drag = await page.locator('.touch-surface').boundingBox();
      await page.mouse.move(drag.x+drag.width*.5,drag.y+drag.height*.35); await page.mouse.down();
      await page.mouse.move(drag.x+drag.width*.55,drag.y+drag.height*.4);
      const motion=await page.evaluate(async()=>{const {unpackTouch,ALWAYS_POINT}=await import('./touch-input.js');const v=testPlayer.pack(0);return {...unpackTouch(v),point:v>=ALWAYS_POINT};});
      assert(motion.active&&motion.unlimited&&motion.point&&motion.x>0&&motion.y>0);
      await page.mouse.up();
      await page.locator('.touch-help-open').click();
      await page.locator('[data-size]').fill('1.4'); await page.locator('[data-save]').click();
      const stored = await page.evaluate(() => JSON.parse(localStorage.getItem('th03.touch.layout.v2')));
      assert.equal(stored[name==='portrait'?'portrait':'landscape'].controls.fire.scale,1.4);
      await context.close();
    }
    // Single-player assists persist independently from multiplayer preferences.
    for(const viewport of [{width:390,height:844},{width:844,height:390}]){
      const context=await browser.newContext({viewport,hasTouch:true}),page=await context.newPage();
      page.on('pageerror',e=>errors.push(e.message));
      await page.goto(base+'/solo.html');
      await page.evaluate(()=>{localStorage.setItem('th03.touch.mode','unlimited');localStorage.setItem('th03.touch.always-point','on');});
      await page.reload();await page.locator('.touch-help-open').click();
      for(const name of ['focus-enabled','focus-points','unlimited','always-point']){
        const control=page.locator(`[data-${name}]`);assert.equal(await control.isChecked(),false);await control.check();
      }
      await page.locator('[data-save]').click();await page.reload();await page.locator('.touch-help-open').click();
      for(const name of ['focus-enabled','focus-points','unlimited','always-point'])assert(await page.locator(`[data-${name}]`).isChecked());
      assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
      await page.screenshot({path:resolve(output,`solo-${viewport.width}-settings.png`),fullPage:true});
      await page.locator('[data-focus-enabled]').uncheck();await page.locator('[data-save]').click();
      assert.equal(await page.locator('[data-layout-control=focus]').isVisible(),false);
      assert.equal(await page.evaluate(()=>localStorage.getItem('th03.touch.mode')),'unlimited');
      await context.close();
    }
    // Test network accumulation and native mailbox flags through real modules.
    const context=await browser.newContext(),page=await context.newPage();
    await page.goto(base+'/local.html');
    const meta=JSON.parse(readFileSync(resolve('web/native-patch.json')));
    const result=await page.evaluate(async meta=>{
      const {Netplay}=await import('./netplay.js');
      const {packTouch,unpackTouch,ALWAYS_POINT}=await import('./touch-input.js');
      const {NativePause}=await import('./native-pause.js');
      const network=new Netplay({role:'guest'});
      network.receive({type:'input',seq:1,actions:['shot'],touch:packTouch(0,{active:true,x:16,y:-32,alwaysPoint:true})});
      network.receive({type:'input',seq:2,actions:['focus'],touch:packTouch(0,{active:true,x:24,y:16,alwaysPoint:true})});
      const a=network.takeTouch(),b=network.takeTouch();
      const heap=new Uint8Array(1024*1024),emulator={state:'running',module:{HEAPU8:heap}};
      const bridge=new NativePause(()=>emulator,meta);bridge.live=1000;heap.set(new TextEncoder().encode(meta.signature),1000);
      heap[1000+meta.fields.phase]=1;bridge.setTouch([a,0]);
      const view=new DataView(heap.buffer);
      const native=[heap[1000+meta.fields.alwaysPoint],view.getInt16(1000+meta.fields.touch+2,true),view.getInt16(1000+meta.fields.touch+4,true)];
      bridge.setTouch([0,0]);
      return {a:unpackTouch(a),b:unpackTouch(b),point:a>=ALWAYS_POINT,native,off:heap[1000+meta.fields.alwaysPoint]};
    },meta);
    assert.equal(result.a.x,40);assert.equal(result.a.y,-16);
    assert.equal(result.b.x,0);assert.equal(result.b.y,0);assert(result.point);
    assert.deepEqual(result.native,[0,40,-16]);assert.equal(result.off,0);
    await page.route('**/app.js',route=>route.fulfill({contentType:'text/javascript',body:
      `import {mountPlayer} from './player-ui.js';
      import {mountControls} from './controls.js';
      import {packTouch} from './touch-input.js';
      const $=id=>document.getElementById(id),children=[...$('screen').children];
      window.testPlayer=mountPlayer($('screen'),{solo:true});const view=document.createElement('div');view.className='screen';view.append(...children);testPlayer.stage.prepend(view);
      testPlayer.setActive(true);testPlayer.setContext({key:'round:1',play:true});
      window.inputEvents=[];for(const type of ['keydown','keyup'])$('canvas').addEventListener(type,e=>inputEvents.push({type,code:e.code}));
      window.mockNetwork={localSeat:1,remote:new Set(['up']),takeTouch:()=>packTouch(0,{alwaysPoint:true}),send:(actions,touch)=>{window.lastSent={actions:[...actions],touch};}};
      window.testControls=mountControls({canvas:$('canvas'),getEmulator:()=>({state:'running'}),getNetwork:()=>mockNetwork,getTouch:()=>testPlayer,dialog:$('settings'),onGesture:()=>{},onOtherInput:()=>{},onPause:()=>{},onMenu:()=>{},onEscape:()=>{},onFocus:()=>{},onTouch:values=>window.lastTouches=values,isPaused:()=>false,inGameplay:()=>true});`}));
    await page.goto(base+'/local.html');
    await page.waitForFunction(()=>!!window.testControls);
    await page.locator('.touch-help-open').click();
    if(await page.locator('[data-touch]').getAttribute('aria-pressed')!=='true')await page.locator('[data-touch]').click();
    await page.locator('[data-save]').click();
    await page.locator('#canvas').focus();await page.keyboard.down('KeyZ');
    await page.waitForFunction(()=>window.inputEvents.some(e=>e.code==='ArrowLeft'&&e.type==='keydown'));
    const seatResult=await page.evaluate(()=>({remoteUp:inputEvents.some(e=>e.code==='KeyT'&&e.type==='keydown'),
      sentShot:lastSent.actions.includes('shot'),wrongShot:inputEvents.some(e=>e.code==='KeyZ'),remotePoint:lastTouches[0]>=2**42}));
    assert(seatResult.remoteUp&&seatResult.remotePoint&&!seatResult.wrongShot);
    await page.keyboard.up('KeyZ');await page.waitForTimeout(100);
    await page.locator('.touch-help-open').click();
    await page.waitForFunction(()=>!window.lastSent.actions.length&&window.lastSent.touch===0);
    assert.deepEqual(errors,[]);
    await page.route('**/app.js',route=>route.fulfill({contentType:'text/javascript',body:
      `import {mountControls} from './controls.js';
      const $=id=>document.getElementById(id);window.roomFocus=false;window.focusMask=-1;
      window.testControls=mountControls({canvas:$('canvas'),getEmulator:()=>({state:'ready',module:{netKey(){}}}),getRoomFocus:()=>roomFocus,
        dialog:$('settings'),onFocus:mask=>window.focusMask=mask,onGesture:()=>{},onOtherInput:()=>{},onPause:()=>{},onMenu:()=>{},onEscape:()=>{},isPaused:()=>false,inGameplay:()=>true});`}));
    await page.reload();await page.waitForFunction(()=>!!window.testControls);
    const policy=await page.evaluate(()=>{
      const input={actions:['focus'],touch:0,focusEnabled:true,points:true};
      testControls.applyFrame([input,input]);const disabled=focusMask;
      roomFocus=true;testControls.applyFrame([input,input]);return {disabled,enabled:focusMask};
    });
    assert.deepEqual(policy,{disabled:0,enabled:3},'Host policy must apply to both seats, ignoring personal frame flags');
    assert.deepEqual(errors,[]);
    console.log('PASS: desktop/portrait/landscape touch UI, persisted toggles/layout, rapid fire/charge/release, drag, remote accumulation and native point flags; DOS game not booted');
  } finally { await browser.close(); }
})().catch(error=>{console.error(error);process.exitCode=1;});
