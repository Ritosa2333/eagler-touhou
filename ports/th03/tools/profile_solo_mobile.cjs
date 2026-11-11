// Desktop control experiment. CPU throttling is a stress test, not a phone model.
const assert=require('node:assert/strict');
const fs=require('node:fs');
const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
const game=process.env.GAME||'03',cached=process.env.CACHE!=='0',mult=Number(process.env.CLOCK||16);
const url=process.env.URL||`http://127.0.0.1:${game==='03'?9874:9886}`;
const old='var getWasmTableEntry=funcPtr=>wasmTable.get(funcPtr);';
const optimized='var wasmTableMirror=[];var getWasmTableEntry=funcPtr=>wasmTableMirror[funcPtr]||(wasmTableMirror[funcPtr]=wasmTable.get(funcPtr));';
(async()=>{
  const browser=await chromium.launch({headless:true,executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe'});
  try{
    const page=await browser.newPage({viewport:{width:844,height:390},hasTouch:true,isMobile:true});
    const errors=[];page.on('pageerror',e=>{errors.push(e.message);console.error('PAGE',e.message);});
    await page.route('**/solo.js',async route=>{
      const response=await route.fetch();let body=await response.text();
      const needle=game==='03'?'emulator.addDiskImage(`3-${lang}.hdi`,disk);':"emulator.addDiskImage('th04-solo.hdi',data);";
      assert(body.includes(needle));body=body.replace(needle,`window.mobileBench={emulator,player,readState:()=>(${game==='03'?"{...assist.bridge.update(),ticks:assist.bridge.live<0?null:new DataView(emulator.module.HEAPU8.buffer).getUint32(assist.bridge.live+assist.bridge.meta.fields.ticks,true)}":'readSoloState()'})};${needle}`);
      if(game==='04')body=body.replace('clk_mult:16,',`clk_mult:${mult},`);
      await route.fulfill({response,body});
    });
    await page.route(/\/np21(?:-60|-solo)?\.js$/,async route=>{
      const response=await route.fetch();let body=await response.text();
      assert(body.includes(old)||body.includes(optimized));
      body=body.replace(cached?old:optimized,cached?optimized:old);
      const anchor='var iterFunc=getWasmTableEntry(func);';assert(body.includes(anchor));
      body=body.replace(anchor,'var rawIter=getWasmTableEntry(func);var iterFunc=()=>{const at=performance.now();rawIter();(globalThis.mobileSamples??=[]).push(performance.now()-at);};');
      await route.fulfill({response,body});
    });
    await page.goto(url+'/solo.html');
    if(game==='03')await page.locator('#clock').selectOption(String(mult));
    await page.locator('#language').selectOption('jp');await page.locator('#start').click();
    await page.waitForFunction(()=>window.mobileBench?.emulator.state==='running',null,{timeout:60000});
    console.log('booted',game,{cached,mult});
    await page.evaluate(()=>mobileBench.player.enter({native:false}));
    await page.waitForTimeout(18000);
    for(let i=0;i<45;i++){
      if(await page.evaluate(()=>{const s=mobileBench.readState();return s?.phase===1||s?.mode===1;}))break;
      await page.locator('#canvas').focus();await page.keyboard.press('KeyZ',{delay:120});await page.waitForTimeout(1000);
    }
    assert(await page.evaluate(()=>{const s=mobileBench.readState();return s?.phase===1||s?.mode===1;}),'must reach gameplay');
    console.log('battle',game);
    const cdp=await page.context().newCDPSession(page),result={game,cached,mult,note:'Desktop Edge; throttling is not a real phone benchmark',samples:[]};
    await page.keyboard.down('KeyZ');
    for(const rate of (process.env.RATES||'1,2').split(',').map(Number)){
      await cdp.send('Emulation.setCPUThrottlingRate',{rate});await page.waitForTimeout(1500);
      const start=await page.evaluate(()=>{window.mobileSamples=[];return {at:performance.now(),state:mobileBench.readState()};});
      await page.waitForTimeout(5000);
      const end=await page.evaluate(()=>({at:performance.now(),state:mobileBench.readState(),costs:window.mobileSamples}));
      const times=end.costs.sort((a,b)=>a-b),total=times.reduce((a,b)=>a+b,0),elapsed=end.at-start.at;
      const sample={rate,elapsedMs:elapsed,callbacks:times.length,callbacksHz:times.length*1000/elapsed,meanMs:total/times.length,p95Ms:times[Math.floor(times.length*.95)],maxMs:times.at(-1),nativeBusyPercent:total/elapsed*100,gameTicksHz:start.state?.ticks!=null&&end.state?.ticks!=null?(end.state.ticks-start.state.ticks)*1000/elapsed:null,start:start.state,end:end.state};
      result.samples.push(sample);console.log(JSON.stringify(sample));
    }
    result.errors=errors;assert.deepEqual(errors,[]);
    fs.writeFileSync(process.argv[2]||`reports/solo-mobile-${game}-${cached?'cached':'uncached'}-${mult}.json`,JSON.stringify(result,null,2)+'\n');
  }finally{await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
