const assert=require('node:assert/strict');
const {existsSync,readFileSync,writeFileSync}=require('node:fs');
const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
(async()=>{
  const browser=await chromium.launch({headless:true,executablePath:[chromium.executablePath(),
    'C:/Program Files/Google/Chrome/Application/chrome.exe','C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe'].find(existsSync)});
  try {
    const page=await browser.newPage(),errors=[];
    page.on('pageerror',e=>errors.push(e.message));
    page.on('console',m=>{if(m.type()==='error')console.log(m.text());});
    if(process.env.TABLE_CACHE==='0')await page.route('**/np21-60.js',async route=>{
      const response=await route.fetch(),source=await response.text();
      const cached='var wasmTableMirror=[];var getWasmTableEntry=funcPtr=>wasmTableMirror[funcPtr]||(wasmTableMirror[funcPtr]=wasmTable.get(funcPtr));';
      assert(source.includes(cached));
      await route.fulfill({response,body:source.replace(cached,'var getWasmTableEntry=funcPtr=>wasmTable.get(funcPtr);')});
    });
    await page.route('**/app.js',route=>route.fulfill({contentType:'text/javascript',body:readFileSync('web/app.js','utf8')+`
      window.musicTest=()=>({status:localBgm?.status,active:localBgm?.active?.id,
        audioTime:localBgm?.context.currentTime,sequence:nativeMusic?.sequence,mailbox:nativeMusic?.at,
        selecting:nativePause?.selecting,phase:nativeState?.phase,error:document.getElementById('status').textContent});
      window.pauseMusicTest=()=>emulator.pause();
      window.musicPerf=()=>({time:performance.now(),steps:emulator.module.localFrameCount,
        cost:emulator.module.localStepMs,uploads:emulator.module.frameUploads});
    `}));
    await page.goto('http://127.0.0.1:9869/local.html');
    await page.locator('#start').click();
    await page.waitForFunction(()=>window.musicTest?.().active==='select-m26',null,{timeout:60000});
    console.log('Selection:',await page.evaluate(()=>musicTest()));
    await page.waitForFunction(()=>window.musicTest().selecting,null,{timeout:30000});
    assert.deepEqual(errors,[]);
    await page.bringToFront();await page.evaluate(()=>window.dispatchEvent(new Event('focus')));
    await page.locator('#canvas').focus();
    await page.keyboard.down('ArrowDown');await page.waitForTimeout(300);await page.keyboard.up('ArrowDown');
    await page.keyboard.down('KeyS');await page.waitForTimeout(300);await page.keyboard.up('KeyS');
    await page.keyboard.down('KeyZ');await page.waitForTimeout(1000);await page.keyboard.up('KeyZ');
    await page.keyboard.down('KeyJ');await page.waitForTimeout(1000);await page.keyboard.up('KeyJ');
    for(let i=0;i<12;i++){
      await page.keyboard.press('Enter',{delay:120});
      await page.waitForTimeout(1500);
      console.log('Progress:',await page.evaluate(()=>musicTest()));
      if(await page.evaluate(()=>/^0[0-8]mm-m26$/.test(musicTest().active)))break;
    }
    await page.screenshot({path:'reports/music-battle.png'});
    await page.waitForFunction(()=>/^0[0-8]mm-m26$/.test(window.musicTest?.().active),null,{timeout:15000});
    const playing=await page.evaluate(()=>musicTest());console.log('Battle:',playing);
    if(process.env.PROFILE){
      const cdp=await page.context().newCDPSession(page),rate=Number(process.env.CPU_RATE||2);
      await cdp.send('Emulation.setCPUThrottlingRate',{rate});
      await page.waitForTimeout(1000);
      const before=await page.evaluate(()=>musicPerf());await page.waitForTimeout(6000);
      const after=await page.evaluate(()=>musicPerf()),steps=after.steps-before.steps;
      const result={cache:process.env.TABLE_CACHE!=='0',cpuThrottle:rate,steps,
        stepsHz:steps*1000/(after.time-before.time),simulateMs:(after.cost-before.cost)/steps,
        uploadsHz:(after.uploads-before.uploads)*1000/(after.time-before.time)};
      await cdp.send('Profiler.enable');await cdp.send('Profiler.start');await page.waitForTimeout(1500);
      const {profile}=await cdp.send('Profiler.stop'),counts=new Map();
      for(const id of profile.samples||[])counts.set(id,(counts.get(id)||0)+1);
      result.hot=profile.nodes.map(n=>({name:n.callFrame.functionName,url:n.callFrame.url,hits:counts.get(n.id)||0})).sort((a,b)=>b.hits-a.hits).slice(0,15);
      writeFileSync(process.env.PROFILE,JSON.stringify(result,null,2));console.log('PROFILE',result);
    }
    await page.evaluate(()=>pauseMusicTest());await page.waitForTimeout(1200);
    const paused=await page.evaluate(()=>musicTest());
    assert.equal(paused.active,playing.active);assert(paused.audioTime>playing.audioTime+1);
    assert.deepEqual(errors,[]);
    console.log('PASS: real DOS selection/battle music events, Opus decoding, independent audio clock while emulator paused');
  } finally {await browser.close();}
})().catch(e=>{console.error(e);process.exit(1);});
