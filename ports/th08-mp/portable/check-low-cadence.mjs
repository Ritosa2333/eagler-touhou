import assert from 'node:assert/strict';
import {writeFileSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const game=process.argv.includes('--th08')?'th08':'th10',port=game==='th08'?8095:8093,browser=await launchBrowser(),rows=[];
try{
 const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));
 await page.addInitScript(()=>{window.displayHz=60;window.nextDisplay=0;window.requestAnimationFrame=cb=>{window.nextDisplay=Math.max(performance.now(),window.nextDisplay+1000/window.displayHz);return setTimeout(()=>cb(performance.now()),Math.max(0,window.nextDisplay-performance.now()));};window.cancelAnimationFrame=clearTimeout;});
 await page.goto(`http://127.0.0.1:${port}/runtime/${game}/${game}.html?manual=1`);await page.waitForFunction(game=>window['__'+game+'Runtime'],game,{timeout:120000});await page.locator('canvas').click();await page.evaluate(game=>window['__'+game+'Runtime'].launch(),game);
 for(const hz of [60,30,15]){
  await page.evaluate(({game,hz})=>{window.displayHz=hz;window.nextDisplay=performance.now();const r=window['__'+game+'Runtime'];r.core.sdl_loop_start(r.app);},{game,hz});
  await page.waitForTimeout(hz===60?6500:1000);
  await page.evaluate(game=>{
   const node=window['__'+game+'Runtime'].Module.SDL3?.audio_playback?.scriptProcessorNode;
   if(!node)throw Error('SDL browser output is unavailable');
   if(window.audioOutputProbe?.node===node)return;
   const probe=window.audioOutputProbe={node,blocks:0,zeroBlocks:0,samples:0},callback=node.onaudioprocess;
   node.onaudioprocess=function(event){callback.call(this,event);const pcm=event.outputBuffer.getChannelData(0);let energy=0;for(const sample of pcm)energy+=sample*sample;probe.blocks++;probe.samples+=pcm.length;if(energy===0)probe.zeroBlocks++;};
  },game);
  const sample=()=>page.evaluate(game=>{const r=window['__'+game+'Runtime'],c=r.core,stats=Array.from(new Uint32Array(c.memory.buffer,c.sdl_stats(),9)),a=Array.from(new Uint32Array(c.memory.buffer,c.sdl_audio_stats(),12));return {time:performance.now(),logic:stats[4],present:stats[5],mixed:a[game==='th08'?2:4],queued:a[5],error:a[game==='th08'?4:8],rms:new Float32Array(c.memory.buffer,c.sdl_audio_stats()+40,1)[0]};},game);
  const outputSample=()=>page.evaluate(()=>{const {blocks,zeroBlocks,samples}=window.audioOutputProbe;return {blocks,zeroBlocks,samples};});
  const before=await sample(),outputBefore=await outputSample();await page.waitForTimeout(3000);const after=await sample(),outputAfter=await outputSample();
  await page.evaluate(game=>window['__'+game+'Runtime'].core.sdl_loop_stop(),game);
  const seconds=(after.time-before.time)/1000,logicHz=(after.logic-before.logic)/seconds,audioRate=(after.mixed-before.mixed)/seconds;
  rows.push({displayHz:hz,logicHz,audioRate,before,after,outputBefore,outputAfter});console.log(JSON.stringify(rows.at(-1)));writeFileSync(`${game}_web/artifacts/architecture-candidate/validation/low-cadence-progress.json`,JSON.stringify(rows,null,2));
  assert(outputAfter.blocks>outputBefore.blocks,'Actual browser audio callbacks must run');assert.equal(outputAfter.zeroBlocks-outputBefore.zeroBlocks,0,'No empty browser output blocks during title music');
  const expectedLogic=Math.min(60,hz);assert(logicHz>expectedLogic-3&&logicHz<expectedLogic+3,'Logic follows original single-tick scheduler without catch-up');assert(audioRate>41000&&audioRate<47500,'PCM refill keeps pace with 44100 Hz playback');assert.equal(after.error,0);assert(after.rms>0);
 }
 assert.deepEqual(errors,[]);writeFileSync(`${game}_web/artifacts/architecture-candidate/validation/low-cadence.json`,JSON.stringify({passed:true,rows,errors},null,2));console.log(JSON.stringify({game,rows}));
}finally{await browser.close();}
