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
    if(process.env.PC98_PAUSE_RESTART==='1'&&path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from('\nlet original;export function testMark(){original=emulator;}export function testSame(){return emulator===original;}export function diagnostics(){return {state:emulator?.state,host:emulator?.externalHost?.status,program:emulator?.module?.UTF8ToString(emulator?.module?._emGetRunningProgram()),practice:emulator?.practice?.debug};}\n')]);
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
    for(const kind of ['target','retry']){
      const started=performance.now(),samples=[];
      if(kind==='target')await frame.click('[data-practice-jump]');
      else {await frame.$eval('canvas',n=>n.focus());await page.keyboard.press('Backspace');await page.keyboard.down('Shift');await page.keyboard.press('KeyF');await page.keyboard.up('Shift');}
      let ready;
      while(performance.now()-started<180000){
        const current=await state(),p=current.native?.practice;
        samples.push({ms:Math.round(performance.now()-started),frames:p?.frames,phase:p?.phase,phaseFrame:p?.phaseFrame,preparing:p?.preparing});
        if(p?.error)throw Error(p.error);
        if(p?.ready&&p.waiting&&p.elapsed===0){ready=current;break;}
        const faults=await page.evaluate(()=>events.filter(e=>e.event==='error'));assert.deepEqual(faults,[]);
        await new Promise(r=>setTimeout(r,100));
      }
      assert(ready,'target must reach native arrival pause');
      assert.equal(ready.native.practice.setup.stage,stage);assert.equal(ready.native.practice.setup.phase,Number(phase));
      assert.equal(ready.native.practice.setup.character,character);assert.equal(ready.native.practice.setup.entry,entry);
      assert.equal(ready.native.practice.lives,lives);assert.equal(ready.native.practice.bombs,bombs);
      if(kind==='target')await frame.waitForFunction(async()=> (await import('./managed-solo.mjs')).managedAPI.practiceState().retry.state==='ready',{timeout:120000});
      const result={game,kind,stage,phase:Number(phase),loadMs:Math.round(performance.now()-started),loading:ready.loading,frames:ready.native.practice.frames,samples};results.push(result);
      console.log(JSON.stringify({...result,samples:undefined}));
      await page.screenshot({path:resolve(output,`${label}-${game}-${kind}.png`)});
      await frame.$eval('canvas',n=>n.focus());await page.keyboard.press('Enter',{delay:80});
      await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.elapsed>0&&!p.waiting;},{timeout:15000}).catch(async error=>{console.error('Resume state',JSON.stringify(await state()));throw error;});
      if(kind==='target'&&process.env.PC98_PRACTICE_LAYER==='1'){
        await page.keyboard.press('Escape');await frame.waitForSelector('.pc98-pause-menu:not([hidden])',{visible:true,timeout:10000});await page.keyboard.press('F12');
        await frame.waitForSelector('.practice-panel.practice-full:not([hidden])',{visible:true});
        assert(await frame.evaluate(()=>{const panel=document.querySelector('.practice-panel'),pause=document.querySelector('.pc98-pause-menu'),a=panel.getBoundingClientRect(),b=pause.getBoundingClientRect(),x=(Math.max(a.left,b.left)+Math.min(a.right,b.right))/2,y=(Math.max(a.top,b.top)+Math.min(a.bottom,b.bottom))/2;return panel.contains(document.elementFromPoint(x,y));}),'practice panel is above the pause menu and receives input');
        await frame.focus('[data-jump=lives]');const before=await frame.$eval('[data-jump=lives]',n=>n.value);await page.keyboard.press('ArrowRight');assert.notEqual(await frame.$eval('[data-jump=lives]',n=>n.value),before,'practice settings remain interactive above pause');
        await page.keyboard.press('F12');assert.equal(await frame.$eval('.pc98-pause-menu',n=>n.hidden),false);await page.keyboard.press('Escape');await frame.waitForSelector('.pc98-pause-menu[hidden]');
      }
      if(kind==='target'&&process.env.PC98_PRACTICE_PERF==='1'){
        const sample=async()=>{await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testDense());const a=await state(),start=performance.now();await new Promise(r=>setTimeout(r,5000));const b=await state();console.log(game,'dense fixture',await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testPerfProbe()));return (b.native.practice.frames-a.native.practice.frames)*1000/(performance.now()-start);};
        const parked=await sample();await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testDisableStandby());const without=await sample();
        console.log(game,'game FPS with parked standby / without standby:',parked.toFixed(1),without.toFixed(1));
        break;
      }
      await page.screenshot({path:resolve(output,`${label}-${game}-${kind}-running.png`)});
    }
    if(process.env.PC98_PAUSE_RESTART==='1'){
      await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testMark());
      await frame.$eval('canvas',n=>n.focus());await page.keyboard.press('Escape');await frame.waitForSelector('.pc98-pause-menu:not([hidden])',{visible:true,timeout:10000});
      await new Promise(r=>setTimeout(r,1000));await frame.$$eval('.pc98-pause-menu button',buttons=>buttons.find(b=>b.textContent==='重新开始').click());
      await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.preparing;},{timeout:15000});
      await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.ready&&p.waiting&&p.elapsed===0;},{timeout:30000}).catch(async e=>{console.log('DEBUG',game,JSON.stringify(await state()),await frame.evaluate(async()=>(await import('./managed-solo.mjs')).diagnostics()),await page.evaluate(()=>events.filter(e=>e.event==='error')));await page.screenshot({path:'restart-debug-'+game+'.png'});throw e;});
      assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testSame()),true,'restart retains the emulator');
      const again=await state();assert.equal(again.native.practice.setup.stage,stage);assert.equal(again.native.practice.setup.phase,Number(phase));
      await frame.$eval('canvas',n=>n.focus());await page.keyboard.press('Enter',{delay:80});await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.elapsed>0&&!p.waiting;},{timeout:15000});
      console.log(game,'PASS native pause restart');
    }
    assert((await frame.evaluate(()=>practiceEffectsCreated))>=2,'standby owns its own original effect driver');
    if(game==='th04'){assert.equal(await frame.evaluate(()=>practiceLoadingEffects),0,'loading and standby must not play sound effects');assert.equal(await frame.evaluate(()=>practiceLoadingMusicResumes),0,'loading must not resume music');}
    if(process.env.PC98_PRACTICE_DEATH==='1'){
      await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testPracticeHit(1));
      await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.missTimer>0;},{timeout:15000});
      assert.equal((await state()).native.practice.outcome,null,'contact and death animation do not end the attempt');
      await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.lives===0&&!p.missTimer&&p.invincibility===0;},{timeout:30000});
      assert.equal((await state()).native.practice.outcome,null,'the last spare life respawns');
      assert.equal((await state()).native.practice.bombs,game==='th04'?2:stage===5?3:5,'respawn restores native Bomb stock');
      await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testPracticeHit(0));
      await frame.evaluate(()=>{globalThis.practiceResultShownAt=null;new MutationObserver(()=>{if(document.querySelector('#practice-result-dialog[open]'))globalThis.practiceResultShownAt??=performance.now();}).observe(document.body,{attributes:true,subtree:true,attributeFilter:['open']});});
      await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.outcome==='failed';},{timeout:30000}).catch(async error=>{console.error('Final death',JSON.stringify(await state()));throw error;});
      const outcomeObservedAt=await frame.evaluate(()=>performance.now());
      await frame.waitForSelector('#practice-result-dialog[open]',{visible:true,timeout:3000});
      const resultShownAt=await frame.evaluate(()=>globalThis.practiceResultShownAt);
      console.log(game,'result menu after observed game over:',Math.round(resultShownAt-outcomeObservedAt),'ms');
      await page.screenshot({path:resolve(output,`${label}-${game}-result.png`)});
      const resultStyle=await frame.$eval('#practice-result-dialog',node=>{const r=node.getBoundingClientRect(),screen=node.parentElement.querySelector('.solo-screen').getBoundingClientRect(),button=node.querySelector('button'),s=getComputedStyle(button);return {left:(r.left-screen.left)/screen.width,top:(r.top-screen.top)/screen.height,width:r.width/screen.width,height:r.height/screen.height,background:s.backgroundColor,selected:button.classList.contains('selected')};});
      assert(Math.abs(resultStyle.left-.05)<.01&&Math.abs(resultStyle.top-1/30)<.01&&Math.abs(resultStyle.width-.6)<.01&&Math.abs(resultStyle.height-14/15)<.01,'result uses pause menu geometry');
      assert.equal(resultStyle.background,'rgb(71, 122, 172)');assert(resultStyle.selected);
      console.log(game,'PASS native spare life and final game over');
    }
    if(process.env.PC98_PRACTICE_CLEAR==='1'){
      await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testPracticeClear());
      await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.outcome==='clear',{timeout:15000});
      await frame.waitForSelector('#practice-result-dialog[open]',{visible:true,timeout:5000});
      assert.equal(await frame.$eval('#practice-result-title',n=>n.textContent),'练习成功');
      await new Promise(r=>setTimeout(r,1500));assert.equal((await state()).native.practice.outcome,'clear');
      assert.equal(await page.evaluate(()=>events.some(e=>e.event==='exit')),false,'clear stays in practice');
      console.log(game,'PASS simulated Boss defeat displays practice success');
    }
    if(process.env.PC98_PRACTICE_REPLAY==='1'){
      assert.equal(await frame.evaluate(game=>[...__eaglerPC98Files.memory.keys()].filter(name=>name.endsWith('.'+game+'r')).length,game),0,'finishing does not save a file');
      await frame.$eval('canvas',n=>n.focus());await page.keyboard.press('Escape');
      await frame.waitForSelector('.pc98-pause-menu:not([hidden])',{visible:true,timeout:10000});
      await frame.$$eval('.pc98-pause-menu button',buttons=>buttons.find(button=>button.textContent==='退出游戏').click());
      assert.equal(await frame.$eval('.pc98-pause-menu',n=>n.hidden),false,'exit presents a replay choice');
      await frame.evaluate(async()=>{await(await import('./managed-solo.mjs')).armTitle();});
      await frame.evaluate(()=>{globalThis.titleSeen=false;globalThis.programsSeen=[];const scan=async()=>{try{const program=await(await import('./managed-solo.mjs')).testProgram();if(!globalThis.programsSeen.includes(program))globalThis.programsSeen.push(program);if(/^(?:OP(?:\.EXE)?|DEMO)$/i.test(program))globalThis.titleSeen=true;}finally{if(!globalThis.titleSeen)setTimeout(scan,20);}};void scan();});
      await frame.$$eval('.pc98-pause-menu button',buttons=>buttons.find(button=>button.textContent==='保存本次 REP').click());
      await frame.waitForFunction(game=>[...__eaglerPC98Files.memory.keys()].some(name=>name.endsWith('.'+game+'r')),{timeout:45000},game);
      await frame.waitForFunction(()=>globalThis.titleSeen,{timeout:60000}).catch(async error=>{console.error('Programs observed',await frame.evaluate(()=>globalThis.programsSeen));throw error;});
      await frame.waitForFunction(async()=>{const api=(await import('./managed-solo.mjs')).managedAPI,s=api.practiceState();return !s.recordingReady&&api.health().frames>0&&s.native?.practice?.mode!=='target';},{timeout:60000});
      assert.equal(await page.evaluate(()=>events.some(e=>e.event==='exit')),false,'returning to title keeps the game open');
      await frame.evaluate(async game=>{
        const {managedAPI}=await import('./managed-solo.mjs');
        const path=[...__eaglerPC98Files.memory.keys()].findLast(name=>name.endsWith('.'+game+'r'));
        managedAPI.stop();await managedAPI.playback({action:'open',path});
      },game);
      await frame.waitForFunction(async()=>{const value=(await import('./managed-solo.mjs')).managedAPI.health().replayResult;return value!==null||parent.events.some(e=>e.event==='error');},{timeout:120000}).catch(async error=>{console.error('Replay state',await state(),await page.evaluate(()=>events.filter(e=>e.event==='error')));throw error;});
      assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.health().replayResult),true,'practice replay reproduces the complete native memory');
      console.log(game,'PASS practice replay memory');
    }
    assert.deepEqual(errors,[]);assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
  }finally{await browser?.close();await new Promise(r=>server.close(r));}
}
await writeFile(resolve(output,label+'.json'),JSON.stringify(results,null,2)+'\n');
