import assert from 'node:assert/strict';
import {existsSync} from 'node:fs';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import puppeteer from 'puppeteer-core';
import {inspectUiAppearance} from './ui-appearance-probe.mjs';
import {readUiLayout,layoutDifferences,inspectInputAlignment,inspectMastheadLayers,inspectPopupLayers,inspectSurfaceMotion,inspectLayoutMotion,captureSurfaceMotion,surfaceMotionFailures} from './ui-layout-probe.mjs';

const origin=process.argv.slice(2).find(argument=>!argument.startsWith('--'))||'http://127.0.0.1:8151/';
const motionOnly=process.argv.includes('--motion-only');
const output=resolve(process.env.UI_AUDIT_OUTPUT||'.cache/ui-system');
const executablePath=process.env.CHROME_PATH||[
  'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',
  'C:/Program Files/Microsoft/Edge/Application/msedge.exe',
  'C:/Program Files/Google/Chrome/Application/chrome.exe',
].find(existsSync);
assert.ok(executablePath,'Set CHROME_PATH to an installed Chromium browser');
await mkdir(output,{recursive:true});
const browser=await puppeteer.launch({headless:true,executablePath});
const frames=[],motionTraces=[],failures=[];
const matrix=[{name:'landscape',width:1280,height:800},{name:'portrait',width:390,height:844}];
const fixtures=name=>new URL('/@fs/'+resolve('tests/visual/'+name).replaceAll('\\','/'),origin).href;
let page,theme,view;
async function click(selector){await page.waitForSelector(selector,{visible:true});await page.click(selector);}
async function button(name,scope='button'){
  const handles=await page.$$(scope);
  for(const handle of handles){
    const matches=await handle.evaluate((e,name)=>!e.closest('[hidden]')&&e.getBoundingClientRect().height>0&&(e.getAttribute('aria-label')===name||e.textContent.trim()===name),name);
    if(matches){await handle.click();return;}
    await handle.dispose();
  }
  throw Error('Visible control missing: '+name);
}
async function go(path){
  await page.goto(path.startsWith('http')?path:new URL(path,origin).href,{waitUntil:'networkidle2'});
  await page.evaluate(()=>document.fonts.ready);
}
async function fixture(name){
  await go(fixtures(name));
  if(await page.evaluate(()=>document.documentElement.dataset.bookTheme)!==theme)await button(theme==='light'?'亮色':'深色');
}
async function shot(module){
  const appearance=await page.evaluate(inspectUiAppearance);
  const inputAlignment=await page.evaluate(inspectInputAlignment);
  const mastheadLayers=await page.evaluate(inspectMastheadLayers);
  const popupLayers=await page.evaluate(inspectPopupLayers);
  const surfaceMotion=await page.evaluate(inspectSurfaceMotion);
  const layoutMotion=await page.evaluate(inspectLayoutMotion);
  const file=`${module}-${theme}-${view.name}.png`;
  const location=page.url();
  frames.push({file,module,theme,orientation:view.name,width:view.width,height:view.height,url:location,appearance,inputAlignment,mastheadLayers,popupLayers,surfaceMotion,layoutMotion});
  if(appearance.errors.length)failures.push({file,errors:appearance.errors});
  if(inputAlignment.errors.length)failures.push({file,errors:inputAlignment.errors});
  if(mastheadLayers.errors.length)failures.push({file,errors:mastheadLayers.errors});
  if(popupLayers.errors.length)failures.push({file,errors:popupLayers.errors});
  if(surfaceMotion.errors.length)failures.push({file,errors:surfaceMotion.errors});
  if(layoutMotion.errors.length)failures.push({file,errors:layoutMotion.errors});
  const frameControl=await page.$('#specimenFraming');
  if(frameControl)await frameControl.click();
  await page.screenshot({path:resolve(output,file),fullPage:false});
  if(frameControl)await frameControl.click();
}
async function closeNotice(){await page.evaluate(()=>document.querySelector('#siteNotice:not([hidden]) #siteNoticeClose')?.click());}
async function closeTouch(){await click('#touchLayoutExit');if(await page.$('dialog[open] #decisionConfirm'))await click('#decisionConfirm');await page.waitForSelector('#touchLayoutEditor',{hidden:true});}
async function stableLayout(before,label){
  assert.deepEqual(layoutDifferences(before,await page.evaluate(readUiLayout)),[],label);
}
async function inputPanelOrigin(){
  return page.$eval('.input-device-panel',element=>{const {x,y}=element.getBoundingClientRect();return{x,y};});
}
async function stableInputPanel(before){
  const after=await inputPanelOrigin();
  for(const axis of ['x','y'])assert.ok(Math.abs(after[axis]-before[axis])<=.5,'input panel moved on '+axis);
}
async function application(){
  await go('/');await page.waitForSelector('#mastheadMenuToggle:not(:disabled)');await page.waitForSelector('#eaglerPreload',{hidden:true});await closeNotice();
  await click('[data-book-view=covers]');await shot('home');
  for(const label of ['编号倒序','按作品名称','按作品编号']){
    const before=await page.evaluate(readUiLayout);
    await click('.book-sort');await page.waitForSelector('[role=option]',{visible:true});await stableLayout(before,'sort open');
    await button(label,'[role=option]');await stableLayout(before,'sort selection');
  }
  await click('[data-book-view=list]');await shot('home-list');await click('[data-book-view=covers]');
  await page.type('#librarySearch','no-matching-visual-specimen');await shot('home-empty');
  // Reload clears the unsaved search.
  await go('/');await page.waitForSelector('#eaglerPreload',{hidden:true});await closeNotice();
  await click('#mastheadMenuToggle');await shot('more-menu');await page.keyboard.press('Escape');
  assert.equal(await page.evaluate(()=>document.activeElement?.id),'mastheadMenuToggle');
  await page.evaluate(()=>window.scrollTo({top:document.documentElement.scrollHeight,behavior:'instant'}));
  await click('.book-game-link[href*="game=th10"]');await page.waitForSelector('.tools.is-open',{visible:true});await shot('options-scrolled');
  await click('#libraryBack');await page.waitForSelector('body:not(.library-tools-open)');
  await page.evaluate(()=>window.scrollTo({top:0,behavior:'instant'}));
  await click('.book-game-link[href*="game=th06"]');await page.waitForSelector('.tools.is-open',{visible:true});await shot('options');
  for(const [name,module]of [['键盘改键','keyboard'],['手柄改键','gamepad']]){
    await page.$eval('.input-settings-entries',element=>element.scrollIntoView({block:'nearest',behavior:'instant'}));
    const before=await page.evaluate(readUiLayout);
    await button(name);await page.waitForSelector('.input-settings-dialog',{visible:true});await stableLayout(before,'input dialog open');await shot(module);
    if(module==='keyboard'){
      const opened=await page.evaluate(readUiLayout),panel=await inputPanelOrigin();
      await click('#keyboardLayoutSelect-trigger');await shot('keyboard-layout-menu');await button('WASD','[role=option]');
      assert.equal(await page.$eval('#keyboardLayoutSelect',e=>e.value),'wasd');
      await click('#keyboardLayoutSelect-trigger');await button('上下左右','[role=option]');
      await button('手柄改键','.input-device-tabs [role=tab]');await stableLayout(opened,'input device switch');await stableInputPanel(panel);await shot('input-device-switch');
      await button('键盘改键','.input-device-tabs [role=tab]');await stableLayout(opened,'input device return');await stableInputPanel(panel);
    }
    await page.keyboard.press('Escape');await page.waitForSelector('.input-settings-dialog',{hidden:true});await stableLayout(before,'input dialog close');
  }
  // Rotate while Radix owns the scroll lock, then compare the underlying sheet
  // before/after close. Separate fixed-size screenshots missed this boundary.
  const originalView=view,originalViewport=page.viewport();
  await button('键盘改键');await page.waitForSelector('.input-settings-dialog',{visible:true});
  try {
    view={name:originalView.name+'-rotated',width:originalViewport.height,height:originalViewport.width};
    await page.setViewport({...originalViewport,width:view.width,height:view.height});
    const settle=()=>page.evaluate(()=>new Promise(resolve=>requestAnimationFrame(()=>requestAnimationFrame(resolve))));
    const sheet=()=>page.$eval('.tools.is-open',element=>{const r=element.getBoundingClientRect();return{x:r.x,y:r.y,width:r.width,height:r.height};});
    await settle();const beforeClose=await sheet();
    await page.keyboard.press('Escape');await page.waitForSelector('.input-settings-dialog',{hidden:true});
    await settle();const afterClose=await sheet();
    for(const property of ['x','y','width','height'])assert.ok(Math.abs(afterClose[property]-beforeClose[property])<=.5,'rotated input close shifted options '+property);
    await shot('keyboard-rotation-close');
  } finally {view=originalView;await page.setViewport(originalViewport);}
  const advanced=await page.$('#advancedOptions [aria-expanded]');if(await advanced.evaluate(e=>e.getAttribute('aria-expanded'))!=='true')await advanced.click();await shot('advanced');
  if(await page.$eval('#mobileOptionsToggle',e=>e.getAttribute('aria-expanded'))!=='true')await click('#mobileOptionsToggle');
  await click('#touchLayoutEdit');await page.waitForSelector('#touchLayoutEditor',{visible:true});await shot('touch');
  await click('#touchMovementMode-trigger');await page.waitForSelector('.ui-select-content',{visible:true});
  assert.equal(await page.$eval('.ui-select-content',e=>e.dataset.uiSurface),'application');await shot('touch-menu');await page.keyboard.press('Escape');
  const fire=await page.$('#touchFire'),box=await fire.boundingBox();
  await page.mouse.move(box.x+box.width/2,box.y+box.height/2);await page.mouse.down();await page.mouse.move(box.x+box.width/2+24,box.y+box.height/2+16,{steps:4});await page.mouse.up();
  assert.equal(await page.$eval('#touchLayoutSave',e=>e.disabled),false);await shot('touch-drag');await closeTouch();
}
async function profile(){
  await go('/profile.html');await page.waitForSelector('.profile-tabs');
  for(const [label,module]of [['概览','overview'],['生涯','career'],['游玩记录','records'],['内容','content'],['好友','friends']]){
    await button(label,'.profile-tabs [role=tab]');await shot('profile-'+module);
    if(module==='career'){await button('全屏查看通关图');await shot('career-viewer');await button('编辑记录','.career-viewer[open] button');await shot('career-editor');await button('关闭全屏通关图');}
  }
  await button('账户设置');
  for(const[label,module]of [['编辑资料','edit'],['隐私','privacy'],['账户与安全','security']]){await button(label,'.profile-settings-heading [role=tab]');await page.keyboard.press('Control+Home');await shot('profile-'+module);await page.keyboard.press('Control+End');await shot('profile-'+module+'-bottom');}
  await go('/login.html');await page.waitForSelector('.account-switch');await shot('account-login');await button('注册账号','.account-switch');await shot('account-register');
}
async function directory(){
  await fixture('directory.html');await page.waitForSelector('#roomList .lobby-room-row');
  await button('普通模式');await shot('directory-normal');await button('列表模式');await shot('directory-list');
  for(const[label,module]of [['创建房间','create'],['输入房间号','join']]){await button(label);await page.waitForSelector('#roomDialog[open]');await shot('directory-'+module);await click('#closeDialog');}
  await button('切换空列表');await shot('directory-empty');
}
async function room(){
  await fixture('room-frame.html');await page.waitForSelector('#mpRoomView');await shot('room-main');
  for(const [label,module]of [['房间 / 难度设置','game'],['快捷发言','chat']]){await button(label,'[role=tab]');await shot('room-'+module);}
  await button('我的角色');await shot('room-personal');await button('完成');await click('.room-spectator-preview-trigger');await shot('room-spectators');await button('完成');await button('设置');await shot('room-options');
}
async function replay(){
  await fixture('replay.html');await button('打开录像');await page.waitForSelector('.replay-workspace-files');await shot('replay-idle');
  await click('[data-replay-watch="replay/th6_01.rpy"]');await click('[data-replay-file="replay/th6_01.rpy"] [data-replay-stage="2"]');await shot('replay-playback');await button('管理 th6_01.rpy');await shot('replay-menu');await button('移动','[role=menuitem]');await shot('replay-move');await button('取消');
  await button('管理 th6_01.rpy');await button('删除','[role=menuitem]');await page.waitForSelector('dialog[open] #decisionCancel');await shot('replay-confirm');await click('#decisionCancel');
  assert.equal(await page.evaluate(()=>document.activeElement?.getAttribute('aria-label')),'管理 th6_01.rpy');
  await fixture('replay.html?game=th09mp');await button('打开录像');await click('[data-replay-file="replay/th9_01.rpy"]');await shot('replay-rounds');
}
async function input(){
  await fixture('input.html');await button('手柄改键');await page.waitForSelector('.input-settings-dialog');await shot('gamepad-device');
  await click('#inputGamepadDevice-trigger');await shot('gamepad-device-menu');await page.keyboard.press('Escape');
  const slider=await page.$('.input-axis-threshold [role=slider]');await slider.press('Home');await slider.press('ArrowRight');await shot('gamepad-threshold');
  const opened=await page.evaluate(readUiLayout);
  await click('.input-context-toolbar .ui-select-trigger');await shot('input-context-menu');await button('本地双人','[role=option]');await stableLayout(opened,'input context switch');await shot('local-input');
  await click('#th09LocalLayout-trigger');await shot('local-layout-menu');await page.keyboard.press('Escape');await button('玩家 2','[role=tab]');await shot('local-player2');
}
async function windows(){
  await fixture('windows.html');
  for(const[label,module]of [['旁观控制','spectator-controls'],['资源导入','resource-import'],['下载任务','transfer-progress'],['下载失败','transfer-error'],['错误详情','startup-error'],['标题联机','title-network'],['联机状态','netplay-connection'],['网络诊断','network-results'],['标定报告','calibration-report']]){
    await button(label,'.window-specimen-nav button');await shot(module);if(module==='calibration-report')await button('关闭','dialog[open] button');
  }
  await button('触控帮助','.window-specimen-nav button');await shot('touch-help');await click('#help-controls .options-group-head');await shot('touch-help-focus');
  await button('格斗操作','.window-specimen-nav button');await shot('fighting-help');
}
async function verifyMotion(name,end,action,{waitForInput=true,expectMotion=true}={}) {
  const pending=page.evaluate(captureSurfaceMotion,{waitForInput,duration:expectMotion?1000:600});
  void pending.catch(()=>{});
  await action();
  const trace=await pending,inspection=await page.evaluate(inspectSurfaceMotion);
  const errors=[...surfaceMotionFailures(trace,{end,expectMotion}),...inspection.errors];
  const assets=await page.$$eval('script[src],link[rel="stylesheet"]',nodes=>nodes.map(node=>node.src||node.href));
  motionTraces.push({name,theme,view:view.name,url:page.url(),assets,trace,errors});
  assert.deepEqual(trace.viewport,{width:view.width,height:view.height},'actual motion viewport');
  assert.equal(trace.theme,theme,'actual motion theme');
  assert.deepEqual(errors,[],name);
}
async function motionAcceptance(mode){
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:mode==='system-reduced'?'reduce':'no-preference'}]);
  await page.evaluateOnNewDocument(({theme,mode})=>{
    localStorage.setItem('eagler.book.theme',JSON.stringify(theme));
    localStorage.setItem('eagler-touhou-less-motion-v1',mode==='site-reduced'?'1':'0');
    localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');
  },{theme,mode});
  await go('/');await page.waitForSelector('#mastheadMenuToggle:not(:disabled)');await page.waitForSelector('#eaglerPreload',{hidden:true});await closeNotice();
  const options={expectMotion:mode==='normal'};
  await verifyMotion(mode+'-open','open',()=>click('#bookGameGrid .book-game-link'),options);
  await verifyMotion(mode+'-close','closed',()=>click('#libraryBack'),options);
  if(mode==='normal'){
    await click('#bookGameGrid .book-game-link');await page.waitForSelector('.tools.is-open',{visible:true});
    await verifyMotion('normal-back-forward','open',async()=>{
      await page.goBack();await page.waitForFunction(()=>!document.body.classList.contains('library-tools-open'));
      await page.goForward();await page.waitForFunction(()=>document.body.classList.contains('library-tools-open'));
    },{waitForInput:false});
    await page.waitForFunction(()=>document.activeElement?.id==='libraryBack');
    await verifyMotion('normal-escape','closed',()=>page.keyboard.press('Escape'));
  }
}
try{
 for(theme of ['light','dark'])for(view of [...matrix,{name:'short-landscape',width:844,height:390}])for(const mode of ['normal','site-reduced','system-reduced']){
  const context=await browser.createBrowserContext();page=await context.newPage();await page.setViewport(view);
  try{await motionAcceptance(mode);}catch(error){failures.push({theme,view:view.name,mode,url:page.url(),error:error.stack});}
  await context.close();
 }
 if(!motionOnly){
 for(theme of ['light','dark'])for(view of matrix){
  const context=await browser.createBrowserContext();page=await context.newPage();await page.setViewport(view);
  await page.evaluateOnNewDocument(theme=>{localStorage.setItem('eagler.book.theme',JSON.stringify(theme));localStorage.setItem('eagler.book.less-motion','true');localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');},theme);
  page.on('pageerror',error=>failures.push({theme,view:view.name,error:error.message}));
  try{await application();await profile();for(const route of ['about.html','faq.html','ui-review.html']){await go('/'+route);await shot(route.replace('.html',''));}await directory();await room();await replay();await input();await windows();}
  catch(error){failures.push({theme,view:view.name,url:page.url(),error:error.stack});}
  await context.close();
 }
 // Short-height layout gets its own independent pass.
 view={name:'short-landscape',width:844,height:390};
 for(theme of ['light','dark']){
  page=await browser.newPage();await page.setViewport(view);await fixture('replay.html?game=th09mp');await button('打开录像');await click('[data-replay-file="replay/th9_01.rpy"]');await shot('replay-rounds');await input();await page.close();
 }
 }
}finally{
 await writeFile(resolve(output,'frames.json'),JSON.stringify(frames,null,2));await writeFile(resolve(output,'motion.json'),JSON.stringify(motionTraces,null,2));await writeFile(resolve(output,'failures.json'),JSON.stringify(failures,null,2));await browser.close();
}
assert.deepEqual(failures,[],'UI audit failed. See frames.json and failures.json in '+output);
console.log(`PASS | ${frames.length} screenshots | ${motionTraces.length} motion traces | light/dark | landscape/portrait/short landscape`);
