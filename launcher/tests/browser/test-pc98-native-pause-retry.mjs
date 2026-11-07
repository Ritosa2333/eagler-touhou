// Real managed runtime: measure targeted practice and retry without a lobby.
// node launcher/tests/browser/test-pc98-practice-load.mjs th02 [th04]
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const repo=resolve(import.meta.dirname,'../../..');
const games=process.argv.slice(2).length?process.argv.slice(2):['th02','th04'];
const label=process.env.PC98_PRACTICE_LABEL||'current';
const output=resolve(repo,'.cache/practice-load');await mkdir(output,{recursive:true});
const results=[];
for(const game of games){
  assert(['th02','th04'].includes(game));
  const root=process.env.PC98_PRACTICE_RUNTIME?resolve(process.env.PC98_PRACTICE_RUNTIME):resolve(repo,'ports',game,'build-eagler');
  const data=await readFile(resolve(repo,'ports',game,'content',game+'.data'));
  const html=`<!doctype html><style>html,body,iframe{border:0;margin:0;width:100%;height:100%;background:#000}</style><iframe id="game" src="/runtime/${game}.html?hosted=1&managedData=1&runtimeEpoch=1"></iframe><script>
  window.events=[];
  window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
  addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data)});
  window.rpc=(command,fields={})=>new Promise((ok,no)=>{const f=document.querySelector('iframe'),request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',receive);no(Error(command+' timed out'))},120000);
    function receive(e){if(e.source!==f.contentWindow||e.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);e.data.ok?ok(e.data):no(Error(e.data.error))}
    addEventListener('message',receive);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:1,request,command,...fields},location.origin)});
  </script>`;
  const server=createServer(async(req,res)=>{try{
    const path=new URL(req.url,'http://localhost').pathname;
    if(path==='/'){res.setHeader('content-type','text/html');res.end(html);return;}
    if(path==='/data'){res.end(data);return;}
    const file=resolve(root,path.slice('/runtime/'.length));
    if(!path.startsWith('/runtime/')||!file.startsWith(root+sep))throw Error('route');
    res.setHeader('content-type',({'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.json':'application/json','.wasm':'application/wasm','.css':'text/css'})[extname(file)]||'application/octet-stream');
    let contents=await readFile(file);
    if(path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from('\nlet previousAttempt;export function testMarkAttempt(){previousAttempt=emulator;} export function testAttemptSame(){return !!emulator&&emulator===previousAttempt;}\n')]);
    if(process.env.PC98_PRACTICE_PERF==='1'&&path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from('\nexport function testDisableStandby(){retryBlocked=true;discardRetry();}\n'+(game==='th04'?"export function testDense(){return emulator.call('practiceTestDense');} export function testPerfProbe(){return emulator.call('practicePerfProbe');}\n":"export function testDense(){globalThis.testDenseMode=true;} export function testPerfProbe(){const d=emulator.practice.debug.d,h=emulator.module.HEAPU8;let bullets=0;for(let i=0;i<150;i++)if(h[d+0x2c18+i*20]===1)bullets++;return {bullets,slowdown:h[d+0x2bfd]};}\n"))]);
    if(process.env.PC98_PRACTICE_PERF==='1'&&game==='th04'&&path==='/runtime/solo-worker.js'){
      contents=Buffer.from(contents.toString().replace("if(method==='soloFault')", "if(method==='practicePerfProbe'){const d=emulator.practice.debug.d,h=emulator.module.HEAPU8;let bullets=0;for(let i=0;i<440;i++)if(h[d+0x5a22+i*26]===1)bullets++;return {bullets,turbo:h[d+0x53a0]};}if(method==='practiceTestDense'){globalThis.testDenseMode=true;return;}if(method==='soloFault')"));
    }
    if(process.env.PC98_PRACTICE_PERF==='1'&&game==='th04'&&path==='/runtime/dosbox-adapter.js')contents=Buffer.from(contents.toString().replace('this.module.practiceSignal=(port,packed)=>this.practice.signal(port,packed);',`this.module.practiceSignal=(port,packed)=>{this.practice.signal(port,packed);if(port!==0x7fe1||!globalThis.testDenseMode)return;const d=this.practice.debug.d,h=this.module.HEAPU8,v=new DataView(h.buffer);h.fill(0,d+0x5a22,d+0x5a22+440*26);for(let i=0;i<300;i++){const at=d+0x5a22+i*26,x=(12+(i%20)*18)*16,y=(24+Math.floor(i/20)*20)*16;h[at]=1;h[at+1]=32;for(const [o,n] of [[2,x],[4,y],[6,x],[8,y]])v.setInt16(at+o,n,true);h[at+19]=2;v.setUint16(at+24,52+i%11,true);}h[d+0x4662]=255;};`));
    if(process.env.PC98_PRACTICE_PERF==='1'&&game==='th02'&&path==='/runtime/dosbox-adapter.js')contents=Buffer.from(contents.toString().replace('this.module.practiceSignal=(port,packed)=>this.practice.signal(port,packed);',`this.module.practiceSignal=(port,packed)=>{this.practice.signal(port,packed);if(port!==0x7fd1||!globalThis.testDenseMode)return;const d=this.practice.debug.d,h=this.module.HEAPU8,v=new DataView(h.buffer);h.fill(0,d+0x2c18,d+0x2c18+150*20);for(let i=0;i<150;i++){const at=d+0x2c18+i*20,x=(12+(i%15)*24)*16,y=(24+Math.floor(i/15)*25)*16;h[at]=1;h[at+1]=2;for(const [o,n] of [[2,x],[4,y],[6,x],[8,y]])v.setInt16(at+o,n,true);v.setUint16(at+14,52+i%11,true);}h[d+0x2b95]=255;};`));
    if(process.env.PC98_PRACTICE_BASELINE==='1'&&path==='/runtime/dosbox-disk.js')contents=Buffer.from(contents.toString().replace('network||practice?30000:',"network?30000:practice?16000:"));
    if(process.env.PC98_PRACTICE_BASELINE==='1'&&game==='th02'&&path==='/runtime/dosbox-disk.js')contents=Buffer.from(contents.toString().replace("network||practice?'30000':","network?'30000':practice?'16000':"));
    if(process.env.PC98_PRACTICE_BASELINE==='1'&&path==='/runtime/practice-engine.js')contents=Buffer.from(contents.toString().replace('heap()[resident+73]=1;','').replace('w8(0x53a0,1);',''));
    if(process.env.PC98_PRACTICE_DEATH==='1'&&path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from(game==='th02'?`\nexport function testPracticeHit(lives){const d=emulator.practice.debug.d,h=emulator.module.HEAPU8;h[d+0xb2c]=lives;h[d+0xb2d]=0;h[d+0x2b94]=1;}\n`:`\nexport function testPracticeHit(lives){return emulator.call('practiceTestHit',[lives]);}\n`)]);
    if(process.env.PC98_PRACTICE_DEATH==='1'&&game==='th04'&&path==='/runtime/solo-worker.js')contents=Buffer.from(contents.toString().replace("if(method==='soloFault')", "if(method==='practiceTestHit'){const d=emulator.practice.debug.d,h=emulator.module.HEAPU8,r=emulator.practice.debug.resident;h[r+11]=args[0]+1;h[r+13]=0;h[d+0x4669]=1;return;}if(method==='soloFault')"));
    if(process.env.PC98_PRACTICE_CLEAR==='1'&&path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from(game==='th02'?`\nexport function testPracticeClear(){const d=emulator.practice.debug.d;emulator.module.HEAPU8[d+0x2bfa]=1;}\n`:`\nexport function testPracticeClear(){return emulator.call('practiceTestClear');}\n`)]);
    if(process.env.PC98_PRACTICE_CLEAR==='1'&&game==='th04'&&path==='/runtime/solo-worker.js')contents=Buffer.from(contents.toString().replace("if(method==='soloFault')", "if(method==='practiceTestClear'){const d=emulator.practice.debug.d;emulator.module.HEAPU8[d+0x53d9]=254;return;}if(method==='soloFault')"));
    if(process.env.PC98_PRACTICE_REPLAY==='1'&&path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from(game==='th02'?"\nexport function testProgram(){const m=emulator.module,r=emulator.practice?.debug.resident;return globalThis.titleOpened?'OP':r>=0&&m.HEAPU8[r+39]?'DEMO':m.UTF8ToString(m._emGetRunningProgram());} export function armTitle(){const fs=emulator.module.FS,open=fs.open;fs.open=function(path,...args){if(/(?:^|[\\\\/])OP(?:\\.EXE)?$/i.test(String(path)))globalThis.titleOpened=true;return open.call(this,path,...args);};}\n":"\nexport function testProgram(){return emulator.call('testProgram');} export function armTitle(){return emulator.call('armTitle');}\n")]);
    if(process.env.PC98_PRACTICE_REPLAY==='1'&&game==='th04'&&path==='/runtime/solo-worker.js')contents=Buffer.from(contents.toString().replace("if(method==='soloFault')","if(method==='armTitle'){const fs=emulator.module.FS,open=fs.open;fs.open=function(path,...args){if(/(?:^|[\\\\/])OP(?:\\.EXE)?$/i.test(String(path)))globalThis.titleOpened=true;return open.call(this,path,...args);};return;}if(method==='testProgram'){const m=emulator.module,r=emulator.practice?.debug.resident;return globalThis.titleOpened?'OP':r>=0&&m.HEAPU8[r+62]?'DEMO':m.UTF8ToString(m._emGetRunningProgram());}if(method==='soloFault')"));
    if(process.env.PC98_PAUSE_RESTART==='1'&&path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from('\nlet original;export function testMark(){original=emulator;}export function testSame(){return emulator===original;}\n')]);
    res.end(contents);
  }catch{res.writeHead(404).end();}});
  await new Promise(r=>server.listen(0,'127.0.0.1',r));
  let browser,page;
  const errors=[];
  try{
    browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
    page=await browser.newPage();await page.setViewport({width:1100,height:800});
    page.on('pageerror',e=>{errors.push(e.message);console.error(game,e.message);});
    page.on('console',m=>{if(m.type()==='error')console.error(game,m.text());});
    await page.goto('http://127.0.0.1:'+server.address().port);
    await page.waitForFunction(()=>events.some(e=>['ready','error'].includes(e.event)),{timeout:120000});
    assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
    const frame=page.frames().find(f=>f.url().includes('runtimeEpoch=1'));
    await frame.evaluate(async()=>{
      const {OriginalSfx}=await import('./original-sfx.mjs'),create=OriginalSfx.create;
      const command=OriginalSfx.prototype.command;globalThis.practiceEffectsCreated=0;globalThis.practiceLoadingEffects=0;
      OriginalSfx.create=async(...args)=>{practiceEffectsCreated++;return create.call(OriginalSfx,...args);};
      OriginalSfx.prototype.command=function(...args){if(!document.querySelector('.practice-preparing').hidden)practiceLoadingEffects++;return command.apply(this,args);};
      const {SoloMusic}=await import('./solo-music.js'),resume=SoloMusic.prototype.resume;globalThis.practiceLoadingMusicResumes=0;
      SoloMusic.prototype.resume=function(...args){if(!document.querySelector('.practice-preparing').hidden)practiceLoadingMusicResumes++;return resume.apply(this,args);};
    });
    await page.evaluate(language=>rpc('configure',{music:'none',language,options:{touchEnabled:false},resources:[],runtimeResources:[],sharedResources:[]}),process.env.PC98_LANGUAGE||'ja');
    await page.evaluate(()=>rpc('launch',{launchSerial:1}));
    const state=()=>frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState());
    await frame.waitForSelector('.managed-practice-open:not([hidden])',{timeout:120000});
    const full=process.env.PC98_FULL_RESTART==='1';
    if(full){
      for(let i=0;i<35;i++){
        if((await state()).native?.practice?.mode==='full'&&(await state()).native?.practice?.ready)break;
        await frame.$eval('canvas',n=>n.focus());await page.keyboard.press('KeyZ',{delay:100});await new Promise(r=>setTimeout(r,600));
      }
      await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.mode==='full'&&p.ready&&p.frames>5;},{timeout:90000});
    }else{
    const stage=Number(process.env.PC98_PRACTICE_STAGE??(game==='th02'?5:6));
    const entry=process.env.PC98_PRACTICE_ENTRY||'boss',character=Number(process.env.PC98_PRACTICE_CHARACTER||0);
    await frame.click('.managed-practice-open');
    await frame.focus('[data-jump=character]');await page.keyboard.press('ArrowDown');
    assert.equal(await frame.evaluate(()=>document.activeElement?.dataset.jump),'difficulty','down moves to the next setting');
    const beforeDifficulty=await frame.$eval('[data-jump=difficulty]',n=>n.value);
    await page.keyboard.press('ArrowRight');
    assert.notEqual(await frame.$eval('[data-jump=difficulty]',n=>n.value),beforeDifficulty,'right adjusts the selected setting');
    await frame.select('[data-jump=difficulty]','1');
    await frame.select('[data-jump=character]',String(character));
    await frame.select('[data-jump=stage]',String(stage));
    await frame.select('[data-jump=entry]',entry);
    const lives=Number(process.env.PC98_PRACTICE_LIVES??2),bombs=Number(process.env.PC98_PRACTICE_BOMBS??3);
    for(const [name,value] of [['lives',lives],['bombs',bombs]]){
      await frame.focus(`[data-jump=${name}]`);await page.keyboard.press('ArrowRight');
      assert.equal(await frame.$eval(`[data-jump=${name}]`,node=>node.value),'1',name+' increases with the right arrow');
      await page.keyboard.press('ArrowLeft');assert.equal(await frame.$eval(`[data-jump=${name}]`,node=>node.value),'0',name+' decreases with the left arrow');
      await frame.$eval(`[data-jump=${name}]`,(node,value)=>{node.value=String(value);},value);
    }
    if(game==='th04'&&stage===6)await frame.select('[data-jump=branch]','gengetsu');
    const phase=process.env.PC98_PRACTICE_PHASE??await frame.$$eval('[data-jump=phase] option',nodes=>(nodes.filter(n=>n.value!=='0').at(-1)||nodes[0]).value);
    await frame.select('[data-jump=phase]',phase);
    await frame.click('[data-practice-jump]');
    const ready=async()=>frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.ready&&p.waiting;},{timeout:180000});
    await ready();await frame.$eval('canvas',n=>n.focus());await page.keyboard.press('Enter',{delay:100});
    await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.elapsed>5&&!p.waiting;},{timeout:30000});
    }
    const key=async(code,key,keyCode)=>{await page.evaluate(x=>rpc('keyboard',{...x,location:0,down:true}),{code,key,keyCode});await new Promise(r=>setTimeout(r,160));await page.evaluate(x=>rpc('keyboard',{...x,location:0,down:false}),{code,key,keyCode});await new Promise(r=>setTimeout(r,160));};
    for(let restartCount=0;restartCount<(full?2:1);restartCount++){
    await key('Escape','Escape',27);await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativePause?.visible,{timeout:15000});
    await page.screenshot({path:resolve(output,`${game}-native-pause.png`)});
    await key('Escape','Escape',27);await frame.waitForFunction(async()=>!(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativePause?.visible,{timeout:15000});
    await new Promise(r=>setTimeout(r,500));await page.screenshot({path:resolve(output,`${game}-resume-debug.png`)});await key('Escape','Escape',27);await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativePause?.visible,{timeout:15000});
    for(let i=0;i<3;i++)await key('ArrowDown','ArrowDown',40);
    await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativePause?.choice===3,{timeout:15000});
    const beforeRestart=full?(await state()).native.practice.restartSetup:null;
    await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testMarkAttempt());
    await key('KeyZ','z',90);
    await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.frames===0||p?.preparing;},{timeout:60000});
    assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testAttemptSame()),true,'restart must keep the same emulator instance');
    if(full){
      await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.mode==='full'&&p.ready&&p.frames>30&&p.stage===0;},{timeout:60000});
    }else{
      await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.ready&&p.waiting;},{timeout:180000});
      await frame.$eval('canvas',n=>n.focus());await page.keyboard.press('Enter',{delay:100});
      await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.elapsed>30&&!p.waiting;},{timeout:30000});
    }
    if(full)assert.deepEqual((await state()).native.practice.restartSetup,beforeRestart,'restart preserves original difficulty, character and starting stock');
    await page.screenshot({path:resolve(output,`${game}-native-retry.png`)});
    const canvasImage=await (await frame.$('canvas')).screenshot({encoding:'base64'});
    const visiblePixels=await page.evaluate(async encoded=>{
      const bitmap=await createImageBitmap(await (await fetch('data:image/png;base64,'+encoded)).blob());
      const c=document.createElement('canvas');c.width=bitmap.width;c.height=bitmap.height;
      const ctx=c.getContext('2d',{willReadFrequently:true});ctx.drawImage(bitmap,0,0);
      const pixels=ctx.getImageData(Math.floor(c.width*.08),Math.floor(c.height*.05),Math.floor(c.width*.48),Math.floor(c.height*.8)).data;let count=0;
      for(let i=0;i<pixels.length;i+=4)if(Math.max(pixels[i],pixels[i+1],pixels[i+2])>24)count++;
      bitmap.close();return count;
    },canvasImage);
    assert(visiblePixels>2000,`retry must display the game, got ${visiblePixels} visible pixels`);
    }
    assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);assert.deepEqual(errors,[]);console.log(game,'native pause retry and resumed gameplay OK');
  }finally{await browser?.close();await new Promise(r=>server.close(r));}
}
