// Compare the same deterministic DOS game frames with/without TH04's cache.
const assert=require('node:assert/strict');
const {existsSync,writeFileSync}=require('node:fs');
const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
(async()=>{
  const browser=await chromium.launch({headless:true,executablePath:[chromium.executablePath(),
    'C:/Program Files/Google/Chrome/Application/chrome.exe','C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe'].find(existsSync)});
  const results=[];
  try{for(const cached of [false,true]){
    const context=await browser.newContext(),page=await context.newPage();
    if(!cached)await page.route('**/np21-lockstep.js',async route=>{
      const response=await route.fetch(),source=await response.text();
      const needle='var wasmTableMirror=[];var getWasmTableEntry=funcPtr=>wasmTableMirror[funcPtr]||(wasmTableMirror[funcPtr]=wasmTable.get(funcPtr));';
      assert(source.includes(needle));await route.fulfill({response,body:source.replace(needle,'var getWasmTableEntry=funcPtr=>wasmTable.get(funcPtr);')});
    });
    await page.goto('http://127.0.0.1:9869/local.html');
    await page.evaluate(async()=>{
      const {NP21}=await import('./vendor/np2/np2-netplay.js'),{installNativePause,nativeAssets}=await import('./disk.js');
      const {NativePause}=await import('./native-pause.js'),{NativeMusic}=await import('./native-music.js');
      const meta=(await(await fetch('disks/manifest.json')).json()).games['3-jp'];
      const response=await fetch(meta.url);
      const disk=new Uint8Array(await new Response(response.body.pipeThrough(new DecompressionStream('gzip'))).arrayBuffer());
      await installNativePause(disk,'jp');disk[meta.keyboardConfig.offset]=1;disk[meta.keyboardConfig.offset+1]=0;
      const batch=new TextEncoder().encode('@ECHO OFF\r\nPATH A:\\DOS;A:\\\r\nA:\r\nCD \\YUMEZIKU\r\nCALL GAME.BAT\r\n');
      disk.fill(0,meta.autoexecOffset,meta.autoexecOffset+meta.autoexecCapacity);disk.set(batch,meta.autoexecOffset);
      new DataView(disk.buffer).setUint32(meta.autoexecSizeOffset,batch.length,true);
      const canvas=document.createElement('canvas');canvas.width=640;canvas.height=400;document.body.append(canvas);
      const emu=await NP21.create({canvas,lockstep:true,lockstepEpoch:946684800000,clk_base:2457600,clk_mult:16,
        DIPswtch:[0x3e,0xf3,0x7b],ExMemory:7,Latencys:40,SampleHz:44100,SNDboard:4,no_mouse:true,use_menu:false,fontfile:'font.bmp'});
      emu.addDiskImage('game.hdi',disk);emu.setHdd(0,'game.hdi');emu.run();
      const patch=(await nativeAssets()).meta,pause=new NativePause(()=>emu,patch),music=new NativeMusic(patch.startup.music);
      let frame=0,selectFrames=0;
      const key=(code,keyCode,down)=>emu.module.netKey(down?'keydown':'keyup',{code,key:code,keyCode,which:keyCode,
        timeStamp:frame*1000/60,location:code.startsWith('Numpad')?3:0,preventDefault(){}});
      const advance=()=>{
        emu.module.netBeginFrame(frame,false);
        if(pause.selecting){
          selectFrames++;
          if(selectFrames===1){key('KeyB',66,true);key('Numpad2',98,true);}
          if(selectFrames===20){key('KeyB',66,false);key('Numpad2',98,false);}
          if(selectFrames===50)key('KeyZ',90,true);
          if(selectFrames===90)key('KeyZ',90,false);
          if(selectFrames===120)key('ArrowLeft',37,true);
          if(selectFrames===160)key('ArrowLeft',37,false);
        }
        emu.step();frame++;const state=pause.update(frame*1000/60);music.read(emu.module.HEAPU8);
        emu.module.netEndFrame();emu.module.netConfirm(frame);return state;
      };
      window.dispatchBench={emu,pause,music,advance,get frame(){return frame;}};
    });
    let ready=false;
    for(let n=0;n<50;n++){
      ready=await page.evaluate(()=>{let s;for(let i=0;i<60;i++)s=dispatchBench.advance();return s?.phase===1;});
      if(ready)break;
    }
    assert(ready,'DOS battle must boot');
    const cdp=await context.newCDPSession(page);await cdp.send('Emulation.setCPUThrottlingRate',{rate:2});
    const samples=[];
    for(let batch=0;batch<6;batch++)samples.push(...await page.evaluate(()=>{
      const costs=[];for(let i=0;i<30;i++){const start=performance.now();dispatchBench.advance();costs.push(performance.now()-start);}return costs;
    }));
    const state=await page.evaluate(()=>{
      const b=dispatchBench,h=b.emu.module.HEAPU8,v=new DataView(h.buffer),at=b.music.at;
      const base=at-b.music.patch.mailbox_com_offset-v.getUint16(at+16,true)*16;
      let hash=2166136261;for(let i=base;i<base+0xa0000;i++)hash=Math.imul(hash^h[i],16777619);
      return {frame:b.frame,hash:hash>>>0,ticks:v.getUint32(b.pause.live+16,true)};
    });
    samples.sort((a,b)=>a-b);results.push({cached,cpuThrottle:2,meanMs:samples.reduce((a,b)=>a+b,0)/samples.length,
      p95Ms:samples[Math.floor(samples.length*.95)],state});
    console.log(results.at(-1));await context.close();
  }
  assert.deepEqual(results[0].state,results[1].state,'Dispatch cache must preserve deterministic game state');
  writeFileSync('reports/dispatch-comparison.json',JSON.stringify(results,null,2));
  console.log('PASS: cached/uncached dispatch produce identical DOS RAM and game ticks');
  }finally{await browser.close();}
})().catch(error=>{console.error(error);process.exitCode=1;});
