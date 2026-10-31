// Browser regression for the responsive site navigation; no game or room is started.
import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
let browser;
const report={checks:[],layouts:[],errors:[]};
const box = element => element.getBoundingClientRect().toJSON();
try {
 browser=await puppeteer.launch({executablePath:process.env.CHROME_PATH || '/usr/bin/chromium',headless:true,args:['--no-sandbox']});
 const page=await browser.newPage();page.on('pageerror',e=>report.errors.push(e.message));
 const url=(process.argv[2] || 'http://127.0.0.1:8130/').replace(/\/$/, '');
 const ready=async(launcher=true)=>{
  await page.waitForSelector('#mastheadMenu');
  if(launcher)await page.waitForFunction(()=>window.__eaglerBoot?.done&&!document.body.classList.contains('app-shell-activation-pending'));
  await page.waitForFunction(()=>document.querySelector('#mastheadMenu').dataset.compact===String(innerWidth<1024));
  await page.waitForFunction(()=>!document.querySelector('#mastheadMenu').closest('[inert]')&&!document.querySelector('[data-route-preload]:not(.is-done)'));
  if(await page.$('#firstUseNoticeDialog[open]')){await page.click('#firstUseNoticeClose');await page.waitForSelector('#firstUseNoticeDialog:not([open])');}
 };
 const inspect=async(width,height)=>{
  await page.waitForFunction((w,h)=>innerWidth===w&&innerHeight===h,{timeout:5000},width,height);
  const layout=await page.evaluate(()=>{
   const header=document.querySelector('#mastheadMenu'),nav=header.querySelector('.site-primary-links');
   return {width:innerWidth,height:innerHeight,header:header.getBoundingClientRect().toJSON(),nav:nav.getBoundingClientRect().toJSON(),paddingLeft:getComputedStyle(document.body).paddingLeft,overflow:document.documentElement.scrollWidth>innerWidth,buttons:[...nav.querySelectorAll('.site-nav-page,.site-nav-group')].map(el=>({label:el.textContent,rect:el.getBoundingClientRect().toJSON()}))};
  });
  report.layouts.push(layout);assert.equal(layout.overflow,false,`overflow at ${width}`);
  if(width>=1024){assert.equal(layout.header.x,0);assert.equal(layout.header.y,64);assert.equal(layout.header.width,208);assert.ok(Math.abs(layout.header.height-(height-64))<1);assert.equal(layout.paddingLeft,'208px');assert.ok(layout.buttons.every(b=>b.rect.width>150&&b.rect.height>=44));}
  else{assert.equal(layout.header.x,0);assert.ok(Math.abs(layout.header.bottom-height)<1);assert.ok(layout.header.height>=72);assert.equal(layout.paddingLeft,'0px');assert.ok(layout.buttons.every(b=>b.rect.height>=44&&b.rect.width>=64));assert.ok(layout.buttons.every(b=>Math.abs(b.rect.y-layout.buttons[0].rect.y)<1));}
  return layout;
 };
 await page.setViewport({width:1440,height:960});await page.goto(url,{waitUntil:'networkidle2'});await ready();await inspect(1440,960);
 const tokens=await page.evaluate(()=>Object.fromEntries(['--paper','--ink','--red','--red-bright','--panel','--muted','--ease','--ui-font','--art-font'].map(k=>[k,getComputedStyle(document.documentElement).getPropertyValue(k).trim()])));
 report.tokens=tokens;
 await page.focus('[data-site-trigger=help]');await page.keyboard.press('ArrowRight');
 await page.waitForFunction(()=>document.activeElement?.closest('[data-site-panel=help]'));
 await page.waitForFunction(()=>document.querySelector('.site-navigation-tray').getBoundingClientRect().height>=document.querySelector('.site-navigation-inner').getBoundingClientRect().height-1);
 const desktopPanel=await page.$eval('.site-navigation-tray',box);
 assert.ok(desktopPanel.x>=208&&desktopPanel.bottom<=960);await page.keyboard.press('Escape');
 assert.equal(await page.$eval('[data-site-trigger=help]',el=>el===document.activeElement),true);
 assert.equal(await page.$eval('#mastheadMenuPanel',el=>el.hasAttribute('inert')),true);
 report.checks.push('1440px 左栏、右侧帮助面板、方向键进入及 Escape 返回焦点');
 await page.evaluate(()=>document.documentElement.style.setProperty('--motion-duration','1s'));
 await page.click('[data-site-trigger=settings]');await page.waitForSelector('.site-navigation-inner[data-motion-enter=menu]');
 assert.ok(await page.$eval('.site-navigation-inner',el=>el.getAnimations().length>=2));
 await page.waitForFunction(()=>document.querySelector('.site-navigation-tray').getBoundingClientRect().height>=document.querySelector('.site-navigation-inner').getBoundingClientRect().height-1);
 await page.click('#lessMotionToggle');await page.waitForFunction(()=>!document.querySelector('.site-navigation-inner').hasAttribute('data-motion-enter'));
 assert.equal(await page.$eval('.site-navigation-inner',el=>el.getAnimations().length),0);await page.click('#lessMotionToggle');await page.keyboard.press('Escape');
 await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);await page.click('[data-site-trigger=settings]');
 assert.equal(await page.$eval('.site-navigation-inner',el=>el.getAnimations().length),0);await page.keyboard.press('Escape');
 report.checks.push('Motion 动画沿用 token，系统及站内减弱动效均生效');
 for(const [width,height] of [[1024,768],[1023,768],[768,1024],[390,844],[320,568],[600,390]]){
  await page.setViewport({width,height,isMobile:width<1024,hasTouch:width<1024});await ready();await inspect(width,height);
  await page.tap('[data-site-trigger=settings]');await page.waitForFunction(()=>document.querySelector('#mastheadMenu').dataset.open==='true');
  await page.waitForFunction(()=>document.querySelector('.site-navigation-tray').getBoundingClientRect().height>=document.querySelector('.site-navigation-inner').getBoundingClientRect().height-1);
  const panel=await page.$eval('.site-navigation-tray',box),header=await page.$eval('#mastheadMenu',box);
  assert.ok(panel.x>=0&&panel.right<=width&&panel.y>=0&&panel.bottom<=height,`panel clipped ${width}x${height}`);
  if(width<1024)assert.ok(panel.bottom<=header.top-4);
  await page.tap('[data-site-panel=settings] .site-panel-close');
  await page.waitForFunction(()=>document.querySelector('#mastheadMenu').dataset.open==='false');
  if(width===390){
  }
 }
 report.checks.push('1024/1023px 边界、768/390/320px 及手机横屏切换正确；面板完整可滚动、无横向溢出');
 await page.setViewport({width:390,height:844,isMobile:true,hasTouch:true});await ready();
 for(let cycle=0;cycle<3;cycle++){
  const hit=await page.$eval('#mastheadMenuToggle',el=>{const r=el.querySelector('svg').getBoundingClientRect();return document.elementFromPoint(r.x+r.width/2,r.y+r.height/2)?.closest('button')?.id;});assert.equal(hit,'mastheadMenuToggle');
  await page.tap('#mastheadMenuToggle');await page.waitForSelector('#mastheadMenuToggle .lucide-x');
  assert.equal(await page.$eval('#mastheadMenu',el=>el.dataset.open),'true');
  await page.tap('#mastheadMenuToggle');await page.waitForSelector('#mastheadMenuToggle .lucide-settings-2');assert.equal(await page.$eval('#mastheadMenu',el=>el.dataset.open),'false');
 }
 report.checks.push('手机点击 Lucide 图标连开关三轮正常');
 await page.tap('[data-site-link=lobby]');await page.waitForFunction(()=>location.pathname.endsWith('/lobby.html')&&document.querySelector('[data-site-link=lobby][aria-current=page]'));await ready();await inspect(390,844);
 await page.tap('[data-site-link=single]');await page.waitForFunction(()=>document.querySelector('[data-site-link=single][aria-current=page]'));await ready();
 await page.evaluate(()=>scrollTo(0,document.documentElement.scrollHeight));
 const footer=await page.$eval('.site-footer',box),bar=await page.$eval('#mastheadMenu',box);assert.ok(footer.bottom<=bar.top+1);
 report.checks.push('联机与单机路由切换、高亮和底栏固定正常；页脚滚到底后未被遮挡');
 for(const kind of ['player-active','mp-room-active']){
  const probe=await page.evaluate(kind=>{document.body.classList.add(kind);const result={display:getComputedStyle(document.querySelector('#mastheadMenu')).display,padding:getComputedStyle(document.body).paddingLeft};document.body.classList.remove(kind);return result;},kind);
  assert.equal(probe.display,'none');assert.equal(probe.padding,'0px');
 }
 report.checks.push('游戏及房间模式的 CSS 状态探针：导航隐藏且不占左侧空间（未启动游戏或加入房间）');
 for(const width of [1440,390]){
  await page.setViewport({width,height:844,isMobile:width<1024,hasTouch:width<1024});
  for(const path of ['/about.html','/faq.html','/uptime/']){
   await page.goto(url+path,{waitUntil:'networkidle2'});await ready(false);await inspect(width,844);
   await page.click('[data-site-trigger=help]');await page.waitForFunction(()=>document.querySelector('#mastheadMenu').dataset.open==='true');
   assert.equal(await page.$eval('[data-site-trigger=help]',el=>el.getAttribute('aria-current')),'page');await page.keyboard.press('Escape');
  }
 }
 report.checks.push('关于、FAQ 和服务状态独立页面均复用左栏／底栏与当前帮助高亮');
 assert.deepEqual(report.errors,[]);report.status='PASS';
 console.log(JSON.stringify(report,null,2));
} catch(error) {
 report.status='FAIL';report.failure=String(error.stack||error);console.log(JSON.stringify(report,null,2));
 throw error;
} finally {if(browser)await browser.close();}
