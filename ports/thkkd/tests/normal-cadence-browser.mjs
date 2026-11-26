import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createReadStream,mkdirSync,writeFileSync} from 'node:fs';
import {resolve,sep,extname} from 'node:path';
import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {replayFixture} from './replay-fixture.mjs';
const args=Object.fromEntries(process.argv.slice(2).map(argument=>{const i=argument.indexOf('=');return [argument.slice(2,i),argument.slice(i+1)];}));
const runtime=resolve('ports/thkkd/runtime'),data=resolve(args.data||'ports/thkkd/build/web-content/thkkd.data'),output=resolve(args.output||'ports/thkkd/build/high-refresh-20261127/normal');mkdirSync(output,{recursive:true});
const server=createServer((request,response)=>{
  const pathname=decodeURIComponent(new URL(request.url,'http://localhost').pathname),file=pathname==='/fixture.data'?data:resolve(runtime,'.'+pathname);
  if(file!==data&&!file.startsWith(runtime+sep)){response.writeHead(404);response.end();return;}
  response.setHeader('Cross-Origin-Opener-Policy','same-origin');response.setHeader('Cross-Origin-Embedder-Policy','require-corp');
  response.setHeader('Content-Type',({'.mjs':'text/javascript','.wasm':'application/wasm','.html':'text/html','.woff2':'font/woff2'})[extname(file)]||'application/octet-stream');
  const stream=createReadStream(file);stream.on('error',()=>{response.statusCode=404;response.end();});stream.pipe(response);
});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const browser=await puppeteer.launch({executablePath:process.env.CHROME_PATH||'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,protocolTimeout:300000,args:['--mute-audio'],defaultViewport:{width:960,height:720}}),page=await browser.newPage();
const report={schema:'thkkd/normal-cadence/1',checks:[],errors:[]};page.on('pageerror',error=>report.errors.push(error.message));
try{
  await page.evaluateOnNewDocument(()=>Object.defineProperty(navigator,'getGamepads',{value:()=>[]}));
  await page.goto(`http://127.0.0.1:${server.address().port}/thkkd.html?standalone=1&runtimeEpoch=1&data=/fixture.data`);await page.waitForFunction(()=>globalThis.__thkkdRuntime,{timeout:120000});await page.click('canvas');await page.waitForFunction(()=>__thkkdRuntime.status().first,{timeout:30000});
  report.rows=await page.evaluate(async()=>{
    const r=__thkkdRuntime,raf=window.requestAnimationFrame,cancel=window.cancelAnimationFrame,nativeStep=r.core._thkkd_step,present=r.platform.renderer.present.bind(r.platform.renderer),queue=new Map(),rows=[];let serial=0,steps=0,presents=0;
    await r.stop();window.requestAnimationFrame=callback=>{const id=--serial;queue.set(id,callback);return id;};window.cancelAnimationFrame=id=>id<0?queue.delete(id):cancel.call(window,id);
    const signature=()=>{let hash=2166136261;const add=value=>{hash=Math.imul(hash^value,16777619)>>>0;},w=address=>r.core._thkkd_word(address);for(const address of [0xa74c08,0xb3e2e8,0xb43110,0xa78e2c,0xa8ce58])add(w(address));for(const [base,count,stride]of [[0xa8cd68,1,240],[0xa87fe8,80,248],[0xa8e660,3000,240]])for(let i=0;i<count;i++)for(let j=0;j<stride;j+=4)add(w(base+i*stride+j));return hash.toString(16);};
    try{
      for(const hz of [60,75,120,144,165,240,360])for(const limited of [true,false]){
        queue.clear();r.platform.seed=0x6d2b79f5;
        await r.command({command:'configure',music:'none',options:{limitPresentationTo60:limited,touchEnabled:false}});await r.command({command:'launch'});
        for(let attempt=0;attempt<20&&r.core._thkkd_word(0xa8ce58)!==100;attempt++){r.input.key('KeyZ',true,'cadence');for(let i=0;i<5;i++)if(nativeStep()<0)throw Error(r.core.UTF8ToString(r.core._thkkd_error()));r.input.key('KeyZ',false,'cadence');for(let i=0;i<30;i++)if(nativeStep()<0)throw Error(r.core.UTF8ToString(r.core._thkkd_error()));}
        if(r.core._thkkd_word(0xa8ce58)!==100)throw Error('Normal menu bootstrap failed: '+r.core._thkkd_word(0xa8ce58));
        steps=presents=0;r.core._thkkd_step=()=>{r.input.key('KeyZ',true,'cadence');r.input.key('ArrowRight',Math.floor(steps/30)%2===0,'cadence');r.input.key('ArrowLeft',Math.floor(steps/30)%2!==0,'cadence');steps++;return nativeStep();};r.platform.renderer.present=(...args)=>{presents++;return present(...args);};
        for(let tick=0;tick<hz*3;tick++){const callbacks=[...queue.values()];queue.clear();for(const callback of callbacks)callback(tick*1000/hz);}
        rows.push({hz,limited,steps,presents,signature:signature(),cursor:r.core._thkkd_word(0xb43110)});
        r.core._thkkd_step=nativeStep;r.platform.renderer.present=present;await r.stop();
      }
    }finally{r.core._thkkd_step=nativeStep;r.platform.renderer.present=present;window.requestAnimationFrame=raf;window.cancelAnimationFrame=cancel;}
    await r.command({command:'configure',music:'none',options:{limitPresentationTo60:false,touchEnabled:false}});await r.command({command:'launch'});return rows;
  });
  for(const row of report.rows){assert.equal(row.steps,180);assert.equal(row.presents,row.limited?180:row.hz*3);assert.equal(row.signature,report.rows[0].signature);assert.equal(row.cursor,report.rows[0].cursor);}
  report.checks.push('production-normal-loop-60-to-360-Hz-same-input-and-native-state','60-Hz-limit-changes-presentation-only');
  await page.evaluate(async bytes=>{const r=__thkkdRuntime;await r.command({command:'write',path:'replay/no-music.rpy',bytes});await r.command({command:'playback',action:'open',path:'replay/no-music.rpy'});await r.command({command:'playback',action:'pause'});await r.command({command:'playback',action:'seek',value:179});},[...replayFixture({count:180,bombFrame:-1})]);
  await page.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)===179);assert.equal(await page.$eval('#error',element=>element.textContent),'');report.checks.push('no-BGM-Replay-seek-and-EOF');assert.deepEqual(report.errors,[]);report.result='passed';console.log(JSON.stringify(report));
}catch(error){report.result='failed';report.error=String(error.stack||error);process.exitCode=1;console.error(error);await page.screenshot({path:resolve(output,'failure.png')});}
finally{writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();await new Promise(resolve=>server.close(resolve));}
