// Real managed runtime: measure targeted practice and retry without a lobby.
// node launcher/tests/browser/test-pc98-practice-load.mjs th02 [th04]
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {emptyPc98Scores} from '../../../common/browser/pc98-empty-scores.mjs';

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
    if(process.env.PC98_TEST_CHECKPOINT==='1'&&path==='/runtime/dosbox-adapter.js'){const source=contents.toString(),marker=game==='th02'?'const inplace=h[base+e.restartPending]===2;':'this.config.practice=config;this.practice=createPracticeEngine(';assert(source.includes(marker));contents=Buffer.from(source.replace(marker,(game==='th02'?marker:'')+'if(h[base+e.restartPending]===2)globalThis.testNativeCheckpointCount=(globalThis.testNativeCheckpointCount||0)+1;'+(game==='th04'?marker:'')));}
    if(process.env.PC98_TEST_CHECKPOINT==='1'&&path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from(game==='th02'?"\nexport function testCheckpointCount(){return globalThis.testNativeCheckpointCount||0;}\n":"\nexport function testCheckpointCount(){return emulator.call('testCheckpointCount');}\n")]);
    if(process.env.PC98_TEST_CHECKPOINT==='1'&&game==='th04'&&path==='/runtime/solo-worker.js')contents=Buffer.from(contents.toString().replace("if(method==='soloFault')","if(method==='testCheckpointCount')return globalThis.testNativeCheckpointCount||0;if(method==='soloFault')"));
    if(path==='/runtime/managed-solo.mjs')contents=Buffer.from(contents.toString().replace('async function returnToTitle(save,restarting=false,current=emulator,practiceConfig=null){','async function returnToTitle(save,restarting=false,current=emulator,practiceConfig=null){globalThis.testHostTransitions=(globalThis.testHostTransitions||0)+1;'));
    if(process.env.PC98_TEST_LEGACY==='1'&&path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from('\nexport function testReplaySources(){return sources;}\n')]);
    if(path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from('\nlet previousAttempt;export function testMarkAttempt(){previousAttempt=emulator;} export function testAttemptSame(){return !!emulator&&emulator===previousAttempt;}\n')]);
    if(process.env.PC98_PRACTICE_PERF==='1'&&path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from('\nexport function testDisableStandby(){retryBlocked=true;discardRetry();}\n'+(game==='th04'?"export function testDense(){return emulator.call('practiceTestDense');} export function testPerfProbe(){return emulator.call('practicePerfProbe');}\n":"export function testDense(){globalThis.testDenseMode=true;} export function testPerfProbe(){const d=emulator.practice.debug.d,h=emulator.module.HEAPU8;let bullets=0;for(let i=0;i<150;i++)if(h[d+0x2c18+i*20]===1)bullets++;return {bullets,slowdown:h[d+0x2bfd]};}\n"))]);
    if(process.env.PC98_PRACTICE_PERF==='1'&&game==='th04'&&path==='/runtime/solo-worker.js'){
      contents=Buffer.from(contents.toString().replace("if(method==='soloFault')", "if(method==='practicePerfProbe'){const d=emulator.practice.debug.d,h=emulator.module.HEAPU8;let bullets=0;for(let i=0;i<440;i++)if(h[d+0x5a22+i*26]===1)bullets++;return {bullets,turbo:h[d+0x53a0]};}if(method==='practiceTestDense'){globalThis.testDenseMode=true;return;}if(method==='soloFault')"));
    }
    if(process.env.PC98_PRACTICE_PERF==='1'&&game==='th04'&&path==='/runtime/dosbox-adapter.js')contents=Buffer.from(contents.toString().replace('this.module.practiceSignal=(port,packed)=>{if(!this.acceptNativePracticeForm(port))this.practice.signal(port,packed);};',`this.module.practiceSignal=(port,packed)=>{if(!this.acceptNativePracticeForm(port))this.practice.signal(port,packed);if(port!==0x7fe1||!globalThis.testDenseMode)return;const d=this.practice.debug.d,h=this.module.HEAPU8,v=new DataView(h.buffer);h.fill(0,d+0x5a22,d+0x5a22+440*26);for(let i=0;i<300;i++){const at=d+0x5a22+i*26,x=(12+(i%20)*18)*16,y=(24+Math.floor(i/20)*20)*16;h[at]=1;h[at+1]=32;for(const [o,n] of [[2,x],[4,y],[6,x],[8,y]])v.setInt16(at+o,n,true);h[at+19]=2;v.setUint16(at+24,52+i%11,true);}h[d+0x4662]=255;};`));
    if(process.env.PC98_PRACTICE_PERF==='1'&&game==='th02'&&path==='/runtime/dosbox-adapter.js')contents=Buffer.from(contents.toString().replace('this.module.practiceSignal=(port,packed)=>this.practice.signal(port,packed);',`this.module.practiceSignal=(port,packed)=>{this.practice.signal(port,packed);if(port!==0x7fd1||!globalThis.testDenseMode)return;const d=this.practice.debug.d,h=this.module.HEAPU8,v=new DataView(h.buffer);h.fill(0,d+0x2c18,d+0x2c18+150*20);for(let i=0;i<150;i++){const at=d+0x2c18+i*20,x=(12+(i%15)*24)*16,y=(24+Math.floor(i/15)*25)*16;h[at]=1;h[at+1]=2;for(const [o,n] of [[2,x],[4,y],[6,x],[8,y]])v.setInt16(at+o,n,true);v.setUint16(at+14,52+i%11,true);}h[d+0x2b95]=255;};`));
    if(process.env.PC98_PRACTICE_BASELINE==='1'&&path==='/runtime/dosbox-disk.js')contents=Buffer.from(contents.toString().replace('network||practice?30000:',"network?30000:practice?16000:"));
    if(process.env.PC98_PRACTICE_BASELINE==='1'&&game==='th02'&&path==='/runtime/dosbox-disk.js')contents=Buffer.from(contents.toString().replace("network||practice?'30000':","network?'30000':practice?'16000':"));
    if(process.env.PC98_PRACTICE_BASELINE==='1'&&path==='/runtime/practice-engine.js')contents=Buffer.from(contents.toString().replace('heap()[resident+73]=1;','').replace('w8(0x53a0,1);',''));
    if(process.env.PC98_PRACTICE_DEATH==='1'&&path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from(game==='th02'?`\nexport function testPracticeHit(lives){const d=emulator.practice.debug.d,h=emulator.module.HEAPU8;h[d+0xb2c]=lives;h[d+0xb2d]=0;h[d+0x2b94]=1;} export function testReadText(){return Array.from(emulator.module.HEAPU8.subarray(emulator.practice.debug.base+0xa0000,emulator.practice.debug.base+0xa1000));}\n`:`\nexport function testPracticeHit(lives){return emulator.call('practiceTestHit',[lives]);} export function testReadText(){return emulator.call('practiceTestText');}\n`)]);
    if(process.env.PC98_PRACTICE_DEATH==='1'&&game==='th04'&&path==='/runtime/solo-worker.js'){
      const hit="h[r+11]=args[0]+1;h[r+13]=0;h[d+0x4662]=0;h[d+0x466a]=0;h[d+0x4669]=1;";
      contents=Buffer.from(contents.toString().replace("if(method==='soloFault')", `if(method==='practiceTestText'){const b=emulator.practice.debug.base;return {bytes:Array.from(emulator.module.HEAPU8.subarray(b+0xa0000,b+0xa1000))};}if(method==='practiceTestHit'){const d=emulator.practice.debug.d,h=emulator.module.HEAPU8,r=emulator.practice.debug.resident;${hit}return;}if(method==='soloFault')`));
    }
    if(process.env.PC98_PRACTICE_CLEAR==='1'&&path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from(game==='th02'?`\nexport function testPracticeClear(){const d=emulator.practice.debug.d;emulator.module.HEAPU8[d+0x2bfa]=1;}\n`:`\nexport function testPracticeClear(){return emulator.call('practiceTestClear');}\n`)]);
    if(process.env.PC98_TEST_PHASE_END==='1'&&game==='th04'&&path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from("\nexport function testBossHp(value){return emulator.call('practiceTestBossHp',[value]);}export function testMidHp(value){return emulator.call('practiceTestMidHp',[value]);}\n")]);
    if(process.env.PC98_TEST_PHASE_END==='1'&&game==='th02'&&path==='/runtime/managed-solo.mjs')contents=Buffer.concat([contents,Buffer.from("\nexport function testBossDamage(value){const d=emulator.practice.debug.d,v=new DataView(emulator.module.HEAPU8.buffer);if(value!==undefined)v.setUint16(d+0x2bea,value,true);return v.getUint16(d+0x2bea,true);}\n")]);
    if(process.env.PC98_TEST_PHASE_END==='1'&&game==='th04'&&path==='/runtime/solo-worker.js')contents=Buffer.from(contents.toString().replace("if(method==='soloFault')","if(method==='practiceTestMidHp'){const d=emulator.practice.debug.d,h=emulator.module.HEAPU8;new DataView(h.buffer).setInt16(d+0x53c2,args[0],true);return;}if(method==='practiceTestBossHp'){const d=emulator.practice.debug.d,h=emulator.module.HEAPU8;new DataView(h.buffer).setInt16(d+0x53d6,args[0],true);return;}if(method==='soloFault')"));
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
      const initial=(await state()).native.practice.restartSetup;
      assert.equal(initial.difficulty,1,'fresh default difficulty is Normal');
      assert.equal(initial.lives,3,'fresh default is three spare lives');
      assert.equal(initial.bombs,2,'fresh default is two Bombs');
      const score=await page.evaluate(game=>rpc('read',{path:game==='th02'?'HUUHI.DAT':'GENSOU.SCR'}),game);
      assert.deepEqual(Uint8Array.from(score.bytes),emptyPc98Scores(game),'fresh native score table is empty');
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
      const initial=Number(await frame.$eval(`[data-jump=${name}]`,node=>node.value));
      await frame.focus(`[data-jump=${name}]`);await page.keyboard.press('ArrowRight');
      assert.equal(await frame.$eval(`[data-jump=${name}]`,node=>node.value),String(initial+1),name+' increases with the right arrow');
      await page.keyboard.press('ArrowLeft');assert.equal(await frame.$eval(`[data-jump=${name}]`,node=>node.value),String(initial),name+' decreases with the left arrow');
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
    if(process.env.PC98_NATIVE_TRANSITION==='1'){
      assert(full,'native transition test requires PC98_FULL_RESTART=1');
      await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testMarkAttempt());
      if(process.env.PC98_TRANSITION_FROM_PAUSE==='1'){
        await key('Escape','Escape',27);
        await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativePause?.visible,{timeout:15000});
      }
      await key('F12','F12',123); await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativeForm?.visible,{timeout:15000});
      const extra=process.env.PC98_TRANSITION_EXTRA==='1';
      const targetStage=game==='th04'?Number(process.env.PC98_TH04_STAGE??1):Number(process.env.PC98_TH02_STAGE??1);
      const targetCharacter=game==='th04'?Number(process.env.PC98_TH04_CHARACTER??1):1;
      if(extra)await key('ArrowLeft','ArrowLeft',37);
      else{for(let i=0;i<targetStage;i++)await key('ArrowRight','ArrowRight',39);await key('ArrowDown','ArrowDown',40);for(let i=0;i<targetCharacter;i++)await key('ArrowRight','ArrowRight',39);await key('ArrowDown','ArrowDown',40);await key('ArrowRight','ArrowRight',39);}
      const gengetsu=extra&&game==='th04'&&process.env.PC98_TRANSITION_GENGETSU==='1';
      const boss=process.env.PC98_TRANSITION_BOSS==='1'||gengetsu,mid=process.env.PC98_TRANSITION_MID==='1';
      if(gengetsu){for(let i=0;i<3;i++)await key('ArrowDown','ArrowDown',40);await key('ArrowRight','ArrowRight',39);for(let i=0;i<5;i++)await key('ArrowDown','ArrowDown',40);await key('ArrowRight','ArrowRight',39);for(let i=0;i<4;i++)await key('ArrowUp','ArrowUp',38);for(let i=0;i<Number(process.env.PC98_GEN_PHASE_INDEX??1);i++)await key('ArrowRight','ArrowRight',39);for(let i=0;i<7;i++)await key('ArrowDown','ArrowDown',40);}
      else if(boss||mid){assert(!extra||game==='th02');for(let i=0;i<(extra?3:1);i++)await key('ArrowDown','ArrowDown',40);await key('ArrowRight','ArrowRight',39);if(boss&&!(game==='th02'&&!extra&&targetStage===4)&&!(game==='th04'&&targetStage>3))await key('ArrowRight','ArrowRight',39);await key('ArrowDown','ArrowDown',40);for(let i=0;i<(mid?0:game==='th02'?Number(process.env.PC98_TH02_BOSS_PHASE??1):Number(process.env.PC98_TH04_BOSS_INDEX??1));i++)await key('ArrowRight','ArrowRight',39);}
      const restrictions=process.env.PC98_TEST_RESTRICTIONS==='1';
      if(restrictions){assert(!boss&&!extra);for(let i=0;i<3;i++)await key('ArrowUp','ArrowUp',38);await key('KeyZ','z',90);for(let i=0;i<4;i++){await key('KeyZ','z',90);await key('ArrowDown','ArrowDown',40);}await key('Escape','Escape',27);await key('ArrowUp','ArrowUp',38);}
      else if(!gengetsu)for(let i=0;i<(boss||mid?6:extra?2:4);i++)await key('ArrowUp','ArrowUp',38);
      const seekStarted=Date.now();await key('KeyZ','z',90);
      try{await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.mode==='target'&&p.ready&&p.waiting;},{timeout:90000});}catch(error){console.error(game,'target entry diagnostic',JSON.stringify((await state()).native));throw error;}
      let p=(await state()).native.practice; assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testAttemptSame()),true,'target must reuse same emulator'); assert.equal(p.setup.stage,extra?(game==='th02'?5:6):targetStage);assert.equal(p.setup.character,extra?0:targetCharacter);assert.equal(p.setup.difficulty,extra?4:2); assert.equal(p.stage,p.setup.stage);
      assert.equal(await frame.evaluate(()=>globalThis.testHostTransitions||0),0,'the guest must initiate the form transition');
      if(boss){assert.equal(p.setup.entry,'boss');const index=Number(process.env.PC98_TH02_BOSS_PHASE??1),th02Phase=(extra||targetStage===4)?[0,1,3,5,7,9][index]:index;assert.equal(p.setup.phase,gengetsu?[0,2,3,4,5,7,8][Number(process.env.PC98_GEN_PHASE_INDEX??1)]:game==='th02'?th02Phase:([[0,2,3,4],[0,2,3,4,5],[0,3],targetCharacter>=2?[0,2,4,6,8,9,11]:[0,2],[0,2,4,5,7,8,10,11,13,14,16,17],[0,2,4,8,12,14,16]])[targetStage][Number(process.env.PC98_TH04_BOSS_INDEX??1)]);if(gengetsu){assert.equal(p.setup.branch,'gengetsu');assert.equal(p.bossUpdate,0xc7da,'the selected encounter must actually be Gengetsu');assert.equal(p.phase,p.setup.phase||2,'the native encounter must reach the selected phase');const phase=p.setup.phase||2;assert.equal(p.bossHp,({2:18700,3:14700,4:12700,5:8900,7:5500,8:3000})[phase],'native attack starting HP');assert.equal(p.phaseEndHp,({2:14700,3:12700,4:8900,5:5500,7:3000,8:0})[phase],'native attack end threshold');}}
      if(process.env.PC98_TEST_PHASE_END==='1'&&game==='th02'&&targetStage===4&&p.setup.phase===3)assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testBossDamage()),0,'Mima phase 3 must reset damage at the native transition');
      if(mid)assert.equal(p.setup.entry,'mid');
      if(restrictions)for(const option of ['lockRank','disableBomb','disableFocus','disableShot'])assert.equal(p.options[option],true,option);
      await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.mode===2,{timeout:15000});
      await frame.evaluate(()=>new Promise(resolve=>requestAnimationFrame(()=>requestAnimationFrame(resolve))));
      console.log(game,'target selection to visible pause menu',Date.now()-seekStarted,'ms');
      if(process.env.PC98_TARGET_SCREENSHOT)await (await frame.$('canvas')).screenshot({path:resolve(output,`${game}-${process.env.PC98_TARGET_SCREENSHOT}-target.png`)});
      await key('Enter','Enter',13); try{await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.elapsed>5,{timeout:30000});}catch(error){console.error(game,'entry resume diagnostic',JSON.stringify((await state()).native));throw error;}
      if(process.env.PC98_TEST_PHASE_END==='1'){
        await key('Backspace','Backspace',8);await key('F1','F1',112);await key('Backspace','Backspace',8);
        await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.options?.invincible,{timeout:10000});
      }
      if(gengetsu||(boss&&game==='th02')){
        await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.elapsed>60&&!p.outcome;},{timeout:30000});
        const live=(await state()).native.practice;assert.equal(live.phase,p.setup.phase||2);if(gengetsu){assert.equal(live.bossUpdate,0xc7da);assert(live.bossHp>live.phaseEndHp,'selected attack must remain alive');}
      }
      if(process.env.PC98_TEST_PHASE_END==='1'&&process.env.PC98_TEST_PHASE_END_AFTER_RETRY!=='1'&&process.env.PC98_TEST_PHASE_END_AFTER_SWITCH!=='1'&&game==='th04'&&boss){
        const before=(await state()).native.practice,threshold=before.phaseEndHp;
        await frame.evaluate(async value=>await (await import('./managed-solo.mjs')).testBossHp(value),threshold+1);
        await frame.waitForFunction(async count=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.elapsed>=count||p?.outcome;},{timeout:15000},before.elapsed+15);
        assert.equal((await state()).native.practice.outcome,null,'one HP above threshold must remain active');
        await frame.evaluate(async value=>await (await import('./managed-solo.mjs')).testBossHp(value),threshold);
        await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativeResult?.visible,{timeout:30000});
        assert.equal((await state()).native.practice.outcome,'clear','reaching native HP threshold must complete the selected attack');
        console.log(game,'native phase end threshold OK',before.stage,before.phase);continue;
      }
      if(process.env.PC98_TEST_PHASE_END==='1'&&game==='th04'&&mid){
        const before=(await state()).native.practice;assert(before.midHp>1,'midboss must start with positive HP');
        await frame.evaluate(async()=>await (await import('./managed-solo.mjs')).testMidHp(1));
        await frame.waitForFunction(async count=>{const q=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return q?.elapsed>=count||q?.outcome;},{timeout:15000},before.elapsed+30);
        assert.equal((await state()).native.practice.outcome,null,'midboss must remain active above zero HP');
        await page.evaluate(()=>rpc('keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:true}));
        await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativeResult?.visible,{timeout:45000});
        await page.evaluate(()=>rpc('keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:false}));
        assert.equal((await state()).native.practice.outcome,'clear');console.log(game,'midboss native defeat OK');continue;
      }
      if(process.env.PC98_TEST_PHASE_END==='1'&&game==='th02'&&boss){
        const before=(await state()).native.practice,threshold=({0:{1:700,2:1400,3:2260},1:{1:701,2:1501,3:2400},4:{1:600,3:700,5:800,7:1500,9:1100},5:{1:1800,3:1800,5:1800,7:1800,9:1300}})[before.setup.stage]?.[before.setup.phase];assert.notEqual(threshold,undefined,'test needs a native threshold for this attack');
        await frame.evaluate(async value=>(await import('./managed-solo.mjs')).testBossDamage(value),threshold-1);
        await frame.waitForFunction(async count=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.elapsed>=count||p?.outcome;},{timeout:15000},before.elapsed+15);
        assert.equal((await state()).native.practice.outcome,null,'damage below threshold must remain active');
        if(before.setup.stage<2&&before.setup.phase===3){
          // Meira's final defeat threshold is checked inside the real shot
          // collision branch, so a live hit must cross the boundary.
          await page.evaluate(()=>rpc('keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:true}));
        }else await frame.evaluate(async value=>(await import('./managed-solo.mjs')).testBossDamage(value),threshold);
        try{await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativeResult?.visible,{timeout:90000});}catch(error){console.error(game,'phase boundary diagnostic',JSON.stringify((await state()).native));throw error;}
        if(before.setup.stage<2&&before.setup.phase===3)await page.evaluate(()=>rpc('keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:false}));
        assert.equal((await state()).native.practice.outcome,'clear','native phase transition must complete the selected attack');
        console.log(game,'native phase end threshold OK',before.stage,before.phase);continue;
      }
      if(process.env.PC98_TEST_PHASE_SWITCH==='1'){
        assert(boss&&process.env.PC98_TEST_CHECKPOINT==='1');
        const prior=await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testCheckpointCount());
        await key('F12','F12',123);await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativeForm?.visible,{timeout:10000});
        const backward=process.env.PC98_TEST_PHASE_SWITCH_BACK==='1';
        for(let i=0;i<4;i++)await key('ArrowDown','ArrowDown',40);await key(backward?'ArrowLeft':'ArrowRight',backward?'ArrowLeft':'ArrowRight',backward?37:39);
        const target=(await state()).native.practice.nativeForm.values[4];assert(backward?target<p.setup.phase:target>p.setup.phase,'select the requested native attack');
        for(let i=0;i<7;i++)await key('ArrowDown','ArrowDown',40);
        const started=Date.now();await key('KeyZ','z',90);
        await frame.waitForFunction(async wanted=>{const n=(await import('./managed-solo.mjs')).managedAPI.practiceState().native,q=n?.practice;return n?.mode===2&&q?.ready&&q.waiting&&q.setup.phase===wanted;},{timeout:90000},target);
        await frame.evaluate(()=>new Promise(resolve=>requestAnimationFrame(()=>requestAnimationFrame(resolve))));
        assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testCheckpointCount()),prior+(backward&&process.env.PC98_TEST_BACK_CHECKPOINT!=='1'?0:1),'phase selection must use a compatible native checkpoint');
        assert.equal(await frame.evaluate(()=>globalThis.testHostTransitions||0),0,'phase selection must not reload MAIN');
        const q=(await state()).native.practice;assert.equal(q.phase,target);assert.equal(q.lives,q.setup.lives,'phase selection restores initial lives');assert.equal(q.bombs,q.setup.bombs,'phase selection restores initial bombs');if(gengetsu){assert.equal(q.bossUpdate,0xc7da);assert.equal(q.bossHp,backward?18700:14700);assert.equal(q.phaseEndHp,backward?14700:12700);}
        console.log(game,backward?'earlier Boss phase selection to visible pause menu':'same-Boss phase selection to visible pause menu',Date.now()-started,'ms');
        await (await frame.$('canvas')).screenshot({path:resolve(output,`${game}-phase-switch.png`)});
        await key('Enter','Enter',13);await frame.waitForFunction(async()=>{const q=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return q?.elapsed>60&&!q.outcome;},{timeout:30000});
        p=(await state()).native.practice;
        if(process.env.PC98_TEST_PHASE_END_AFTER_SWITCH==='1'&&game==='th04'){
          const threshold=p.phaseEndHp;await frame.evaluate(async value=>await (await import('./managed-solo.mjs')).testBossHp(value),threshold+1);
          await frame.waitForFunction(async count=>{const q=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return q?.elapsed>=count||q?.outcome;},{timeout:15000},p.elapsed+15);
          assert.equal((await state()).native.practice.outcome,null,'switched attack stays active above its end threshold');
          await frame.evaluate(async value=>await (await import('./managed-solo.mjs')).testBossHp(value),threshold);
          await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativeResult?.visible,{timeout:30000});
          assert.equal((await state()).native.practice.outcome,'clear');console.log(game,'switched attack end threshold OK',p.stage,p.phase);continue;
        }
      }
      if(process.env.PC98_TEST_FPS==='1'&&game==='th04'){
        await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testDense());
        const sample=async()=>{const a=(await state()).native.practice.frames,start=Date.now();await new Promise(resolve=>setTimeout(resolve,6000));const b=(await state()).native.practice.frames;return (b-a)*1000/(Date.now()-start);};
        const off=await sample();await key('Tab','Tab',9);await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.options?.showInfo,{timeout:10000});const on=await sample();
        console.log(game,'dense FPS TAB off/on',off.toFixed(1),on.toFixed(1),JSON.stringify(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testPerfProbe())));continue;
      }
      if(process.env.PC98_TEST_INFO==='1'){
        await key('Tab','Tab',9);
        await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.options?.showInfo,{timeout:10000});
        await new Promise(resolve=>setTimeout(resolve,1000));
        if(process.env.PC98_TEST_INFO_BOMB==='1'){
          for(let i=0;i<6;i++){
            if(i%2===0)await key('KeyX','x',88);
            const encoded=await (await frame.$('canvas')).screenshot({encoding:'base64'});
            const stray=await page.evaluate(async encoded=>{const bitmap=await createImageBitmap(await(await fetch('data:image/png;base64,'+encoded)).blob()),c=new OffscreenCanvas(bitmap.width,bitmap.height),ctx=c.getContext('2d');ctx.drawImage(bitmap,0,0);const x=Math.floor(bitmap.width*960/1100),y=Math.floor(bitmap.height*525/688),w=Math.floor(bitmap.width*30/1100),h=Math.floor(bitmap.height*28/688),pixels=ctx.getImageData(x,y,w,h).data;let n=0;for(let i=0;i<pixels.length;i+=4)if(pixels[i]>8||pixels[i+1]>8||pixels[i+2]>8)n++;return n;},encoded);
            assert.equal(stray,0,'native bullet counter must stay masked while HUD changes');
          }
          console.log(game,'TAB bullet counter mask stays black during HUD updates');
        }
        if(process.env.PC98_TEST_INFO_MOVING==='1')await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.elapsed>800,{timeout:45000});
        const infoImage=await (await frame.$('canvas')).screenshot({path:resolve(output,`${game}-practice-info.png`),encoding:'base64'});
        await key('Tab','Tab',9);
        await frame.waitForFunction(async()=>!(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.options?.showInfo,{timeout:10000});
        await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.elapsed>60,{timeout:30000});
        if(process.env.PC98_TEST_INFO_MOVING==='1'){
          await new Promise(resolve=>setTimeout(resolve,1000));const closedImage=await (await frame.$('canvas')).screenshot({path:resolve(output,`${game}-practice-info-closed.png`),encoding:'base64'});
          if(game==='th04'){
            const check=await page.evaluate(async images=>{const pixels=[];let colored=0;for(const encoded of images){const b=await createImageBitmap(await(await fetch('data:image/png;base64,'+encoded)).blob()),c=new OffscreenCanvas(b.width,b.height),ctx=c.getContext('2d');ctx.drawImage(b,0,0);const x=Math.floor(b.width*780/1100),y=Math.floor(b.height*628/688),w=Math.floor(b.width*120/1100),h=Math.floor(b.height*37/688);pixels.push(ctx.getImageData(x,y,w,h).data);}let different=0;for(let i=0;i<pixels[0].length;i+=4){if(pixels[0][i]>8||pixels[0][i+1]>8||pixels[0][i+2]>8)colored++;if(pixels[0][i]!==pixels[1][i]||pixels[0][i+1]!==pixels[1][i+1]||pixels[0][i+2]!==pixels[1][i+2])different++;}return {colored,different};},[infoImage,closedImage]);
            assert(check.colored>0,'original difficulty must be visible');assert.equal(check.different,0,'TAB must preserve the original difficulty pixels');console.log(game,'TAB preserves original difficulty pixels');
          }
        }
      }
        if(process.env.PC98_TEST_ASSIST_PERSIST==='1'){
          await key('Backspace','Backspace',8);await key('F1','F1',112);await key('Backspace','Backspace',8);
          await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.options?.invincible,{timeout:10000});
          await key('F12','F12',123);await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativeForm?.visible,{timeout:10000});
          assert.deepEqual((await state()).native.practice.nativeForm.values.slice(0,3),[1,1,2],'game retains the selected target');
          await key('Escape','Escape',27);await frame.waitForFunction(async()=>!(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativeForm?.visible,{timeout:10000});
        }
      if(process.env.PC98_TEST_RESULT_SELECT==='1'){
        assert.equal(process.env.PC98_PRACTICE_DEATH,'1');assert((!extra&&!boss)||process.env.PC98_TEST_CHECKPOINT==='1');
        const priorCheckpoint=process.env.PC98_TEST_CHECKPOINT==='1'?await frame.evaluate(async()=>await (await import('./managed-solo.mjs')).testCheckpointCount()):0;
        await page.evaluate(()=>rpc('keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:true}));
        await frame.evaluate(async()=>await (await import('./managed-solo.mjs')).testPracticeHit(0));
          try{await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativeResult?.visible,{timeout:30000});}
          catch(error){console.error(game,'death diagnostic',JSON.stringify((await state()).native));throw error;}
        await new Promise(resolve=>setTimeout(resolve,1500));
        assert.equal((await state()).native.practice.nativeResult.visible,true,'held fire must not select retry');
        await page.evaluate(()=>rpc('keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:false}));
        await new Promise(resolve=>setTimeout(resolve,250));
        if(process.env.PC98_TEST_RESULT_RETRY==='1'){
          const started=Date.now();await key('KeyZ','z',90);
          await frame.waitForFunction(async()=>{const current=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return current?.ready&&current.waiting&&!current.outcome;},{timeout:90000});
          assert.equal(await frame.evaluate(()=>globalThis.testHostTransitions||0),0,'result retry must start in the guest');
          if(process.env.PC98_TEST_CHECKPOINT==='1')assert.equal(await frame.evaluate(async()=>await (await import('./managed-solo.mjs')).testCheckpointCount()),priorCheckpoint+1,'result retry must restore the game-owned checkpoint');
          assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testAttemptSame()),true);
          await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.mode===2,{timeout:15000});
          await frame.evaluate(()=>new Promise(resolve=>requestAnimationFrame(()=>requestAnimationFrame(resolve))));
          console.log(game,'result RETRY to visible pause menu',Date.now()-started,'ms');
          await key('Enter','Enter',13);
          await frame.waitForFunction(async()=>{const current=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return current?.elapsed>5&&!current.waiting;},{timeout:30000});
          console.log(game,'native result retry OK');continue;
        }
        if(process.env.PC98_TEST_RESULT_SAVE==='1'){
          await key('ArrowDown','ArrowDown',40);await key('KeyZ','z',90);
          await frame.waitForFunction(async()=>{const api=(await import('./managed-solo.mjs')).managedAPI;const files=await api.files({command:'list'});return files.files.some(f=>f.path.startsWith('replay/'));},{timeout:30000});
          await key('ArrowDown','ArrowDown',40);await key('ArrowDown','ArrowDown',40);await key('KeyZ','z',90);
          await frame.waitForFunction(async()=>{const current=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return current?.ready&&current.waiting&&!current.outcome;},{timeout:90000});
          assert.equal(await frame.evaluate(()=>globalThis.testHostTransitions||0),0,'retry after save must start in the guest');
          console.log(game,'native result save and retry OK');continue;
        }
        await key('ArrowDown','ArrowDown',40);await key('ArrowDown','ArrowDown',40);await key('KeyZ','z',90);
        await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativeForm?.visible,{timeout:15000});
        const form=(await state()).native.practice.nativeForm;assert.deepEqual(form.values.slice(0,3),[1,1,2]);assert.equal(form.settings[10],3);
        await key('Escape','Escape',27);await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativeResult?.visible,{timeout:15000});
        await key('KeyZ','z',90);await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativeForm?.visible,{timeout:15000});
        await key('ArrowUp','ArrowUp',38);await key('ArrowUp','ArrowUp',38);await key('KeyZ','z',90);
        await frame.waitForFunction(async()=>{const current=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return current?.ready&&current.waiting&&!current.outcome;},{timeout:90000});
        assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testAttemptSame()),true,'select must reuse the emulator');
        assert.equal(await frame.evaluate(()=>globalThis.testHostTransitions||0),0,'result selection must start in the guest');
        console.log(game,'native result selection OK');continue;
      }
      if(process.env.PC98_TEST_LONG_RETRY==='1'){
        await key('Backspace','Backspace',8);await key('F1','F1',112);await key('Backspace','Backspace',8);
        await frame.waitForFunction(async()=>{const q=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return q?.elapsed>800&&!q.outcome;},{timeout:60000});
      }
      if(process.env.PC98_TEST_PAUSE_RETRY==='1'){
        for(let retry=0;retry<3;retry++){
        await key('Escape','Escape',27);
        await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativePause?.visible,{timeout:15000});
        for(let i=0;i<3;i++)await key('ArrowDown','ArrowDown',40);
        await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativePause?.choice===3,{timeout:15000});
        const checkpointBefore=process.env.PC98_TEST_CHECKPOINT==='1'?await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testCheckpointCount()):0;
        const started=Date.now();
        await page.evaluate(()=>rpc('keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:true}));
        await new Promise(resolve=>setTimeout(resolve,160));
        await page.evaluate(()=>rpc('keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:false}));
        try{await frame.waitForFunction(async()=>{const n=(await import('./managed-solo.mjs')).managedAPI.practiceState().native;return n?.mode===2&&n.practice?.ready&&n.practice.waiting&&!n.practice.nativePause?.visible;},{timeout:20000});}catch(error){console.error(game,'pause retry diagnostic',JSON.stringify((await state()).native));throw error;}
        if(process.env.PC98_TEST_CHECKPOINT==='1')assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testCheckpointCount()),checkpointBefore+1,'pause Restart must restore the in-game checkpoint');
        assert.equal(await frame.evaluate(()=>globalThis.testHostTransitions||0),0,'pause Restart must not reload MAIN');
        await frame.evaluate(()=>new Promise(resolve=>requestAnimationFrame(()=>requestAnimationFrame(resolve))));
        const duration=Date.now()-started;console.log(game,'pause RESTART to visible pause menu',duration,'ms');
        assert(duration<3000,'pause menu must appear within three seconds');
        const after=(await state()).native.practice;assert.deepEqual(after.setup,p.setup);assert.equal(after.stage,p.stage);if(boss)assert.equal(after.phase,p.phase);
        await (await frame.$('canvas')).screenshot({path:resolve(output,`${game}-pause-restart-visible.png`)});
        await key('Enter','Enter',13);await frame.waitForFunction(async()=>{const q=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return q?.elapsed>60&&!q.outcome;},{timeout:30000});
        }
      }
      if(process.env.PC98_TEST_QUICK_RETRY==='1'){
        for(let retry=0;retry<3;retry++){
        const retryStarted=Date.now();
        const priorCheckpoint=process.env.PC98_TEST_CHECKPOINT==='1'?await frame.evaluate(async()=>await (await import('./managed-solo.mjs')).testCheckpointCount()):0;
        await page.evaluate(()=>rpc('keyboard',{code:'ShiftLeft',key:'Shift',keyCode:16,location:1,down:true}));
        await key('KeyF','f',70);
        await page.evaluate(()=>rpc('keyboard',{code:'ShiftLeft',key:'Shift',keyCode:16,location:1,down:false}));
        try{await frame.waitForFunction(async()=>{const current=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return current?.ready&&current.waiting&&!current.outcome;},{timeout:30000});}catch(error){console.error(game,'quick retry diagnostic',JSON.stringify((await state()).native));throw error;}
          console.log(game,'native quick retry initialization',Date.now()-retryStarted,'ms');
        assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testAttemptSame()),true);
        const current=(await state()).native.practice;assert.deepEqual(current.setup,p.setup);
          assert.equal(await frame.evaluate(()=>globalThis.testHostTransitions||0),0,'quick retry must start in the guest');
          if(process.env.PC98_TEST_CHECKPOINT==='1')assert.equal(await frame.evaluate(async()=>await (await import('./managed-solo.mjs')).testCheckpointCount()),priorCheckpoint+1,'retry must restore the game-owned checkpoint');
          if(process.env.PC98_TEST_ASSIST_PERSIST==='1')assert.equal(current.options.invincible,true,'native retry must preserve assist flags');
        await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.mode===2,{timeout:15000});
        if(game==='th04'&&boss)assert.equal(current.bossHp,p.bossHp,'repeated retries must restore initial boss HP');
        await key('Enter','Enter',13);await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.elapsed>5,{timeout:30000});
          if(process.env.PC98_TEST_CHECKPOINT==='1'&&boss){await frame.waitForFunction(async()=>{const q=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return q?.elapsed>60&&!q.outcome;},{timeout:30000});const resumed=(await state()).native.practice;assert.equal(resumed.phase,p.phase,'checkpoint retry must continue the selected attack');await (await frame.$('canvas')).screenshot({path:resolve(output,`${game}-checkpoint-retry.png`)});}
        }
          if(process.env.PC98_TEST_PHASE_END_AFTER_RETRY==='1'&&game==='th04'){
            const before=(await state()).native.practice,threshold=before.phaseEndHp;
            await frame.evaluate(async value=>await (await import('./managed-solo.mjs')).testBossHp(value),threshold+1);
            await frame.waitForFunction(async count=>{const q=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return q?.elapsed>=count||q?.outcome;},{timeout:15000},before.elapsed+15);
            assert.equal((await state()).native.practice.outcome,null,'checkpoint attack must stay active above its threshold');
            await frame.evaluate(async value=>await (await import('./managed-solo.mjs')).testBossHp(value),threshold);
            await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice?.nativeResult?.visible,{timeout:30000});
            assert.equal((await state()).native.practice.outcome,'clear','checkpoint attack must finish at its native threshold');
            console.log(game,'checkpoint phase end threshold OK',before.stage,before.phase);continue;
          }
      }
      if(process.env.PC98_TEST_LOADING_CLEAR==='1'){
        assert.equal(process.env.PC98_PRACTICE_DEATH,'1');
        const result=await frame.evaluate(async()=>(await import('./managed-solo.mjs')).testReadText()),text=Array.isArray(result)?result:result.bytes;
        const row=Array.from({length:26},(_,i)=>String.fromCharCode(text[11*160+(13+i)*2])).join('');
        assert(!row.includes('LOADING PRACTICE'),'native loading text must disappear after arrival');
        console.log(game,'native loading cleanup OK');continue;
      }
      await frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.finishRecording());
      await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState().recordingReady,{timeout:30000});
      const path=await frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.saveRecording()); assert(path);
      if(process.env.PC98_TEST_LEGACY==='1')await frame.evaluate(async({path,game})=>{
        const store=globalThis.__eaglerPC98Files,bytes=store.memory.get(path),view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),size=view.getUint32(8,true),header=JSON.parse(new TextDecoder().decode(bytes.subarray(12,12+size)));
        const overrides={'../practice-attempt.js':game==='th02'?'ac1563453bc96f4dc01b00f3f5f1f07f6b6323e178fb864ee9507e5c834ec2cd':'091e33204a297f396901c0b13d1da7f6686b4811dd7d59139f2d3e6bcb0616d9','./external-host.js':'a8ccea815c47f0ab5ae478f2af4e7a9365a22d12bcf9337fe5e6faa7610a4bbf',...(game==='th04'?{'../solo-worker.js':'c606d752e295310d381628ec109960c1f9661b7b9dbdbf352fd46c176b60d48e'}:{})};
        header.identity=await (await import('./replay/tape.js')).fingerprint((await import('./managed-solo.mjs')).testReplaySources(),overrides);
        const encoded=new TextEncoder().encode(JSON.stringify(header)),prefix=bytes.slice(0,12);new DataView(prefix.buffer).setUint32(8,encoded.length,true);
        await store.write(path,new Uint8Array(await new Blob([prefix,encoded,bytes.subarray(12+size)]).arrayBuffer()));
      },{path,game});
      await page.evaluate(()=>rpc('network-cancel'));
      await page.evaluate(()=>rpc('configure',{music:'none',language:'ja',options:{replayViewer:true},resources:[],runtimeResources:[],sharedResources:[]}));
      await page.evaluate(()=>rpc('launch',{launchSerial:2}));
      const info=await page.evaluate(path=>rpc('playback',{action:'inspect',path}),path);assert.equal(info.description.stages[0].id,0);
      await page.evaluate(path=>rpc('playback',{action:'open',path}),path);
      await page.evaluate(()=>rpc('playback',{action:'pause'}));
      const pausedFrame=await frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.health().gameFrame);
      await new Promise(resolve=>setTimeout(resolve,500));
      assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.health().gameFrame),pausedFrame,'paused replay must not advance');
      await page.evaluate(()=>rpc('playback',{action:'rate',value:2}));
      await page.evaluate(()=>rpc('playback',{action:'rate',value:1}));
      const total=await page.evaluate(()=>events.filter(e=>e.event==='runtime-info'&&e.playback?.total>0).at(-1).playback.total);
      const target=Math.floor(total/2);await page.evaluate(value=>rpc('playback',{action:'seek',value}),target);
      await page.waitForFunction(target=>events.some(e=>e.event==='runtime-info'&&e.playback?.paused&&e.playback?.frame>=target&&!e.playback.seeking),{timeout:120000},target);
      await page.evaluate(()=>rpc('playback',{action:'seek',value:0}));
      await frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.playback({action:'play'}));
      await frame.waitForFunction(async()=>(await import('./managed-solo.mjs')).managedAPI.health().replayResult!==null,{timeout:180000});
      assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.health().replayResult),true,'same-instance transition must replay deterministically');
      await frame.evaluate(async()=>{const api=(await import('./managed-solo.mjs')).managedAPI;await api.playback({action:'seek',value:0});await api.playback({action:'pause'});});
      assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.health().replayResult),null,'restart must open a fresh playback');
      await frame.evaluate(async(path)=>{const api=(await import('./managed-solo.mjs')).managedAPI;await api.playback({action:'open',path});await api.playback({action:'pause'});},path);
      const moved=path.replace(/(\.th0[24]r)$/, '-moved$1');
      await page.evaluate(async({path,moved})=>{const file=await rpc('read',{path});await rpc('write',{path:moved,bytes:file.bytes});await rpc('playback',{action:'relocate',path,target:moved});await rpc('remove',{path:moved});},{path,moved});
      assert.equal(await page.evaluate(moved=>events.some(event=>event.event==='runtime-info'&&event.playback?.path===moved),moved),true,'relocation must publish the new replay path');
      console.log(game,'native practice transition and replay OK');
      continue;
    }
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
