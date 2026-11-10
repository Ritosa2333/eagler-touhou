import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {launchBrowser} from '../../../../tools/browser/launch.mjs';

const required=name=>{assert.ok(process.env[name],'Set '+name);return process.env[name];};
const runtime=resolve(required('EAGLER_TH17_BUILD'));
const shared=resolve(required('EAGLER_TH17_SHARED'));
const music=resolve(required('EAGLER_TH17_MUSIC'));
const data=await readFile(required('EAGLER_TH17_DATA'));
const replay=await readFile(required('EAGLER_TH17_REPLAY'));
const output=resolve(required('TH17_STORY_EVIDENCE'));
const stages=(process.env.TH17_STORY_STAGES||'1,3,4,5,6').split(',').map(Number);
const windowFrames=Number(process.env.TH17_STORY_FRAMES||900);
const cpuThrottle=Number(process.env.TH17_STORY_CPU_THROTTLE||1);
const glTrace=process.argv.includes('--gl-trace');
const cpuProfile=process.argv.includes('--cpu-profile');
const scale=Number(process.env.TH17_STORY_RENDER_SCALE||2);
assert.ok(stages.every(stage=>Number.isInteger(stage)&&stage>=1&&stage<=6));
assert.ok(scale===1||scale===2);
const resources=['/msgothic.ttc','/unifont.otf'].map(path=>({path,url:path}));
const tracks=['th17_01',...new Set(stages.flatMap(stage=>
 [`th17_${String(stage*2+1).padStart(2,'0')}`,`th17_${String(stage*2+2).padStart(2,'0')}`]))];
const ogg=tracks.map(name=>({path:'/music/'+name+'.ogg',url:'/music/'+name+'.ogg'}));
const assets=new Map([['/game.dat',data],['/replay.rpy',replay]]);
for(const {path} of [...resources,...ogg])
 assets.set(path,await readFile(resolve(path.startsWith('/music/')?music:shared,path.split('/').at(-1))));
const manifest=JSON.parse(await readFile(resolve(runtime,'manifest.json')));
const html=`<!doctype html><button id="start">Start</button>
 <iframe style="width:1280px;height:960px;border:0" src="/runtime/th17.html?managedData=1&gameGeneration=test&runtimeEpoch=1"></iframe>
 <script>
 window.events=[];
 window.addEventListener('message',event=>{if(event.source===document.querySelector('iframe').contentWindow&&event.origin===location.origin)events.push(event.data)});
 window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/game.dat')).arrayBuffer()});
 document.querySelector('button').onclick=()=>{window.__touhouAudioContext=new AudioContext();void __touhouAudioContext.resume();};
 </script>`;
