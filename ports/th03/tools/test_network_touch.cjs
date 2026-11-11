const assert=require('node:assert/strict');
const {existsSync,mkdirSync}=require('node:fs');
const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
(async()=>{
 const browser=await chromium.launch({headless:true,executablePath:[chromium.executablePath(),'C:/Program Files/Google/Chrome/Application/chrome.exe','C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe'].find(existsSync)});
 const base=process.env.TH03_URL||'http://127.0.0.1:9874';const errors=[];mkdirSync('reports/network-touch',{recursive:true});
 try{
  for(const viewport of [{width:390,height:844},{width:844,height:390}]){
   const context=await browser.newContext({viewport,hasTouch:true});const page=await context.newPage();page.on('pageerror',e=>errors.push(e.message));
   await context.addInitScript(()=>{
    if(!localStorage.getItem('th03.touch.layout.v2'))localStorage.setItem('th03.touch.layout.v2',JSON.stringify({
     portrait:{controls:{confirm:{x:.8,y:.7,scale:1}}},landscape:{controls:{confirm:{x:.8,y:.7,scale:1}}}
    }));
   });
   await page.route('**/app.js',route=>route.fulfill({contentType:'text/javascript',body:`
    import {mountPlayer} from './player-ui.js';import {mountControls} from './controls.js';
    const canvas=document.getElementById('canvas');window.events=[];window.down=new Set();window.focusMask=0;window.allowed=false;window.playing=true;
    const emulator={state:'running',config:{lockstep:true},module:{netInfo:()=>({now:0}),netKey:(type,e)=>{events.push([type,e.code]);type==='keydown'?down.add(e.code):down.delete(e.code);}}};
    window.testPlayer=mountPlayer(document.getElementById('screen'),{solo:true,network:true});testPlayer.stage.prepend(canvas);
    window.testControls=mountControls({canvas,getEmulator:()=>emulator,getNetwork:()=>null,getTouch:()=>testPlayer,getRoomFocus:()=>true,getRoomUnlimited:()=>allowed,
     dialog:document.getElementById('settings'),onGesture(){},onOtherInput(){},onPause(){},onMenu(){},onEscape(){},onFocus:mask=>focusMask=mask,onTouch:value=>window.touches=value,isPaused:()=>false,inGameplay:()=>playing});
    testPlayer.setActive(true);testPlayer.setContext({key:'round:1',play:true});testPlayer.enter({native:false});canvas.focus();`}));
   await page.goto(base+'/local.html');await page.waitForFunction(()=>window.testControls);
   await page.waitForTimeout(150);assert.equal(await page.evaluate(()=>events.length),0,'No idle autofire');
   // Press+release between browser frames must still generate a single native shot.
   await page.evaluate(()=>{for(const type of ['keydown','keyup'])window.dispatchEvent(new KeyboardEvent(type,{code:'KeyZ',bubbles:true}));});
   await page.waitForTimeout(280);assert.equal(await page.evaluate(()=>events.filter(([t,c])=>t==='keydown'&&c==='KeyZ').length),1);
   await page.evaluate(()=>events.length=0);await page.keyboard.down('z');await page.waitForTimeout(1050);await page.keyboard.up('z');await page.waitForTimeout(80);
   const count=await page.evaluate(()=>events.filter(([t,c])=>t==='keydown'&&c==='KeyZ').length);assert(count>=12&&count<=16,`Expected native maximum 15Hz with browser scheduling jitter, observed ${count}`);
   await page.keyboard.down('Shift');await page.waitForTimeout(90);assert(await page.evaluate(()=>down.has('KeyZ')));assert.equal(await page.evaluate(()=>focusMask),0);
   await page.keyboard.down('Space');await page.waitForTimeout(50);assert.equal(await page.evaluate(()=>focusMask),1);assert(await page.evaluate(()=>down.has('KeyZ')));
   await page.keyboard.up('Shift');await page.keyboard.up('Space');await page.waitForTimeout(100);assert.equal(await page.evaluate(()=>down.has('KeyZ')),false);
   await page.keyboard.down('k');await page.keyboard.down('ControlLeft');await page.waitForTimeout(50);assert(await page.evaluate(()=>down.has('ArrowLeft')));assert.equal(await page.evaluate(()=>focusMask),2);
   await page.keyboard.up('k');await page.keyboard.up('ControlLeft');await page.waitForTimeout(100);
   const rapid=page.locator('[data-rapid]'),charge=page.locator('[data-charge]');
   assert(await rapid.isVisible());assert(await charge.isVisible());
   assert.equal(await page.locator('[data-layout-control=confirm], [data-pulse="256"]').count(),0,'The redundant touch confirmation button is removed');
   assert.equal(await page.locator('[data-selected] option[value=confirm]').count(),0,'Old stored confirmation layouts do not recreate the button');
   const box=await rapid.boundingBox();assert(box.x>=0&&box.y>=0&&box.x+box.width<=viewport.width&&box.y+box.height<=viewport.height);
   await rapid.hover();await page.mouse.down();await page.waitForTimeout(430);await page.mouse.up();await page.waitForTimeout(100);assert.equal(await page.evaluate(()=>down.has('KeyZ')),false);
   await charge.hover();await page.mouse.down();await page.waitForTimeout(300);assert(await page.evaluate(()=>down.has('KeyZ')));await page.mouse.up();await page.waitForTimeout(100);assert.equal(await page.evaluate(()=>down.has('KeyZ')),false);
   await page.evaluate(()=>{playing=false;testPlayer.setContext({key:'original:menu',play:false});events.length=0;});
   await page.keyboard.down('z');await page.waitForTimeout(700);await page.keyboard.up('z');await page.waitForTimeout(100);
   assert(await page.evaluate(()=>events.filter(([t,c])=>t==='keydown'&&c==='KeyZ').length>=5),'Holding Z repeatedly confirms outside combat');
   await page.evaluate(()=>events.length=0);await rapid.hover();await page.mouse.down();await page.waitForTimeout(700);await page.mouse.up();await page.waitForTimeout(100);
   assert(await page.evaluate(()=>events.filter(([t,c])=>t==='keydown'&&c==='KeyZ').length>=5),'Touch rapid repeatedly advances dialogue');
   assert.equal(await page.evaluate(()=>down.has('KeyZ')),false,'Dialogue rapid releases Z after lifting the finger');
   const confirmations=await page.evaluate(()=>{testControls.release();testControls.restore(null);events.length=0;
    for(let i=0;i<12;i++)testControls.applyFrame([0,1].map(()=>({actions:['rapid'],touch:0})));
    testControls.applyFrame([0,1].map(()=>({actions:[],touch:0})));
    return ['KeyZ','ArrowLeft'].map(code=>events.filter(([,c])=>c===code).map(([type])=>type));});
   assert.deepEqual(confirmations,Array.from({length:2},()=>['keydown','keyup','keydown','keyup','keydown','keyup']),'Both synchronized seats repeat dialogue confirmation');
   await page.evaluate(()=>{playing=true;testPlayer.setContext({key:'round:1',play:true});});
   // A stored unlimited preference cannot override the room policy.
   await page.evaluate(()=>{document.querySelector('[data-unlimited]').checked=true;testPlayer.setUnlimitedAllowed(false);});
   assert(await page.locator('[data-unlimited]').isDisabled());
   const surface=await page.locator('.touch-surface').boundingBox();await page.mouse.move(surface.x+surface.width*.45,surface.y+surface.height*.4);await page.mouse.down();await page.mouse.move(surface.x+surface.width*.5,surface.y+surface.height*.5);
   assert.equal(await page.evaluate(()=>Math.floor(testPlayer.pack(0)/2**41)%2),0);await page.mouse.up();
   // Both remote lanes are sanitized at simulation application, regardless of sender UI.
   assert.deepEqual(await page.evaluate(()=>{testControls.applyFrame([0,1].map(()=>({actions:[],touch:2**40+2**41+4096})));return touches.map(v=>Math.floor(v/2**41)%2);}),[0,0]);
   await page.screenshot({path:`reports/network-touch/${viewport.width}-hold-controls.png`});
   await page.locator('.touch-help-open').click();await page.locator('[data-selected]').selectOption('rapid');await page.locator('[data-size]').fill('1.2');await page.locator('[data-save]').click();
   await page.reload();await page.waitForFunction(()=>window.testControls);assert.equal(await rapid.evaluate(n=>n.style.getPropertyValue('--control-scale')),'1.2');
   await context.close();
  }
  assert.deepEqual(errors,[]);console.log('PASS: keyboard/touch dialogue repeat, both synchronized seats, removed confirmation/legacy layout, tap/hold, charge/focus, no autofire, host policy and portrait/landscape layout');
 }finally{await browser.close();}
})().catch(e=>{console.error(e);process.exit(1);});
