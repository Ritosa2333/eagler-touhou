import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createReadStream,readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve,sep,extname} from 'node:path';
import {execFileSync} from 'node:child_process';
import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {replayFixture} from './replay-fixture.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(value=>{const at=value.indexOf('=');return [value.slice(2,at),value.slice(at+1)];}));
const output=resolve(args.output||'ports/thkkd/build/render-overhead'),runtime=resolve(args.runtime||'ports/thkkd/runtime');
const data=resolve(args.data||'ports/thkkd/build/web-content/thkkd.data');
const referenceCommit=args.commit||'497969450a26f0d809d6afd8d96d41b5d0f90cea';
const reference=args.reference?readFileSync(args.reference):execFileSync('git',['show',referenceCommit+':ports/thkkd/web/renderer.mjs']);
mkdirSync(output,{recursive:true});
const server=createServer((request,response)=>{
  const pathname=decodeURIComponent(new URL(request.url,'http://localhost').pathname);
  const path=pathname==='/fixture.data'?data:resolve(runtime,'.'+pathname);
  response.setHeader('Cross-Origin-Opener-Policy','same-origin');response.setHeader('Cross-Origin-Embedder-Policy','require-corp');
  if(pathname==='/reference-renderer.mjs'){response.setHeader('Content-Type','text/javascript');response.end(reference);return;}
  if(path!==data&&!path.startsWith(runtime+sep)){response.writeHead(404);response.end();return;}
  response.setHeader('Content-Type',({'.mjs':'text/javascript','.wasm':'application/wasm','.html':'text/html','.woff2':'font/woff2'})[extname(path)]||'application/octet-stream');
  const input=createReadStream(path);input.on('error',()=>{response.statusCode=404;response.end();});input.pipe(response);
});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const url=`http://127.0.0.1:${server.address().port}`;
const browser=await puppeteer.launch({executablePath:process.env.CHROME_PATH||'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,protocolTimeout:300000,defaultViewport:{width:960,height:720}});
const page=await browser.newPage(),report={schema:'thkkd/render-overhead/1',referenceCommit,cases:[],errors:[]};
page.on('pageerror',error=>report.errors.push(error.message));
try{
  await page.goto(url+'/thkkd.html?standalone=1&runtimeEpoch=1&data=/fixture.data');
  await page.waitForFunction(()=>globalThis.__thkkdRuntime,{timeout:120000});await page.click('canvas');
  await page.waitForFunction(()=>__thkkdRuntime.status().first,{timeout:30000});
  await page.evaluate(async()=>{
    window.__referenceRenderer=(await import('/reference-renderer.mjs')).Renderer;
    window.__candidateRenderer=__thkkdRuntime.platform.renderer.constructor;
    await __thkkdRuntime.stop();
  });
  report.browser=await browser.version();
  report.gpu=await page.evaluate(()=>{const gl=__thkkdRuntime.platform.renderer.gl,extension=gl.getExtension('WEBGL_debug_renderer_info');return extension?gl.getParameter(extension.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER);});
  report.commandPreparation=await page.evaluate(()=>{
    const create=Class=>Object.assign(Object.create(Class.prototype),{current:[],commands:[],previous:new Map(),identities:new Map(),brightness:[.7,.8,.9],mode:1,alpha:.8,area:[32,16,416,464],tick:0,scene:'game'});
    const renderers={before:create(__referenceRenderer),after:create(__candidateRenderer)};
    const graph={texture:1,width:256,height:256,x:16,y:32,w:32,h:32},sprites=Array.from({length:1200},(_,i)=>({identity:'sprite:'+i,x:i%640,y:i%480,scale:.5+(i%9)/4,angle:i/100}));
    function frame(r){r.clear();for(const s of sprites){r.identity=s.identity;r.sprite(graph,s.x,s.y,s.scale,s.angle);}r.tick++;r.commit();}
    for(let i=0;i<30;i++){frame(renderers.before);frame(renderers.after);}
    const samples={before:[],after:[]};
    for(let round=0;round<40;round++)for(const name of round%2?['after','before']:['before','after']){const start=performance.now();frame(renderers[name]);samples[name].push(performance.now()-start);}
    const median=name=>samples[name].sort((a,b)=>a-b).slice(8,32).reduce((a,b)=>a+b,0)/24;
    return {spritesPerFrame:1200,beforeMs:median('before'),afterMs:median('after'),samples};
  });
  const variants=args.quick==='1'?[0]:Array.from({length:9},(_,i)=>i);
  for(const variant of variants){
    const bytes=replayFixture({character:Math.floor(variant/3),shot:variant%3,count:1000,bombFrame:180});
    await page.evaluate(async bytes=>{const r=__thkkdRuntime;await r.command({command:'write',path:'replay/render-cost.rpy',bytes});await r.command({command:'playback',action:'open',path:'replay/render-cost.rpy'});await r.command({command:'playback',action:'pause'});},[...bytes]);
    for(const cursor of [150,210,800]){
      await page.evaluate(cursor=>__thkkdRuntime.command({command:'playback',action:'seek',value:cursor}),cursor);
      await page.waitForFunction(cursor=>__thkkdRuntime.core._thkkd_word(0xb43110)===cursor,{timeout:120000},cursor);
      const result=await page.evaluate(()=>{
        const runtime=__thkkdRuntime,r=runtime.platform.renderer,gl=r.gl,old=r.onPresent;r.onPresent=null;
        const before=__referenceRenderer.prototype.present,after=__candidateRenderer.prototype.present;
        const state=()=>{let h=2166136261;for(const address of [0xa74c08,0xb43110,0xb3e2ec,0xb3e2e8,0xa78e2c])h=Math.imul(h^runtime.core._thkkd_word(address),16777619)>>>0;return h;};
        const initial=state(),snapshot=JSON.stringify(r.commands,(key,value)=>key==='texture'?undefined:value);
        const pixels=()=>{const bytes=new Uint8Array(640*480*4);gl.readPixels(0,0,640,480,gl.RGBA,gl.UNSIGNED_BYTE,bytes);return bytes;};
        let equal=true;
        for(const alpha of [0,.5,1]){before.call(r,alpha);const a=pixels();after.call(r,alpha);const b=pixels();equal&&=a.every((value,i)=>value===b[i]);}
        const measure=fn=>{
          const samples=[];for(let round=0;round<50;round++){gl.finish();const start=performance.now();fn.call(r,.5);if(round>=10)samples.push(performance.now()-start);}gl.finish();
          samples.sort((a,b)=>a-b);return samples.slice(8,32).reduce((a,b)=>a+b,0)/24;
        };
        for(let i=0;i<10;i++){before.call(r,.5);after.call(r,.5);}
        const costs={before:[],after:[]};
        for(let round=0;round<3;round++)for(const name of round%2?['after','before']:['before','after'])costs[name].push(measure(name==='before'?before:after));
        const count=fn=>{
          const names=['bufferData','drawArrays','bindTexture','uniform1i','blendEquation','blendFunc','scissor'],saved={},counts={};
          try{for(const name of names){saved[name]=gl[name];counts[name]=0;gl[name]=function(...args){counts[name]++;return saved[name].apply(this,args);};}fn.call(r,0);return counts;}
          finally{for(const name of names)gl[name]=saved[name];}
        };
        const calls={before:count(before),after:count(after)};
        const unchanged=state()===initial&&snapshot===JSON.stringify(r.commands,(key,value)=>key==='texture'?undefined:value);
        const image=document.querySelector('canvas').toDataURL();r.onPresent=old;
        return {equal,unchanged,commands:r.commands.length,state:initial,costs,calls,image};
      });
      assert.ok(result.equal,'同帧三个插值位置的 RGBA 像素完全一致');assert.ok(result.unchanged,'绘制保持逻辑状态与命令快照只读');
      writeFileSync(resolve(output,`character-${variant}-${cursor}.png`),Buffer.from(result.image.split(',')[1],'base64'));delete result.image;
      report.cases.push({variant,cursor,...result});console.log(JSON.stringify({variant,cursor,commands:result.commands,costs:result.costs,calls:result.calls}));
    }
  }
  assert.equal(report.errors.length,0);report.result='passed';
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);}
finally{writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();await new Promise(resolve=>server.close(resolve));}
