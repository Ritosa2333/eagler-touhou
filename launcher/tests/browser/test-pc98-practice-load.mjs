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
    res.end(await readFile(file));
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
      globalThis.practiceEffectsCreated=0;
      OriginalSfx.create=async(...args)=>{practiceEffectsCreated++;return create.call(OriginalSfx,...args);};
    });
    await page.evaluate(()=>rpc('configure',{music:'none',options:{touchEnabled:false},resources:[],runtimeResources:[],sharedResources:[]}));
    await page.evaluate(()=>rpc('launch',{launchSerial:1}));
    const state=()=>frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.practiceState());
    await frame.waitForSelector('.managed-practice-open:not([hidden])',{timeout:120000});
    const stage=Number(process.env.PC98_PRACTICE_STAGE??(game==='th02'?5:6));
    const entry=process.env.PC98_PRACTICE_ENTRY||'boss',character=Number(process.env.PC98_PRACTICE_CHARACTER||0);
    await frame.click('.managed-practice-open');
    await frame.select('[data-jump=character]',String(character));
    await frame.select('[data-jump=stage]',String(stage));
    await frame.select('[data-jump=entry]',entry);
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
      assert.equal(ready.retry.state,'idle','no standby emulator');
      const result={game,kind,stage,phase:Number(phase),loadMs:Math.round(performance.now()-started),loading:ready.loading,frames:ready.native.practice.frames,samples};results.push(result);
      console.log(JSON.stringify({...result,samples:undefined}));
      await page.screenshot({path:resolve(output,`${label}-${game}-${kind}.png`)});
      await frame.$eval('canvas',n=>n.focus());await page.keyboard.press('Enter',{delay:80});
      await frame.waitForFunction(async()=>{const p=(await import('./managed-solo.mjs')).managedAPI.practiceState().native?.practice;return p?.elapsed>0&&!p.waiting;},{timeout:15000}).catch(async error=>{console.error('Resume state',JSON.stringify(await state()));throw error;});
      await page.screenshot({path:resolve(output,`${label}-${game}-${kind}-running.png`)});
    }
    assert.equal(await frame.evaluate(()=>practiceEffectsCreated),1,'restarts reuse the existing original effect driver');
    if(process.env.PC98_PRACTICE_REPLAY==='1'){
      await frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.finishRecording());
      await frame.waitForFunction(game=>[...__eaglerPC98Files.memory.keys()].some(name=>name.endsWith('.'+game+'r')),{timeout:45000},game);
      await frame.evaluate(async game=>{
        const {managedAPI}=await import('./managed-solo.mjs');
        const path=[...__eaglerPC98Files.memory.keys()].findLast(name=>name.endsWith('.'+game+'r'));
        managedAPI.stop();await managedAPI.playback({action:'open',path});
      },game);
      await frame.waitForFunction(async()=>{const value=(await import('./managed-solo.mjs')).managedAPI.health().replayResult;return value!==null;},{timeout:120000});
      assert.equal(await frame.evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.health().replayResult),true,'practice replay reproduces the complete native memory');
      console.log(game,'PASS practice replay memory');
    }
    assert.deepEqual(errors,[]);assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
  }finally{await browser?.close();await new Promise(r=>server.close(r));}
}
await writeFile(resolve(output,label+'.json'),JSON.stringify(results,null,2)+'\n');