const mime={'.mjs':'text/javascript','.wasm':'application/wasm','.html':'text/html','.json':'application/json','.css':'text/css','.ogg':'audio/ogg'};
const server=createServer(async(req,res)=>{
 try{
  const path=new URL(req.url,'http://localhost').pathname;
  res.setHeader('Cross-Origin-Opener-Policy','same-origin');
  res.setHeader('Cross-Origin-Embedder-Policy','require-corp');
  if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
  res.setHeader('Content-Type',mime[extname(path)]||'application/octet-stream');
  if(assets.has(path)){res.end(assets.get(path));return;}
  const file=resolve(runtime,path.slice('/runtime/'.length));
  if(!path.startsWith('/runtime/')||!file.startsWith(runtime+sep)){res.writeHead(404).end();return;}
  res.end(await readFile(file));
 }catch(error){res.writeHead(404).end(String(error));}
});
await new Promise(yes=>server.listen(0,'127.0.0.1',yes));
let browser;
try{
 browser=await launchBrowser();
 const results=[];
 for(const stage of stages){
  const page=await browser.newPage({viewport:{width:1320,height:1030}});
  try{
   const errors=[];page.on('pageerror',error=>errors.push(String(error)));
   const cdp=await page.context().newCDPSession(page);
   await cdp.send('Emulation.setCPUThrottlingRate',{rate:cpuThrottle});
   await page.goto('http://127.0.0.1:'+server.address().port);
   await page.click('#start');
   await page.waitForFunction(()=>events.some(event=>event.event==='ready'),null,{timeout:90000});
   const frame=page.frames().find(frame=>frame.url().includes('/runtime/'));
   await frame.evaluate(async({stage,windowFrames,glTrace})=>{
    const core=Module;
    const open=core._th17_open_replay;
    core._th17_open_replay=(path)=>open(path,stage);
    core.FS.writeFile('/save/replay/th17_ud0001.rpy',new Uint8Array(await(await fetch('/replay.rpy')).arrayBuffer()));
    const submit=core.onGameFrame;
    const video=document.createElement('video');
    video.muted=true;video.playsInline=true;
    video.style.cssText='position:fixed;top:0;left:0;width:2px;height:2px;pointer-events:none';
    video.srcObject=core.canvas.captureStream();document.body.append(video);
    void video.play();
    let captureStart,lastCapture,measuring=false;
    const captureGaps=[];
    const observe=(_,metadata)=>{
     if(measuring){
      captureStart??=metadata;
      if(lastCapture&&metadata.presentedFrames===lastCapture.presentedFrames+1)
       captureGaps.push(metadata.presentationTime-lastCapture.presentationTime);
      lastCapture=metadata;
     }
     if(!globalThis.storyResult)video.requestVideoFrameCallback(observe);
    };
    video.requestVideoFrameCallback(observe);
    const calls={};
    let begin=0,firstFrame,previous,commits=0;
    const callbacks=[],gaps=[];
    core.onGameFrame=(ok,ms,ticks,presented)=>{
     submit(ok,ms,ticks,presented);
     if(!ok||core._th17_phase()!==1)return;
     const now=performance.now(),frame=core._th17_frame();
     if(frame<300)return;
     if(!begin){
      begin=now;firstFrame=frame;measuring=true;
      if(glTrace){
       const gl=core.canvas.getContext('webgl2');
       for(const name of ['drawArrays','drawElements','drawArraysInstanced','drawElementsInstanced','bufferData','blitFramebuffer','clear','bindFramebuffer','bindTexture']){
        const original=gl[name];
        gl[name]=function(...args){calls[name]=(calls[name]||0)+1;return original.apply(this,args);};
       }
      }
     }
     callbacks.push(ms);
     if(presented){if(previous)gaps.push(now-previous);previous=now;++commits;}
     if(frame-firstFrame<windowFrames)return;
     const stats=values=>{
      values.sort((a,b)=>a-b);
      return {p50:values[Math.floor(values.length*.5)],p95:values[Math.floor(values.length*.95)],max:values.at(-1),over25:values.filter(ms=>ms>25).length};
     };
     const elapsed=now-begin,info=core.canvas.getContext('webgl2');
     const adapter=info.getExtension('WEBGL_debug_renderer_info');
     globalThis.storyResult={stage,frameCount:frame-firstFrame,canvas:[core.canvas.width,core.canvas.height],
      renderer:adapter?info.getParameter(adapter.UNMASKED_RENDERER_WEBGL):null,submittedFps:(commits-1)*1000/elapsed,
      canvasFps:lastCapture&&captureStart?(lastCapture.presentedFrames-captureStart.presentedFrames)*1000/
       (lastCapture.presentationTime-captureStart.presentationTime):null,
      callbacks:stats(callbacks),submitGaps:stats(gaps),canvasGaps:stats(captureGaps),...(glTrace?{calls}:{})};
     measuring=false;core.onGameFrame=submit;
     for(const track of video.srcObject.getTracks())track.stop();
    };
   },{stage,windowFrames,glTrace});
   async function command(command,options={}){
    const request='story-'+command;
    await page.evaluate(message=>document.querySelector('iframe').contentWindow.postMessage(message,location.origin),
     {protocol:'eagler-touhou/1',game:'th17',epoch:1,request,command,...options});
    await page.waitForFunction(request=>events.some(event=>event.request===request),request,{timeout:30000});
    const reply=await page.evaluate(request=>events.find(event=>event.request===request),request);
    assert.equal(reply.ok,true,JSON.stringify(reply));
   }
   await command('configure',{music:'ogg',sharedResources:resources,resources:ogg,options:{replayPath:'replay/th17_ud0001.rpy',renderScale:scale}});
   await command('launch');
   if(cpuProfile){
    await cdp.send('Profiler.enable');
    await cdp.send('Profiler.setSamplingInterval',{interval:1000});
    await cdp.send('Profiler.start');
   }
   await frame.waitForFunction(()=>globalThis.storyResult,null,{timeout:120000});
   const result=await frame.evaluate(()=>storyResult);
   assert.deepEqual(result.canvas,[640*scale,480*scale]);
   assert.ok(result.canvasFps>0);
   assert.deepEqual(errors,[]);
   const gpu=(await page.evaluate(()=>events.filter(event=>event.event==='frame-health'&&Number.isFinite(event.gpuMs)))).map(event=>event.gpuMs);
   results.push({...result,gpuMs:gpu});
   console.log(JSON.stringify(results.at(-1)));
   await mkdir(output,{recursive:true});
   if(cpuProfile){
    const {profile}=await cdp.send('Profiler.stop');
    await writeFile(resolve(output,'stage-'+stage+'.cpuprofile'),JSON.stringify(profile));
   }
   await writeFile(resolve(output,'story-report.json'),JSON.stringify({version:manifest.version,wasm:manifest.execution.sha256,cpuThrottle,glTrace,results},null,2)+'\n');
  }finally{await page.close();}
 }
}finally{await browser?.close();await new Promise(done=>server.close(done));}
