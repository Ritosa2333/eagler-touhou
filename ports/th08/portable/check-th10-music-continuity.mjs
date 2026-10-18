import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const base=process.env.TEST_URL??'http://127.0.0.1:8090';
const output=process.env.AUDIO_REPORT??'th10_web/artifacts/sdl3/music-continuity-before.json';
const browser=await launchBrowser();
try {
 const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));
 await page.goto(base+'/runtime/th10/th10.html?manual=1');
 await page.waitForFunction(()=>window.__th10Runtime,null,{timeout:120000});
 await page.locator('canvas').click();
 const report=await page.evaluate(()=>{
  const {core:c,Module}=window.__th10Runtime,alloc=c.graphics_allocate;
  const text=s=>{const p=alloc(s.length+1);new Uint8Array(c.memory.buffer,p,s.length+1).set(new TextEncoder().encode(s+'\0'));return p;};
  const rate=44100,total=rate*16,intro=12348,source=new Int16Array(total*2);
  for(let i=0;i<total;i++){source[i*2]=Math.round(10000*Math.sin(i*.047)+3000*Math.sin(i*.0071));source[i*2+1]=Math.round(9000*Math.cos(i*.023));}
  Module.FS.writeFile('/game/probe.dat',new Uint8Array(source.buffer));
  const fmt=new Uint8Array(104),v=new DataView(fmt.buffer);fmt.set(new TextEncoder().encode('probe.wav'));
  for(const [o,n]of [[16,0],[20,source.byteLength],[24,intro*4],[28,source.byteLength],[36,rate],[40,rate*4]])v.setUint32(o,n,true);
  for(const [o,n]of [[32,1],[34,2],[44,4],[46,16]])v.setUint16(o,n,true);
  Module.FS.writeFile('/game/probe.fmt',fmt);
  const fileName=text('th10.dat'),formatName=text('thbgm.fmt'),musicName=text('probe.dat'),trackName=text('probe.wav'),args=alloc(64),out=alloc(64),pcm=alloc(8192*8);
  const files=c.files_create(),audio=c.audio_create(files),manager=c.audio_manager(audio),u32=p=>new DataView(c.memory.buffer).getUint32(p,true);
  const setup=[c.files_attach(files,fileName),c.audio_initialize(audio,1),c.audio_formats(audio,formatName)];
  if(!setup[0]||setup[1]!==0||setup[2]!==0)throw Error('Audio setup failed: '+JSON.stringify(setup));
  new Uint8Array(c.memory.buffer,u32(manager+0x1f84),fmt.length).set(fmt);
  c.sdl_audio_pause(1);
  const command=(kind)=>{c.audio_queue_music(audio,kind,-1,trackName);for(let i=0;u32(manager+0x1f88);i++){if(i>64)throw Error('Music command stuck');c.audio_update(audio);}};
  const call=(id,op,a=[])=>{new Uint32Array(c.memory.buffer,args,16).set(a);return c.audio_call(id,op,args);};
  // Reference: identical mixer, format and gain, but a complete immutable PCM
  // source. This includes miniaudio's filter delay without assuming its shape.
  const testFrames=Math.ceil(rate*20/1024)*1024,baseline=new Float32Array(testFrames*2),device=c.sdl_audio_device(),format=alloc(18),desc=alloc(36);
  new Uint8Array(c.memory.buffer,format,18).set(fmt.subarray(32,50));
  new Uint32Array(c.memory.buffer,desc,9).set([36,0x80c8,(testFrames+4096)*4,0,format,0,0,0,0]);
  call(device,2,[desc,out,0]);const reference=u32(out);call(reference,14,[0,(testFrames+4096)*4,out,out+4,out+8,out+12,0]);
  const raw=new Int16Array(c.memory.buffer,u32(out),(testFrames+4096)*2);
  for(let i=0;i<testFrames+4096;i++){const frame=i<total?i:intro+(i-total)%(total-intro);raw[i*2]=source[frame*2];raw[i*2+1]=source[frame*2+1];}
  call(reference,10,[0,0,0]);for(let frame=0;frame<testFrames;frame+=1024){c.sdl_audio_render(pcm,1024);baseline.set(new Float32Array(c.memory.buffer,pcm,2048),frame*2);}
  call(reference,11);call(reference,1);call(device,1);
  const scenarios=[];
  for(const scenario of ['steady','producer-ahead','producer-behind','long-gap','pause-resume']){
   if(c.audio_start_file(audio,musicName)!==0)throw Error('Music start failed');command(2);
   call(u32(u32(u32(manager+0x5208)+4)),8,[0]);
   let rendered=0,elapsed=0,mismatches=0,first=null,maxError=0;
   const render=n=>{
    if(c.sdl_audio_render(pcm,n)!==n)throw Error('Incomplete PCM');
    const samples=new Float32Array(c.memory.buffer,pcm,n*2);
    for(let i=0;i<n;i++){const frame=rendered+i;for(let ch=0;ch<2;ch++){
     const expected=baseline[frame*2+ch],actual=samples[i*2+ch],error=Math.abs(actual-expected);maxError=Math.max(maxError,error);
     if(error>1e-6){mismatches++;first??={frame,channel:ch,actual,expected,elapsed};}
    }}rendered+=n;
   };
   render(6144);
   for(let step=0;rendered<rate*20;step++){
    if(scenario==='pause-resume'&&step>0&&step%100===0){command(6);c.audio_advance(audio,1250);command(7);}
    const target=Math.floor((rendered-6144+1024)/rate*1000)+(scenario==='producer-ahead'?400:scenario==='producer-behind'?-400:0);
    if(scenario!=='long-gap'||step%20===0){const ms=Math.max(0,target-elapsed);c.audio_advance(audio,ms);elapsed+=ms;}
    render(1024);
   }
   const stream=u32(manager+0x5208),buffer=u32(u32(stream+4));call(buffer,13,[out,0]);
   scenarios.push({scenario,frames:rendered,mismatches,first,maxError,position:u32(out),writeCursor:u32(stream+0x64)});
   command(4);
  }
  c.audio_destroy(audio);c.files_destroy(files);
  const finalStats=Array.from(new Uint32Array(c.memory.buffer,c.sdl_audio_stats(),12));
  if(finalStats[1]||finalStats[2]||finalStats[8])throw Error('Audio ownership/error check failed: '+finalStats);
  c.sdl_audio_shutdown();
  return {scenarios,remainingBuffers:finalStats[1],remainingEvents:finalStats[2],passed:scenarios.every(s=>s.mismatches===0)};
 });
 const manifest=await(await fetch(base+'/manifest.json')).json();report.wasm=manifest.execution.sha256;report.errors=errors;
 mkdirSync(output.slice(0,output.lastIndexOf('/')),{recursive:true});writeFileSync(output,JSON.stringify(report,null,2));console.log(JSON.stringify(report,null,2));
 assert.deepEqual(errors,[]);assert(report.passed,'Music PCM must remain continuous through ring refills and track looping');
} finally {await browser.close();}
