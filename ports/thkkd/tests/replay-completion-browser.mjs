import assert from 'node:assert/strict';
import {readFileSync,readdirSync,mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {productionBrowser,runtimeCommand,wait} from './production-driver.mjs';
import {nativeReplay,encodeExtended} from '../web/replay.mjs';
import {replayFixture} from './replay-fixture.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(value=>{const at=value.indexOf('=');return [value.slice(2,at),value.slice(at+1)];}));
if(!args.replays)throw Error('需要 --replays=本机原版录像目录');
const output=resolve(args.output||'ports/thkkd/build/replay-completion');mkdirSync(output,{recursive:true});
const report={schema:'thkkd/replay-completion/1',checks:[],cases:[]};
const driver=await productionBrowser({url:args.url||'http://127.0.0.1:18180/',report,protocolTimeout:1200000}),{page,browser}=driver;
try{
  const expectedBuildId=JSON.parse(readFileSync(new URL('../runtime/manifest.json',import.meta.url),'utf8')).buildId;
  await driver.library();const frame=await driver.start({expectedBuildId});
  await page.evaluate(()=>{
    window.__completionPlayback=null;
    addEventListener('message',event=>{const m=event.data;if(event.source===document.querySelector('#gameFrame')?.contentWindow&&m?.event==='runtime-info'&&m.playback)__completionPlayback=m.playback;});
  });
  await frame.evaluate(()=>{
    const r=__thkkdRuntime,step=r.core._thkkd_step;
    window.__completionTrace={active:false,last:0,steps:0,skipped:0,backwards:0};
    r.core._thkkd_step=(...args)=>{const result=step(...args),t=__completionTrace;
      if(t.active&&r.core._thkkd_word(0xb4310c)===1){const cursor=r.core._thkkd_word(0xb43110);if(cursor<t.last)t.backwards++;if(cursor>t.last+1)t.skipped++;t.last=cursor;t.steps++;}return result;};
  });
  const synthetic=replayFixture({count:360,bombFrame:80});
  const extended=await encodeExtended(replayFixture({count:480,bombFrame:100}),new Map([[60,{x:100,y:200}],[120,{x:150,y:220}],[240,{x:200,y:240}]]),true);
  const chosen=args.cases?new Set(args.cases.split(',')):null;
  const original=readdirSync(args.replays).filter(name=>/^corpus-\d+\.rpy$/.test(name)&&(!chosen||chosen.has(name.slice(0,-4)))).map(name=>({id:name.slice(0,-4),bytes:readFileSync(resolve(args.replays,name)),extension:'rpy'})).sort((a,b)=>a.bytes.length-b.bytes.length);
  const cases=[{id:'normal-speed',bytes:synthetic,extension:'rpy',normal:true},{id:'extended-motion',bytes:extended,extension:'thkkdr',count:480},...original];
  for(const item of cases){
    const count=item.count||nativeReplay(new Uint8Array(item.bytes)).count,target=count-1,path='replay/completion.'+item.extension;
    await frame.evaluate(()=>{__completionTrace.active=false;});
    await runtimeCommand(page,'write',{path,bytes:[...item.bytes]});
    await frame.evaluate(()=>Object.assign(__completionTrace,{active:true,last:0,steps:0,skipped:0,backwards:0}));
    await page.evaluate(()=>{__completionPlayback=null;});
    const started=Date.now();await runtimeCommand(page,'playback',{action:'open',path});
    await runtimeCommand(page,'playback',{action:'rate',value:item.normal?1:0});
    if(item.normal){
      await wait(550);await runtimeCommand(page,'playback',{action:'pause'});
      const cursor=await frame.evaluate(()=>__thkkdRuntime.core._thkkd_word(0xb43110));assert.ok(cursor>0&&cursor<target);
      await wait(350);assert.equal(await frame.evaluate(()=>__thkkdRuntime.core._thkkd_word(0xb43110)),cursor);
      await runtimeCommand(page,'playback',{action:'play'});report.checks.push('normal-speed-pause-and-resume');
    }
    console.log(JSON.stringify({id:item.id,status:'playing',frames:count,rate:item.normal?1:0}));
    let last=0,lastProgress=Date.now(),lastPrint=Date.now();
    while(Date.now()-started<1200000){
      const status=await frame.evaluate(()=>({cursor:__thkkdRuntime.core._thkkd_word(0xb43110),error:document.querySelector('#error').textContent}));
      assert.equal(status.error,'');assert.ok(status.cursor>=last,'播放游标持续前进');
      if(status.cursor>last){last=status.cursor;lastProgress=Date.now();}
      if(status.cursor===target)break;
      assert.ok(Date.now()-lastProgress<30000,'录像游标持续推进');
      if(Date.now()-lastPrint>30000){console.log(JSON.stringify({id:item.id,cursor:status.cursor,total:target}));lastPrint=Date.now();}
      await wait(500);
    }
    assert.equal(last,target,'实际播放到最后输入');
    await page.waitForFunction(target=>__completionPlayback?.ended&&__completionPlayback.frame===target,{timeout:10000},target);
    const ending=await frame.evaluate(()=>({...__completionTrace,cursor:__thkkdRuntime.core._thkkd_word(0xb43110)}));
    await wait(250);const settled=await frame.evaluate(()=>({...__completionTrace,cursor:__thkkdRuntime.core._thkkd_word(0xb43110)}));
    assert.equal(settled.cursor,target);assert.equal(settled.steps,ending.steps);assert.equal(settled.skipped,0);assert.equal(settled.backwards,0);
    assert.equal(settled.steps,target,'每个原生输入连续执行一次');
    const row={id:item.id,inputSha256:createHash('sha256').update(item.bytes).digest('hex'),count,finalCursor:target,steps:settled.steps,rate:item.normal?1:0,seconds:(Date.now()-started)/1000,ended:true,stoppedAtEnd:true,continuous:true};
    report.cases.push(row);writeFileSync(resolve(output,'progress.json'),JSON.stringify(report,null,2));console.log(JSON.stringify(row));
  }
  report.checks.push('original-and-extended-play-to-last-input','native-cursor-continuity','launcher-reports-ended','no-steps-after-end');
  assert.equal(report.errors.length,0);report.result='passed';await page.screenshot({path:resolve(output,'ended.png')});
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);await page.screenshot({path:resolve(output,'failure.png')});}
finally{await driver.collect();writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();}
