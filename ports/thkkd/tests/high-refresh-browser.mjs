import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,readFileSync} from 'node:fs';
import {execFileSync} from 'node:child_process';
import {resolve} from 'node:path';
import {productionBrowser,runtimeCommand,wait,observeCadence} from './production-driver.mjs';
import {replayFixture} from './replay-fixture.mjs';
const args=Object.fromEntries(process.argv.slice(2).map(argument=>{const i=argument.indexOf('=');return [argument.slice(2,i),argument.slice(i+1)];}));
const output=resolve(args.output||'ports/thkkd/build/high-refresh-20261127/gpu');mkdirSync(output,{recursive:true});
const referenceCommit=args.commit||'63a593f70f8900f1435debbc86c2cd2f9f68460d';
const reference=execFileSync('git',['show',referenceCommit+':ports/thkkd/web/renderer.mjs'],{encoding:'utf8'});
const report={schema:'thkkd/high-refresh-browser/1',referenceCommit,checks:[],nativePixels:[]};
const driver=await productionBrowser({url:args.url||'http://127.0.0.1:19027/',profile:args.profile,report,unlimited:true});const {page,browser}=driver;
try{
  await page.setBypassServiceWorker(true);await page.setCacheEnabled(false);await driver.library();
  await page.click('#gameSettingsOpen');await page.waitForSelector('#libraryBack',{visible:true});
  await page.evaluate(()=>{for(const element of document.querySelectorAll('*')){let fiber=element[Object.keys(element).find(key=>key.startsWith('__reactFiber$'))];while(fiber){const session=fiber.memoizedProps?.session;if(session?.settings){session.settings.setOption('frameLimit60Enabled',false);session.settings.setOption('touchEnabled',false);return;}fiber=fiber.return;}}throw Error('Settings session missing');});
  await page.click('#libraryBack');const frame=await driver.start({expectedBuildId:JSON.parse(readFileSync(new URL('../runtime/manifest.json',import.meta.url),'utf8')).buildId});
  await frame.evaluate(async source=>{window.__referenceRenderer=(await import(source)).Renderer;},'data:text/javascript;base64,'+Buffer.from(reference).toString('base64'));
  for(const [character,shot] of [[0,0],[1,1],[2,1]]){
    await runtimeCommand(page,'write',{path:'replay/high-refresh.rpy',bytes:[...replayFixture({character,shot,practice:true,count:1200,bombFrame:180})]});
    await runtimeCommand(page,'playback',{action:'open',path:'replay/high-refresh.rpy'});await runtimeCommand(page,'playback',{action:'pause'});await runtimeCommand(page,'playback',{action:'seek',value:230});
    await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)===230);
    const result=await frame.evaluate(()=>{
      const r=__thkkdRuntime.platform.renderer,gl=r.gl;gl.bindVertexArray(null);const reference=new __referenceRenderer(r.canvas);
      reference.commands=r.commands;reference.previous=r.previous;
      const pixels=()=>{const bytes=new Uint8Array(640*480*4);gl.readPixels(0,0,640,480,gl.RGBA,gl.UNSIGNED_BYTE,bytes);return bytes;};
      reference.present(0);const before=pixels();r.present(0);const after=pixels();let changed=0;for(let i=0;i<before.length;i++)changed+=Number(before[i]!==after[i]);
      gl.deleteBuffer(reference.buffer);gl.deleteProgram(reference.program);gl.deleteTexture(reference.white.texture);
      return {changed,commands:r.commands.length,image:r.canvas.toDataURL()};
    });
    assert.equal(result.changed,0,'原始逻辑帧的 RGBA 像素与 r7 完全一致');writeFileSync(resolve(output,`native-${character}-${shot}.png`),Buffer.from(result.image.split(',')[1],'base64'));delete result.image;report.nativePixels.push({character,shot,...result});
  }
  report.checks.push('native-logic-frames-match-r7-pixels-exactly');
  report.gpu=await frame.evaluate(async()=>{
    const native=__thkkdRuntime,r=native.platform.renderer,gl=r.gl,canvas=r.canvas,Renderer=r.constructor;
    gl.bindVertexArray(null);const reference=new __referenceRenderer(canvas),candidate=new Renderer(canvas);
    const pixels=()=>{const bytes=new Uint8Array(640*480*4);gl.readPixels(0,0,640,480,gl.RGBA,gl.UNSIGNED_BYTE,bytes);return bytes;};
    const drawReference=alpha=>{gl.useProgram(reference.program);gl.bindVertexArray(null);gl.bindBuffer(gl.ARRAY_BUFFER,reference.buffer);reference.present(alpha);};
    const drawCandidate=alpha=>candidate.present(alpha);
    const state=()=>native.core.HEAPU8.slice();const before=state();const cases=[];
    for(const [oldAngle,angle,oldScale,scale,center] of [[170,179,1,1,true],[179,-179,1.8,2,true],[15,30,1,1.1,false]]){
      const graph={...candidate.white,width:32,height:16};
      for(const [tick,x,y,a,s]of [[0,200,220,oldAngle,oldScale],[1,208,224,angle,scale]]){candidate.clear();candidate.identity='rotating';candidate.tick=tick;candidate.scene='synthetic';candidate.sprite(graph,x,y,s,a*Math.PI/180,false,center);candidate.commit();}
      drawCandidate(.5);const actual=pixels();
      const delta=Math.atan2(Math.sin((angle-oldAngle)*Math.PI/180),Math.cos((angle-oldAngle)*Math.PI/180));
      reference.clear();reference.identity=null;reference.sprite(graph,212,226,scale+(scale-oldScale)*.5,angle*Math.PI/180+delta*.5,false,center);reference.commit();drawReference(0);const expected=pixels();
      let changed=0;for(let i=0;i<actual.length;i++)changed+=Number(actual[i]!==expected[i]);cases.push({oldAngle,angle,oldScale,scale,center,changedChannels:changed});
      if(changed>32)throw Error('GPU rotation/scale differs from expected geometry: '+changed);
    }
    reference.commands=r.commands;reference.previous=r.previous;candidate.commands=r.commands;candidate.previous=r.previous;
    drawReference(.5);drawCandidate(.5);
    const savedUpload=gl.bufferData,counts={before:0,after:0};let current;
    gl.bufferData=function(...args){counts[current]++;return savedUpload.apply(this,args);};
    const times={before:[],after:[]};
    try{for(let round=0;round<4;round++)for(const name of round%2?['after','before']:['before','after']){current=name;gl.finish();const start=performance.now();for(let i=0;i<60;i++)(name==='before'?drawReference:drawCandidate)((i%4)/4);times[name].push((performance.now()-start)/60);}}
    finally{gl.bufferData=savedUpload;}
    const after=state(),sha=async bytes=>[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(n=>n.toString(16).padStart(2,'0')).join('');
    for(const renderer of [reference,candidate]){gl.deleteBuffer(renderer.buffer);gl.deleteProgram(renderer.program);gl.deleteTexture(renderer.white.texture);if(renderer.vao)gl.deleteVertexArray(renderer.vao);}
    r.present(0);
    return {geometry:cases,uploads:counts,cpuMsPerPresent:times,commands:r.commands.length,memoryBefore:await sha(before),memoryAfter:await sha(after)};
  });
  assert.equal(report.gpu.memoryBefore,report.gpu.memoryAfter);assert.equal(report.gpu.uploads.before,240);assert.equal(report.gpu.uploads.after,0);
  report.checks.push('GPU-shortest-angle-and-scale-preserve-shape','extra-presents-reuse-GPU-buffer','extra-presents-preserve-complete-WASM-memory');
  report.cadence=await observeCadence(frame,1400);assert.equal(report.cadence.steps,0);assert.ok(report.cadence.presentHz>75,'后台解除限频后额外呈现');
  await frame.evaluate(()=>{const r=__thkkdRuntime,show=r.platform.renderer.present.bind(r.platform.renderer);window.__endSamples=[];r.platform.renderer.present=(alpha,...rest)=>{if(r.core._thkkd_word(0xb43110)>=1199)__endSamples.push(alpha);return show(alpha,...rest);};});
  await runtimeCommand(page,'playback',{action:'seek',value:1195});await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)===1195);await runtimeCommand(page,'playback',{action:'play'});await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)===1199);await wait(130);
  report.endAlphas=await frame.evaluate(()=>__endSamples);assert.ok(report.endAlphas.length>0);assert.ok(report.endAlphas.every(alpha=>alpha===0));report.checks.push('Replay-EOF-freezes-presentation-immediately');
  await driver.collect();assert.equal(report.errors.length,0);report.result='passed';console.log(JSON.stringify({result:report.result,checks:report.checks,gpu:report.gpu,cadence:report.cadence.presentHz}));
}catch(error){report.result='failed';report.error=String(error.stack||error);process.exitCode=1;console.error(error);await page.screenshot({path:resolve(output,'failure.png')});}
finally{await driver.collect();writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();}
