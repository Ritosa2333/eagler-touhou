import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createReadStream,readFileSync,mkdirSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve,sep,extname} from 'node:path';
import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {replayFixture} from './replay-fixture.mjs';
import {nativeReplay,encodeExtended} from '../web/replay.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(argument=>{const i=argument.indexOf('=');return [argument.slice(2,i),argument.slice(i+1)];}));
const runtime=resolve(args.runtime||'ports/thkkd/runtime'),data=resolve(args.data||'ports/thkkd/build/web-content/thkkd.data');
const output=resolve(args.output||'ports/thkkd/build/replay-entry');mkdirSync(output,{recursive:true});
const server=createServer((request,response)=>{
  const pathname=decodeURIComponent(new URL(request.url,'http://localhost').pathname),file=pathname==='/fixture.data'?data:resolve(runtime,'.'+pathname);
  if(file!==data&&!file.startsWith(runtime+sep)){response.writeHead(404);response.end();return;}
  response.setHeader('Cross-Origin-Opener-Policy','same-origin');response.setHeader('Cross-Origin-Embedder-Policy','require-corp');
  response.setHeader('Content-Type',({'.mjs':'text/javascript','.wasm':'application/wasm','.html':'text/html','.woff2':'font/woff2'})[extname(file)]||'application/octet-stream');
  const stream=createReadStream(file);stream.on('error',()=>{response.statusCode=404;response.end();});stream.pipe(response);
});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const browser=await puppeteer.launch({executablePath:process.env.CHROME_PATH||'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,protocolTimeout:300000,args:['--mute-audio'],defaultViewport:{width:960,height:720}}),page=await browser.newPage();
const report={schema:'thkkd/replay-entry/1',build:JSON.parse(readFileSync(resolve(runtime,'manifest.json'),'utf8')),rows:[],checks:[],errors:[]};
page.on('pageerror',error=>report.errors.push(error.message));
try{
  await page.evaluateOnNewDocument(()=>Object.defineProperty(navigator,'getGamepads',{value:()=>[]}));
  await page.goto(`http://127.0.0.1:${server.address().port}/thkkd.html?standalone=1&runtimeEpoch=1&data=/fixture.data`);
  await page.waitForFunction(()=>globalThis.__thkkdRuntime,{timeout:120000});await page.click('canvas');await page.waitForFunction(()=>__thkkdRuntime.status().first,{timeout:30000});
  await page.evaluate(async()=>{
    const r=__thkkdRuntime;await r.stop();const raf=window.requestAnimationFrame;window.requestAnimationFrame=()=>0;
    const w=address=>r.core._thkkd_word(address),check=value=>{if(value<0)throw Error(r.core.UTF8ToString(r.core._thkkd_error()));};
    const step=(count=1)=>{for(let i=0;i<count;i++)check(r.core._thkkd_step());};
    const key=(code,settle=3)=>{r.input.key(code,true,'replay-entry');step(1);r.input.key(code,false,'replay-entry');step(settle);};
    const state=()=>({outer:w(0xa8ce58),gameScene:w(0xa78e2c),eof:w(0xb43c00),phase:w(0xa87f48),choice:w(0xa87f20),selection:w(0xa87f30),part:w(0xa77390),boss:w(0xa7ea50),spell:w(0xa7eba4),difficulty:w(0xa7e610),stage:w(0xb3e2ec),character:w(0xa74c0c),shot:w(0xa7e614),mode:w(0xa7e618),lives:w(0xa8cd7c),bombs:w(0xa8cd80),cursor:w(0xb43110),replayMode:w(0xb4310c)});
    const choose=index=>{for(let i=0;i<10&&w(0xa87f20)!==index;i++)key('ArrowDown');if(w(0xa87f20)!==index)throw Error('Title selection failed: '+JSON.stringify(state()));key('KeyZ',45);};
    async function reset(){await r.stop();await r.command({command:'configure',music:'none',options:{limitPresentationTo60:false}});await r.command({command:'launch'});step(100);if(w(0xa8ce58)!==10)throw Error('Title bootstrap failed: '+JSON.stringify(state()));}
    function stalePractice(){choose(3);key('KeyZ',45);key('KeyZ');key('KeyZ');if(w(0xa87f48)!==20)throw Error('Practice selection failed: '+JSON.stringify(state()));for(let i=0;i<5;i++)key('ArrowDown');if(w(0xa87f30)!==5)throw Error('Practice cursor missing');for(let i=0;i<5&&w(0xa8ce58)!==10;i++)key('KeyX',45);if(w(0xa8ce58)!==10)throw Error('Practice return failed');}
    async function open(bytes,entry,{stale=false,extended=false}={}){
      await reset();if(stale)stalePractice();const before=state();
      const path=entry==='native'?'replay/thkkd00.rpy':'replay/entry.'+(extended?'thkkdr':'rpy');
      await r.command({command:'write',path,bytes});
      if(entry==='native'){choose(4);key('KeyZ',0);for(let i=0;i<3&&w(0xa8ce58)!==100;i++)step();}
      else {await r.command({command:'playback',action:'open',path});await r.command({command:'playback',action:'pause'});}
      const initial=state();if(initial.outer!==100)throw Error('Replay entry failed: '+JSON.stringify(initial));
      return {before,initial};
    }
    window.__entryTest={r,raf,w,step,key,state,open,reset,stalePractice};
  });
  const checkOmni=(row,character,boss)=>{
    assert.equal(row.initial.selection,boss,'恢复录像保存的 Omniverse 选关');assert.equal(row.initial.part,boss);
    assert.equal(row.initial.character,character);assert.equal(row.initial.stage,7);
    if(row.initial.mode===0)assert.equal(row.initial.spell,boss<5?153+boss:158+(boss-5)*3+character);
    assert.equal(row.after.boss,boss);assert.equal(row.after.part,boss);
    assert.equal(row.after.spell,boss<5?153+boss:158+(boss-5)*3+character);
  };
  for(const entry of ['workbench','native'])for(let character=0;character<3;character++)for(let boss=0;boss<8;boss++){
    const bytes=replayFixture({character,shot:character,difficulty:5,stage:7,boss,count:500,bombFrame:-1});
    const row=await page.evaluate(async({bytes,entry})=>{const t=__entryTest,result=await t.open(bytes,entry,{stale:entry==='native'});t.step(220);t.r.platform.renderer.present(0);return {...result,after:t.state()};},{bytes:[...bytes],entry});
    report.rows.push({entry,character,boss,...row});checkOmni(row,character,boss);
    if(entry==='native'){assert.equal(row.initial.mode,1);assert.equal(row.initial.lives,8);assert.equal(row.initial.bombs,8);}
    else {assert.equal(row.initial.mode,0);assert.equal(row.initial.lives,0);assert.equal(row.initial.bombs,0);}
  }
  report.checks.push('both-entries-eight-challenges-three-characters','native-entry-overrides-stale-practice-choice-and-preserves-resources');
  for(const entry of ['workbench','native'])for(const spec of [{difficulty:2,stage:3},{difficulty:3,stage:3,practice:true},{difficulty:4,stage:6}]){
    const row=await page.evaluate(async({bytes,entry})=>{const t=__entryTest,result=await t.open(bytes,entry,{stale:true});t.step(220);return {...result,after:t.state()};},{bytes:[...replayFixture({...spec,boss:7,count:500,bombFrame:-1})],entry});
    assert.equal(row.initial.selection,entry==='native'?5:0);assert.equal(row.initial.part,0);assert.equal(row.initial.stage,spec.practice?3:spec.stage===6?6:0);
    report.rows.push({entry,...spec,...row});
  }
  report.checks.push('normal-practice-extra-entry-regression');
  for(const boss of [8,0xffffffff]){
    const row=await page.evaluate(async bytes=>__entryTest.open(bytes,'native',{stale:true}),[...replayFixture({difficulty:5,stage:7,boss,count:500,bombFrame:-1})]);
    assert.equal(row.initial.selection,5);assert.equal(row.initial.part,5);report.rows.push({entry:'native',invalidBoss:boss,...row});
  }
  report.checks.push('out-of-range-boss-does-not-overwrite-menu-selection');
  const extended=await encodeExtended(replayFixture({difficulty:5,stage:7,boss:6,character:2,count:500,bombFrame:-1}),new Map(),false);
  const extRow=await page.evaluate(async bytes=>{const t=__entryTest,result=await t.open(bytes,'workbench',{extended:true});t.step(220);return {...result,after:t.state()};},[...extended]);checkOmni(extRow,2,6);report.rows.push({entry:'extended',...extRow});
  report.checks.push('extended-replay-uses-native-challenge-metadata');
  if(args.replay){
    const bytes=readFileSync(args.replay),metadata=nativeReplay(new Uint8Array(bytes)).metadata,target=metadata[5]-1;
    const row=await page.evaluate(async bytes=>{const t=__entryTest,result=await t.open(bytes,'native');t.step(220);const after=t.state();t.r.platform.renderer.present(0);return {...result,after};},[...bytes]);checkOmni(row,metadata[6],metadata[4]);
    await page.screenshot({path:resolve(output,'native-challenge.png')});
    const nativeEnd=await page.evaluate(target=>{const t=__entryTest;let steps=0,previous=t.w(0xb43110);while(t.w(0xb43110)<target&&steps++<target+100){t.step();const cursor=t.w(0xb43110);if(cursor!==previous+1)throw Error('Native replay cursor discontinuity');previous=cursor;}return {...t.state(),steps};},target);assert.equal(nativeEnd.cursor,target);
    const nativeStopped=await page.evaluate(()=>{__entryTest.step(60);return __entryTest.state();});
    assert.equal(nativeStopped.cursor,target);assert.equal(nativeStopped.gameScene,1);assert.equal(nativeStopped.eof&1,1);
    await page.evaluate(async bytes=>{const t=__entryTest;await t.open(bytes,'workbench');window.requestAnimationFrame=t.raf;await t.r.command({command:'playback',action:'seek',value:240});},[...bytes]);
    await page.waitForFunction(()=>__entryTest.w(0xb43110)===240);
    await page.evaluate(async()=>__entryTest.r.command({command:'playback',action:'seek',value:0}));
    await page.waitForFunction(()=>__entryTest.w(0xb43110)===0);
    assert.equal(await page.evaluate(()=>__entryTest.w(0xa77390)),metadata[4]);
    await page.evaluate(async()=>{window.__entryPlayback=null;addEventListener('message',event=>{if(event.data?.event==='runtime-info'&&event.data.playback)__entryPlayback=event.data.playback;});await __entryTest.r.command({command:'playback',action:'rate',value:0});await __entryTest.r.command({command:'playback',action:'play'});});
    await page.waitForFunction(target=>__entryPlayback?.ended&&__entryPlayback.frame===target,{timeout:120000},target);
    const end=await page.evaluate(()=>({state:__entryTest.state(),playback:__entryPlayback}));await new Promise(resolve=>setTimeout(resolve,250));
    assert.equal(await page.evaluate(()=>__entryTest.w(0xb43110)),target);assert.equal(await page.$eval('#error',element=>element.textContent),'');
    report.sample={sha256:createHash('sha256').update(bytes).digest('hex'),metadata,entry:row,nativeEnd,nativeStopped,workbenchEnd:end};
    report.checks.push('provided-replay-native-continuous-to-EOF-pause','provided-replay-workbench-seek-restart-and-natural-EOF');
  }
  assert.deepEqual(report.errors,[]);report.result='passed';console.log(JSON.stringify({result:report.result,rows:report.rows.length,checks:report.checks,sample:report.sample?.workbenchEnd.state}));
}catch(error){report.result='failed';report.error=String(error.stack||error);process.exitCode=1;console.error(error);await page.screenshot({path:resolve(output,'failure.png')});}
finally{writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();await new Promise(resolve=>server.close(resolve));}
