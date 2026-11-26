import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {productionBrowser,nativeState,runtimeCommand,wait} from './production-driver.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(argument=>{const at=argument.indexOf('=');if(!argument.startsWith('--')||at<0)throw Error('参数格式为 --名称=值');return [argument.slice(2,at),argument.slice(at+1)];}));
if(!args.replay)throw Error('需要 --replay=已有授权录像文件');
const output=resolve(args.output||fileURLToPath(new URL('../build/cpu-release-r1/performance',import.meta.url)));
mkdirSync(output,{recursive:true});
const replay=readFileSync(args.replay),target=Number(args.target||50000),steps=Number(args.steps||900);
assert.ok(Number.isSafeInteger(target)&&target>=0&&Number.isSafeInteger(steps)&&steps>=120);
const report={schema:'thkkd/cpu-performance/1',date:'2026-11-26',display:'60 Hz presentation limit',viewport:{width:1280,height:900},input:{sha256:createHash('sha256').update(replay).digest('hex'),bytes:replay.length,target},stepsPerSample:steps,cases:[],calibration:[]};
const driver=await productionBrowser({url:args.url||'http://127.0.0.1:18174/',report,protocolTimeout:700000}),{browser,page,key}=driver;
let frame;
async function calibrate(rate){
 await page.emulateCPUThrottling(rate);
 const result=await frame.evaluate(()=>{
  const samples=[];let check=0;
  for(let run=0;run<6;run++){let hash=1234567;const start=performance.now();for(let i=0;i<12000000;i++)hash=(Math.imul(hash^i,1664525)+1013904223)|0;const ms=performance.now()-start;check=hash;if(run>1)samples.push(ms);}
  samples.sort((a,b)=>a-b);return {iterations:12000000,samples,medianMs:(samples[1]+samples[2])/2,check};
 });
 report.calibration.push({rate,...result});await wait(1000);
}
async function arm({scene,rate,startCursor=null}){
 await frame.evaluate(({scene,rate,startCursor,steps})=>{
  const r=__thkkdRuntime,w=address=>r.core._thkkd_word(address);
  function state(){
   let bullets=0,hash=2166136261;const word=value=>{for(let i=0;i<4;i++){hash^=(value>>>(i*8))&255;hash=Math.imul(hash,16777619)>>>0;}};
   for(const address of [0xa74c08,0xb3e2ec,0xb3e2e8,0xa77390,0xa733c0,0xa78e2c])word(w(address));
   for(let i=0;i<60;i++)word(w(0xa8cd68+i*4));
   for(let i=0;i<80;i++){const base=0xa87fe8+i*248;if(!w(base))continue;word(i);for(const offset of [0,4,8,12,20,36,56,60,64,68,72,76,80,84])word(w(base+offset));}
   for(let i=0;i<3000;i++){const base=0xa8e660+i*240;if(!w(base))continue;bullets++;word(i);for(const offset of [0,4,8,12,16,64,68,72,76,80,84,88,92])word(w(base+offset));}
   return {cursor:w(0xb43110),stage:w(0xb3e2ec),frame:w(0xb3e2e8),part:w(0xa77390),subphase:w(0xa733c0),scene:w(0xa78e2c),dialogue:w(0xa73060),random:w(0xa74c08),bullets,commands:r.platform.renderer.commands.length,stateDigest:hash.toString(16).padStart(8,'0')};
  }
  if(!window.__cpuObserver){
   const step=r.core._thkkd_step,present=r.platform.renderer.present.bind(r.platform.renderer);
   r.core._thkkd_step=(...args)=>{
    const began=performance.now(),result=step(...args),finished=performance.now(),m=window.__cpuSample;
    if(m&&!m.done){
     const cursor=w(0xb43110);
     if(m.started){
      m.stepMs.push(finished-began);m.stepTimes.push(finished);m.count++;
      if(m.count===m.steps){m.end=finished;m.done=true;m.after=state();}
     }else if(m.startCursor===null||cursor===m.startCursor){m.before=state();m.start=performance.now();m.started=true;}
     else if(m.startCursor!==null&&cursor>m.startCursor)throw Error('性能起点游标已越过');
    }
    return result;
   };
   r.platform.renderer.present=(...args)=>{const began=performance.now(),result=present(...args),finished=performance.now(),m=window.__cpuSample;if(m?.started&&!m.done){m.presentTimes.push(finished);m.presentMs.push(finished-began);}return result;};
   window.__cpuObserver=true;
  }
  window.__cpuSample={scene,rate,startCursor,steps,started:false,done:false,count:0,stepMs:[],stepTimes:[],presentTimes:[],presentMs:[]};
 },{scene,rate,startCursor,steps});
}
async function collect(){
 await frame.waitForFunction(()=>window.__cpuSample?.done,{timeout:240000,polling:200});
 return frame.evaluate(()=>{
  const m=__cpuSample,seconds=(m.end-m.start)/1000;
  const stats=values=>{const sorted=[...values].sort((a,b)=>a-b),q=fraction=>sorted[Math.floor((sorted.length-1)*fraction)]??0;return {count:values.length,mean:values.reduce((a,b)=>a+b,0)/Math.max(1,values.length),p50:q(.5),p95:q(.95),p99:q(.99),max:sorted.at(-1)??0};};
  const intervals=m.presentTimes.slice(1).map((time,index)=>time-m.presentTimes[index]);
  return {scene:m.scene,cpuRate:m.rate,seconds,logicalSteps:m.count,logicHz:m.count/seconds,presentations:m.presentTimes.length,presentHz:m.presentTimes.length/seconds,consumedInputFrames:m.after.cursor-m.before.cursor,inputHz:(m.after.cursor-m.before.cursor)/seconds,stepMs:stats(m.stepMs),presentCallMs:stats(m.presentMs),presentIntervalMs:stats(intervals),before:m.before,after:m.after,hudFPS:document.querySelector('#fps')?.textContent};
 });
}
async function sample(options,trigger){await arm(options);if(trigger)await trigger();const result=await collect();assert.equal(result.logicalSteps,steps);assert.ok(result.logicHz>0&&result.presentHz>0);report.cases.push(result);writeFileSync(output+'/progress.json',JSON.stringify({cases:report.cases,calibration:report.calibration},null,2));console.log(JSON.stringify({scene:result.scene,cpuRate:result.cpuRate,presentHz:result.presentHz,logicHz:result.logicHz,stepMs:result.stepMs.mean,before:result.before.cursor,after:result.after.cursor}));return result;}
async function restart(){
 await page.keyboard.up('z');await key('Escape');await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xa78e2c)===1);await key('r',50,0);await frame.waitForFunction(()=>{const w=address=>__thkkdRuntime.core._thkkd_word(address);return w(0xa78e2c)===0&&w(0xb43110)<100;});await page.keyboard.down('z');
}
try{
 await driver.library();frame=await driver.start();report.hardware=await frame.evaluate(()=>{const r=__thkkdRuntime,gl=r.platform.renderer.gl,extension=gl.getExtension('WEBGL_debug_renderer_info');return {userAgent:navigator.userAgent,cores:navigator.hardwareConcurrency,renderer:extension?gl.getParameter(extension.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER),canvas:{width:gl.canvas.width,height:gl.canvas.height},wasmHeapBytes:r.core.HEAPU8.byteLength};});
 for(const rate of [1,4]){await calibrate(rate);await sample({scene:'title',rate});}
 const [normal,slow]=report.calibration;assert.equal(normal.check,slow.check);report.calibrationRatio=slow.medianMs/normal.medianMs;assert.ok(report.calibrationRatio>1.5,'实际 iframe 工作量应观察到 CPU 限速');
 await page.emulateCPUThrottling(1);await frame.click('canvas');for(let count=0;count<8&&(await nativeState(frame)).outer!==100;count++)await key('z');assert.equal((await nativeState(frame)).outer,100);
 for(const rate of [1,4]){await page.emulateCPUThrottling(1);await restart();await page.emulateCPUThrottling(rate);await sample({scene:'live-stage-1',rate,startCursor:120});assert.equal(report.cases.at(-1).after.scene,0);await page.keyboard.up('z');}
 await page.emulateCPUThrottling(1);await runtimeCommand(page,'write',{path:'replay/cpu-benchmark.rpy',bytes:[...replay]});
 for(const rate of [1,4]){
  await page.emulateCPUThrottling(1);await runtimeCommand(page,'playback',{action:'open',path:'replay/cpu-benchmark.rpy'});await runtimeCommand(page,'playback',{action:'pause'});await runtimeCommand(page,'playback',{action:'seek',value:target});
  await frame.waitForFunction(target=>__thkkdRuntime.core._thkkd_word(0xb43110)===target,{timeout:650000,polling:500},target);
  await page.emulateCPUThrottling(rate);const result=await sample({scene:'EX-dense-replay',rate,startCursor:target+120},()=>runtimeCommand(page,'playback',{action:'play'}));
  await runtimeCommand(page,'playback',{action:'pause'});assert.equal(result.after.scene,0);assert.equal(result.consumedInputFrames,steps);assert.ok(result.before.bullets>=200,'使用密集弹幕片段');await page.screenshot({path:output+`/dense-cpu-${rate}.png`});
 }
 const dense=report.cases.filter(item=>item.scene==='EX-dense-replay');assert.equal(dense[0].before.stateDigest,dense[1].before.stateDigest);assert.equal(dense[0].after.stateDigest,dense[1].after.stateDigest);report.sameReplaySceneDigest=true;
 assert.equal(report.errors.length,0);const telemetry=await driver.collect();report.runtimeErrors=telemetry.filter(event=>event.event==='error'||event.event==='audio-health'&&['blocked','error'].includes(event.state));assert.equal(report.runtimeErrors.length,0);report.result='measured';
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);await page.screenshot({path:output+'/failure.png'}).catch(()=>{});}
finally{await page.emulateCPUThrottling(1).catch(()=>{});await driver.collect();writeFileSync(output+'/result.json',JSON.stringify(report,null,2));await browser.close();}
