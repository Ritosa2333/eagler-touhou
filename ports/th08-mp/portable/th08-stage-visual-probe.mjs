import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const out='th08_web/artifacts/stage-visual';mkdirSync(out,{recursive:true});
const startStage=Number(process.env.STAGE??1),tag=process.env.TAG??'before',base=process.env.TEST_URL??'http://127.0.0.1:8088';
const browser=await launchBrowser(),page=await browser.newPage({viewport:{width:640,height:480}}),errors=[];
page.on('pageerror',e=>errors.push(e.stack));page.on('console',m=>{if(m.type()==='warning'||m.type()==='error')console.log(m.type(),m.text().slice(0,300));});
try{
 await page.addInitScript(()=>{Date.now=()=>100000;});
 await page.goto(base+'/runtime/th08/th08.html?manual=1');await page.waitForFunction(()=>window.__th08Runtime||document.querySelector('#error').textContent,null,{timeout:120000});await page.locator('canvas').click();
 await page.evaluate(async bytes=>{
  const r=__th08Runtime,c=r.core;await r.command({command:'write',path:'replay/th8_01.rpy',bytes});await r.launch();c.sdl_touch_options(0,0,1);
  const pointer=c.allocate(64),mapping=[[1,'KeyZ'],[2,'KeyX'],[4,'ShiftLeft'],[8,'Escape'],[16,'ArrowUp'],[32,'ArrowDown'],[256,'ControlLeft']];let previous=0;
  const step=(input=0)=>{for(const[bit,key]of mapping)if(!!(input&bit)!==!!(previous&bit)){new Uint8Array(c.memory.buffer,pointer,64).set(new TextEncoder().encode(key+'\0'));c.sdl_key(pointer,!!(input&bit));}previous=input;if(c.sdl_loop_tick(r.app,1/60,17))throw Error('Native game failed '+r.status());};
  const wait=n=>{for(let i=0;i<n;i++)step();},key=k=>{for(let i=0;i<3;i++)step(k);wait(42);};
  const title=()=>({scene:c.status(r.app,0),screen:c.status(r.app,8),cursor:c.status(r.app,9)});
  const trace=()=>Array.from(new Uint32Array(c.memory.buffer,c.trace(r.app),68));window.probe={c,r,step,wait,key,title,trace};wait(100);
 },Array.from(readFileSync('th08_web/artifacts/cpp/campaign/character0-final/original.rpy')));
 for(const input of [16,16,16,16,16,1,1]){console.log('menu',await page.evaluate(k=>{probe.key(k);return probe.title();},input));}
 await page.screenshot({path:out+'/'+tag+'-menu.png'});
 console.log('start',await page.evaluate(stage=>{for(let i=1;i<stage;i++)probe.key(32);probe.key(1);probe.key(1);return {menu:probe.title(),trace:probe.trace().slice(0,11)};},startStage));
 const snapshots=[],rows=[],max=Number(process.env.TICKS??42000);let ticks=0,lastStage=-1;
 const verify=process.argv.includes('--verify');
 await page.evaluate(()=>{
  const gl=document.querySelector('canvas').getContext('webgl2'),draw=gl.drawArrays,programs=new Map();
  probe.gpuCheck={enabled:false,spriteBatches:0,incorrect:[]};
  gl.drawArrays=function(mode,first,count){
   if(probe.gpuCheck.enabled&&mode===gl.TRIANGLES&&gl.getVertexAttrib(0,gl.VERTEX_ATTRIB_ARRAY_STRIDE)===28){
    const program=gl.getParameter(gl.CURRENT_PROGRAM);let args=programs.get(program);
    if(!args){const fragment=gl.getAttachedShaders(program).find(s=>gl.getShaderParameter(s,gl.SHADER_TYPE)===gl.FRAGMENT_SHADER),source=gl.getShaderSource(fragment);args=['colorArg2','alphaArg2'].map(name=>Number(source.match(new RegExp('const int '+name+'=(\\d+);'))?.[1]??-1));programs.set(program,args);}
    probe.gpuCheck.spriteBatches++;if(args.some(v=>v!==0))probe.gpuCheck.incorrect.push(args);
   }
   return draw.call(this,mode,first,count);
  };
 });
 while(ticks<max){const result=await page.evaluate(async()=>{let used=0;for(let i=0;i<300;i++){probe.gpuCheck.enabled=i===299;probe.step();used++;if(i%30===29)await new Promise(r=>setTimeout(r,0));}const c=probe.c;return {used,menu:probe.title(),trace:probe.trace().slice(0,11),visual:c.visual_diagnostics?Array.from(new Uint32Array(c.memory.buffer,c.visual_diagnostics(probe.r.app),4)):null,error:document.querySelector('canvas').getContext('webgl2')?.getError()};});ticks+=result.used;rows.push({ticks,...result});
  const stage=result.trace[1];if(stage!==lastStage||ticks%3000===0){const name=tag+'-s'+(stage+1)+'-t'+ticks;await page.screenshot({path:out+'/'+name+'.png'});snapshots.push({name,ticks,...result});console.log(JSON.stringify(snapshots.at(-1)));}lastStage=stage;
  if(result.menu.scene===1&&ticks>300)break;
 }
 const gpu=await page.evaluate(()=>probe.gpuCheck),manifest=await(await fetch(base+'/manifest.json')).json();
 const report={passed:false,wasm:manifest.execution.sha256,startStage,ticks,snapshots,rows,gpu,errors};
 if(verify){
  assert.equal(errors.length,0);assert(rows.every(r=>r.error===0),'No WebGL error');
  assert(rows.some(r=>r.visual?.[2]===1&&r.visual[0]===255),'Mystia must activate the actual AsciiManager mask');
  const stage3=rows.filter(r=>r.visual?.[2]===2);assert(stage3.length>=10,'Stage 3 must continue beyond its transition');assert(stage3.every(r=>r.visual[0]===0),'Night blindness must be cleared for stage 3');
  assert(gpu.spriteBatches>100,'Inspect actual GPU sprite submissions');assert.deepEqual(gpu.incorrect,[],'Every sprite batch restores original diffuse arguments');
  const baseline=JSON.parse(readFileSync(out+'/before-transition.json'));for(const old of baseline.snapshots){const current=rows.find(r=>r.ticks===old.ticks);if(current)assert.deepEqual(current.trace,old.trace,'Visual fixes preserve gameplay at tick '+old.ticks);}
  report.passed=true;
 }
 for(const s of snapshots)s.imageSha256=createHash('sha256').update(readFileSync(out+'/'+s.name+'.png')).digest('hex');
 writeFileSync(out+'/'+tag+'.json',JSON.stringify(report,null,2));if(errors.length)throw Error(errors.join('\n'));console.log(JSON.stringify({passed:report.passed,wasm:report.wasm,ticks,spriteBatches:gpu.spriteBatches,incorrect:gpu.incorrect.length}));
}finally{await browser.close();}
