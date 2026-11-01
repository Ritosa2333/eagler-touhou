import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
const browser = await puppeteer.launch({executablePath:process.env.CHROME_PATH || '/usr/bin/chromium',headless:true,args:['--no-sandbox']});
const report={checks:[],errors:[],consoleErrors:[],nativeTransitionSkips:[]};
const base=(process.argv[2] || 'http://127.0.0.1:8130/').replace(/\/$/,'');
try {
 const page=await browser.newPage();
 page.on('pageerror',error=>{
  const detail={message:error.message,stack:error.stack,url:page.url()};
  // Chrome's outgoing native transition is skipped after the document hides.
  // Keep that browser-only cancellation visible, without masking any application stack.
  if(error.message==='AbortError: Transition was skipped' && error.stack==='DOMException: AbortError: Transition was skipped') report.nativeTransitionSkips.push(detail);
  else report.errors.push(detail);
 });
 page.on('console',entry=>{if(entry.type()==='error'&&!entry.text().startsWith('Failed to load resource:'))report.consoleErrors.push(entry.text());});
 await page.evaluateOnNewDocument(()=>{
  // The outgoing native ViewTransition.ready rejects when the document becomes hidden.
  // https://developer.chrome.com/docs/web-platform/view-transitions/cross-document
  for (const name of ['pageswap','pagereveal']) window.addEventListener(name, event => {
    event.viewTransition?.ready.catch(error => {
      if (error.name !== 'AbortError') throw error;
    });
  });
  localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');
  localStorage.setItem('eagler-touhou-less-motion-v1','1');
 });
 const ready=async(launcher=false)=>{
  await page.waitForSelector('#mastheadMenu');
  await page.waitForFunction(()=>!document.querySelector('#mastheadMenu').closest('[inert]'));
  await page.waitForFunction(()=>document.getAnimations().filter(animation=>animation.animationName?.includes('view-transition')).every(animation=>animation.playState!=='running'));
  if(launcher)await page.waitForFunction(()=>(!window.__eaglerBoot||window.__eaglerBoot.done)&&!document.body.classList.contains('app-shell-activation-pending'));
 };
 const choose=async locale=>{
  await page.click('.site-topbar .mizuki-select-trigger');
  await page.waitForSelector(`.mizuki-select-menu:not([hidden]) [data-value="${locale}"]`);
  await page.click(`.mizuki-select-menu:not([hidden]) [data-value="${locale}"]`);
  await page.waitForFunction(locale=>document.documentElement.lang===locale,{},locale);
 };
 const help=async()=>{
  await page.click('[data-site-trigger=help]');
  await page.waitForSelector('#mastheadMenuPanel:not([hidden])');
  await page.waitForFunction(()=>document.querySelector('#mastheadMenuPanel').getAnimations().every(a=>a.playState!=='running'));
 };
 await page.setViewport({width:1440,height:960});
 await page.goto(base+'/ja.html',{waitUntil:'networkidle2'});await ready(true);
 await page.evaluate(()=>localStorage.removeItem('eagler-touhou-ui-locale-v1'));
 await help();
 assert.equal(await page.$eval('a[href*="faq.html"]',node=>new URL(node.href).searchParams.get('lang')),'ja');
 await page.click('a[href*="faq.html"]');await ready();
 await page.waitForFunction(()=>document.querySelector('.faq-content h1')?.textContent==='よくある質問');
 assert.ok((await page.title()).includes('よくある質問'));
 assert.ok((await page.$eval('.faq-content',node=>node.textContent)).includes('セーブデータ'));
 assert.ok(await page.$eval('.faq-content a[href*="github.com"]',node=>node.rel.includes('noopener')));
 report.checks.push('首次访问日语入口进入 FAQ，正文、标题、文档语言及外链安全属性一致');
 for(const [locale,heading] of [['en','Frequently asked questions'],['zh-CN','常见问题'],['ja','よくある質問']]){
  await choose(locale);
  await page.waitForFunction(heading=>document.querySelector('.faq-content h1')?.textContent===heading,{},heading);
  await page.reload({waitUntil:'networkidle2'});await ready();
  assert.equal(await page.$eval('.faq-content h1',node=>node.textContent),heading);
  assert.equal(await page.$eval('.about-back a',node=>new URL(node.href).pathname),locale==='en'?'/en.html':locale==='ja'?'/ja.html':'/');
 }
 report.checks.push('FAQ 可通过真实选择器往返中英日三语，刷新后正文和返回入口保持一致');
 for(const [locale,word] of [['ja','オフライン'],['en','Offline access'],['zh-CN','离线访问']]){
  await choose(locale);await help();await page.click('#firstUseNoticeOpen');
  await page.waitForSelector('#firstUseNoticeDialog[open]');
  await page.waitForFunction(()=>document.querySelector('#firstUseNoticeDialog').getAnimations({subtree:true}).every(animation=>animation.playState!=='running'));
  await page.waitForFunction(word=>document.querySelector('#firstUseNoticeText')?.textContent.includes(word),{},word);
  await page.click('#firstUseNoticeClose');await page.waitForSelector('#firstUseNoticeDialog:not([open])');
 }
 report.checks.push('首次使用须知正文随中英日切换，复用已加载内容时不会残留上一次语言');
 await page.goto(base+'/about.html?lang=ja',{waitUntil:'networkidle2'});await ready();
 await page.waitForFunction(()=>document.querySelector('[data-localized-content]')?.textContent.includes('開発への参加'));
 assert.ok(await page.$('a[href^="mailto:"]'));
 await choose('en');await page.waitForFunction(()=>document.querySelector('[data-localized-content]')?.textContent.includes('Contribute'));
 assert.equal(await page.$eval('.about-back a',node=>new URL(node.href).pathname),'/en.html');
 report.checks.push('关于页正文、浏览器标题、返回入口和联系链接与当前语言同步');
 await page.goto(base+'/lobby.html?lang=ja',{waitUntil:'networkidle2'});await ready(true);
 await page.waitForSelector('#lobbyGuideOpen');await page.click('#lobbyGuideOpen');
 await page.waitForSelector('#mpGuideDialog[open] [data-mp-rule-guide]');
 assert.ok((await page.$eval('#mpGuideContent',node=>node.textContent)).includes('共通ルール'));
 await page.click('#mpGuideContent [role=tab][data-game=th15]');
 assert.equal(await page.$eval('#mpGuideContent [role=tab][aria-selected=true]',node=>node.dataset.game),'th15');
 await page.click('#mpGuideContent .multiplayer-rule-panel[data-game=th15] [data-scope=specific] summary');
 assert.ok((await page.$eval('#mpGuideContent .multiplayer-rule-panel[data-game=th15]',node=>node.textContent)).includes('1,250,000'));
 await page.click('#mpGuideClose');await page.waitForSelector('#mpGuideDialog:not([open])');
 report.checks.push('日语联机玩法的作品标签、专属规则与数值正常，未创建或加入房间');
 await page.setViewport({width:390,height:844,isMobile:true,hasTouch:true});
 await page.goto(base+'/faq.html?lang=ja',{waitUntil:'networkidle2'});await ready();
 await page.waitForFunction(()=>document.querySelector('.faq-content h1')?.textContent==='よくある質問');
 assert.ok(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
 if(process.env.SCREENSHOT_DIR)await page.screenshot({path:`${process.env.SCREENSHOT_DIR}/faq-ja-mobile.png`});
 await choose('en');await page.waitForFunction(()=>document.querySelector('.faq-content h1')?.textContent==='Frequently asked questions');
 assert.ok(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
 assert.equal(await page.$eval('#mastheadMenu',node=>getComputedStyle(node).position),'fixed');
 await page.setJavaScriptEnabled(false);
 await page.goto(base+'/compatibility.ja.html',{waitUntil:'networkidle2'});
 assert.ok((await page.title()).includes('互換性'));
 assert.ok((await page.$eval('body',node=>node.textContent)).includes('グラフィックアクセラレーション'));
 await page.goto(base+'/compatibility.en.html',{waitUntil:'networkidle2'});
 assert.ok((await page.title()).includes('Compatibility'));
 report.checks.push('兼容性帮助提供静态英日版本，禁用 JavaScript 后仍可阅读并切换语言');
 report.checks.push('手机帮助页保留顶栏和底栏，三语正文可读且没有横向溢出');
 assert.deepEqual(report.errors,[]);assert.deepEqual(report.consoleErrors,[]);
 report.status='PASS';console.log(JSON.stringify(report,null,2));
}catch(error){report.status='FAIL';report.failure=String(error.stack||error);console.log(JSON.stringify(report,null,2));throw error;}
finally{await browser.close();}
