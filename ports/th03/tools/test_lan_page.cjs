// Lobby integration. Set TH03_REAL_RUNTIME=1 to boot DOS instead of stubbing the iframe.
const assert = require('node:assert/strict');
const {mkdirSync, existsSync} = require('node:fs');
const {resolve} = require('node:path');
const {chromium} = require(process.env.PLAYWRIGHT_PATH || 'C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');

(async () => {
  const executablePath = process.env.CHROMIUM_PATH || [chromium.executablePath(),
    'C:/Program Files/Google/Chrome/Application/chrome.exe',
    'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe'].find(existsSync);
  const browser = await chromium.launch({headless: true, executablePath});
  const output = resolve('reports/startup-check'); mkdirSync(output, {recursive: true});
  const base = process.env.TH03_URL || 'http://127.0.0.1:9869';
  const contexts = [await browser.newContext({viewport: {width: 1280, height: 900}}),
    await browser.newContext({viewport: {width: 390, height: 844}})];
  const pages = await Promise.all(contexts.map(c => c.newPage()));
  const errors = []; pages.forEach(page => page.on('pageerror', e => errors.push(e.message)));
  const real=process.env.TH03_REAL_RUNTIME==='1';
  try {
    for (const page of pages) {
      if(!real)await page.route('**/local.html?*', route => route.fulfill({contentType: 'text/html', body:
        '<!doctype html><html><body><p>Game runtime test stub</p><script>parent.postMessage({protocol:"th03-lan/1",event:"progress",message:"资源加载测试就绪",loaded:true},location.origin);</script></body></html>'}));
      await page.goto(base + '/index.html');
      assert.equal(await page.locator('.mode-card').count(),4);
      await page.screenshot({path:resolve(output,`launcher-${pages.indexOf(page)}.png`),fullPage:true});
      await page.goto(base + '/lan.html?network='+(pages.indexOf(page)?'public':'lan'));
      await page.locator('#configure-controls').click();
      const settings=page.frameLocator('#controls-dialog iframe');
      await settings.getByRole('button',{name:'1P 低速键位',exact:true}).click();
      await page.keyboard.press('KeyV');
      assert.equal(await settings.getByRole('button',{name:'1P 低速键位',exact:true}).textContent(),'V');
      await page.screenshot({path:resolve(output,`controls-${pages.indexOf(page)}.png`),fullPage:true});
      await page.getByRole('button',{name:'关闭改键设置',exact:true}).click();
      await page.locator('#configure-controls').click();
      assert.equal(await settings.getByRole('button',{name:'1P 低速键位',exact:true}).textContent(),'V');
      await settings.locator('#defaults').click();
      await page.getByRole('button',{name:'关闭改键设置',exact:true}).click();
      await page.screenshot({path:resolve(output,`outside-${pages.indexOf(page)}.png`),fullPage:true});
    }
    const [host, guest] = pages;
    await host.locator('#connection-settings summary').click();
    await host.locator('#mode').selectOption('relay'); await host.locator('#create').click();
    await host.locator('#lobby').waitFor({state: 'visible'});
    const code = (await host.locator('#room-info').textContent()).match(/\d{4}/)[0];
    await guest.locator('#room').fill(code); await guest.locator('#join').click();
    await guest.locator('#lobby').waitFor({state: 'visible'});
    assert.equal(await guest.locator('#mode').inputValue(), 'relay');
    assert.equal(await host.locator('body').getAttribute('data-view'),'room');
    assert.equal(await host.locator('#entry').isVisible(),false);
    assert.equal(await host.locator('#host-seat').getAttribute('data-local'),'true');
    assert.equal(await guest.locator('#guest-seat').getAttribute('data-local'),'true');
    assert(await guest.locator('#language').isDisabled());
    assert(await guest.locator('#host-seat-choice').isDisabled());
    await host.locator('#ready').click(); await guest.locator('#ready').click();
    await host.locator('#start').waitFor({state: 'visible'});
    await host.waitForFunction(() => !document.getElementById('start').disabled);
    await host.locator('#host-seat-choice').selectOption('1');
    for(const page of pages){
      await page.waitForFunction(()=>document.getElementById('host-seat-choice').value==='1' && document.getElementById('ready').textContent==='准备');
      assert.equal(await page.locator('.seat-row > article').first().getAttribute('id'),'guest-seat');
      assert.match(await page.locator('#host-seat strong').textContent(),/P2 · 右侧 · 房主/);
      assert.match(await page.locator('#guest-seat strong').textContent(),/P1 · 左侧 · 客机/);
    }
    assert(await host.locator('#start').isDisabled());
    assert.match(await host.locator('#seat-help').textContent(),/你是 P2 · 右侧/);
    assert.match(await guest.locator('#seat-help').textContent(),/你是 P1 · 左侧/);
    await host.locator('#host-seat-choice').selectOption('0');
    await guest.waitForFunction(()=>document.getElementById('host-seat-choice').value==='0');
    assert.equal(await host.locator('.seat-row > article').first().getAttribute('id'),'host-seat');
    await host.locator('#host-seat-choice').selectOption('1');
    assert(await guest.locator('#touch-unlimited-allowed').isDisabled());
    await host.locator('#touch-unlimited-allowed').uncheck();
    await guest.waitForFunction(()=>!document.getElementById('touch-unlimited-allowed').checked);
    await host.locator('#language').selectOption('cn');
    await host.waitForFunction(() => document.getElementById('start').disabled);
    await guest.waitForFunction(() => document.getElementById('language').value === 'cn' && document.getElementById('ready').textContent === '准备');
    await host.locator('#ready').click(); await guest.locator('#ready').click();
    await host.waitForFunction(() => !document.getElementById('start').disabled);
    for (const [i, page] of pages.entries()) {
      assert(await page.evaluate(() => document.documentElement.scrollWidth <= innerWidth));
      await page.screenshot({path: resolve(output, i ? 'lobby-mobile.png' : 'lobby-desktop.png'), fullPage: true});
    }
    await host.locator('#start').click();
    for (const [i, page] of pages.entries()) {
      await page.locator('#game iframe').waitFor();
      assert(await page.locator('#host-seat-choice').isDisabled());
      await page.waitForFunction(message => document.getElementById('host-progress').textContent.includes(message)
        && document.getElementById('guest-progress').textContent.includes(message),real?'游戏已启动':'资源加载测试就绪',{timeout:120000});
      if(real){
        const frame=page.frames().find(frame=>frame.url().includes('local.html?'));
        await frame.waitForFunction(()=>window.th03SyncState?.frame>10,null,{timeout:60000});
        assert.equal(await frame.locator('.player-label').textContent(),i?'1P':'2P');
        await frame.locator('.network-controls-open').click();
        assert(await frame.locator('#settings').isVisible());
        await frame.getByRole('button',{name:'关闭操作设置',exact:true}).click();
        await frame.locator('.touch-help-open').click();
        await frame.locator('[data-network-controls]').click();
        assert(await frame.locator('#settings').isVisible());
        assert.equal(await frame.locator('#bindings th').nth(1).textContent(),'本机键盘');
        assert(await frame.locator('#pad-0').isVisible());
        assert.equal(await frame.locator('#pad-1').isVisible(),false);
        await frame.getByRole('button',{name:'关闭操作设置',exact:true}).click();
        await frame.locator('[data-save]').click();
        assert.equal(await frame.locator('.masthead').isVisible(),false);
        assert.equal(await frame.locator('.launch-settings').isVisible(),false);
        assert.equal(await frame.locator('.footer').isVisible(),false);
        const height=(await page.locator('#game iframe').boundingBox()).height;
        if(height>=1800)console.log('EMBEDDED LAYOUT',await frame.evaluate(()=>[...document.querySelector('main').children].map(node=>({tag:node.tagName,id:node.id,class:node.className,height:node.getBoundingClientRect().height,display:getComputedStyle(node).display}))));
        assert(height<1800);
        await page.screenshot({path:resolve(output,`playing-${i}.png`),fullPage:true});
      }
      const src = new URL(await page.locator('#game iframe').getAttribute('src'), base);
      assert.equal(src.searchParams.get('role'), i ? 'guest' : 'host');
      assert.equal(src.searchParams.get('language'), 'cn');
      assert.equal(src.searchParams.get('embedded'), '1');
      assert(new URL(page.url()).pathname.endsWith('/lan.html'));
      const gameFrame=page.frames().find(frame=>frame.url().includes('local.html?'));
      await gameFrame.evaluate(()=>parent.postMessage({protocol:'th03-lan/1',event:'view',immersive:true},location.origin));
      await page.waitForFunction(()=>document.querySelector('#game iframe').classList.contains('game-immersive'));
      const fullBounds=await page.locator('#game iframe').boundingBox();
      assert.equal(fullBounds.x,0);assert.equal(fullBounds.y,0);
      assert.equal(Math.round(fullBounds.width),page.viewportSize().width);
      assert.equal(Math.round(fullBounds.height),page.viewportSize().height);
      await gameFrame.evaluate(()=>parent.postMessage({protocol:'th03-lan/1',event:'view',immersive:false},location.origin));
      await page.waitForFunction(()=>!document.querySelector('#game iframe').classList.contains('game-immersive'));
    }
    assert.deepEqual(errors, []);
    await host.locator('#leave').click();
    await guest.waitForFunction(() => document.getElementById('game').children.length === 0);
    console.log(`PASS: desktop/mobile lobby, authoritative settings, readiness reset, inline autostart/progress and leave; ${real?'real synchronized runtimes':'runtime stubbed'}`);
  } finally { await browser.close(); }
})().catch(error => { console.error(error); process.exitCode = 1; });
