// Actual retail stage6 + ordinary THPrac runtime. No synthetic bullet pattern.
import {createServer} from 'node:http';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {execFileSync} from 'node:child_process';
import {launchBrowser} from '../browser/launch.mjs';
const root=resolve(import.meta.dirname,'../../ports/th12'),output=resolve(root,'../../artifacts/th12-byakuren');
await mkdir(output,{recursive:true});
const label=process.env.TH12_PROFILE_LABEL??'current';
const baseline=process.env.TH12_PROFILE_BASELINE==='1',locked=process.env.TH12_PROFILE_LOCKED==='1',profileEnabled=process.env.TH12_PROFILE_CPU==='1';
const cpuRate=Number(process.env.TH12_PROFILE_CPU_RATE??1);
const stage=Number(process.env.TH12_PROFILE_STAGE??6),section=Number(process.env.TH12_PROFILE_SECTION??45),difficulty=stage===7?4:Number(process.env.TH12_PROFILE_DIFFICULTY??0);
const loadout=Number(process.env.TH12_PROFILE_LOADOUT??5),bomb=process.env.TH12_PROFILE_BOMB==='1',frames=Number(process.env.TH12_PROFILE_FRAMES??1800),jsonOnly=process.env.TH12_PROFILE_JSON==='1';
const verify=process.env.TH12_PROFILE_VERIFY==='1';
const cadence=Number(process.env.TH12_PROFILE_CADENCE??120);
if(![6,7].includes(stage)||!Number.isInteger(section)||!Number.isInteger(loadout)||loadout<0||loadout>5||!Number.isInteger(frames)||frames<300||frames>3600)throw Error('Invalid scenario');
if(!Number.isInteger(cpuRate)||cpuRate<1||cpuRate>8)throw Error('Invalid CPU rate');
const baselineSources=new Map(baseline?['renderer.mjs','presentation-interpolation.mjs'].map(name=>['/src/'+name,execFileSync('git',['show','HEAD:ports/th12/src/'+name],{cwd:resolve(root,'../..')})]):[]);
if(!/^[a-z0-9-]+$/.test(label))throw Error('Invalid profile label');
const html=`<!doctype html><meta charset="utf-8"><style>body{margin:0;background:#080808}canvas{width:640px;height:480px}</style><canvas width="640" height="480"></canvas><script type="module">
import create from '/site/runtime/core.mjs';import {Renderer} from '/src/renderer.mjs';
import {StageLoader} from '/src/stage-loader.mjs';import {PracticeBridge} from '/src/practice/bridge.mjs';import {readCorePresentationMotion} from '/src/presentation-interpolation.mjs';import {needsPresentationInterpolation} from '/src/presentation-cadence.mjs';window.needsInterpolation=needsPresentationInterpolation;
window.c=await create();${jsonOnly?'delete c._th12_presentation_motion_binary;':''}const atlases=await (await fetch('/assets/atlases.json')).json();
window.r=new Renderer(document.querySelector('canvas'),atlases);
await r.prepare();await r.prepareStage(${stage});
const loader=new StageLoader(c,atlases,async name=>new Uint8Array(await (await fetch('/assets/'+name)).arrayBuffer()));await loader.load(${stage});
window.bridge=new PracticeBridge(c);bridge.setEnabled(true);bridge.configure({stage:${stage-1},section:${section},power:400},{difficulty:${difficulty}});
bridge.start({character:${loadout>>1},shot:${loadout%2},difficulty:${difficulty},seed:0x1234});bridge.setTools(1|256|4);
window.json=name=>{const p=c['_th12_'+name](),n=c['_th12_'+name+'_size']();return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(p,p+n)));};
window.verifyMotion=()=>{
  const before=JSON.stringify(json('state')),binary=readCorePresentationMotion(c),legacy=Object.create(c);legacy._th12_presentation_motion_binary=undefined;
  const old=readCorePresentationMotion(legacy);
  const compare=(a,b,path)=>{
    if(typeof a==='number'){if(a!==b&&Math.fround(a)!==b)throw Error('Motion mismatch '+path+': '+a+' != '+b);return;}
    if(a&&typeof a==='object'){
      if(a instanceof Map)return;
      if(Array.isArray(a)&&a.length!==b?.length)throw Error('Motion length '+path);
      for(const key of Object.keys(a))compare(a[key],b?.[key],path+'.'+key);
    }else if(a!==b)throw Error('Motion mismatch '+path);
  };
  compare(old,binary,'motion');if(before!==JSON.stringify(json('state')))throw Error('Presentation export mutated game state');
};
window.ready=true;
</script>`;
const server=createServer(async(req,res)=>{
  try{
    const url=new URL(req.url,'http://localhost');
    if(url.pathname==='/'){res.setHeader('Content-Type','text/html;charset=utf-8');res.end(html);return;}
    if(baselineSources.has(url.pathname)){res.setHeader('Content-Type','text/javascript');res.end(baselineSources.get(url.pathname));return;}
    const relative=url.pathname.startsWith('/assets/')?'/site'+url.pathname:url.pathname;
    const file=resolve(root,'.'+decodeURIComponent(relative));if(!file.startsWith(root+sep))throw Error('Invalid path');
    res.setHeader('Content-Type',({'.mjs':'text/javascript','.wasm':'application/wasm','.json':'application/json','.png':'image/png'})[extname(file)]??'application/octet-stream');res.end(await readFile(file));
  }catch(e){res.statusCode=404;res.end(e.message);}
});
await new Promise(done=>server.listen(0,'127.0.0.1',done));
const browser=await launchBrowser(),page=await browser.newPage({viewport:{width:640,height:480}}),errors=[];
page.on('pageerror',e=>errors.push(e.message));
try{
  await page.goto('http://127.0.0.1:'+server.address().port);await page.waitForFunction(()=>window.ready,undefined,{timeout:120000});
  const cdp=await page.context().newCDPSession(page);await cdp.send('Emulation.setCPUThrottlingRate',{rate:cpuRate});
  if(profileEnabled){await cdp.send('Profiler.enable');await cdp.send('Profiler.start');}
  const report=await page.evaluate(async({baseline,locked,bomb,frames,verify,cadence})=>{
    const rows=[],stats={},windows={normal:[],bomb:[]};let maxBullets=0,previous=null;
    const run=(name,fn)=>{const t=performance.now(),result=fn();(stats[name]??=[]).push(performance.now()-t);return result;};
    for(let i=0;i<frames;i++){
      const frameStart=performance.now(),useBomb=bomb&&(i===300||i===900);
      run('tick',()=>c._th12_tick(bomb?1|(useBomb?2:0):0,useBomb?2:0));
      const interpolate=!locked&&(baseline||needsInterpolation(1000/cadence));
      const current=run('capture',()=>r.capturePresentation(c,{alwaysHitbox:true,...(!baseline&&!interpolate?{presentationContinuity:false}:{})}));
      run('render',()=>r.renderPresentation(previous,current,interpolate?.5:1));previous=current;
      if(i>=120)windows[bomb&&(i>=300&&i<450||i>=900&&i<1050)?'bomb':'normal'].push(performance.now()-frameStart);
      if(verify&&i%30===0)verifyMotion();
      maxBullets=Math.max(maxBullets,current.records.filter(p=>p.kind===1).length);
      if(i%300===299){rows.push({frame:i+1,poses:current.poses.length,records:current.records.length,draws:r.drawCount});await new Promise(resolve=>requestAnimationFrame(resolve));}
    }
    for(const [key,values]of Object.entries(windows))if(values.length)stats[key]=values;
    for(const [key,values]of Object.entries(stats)){if(!['normal','bomb'].includes(key))values.splice(0,120);values.sort((a,b)=>a-b);stats[key]={mean:values.reduce((a,b)=>a+b,0)/values.length,p50:values[values.length>>1],p95:values[Math.floor(values.length*.95)],max:values.at(-1)};}
    const gl=r.gl,debug=gl.getExtension('WEBGL_debug_renderer_info');
    return {stats,rows,maxBullets,renderer:debug?gl.getParameter(debug.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER),final:json('state'),practice:bridge.snapshot()};
  },{baseline,locked,bomb,frames,verify,cadence});
  report.errors=errors;await writeFile(resolve(output,label+'.json'),JSON.stringify(report,null,2)+'\n');
  let hot=[];if(profileEnabled){
    const {profile}=await cdp.send('Profiler.stop');await writeFile(resolve(output,label+'-cpu.json'),JSON.stringify(profile));
    const counts=new Map();for(const id of profile.samples??[])counts.set(id,(counts.get(id)??0)+1);
    hot=profile.nodes.map(n=>({name:n.callFrame.functionName,url:n.callFrame.url,line:n.callFrame.lineNumber+1,samples:counts.get(n.id)??0})).sort((a,b)=>b.samples-a.samples).slice(0,18);
  }
  await page.screenshot({path:resolve(output,label+'.png')});
  console.log(JSON.stringify({label,stats:report.stats,rows:report.rows,maxBullets:report.maxBullets,renderer:report.renderer,hot,errors}));
}finally{await browser.close();await new Promise(done=>server.close(done));}
