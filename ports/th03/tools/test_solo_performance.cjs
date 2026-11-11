// Real NP21 pages: opt-in recorder, native hooks, report download and cleanup.
const assert=require('node:assert/strict');
const fs=require('node:fs');
const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
(async()=>{
  const browser=await chromium.launch({headless:true,executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe'});
  try{for(const [game,port] of [['03',9874],['04',9886]].filter(([game])=>(process.env.GAMES||'03,04').split(',').includes(game))){
    const context=await browser.newContext({viewport:{width:844,height:390},hasTouch:true,isMobile:true});
    const page=await context.newPage(),errors=[],requests=[];
    page.on('pageerror',e=>{errors.push(e.message);console.error(e.message);});page.on('request',r=>requests.push(r.url()));
    await page.route('**/solo.js',async route=>{
      const response=await route.fetch();let body=await response.text();
      const needle=game==='03'?'await emulator.addDiskImage(`3-${lang}.hdi`,disk);':"emulator.addDiskImage('th04-solo.hdi',data);";
      assert(body.includes(needle));body=body.replace(needle,`window.perfTest={emulator,player,playing:()=>${game==='03'?'assist.bridge.markers().length>0':'readSoloState()?.mode===1'}};${needle}`);
      await route.fulfill({response,body});
    });
    await page.goto(`http://127.0.0.1:${port}/solo.html`);
    // Recording before launch does not install a loop or pretend to have data.
    await page.evaluate(()=>document.querySelector('[data-perf-record]').click());
    assert.match(await page.locator('[data-perf-result]').textContent(),/请先启动/);
    await page.locator('#start').click();
    await page.waitForFunction(()=>window.perfTest?.emulator.state==='running',null,{timeout:60000});
    assert(await page.evaluate(()=>!perfTest.emulator.module.observeLocalFrame));
    await page.evaluate(()=>perfTest.player.enter({native:false}));
    await page.waitForTimeout(23000);
    for(let i=0;i<45;i++){
      if(await page.evaluate(()=>perfTest.playing()))break;
      await page.locator('#canvas').focus();await page.keyboard.press('KeyZ',{delay:120});await page.waitForTimeout(1000);
    }
    assert(await page.evaluate(()=>perfTest.playing()));
    await page.locator('.touch-help-open').click();
    await page.locator('[data-perf-record]').click();
    assert(await page.evaluate(()=>typeof perfTest.emulator.module.observeLocalFrame==='function'));
    await page.locator('[data-close]').click();
    assert(!(await page.locator('[data-perf-result]').isVisible()),'report must not cover gameplay');
    await page.waitForTimeout(15800);
    assert(await page.evaluate(()=>!perfTest.emulator.module.observeLocalFrame),'recorder unhooks when complete');
    await page.locator('.touch-help-open').click();
    const downloaded=page.waitForEvent('download');await page.locator('[data-perf-download]').click();
    const artifact=await downloaded,report=JSON.parse(fs.readFileSync(await artifact.path(),'utf8'));
    assert.equal(report.game.continuousBattle,true);assert(report.game.ticksHz>0);
    assert.equal(report.completion,'complete');assert(report.durationMs>=14500);
    assert(report.nativeCallbacks.count>0&&report.nativeCallbacks.meanMs>0);
    assert(report.animationFrames.count>0);assert(report.environment.userAgent);
    assert.equal(report.emulator.clockMultiplier,game==='03'?32:16);
    assert(!JSON.stringify(report).includes('127.0.0.1'),'no visited URLs in report');
    fs.mkdirSync('reports/solo-performance-check',{recursive:true});
    fs.writeFileSync(`reports/solo-performance-check/th${game}.json`,JSON.stringify(report,null,2)+'\n');
    await page.screenshot({path:`reports/solo-performance-check/th${game}.png`});
    // Interrupting via visibility must also release the hook and mark partial data.
    await page.locator('[data-perf-record]').click();
    await page.evaluate(()=>{
      Object.defineProperty(document,'hidden',{configurable:true,value:true});
      document.dispatchEvent(new Event('visibilitychange'));
    });
    assert(await page.evaluate(()=>!perfTest.emulator.module.observeLocalFrame));
    assert.match(await page.locator('[data-perf-result]').textContent(),/记录已中止/);
    assert(!requests.some(url=>/\/api\/|np21-lockstep|np21-rollback/.test(url)));
    if(game==='03')assert(requests.some(url=>url.endsWith('/np21-solo.js')));
    assert.deepEqual(errors,[]);console.log(`PASS TH${game}: actual solo battle, opt-in 15s recording/download, no overlay, idle/visibility cleanup`);
    await context.close();
  }}finally{await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
