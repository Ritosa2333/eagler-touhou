// Real retail scenarios: fixed input, whole-core state, immutable endpoints and pixels.
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,sep,extname} from 'node:path';
import {execFileSync} from 'node:child_process';
import {launchBrowser} from '../browser/launch.mjs';
const repo=resolve(import.meta.dirname,'../..'),root=resolve(repo,'ports/th12');
const assets=resolve(process.env.TH12_PROFILE_ASSETS??resolve(root,'site/assets'));
if(!process.env.TH12_PROFILE_REFERENCE_RUNTIME)throw Error('Set TH12_PROFILE_REFERENCE_RUNTIME to the reference core directory');
const referenceRuntime=resolve(process.env.TH12_PROFILE_REFERENCE_RUNTIME);
const revision=execFileSync('git',['rev-parse','--verify',(process.argv[2]??'HEAD')+'^{commit}'],{cwd:repo,encoding:'utf8'}).trim();
const sources=new Map(['renderer.mjs','draw-scheduler.mjs','presentation-interpolation.mjs'].map(name=>['/reference/src/'+name,execFileSync('git',['show',revision+':ports/th12/src/'+name],{cwd:repo})]));
const server=createServer(async(req,res)=>{try{
  const path=new URL(req.url,'http://localhost').pathname;
  if(path==='/'){res.setHeader('Content-Type','text/html');res.end('<!doctype html><meta charset="utf-8">');return;}
  res.setHeader('Content-Type',({'.mjs':'text/javascript','.json':'application/json','.wasm':'application/wasm','.png':'image/png'})[extname(path)]??'application/octet-stream');
  if(sources.has(path)){res.end(sources.get(path));return;}
  const isAsset=path.startsWith('/assets/'),isReference=path.startsWith('/reference/runtime/');
  const base=isAsset?assets:isReference?referenceRuntime:root;
  const relative=isAsset?path.slice(8):isReference?path.slice('/reference/runtime/'.length):'.'+path.replace(/^\/reference\//,'/');
  const file=resolve(base,decodeURIComponent(relative));if(!file.startsWith(base+sep))throw Error('Invalid path');
  res.end(await readFile(file));
}catch(error){res.statusCode=404;res.end(error.message);}});
await new Promise(done=>server.listen(0,'127.0.0.1',done));
const browser=await launchBrowser(),reports=[],output=resolve(repo,'artifacts/th12-performance');
try{
  for(const scenario of [{stage:6,section:45,difficulty:0,frames:1800,bomb:false},{stage:7,section:69,difficulty:4,frames:1200,bomb:true}]){
    const page=await browser.newPage(),errors=[];page.on('pageerror',error=>errors.push(error.message));
    await page.goto('http://127.0.0.1:'+server.address().port);
    const report=await page.evaluate(async scenario=>{
      const {default:oldCreate}=await import('/reference/runtime/core.mjs'),{default:newCreate}=await import('/site/runtime/core.mjs');
      const {Renderer:Old}=await import('/reference/src/renderer.mjs'),{Renderer:New}=await import('/src/renderer.mjs');
      const {StageLoader}=await import('/src/stage-loader.mjs'),{PracticeBridge}=await import('/src/practice/bridge.mjs');
      const atlases=await(await fetch('/assets/atlases.json')).json(),pairs=[];
      for(const [create,Renderer] of [[oldCreate,Old],[newCreate,New]]){
        const core=await create(),canvas=document.createElement('canvas');canvas.width=640;canvas.height=480;document.body.append(canvas);
        const renderer=new Renderer(canvas,atlases);await renderer.prepare();await renderer.prepareStage(scenario.stage);
        const loader=new StageLoader(core,atlases,async name=>new Uint8Array(await(await fetch('/assets/'+name)).arrayBuffer()));await loader.load(scenario.stage);
        const bridge=new PracticeBridge(core);bridge.setEnabled(true);bridge.configure({stage:scenario.stage-1,section:scenario.section,power:400},{difficulty:scenario.difficulty});
        bridge.start({character:2,shot:1,difficulty:scenario.difficulty,seed:0x1234});bridge.setTools(1|256|4);
        pairs.push({core,renderer,previous:null,saved:null});
      }
      let comparedFrames=0;const snapshots=[];
      const state=core=>{const pointer=core._th12_state(),size=core._th12_state_size();return new TextDecoder().decode(core.HEAPU8.subarray(pointer,pointer+size));};
      for(let tick=0;tick<scenario.frames;tick++){
        const bomb=scenario.bomb&&(tick===300||tick===900),held=scenario.bomb?1|(bomb?2:0):0;
        for(const {core} of pairs)core._th12_tick(held,bomb?2:0);
        if((tick+1)%30)continue;
        if(state(pairs[0].core)!==state(pairs[1].core))throw Error('Core state mismatch at '+(tick+1));
        const pixels=[],draws=[];
        for(const pair of pairs){
          const {core,renderer}=pair,current=renderer.capturePresentation(core,{alwaysHitbox:true,presentationContinuity:false});
          if(pair.previous&&JSON.stringify(pair.previous)!==pair.saved)throw Error('Previous endpoint mutated');
          pair.previous=current;pair.saved=JSON.stringify(current);renderer.renderPresentation(null,current,1);
          const gl=renderer.gl,bytes=new Uint8Array(640*480*4);gl.readPixels(0,0,640,480,gl.RGBA,gl.UNSIGNED_BYTE,bytes);
          if(gl.getError())throw Error('WebGL error');pixels.push(bytes);draws.push(renderer.drawCount);
        }
        let differences=0;for(let i=0;i<pixels[0].length;i++)if(pixels[0][i]!==pixels[1][i])differences++;
        if(differences||draws[0]!==draws[1])throw Error('Rendering mismatch at '+(tick+1)+': pixels='+differences+' draws='+draws);
        comparedFrames++;snapshots.push({tick:tick+1,draws:draws[1],differentBytes:differences});
        await new Promise(done=>requestAnimationFrame(done));
      }
      return {...scenario,comparedFrames,snapshots};
    },scenario);
    if(errors.length)throw Error(errors.join('\n'));reports.push(report);await page.close();
  }
  await mkdir(output,{recursive:true});
  const report={revision,browser:browser.version(),referenceBuild:JSON.parse(await readFile(resolve(referenceRuntime,'build.json'),'utf8')),currentBuild:JSON.parse(await readFile(resolve(root,'site/runtime/build.json'),'utf8')),reports};
  await writeFile(resolve(output,'real-scene-equivalence.json'),JSON.stringify(report,null,2)+'\n');
  console.log(JSON.stringify({revision,reports:reports.map(({snapshots,...scenario})=>scenario)}));
}finally{await browser.close();await new Promise(done=>server.close(done));}
