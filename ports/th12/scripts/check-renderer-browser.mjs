// Compare the rendering change against a Git revision, using generated textures.
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve,sep,extname} from 'node:path';
import {launchBrowser} from '../../../tools/browser/launch.mjs';

const root=resolve(import.meta.dirname,'..'),repo=resolve(root,'../..');
const revision=process.argv[2]??'HEAD';
const commit=execFileSync('git',['rev-parse','--verify',revision+'^{commit}'],{cwd:repo,encoding:'utf8'}).trim();
const baseline=new Map(['renderer.mjs','draw-scheduler.mjs','sound-player.mjs'].map(name=>['/baseline/src/'+name,execFileSync('git',['show',commit+':ports/th12/src/'+name],{cwd:repo})]));
const server=createServer(async(req,res)=>{
  try{
    const url=new URL(req.url,'http://localhost');
    if(url.pathname==='/'){res.setHeader('Content-Type','text/html');res.end('<!doctype html><meta charset="utf-8">');return;}
    res.setHeader('Content-Type','text/javascript');
    if(baseline.has(url.pathname)){res.end(baseline.get(url.pathname));return;}
    const relative=decodeURIComponent(url.pathname.replace(/^\/(?:baseline|current)\//,''));
    const file=resolve(root,relative);
    if(!file.startsWith(root+sep)||extname(file)!=='.mjs')throw Error('Invalid module');
    res.end(await readFile(file));
  }catch(error){res.statusCode=404;res.end(error.message);}
});
await new Promise(done=>server.listen(0,'127.0.0.1',done));
let browser;
try{
  browser=await launchBrowser();const page=await browser.newPage(),errors=[];
  page.on('pageerror',error=>errors.push(error.message));
  await page.goto('http://127.0.0.1:'+server.address().port);
  const report=await page.evaluate(async()=>{
    const {Renderer:Before}=await import('/baseline/src/renderer.mjs');
    const {Renderer:After}=await import('/current/src/renderer.mjs');
    const {createRendererFixture,drawRendererWorkload,rendererWorkloads}=await import('/current/tests/helpers/renderer-workload.mjs');
    const make=Renderer=>{
      const canvas=document.createElement('canvas');canvas.width=640;canvas.height=480;document.body.append(canvas);
      const renderer=new Renderer(canvas,{bullet:{sprites:{405:{entry:0,x:0,y:0,w:16,h:16}}}}),gl=renderer.gl;
      for(let entry=0;entry<2;entry++){
        const texture=gl.createTexture(),pixels=new Uint8Array(256*256*4);
        for(let i=0;i<pixels.length;i+=4){const n=i/4;pixels.set([(n%256)*entry,Math.floor(n/256),255-entry*100,(n%7)*40],i);}
        gl.bindTexture(gl.TEXTURE_2D,texture);gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,256,256,0,gl.RGBA,gl.UNSIGNED_BYTE,pixels);
        renderer.textures.set('bullet:'+entry,{texture,w:256,h:256,defaultWrap:gl.REPEAT});
      }
      return renderer;
    };
    const before=make(Before),after=make(After),rows=[];
    const mixed=renderer=>{
      const g=renderer.gl;renderer.begin();g.uniform3fv(renderer.fogColor,[.2,.3,.4]);
      const quad=(entry,point,blend)=>renderer.region('bullet',entry,12,20,32,48,100,140,80,100,.3,.8,[1,.6,.3],blend,{point});
      quad(0,false,0);quad(1,true,1);quad(0,true,2);quad(0,false,0);
      renderer.stageSurface(14,[.4,-.4]);
      const vertices=[{x:200,y:60,u:.1,v:.2,rhw:1,color:0xff00ff80},
        {x:220,y:80,u:.2,v:.3,rhw:2,color:0x8000ffff},{x:240,y:120,u:.3,v:.4,rhw:.5,color:0xff0000ff},
        {x:280,y:140,u:.4,v:.5,rhw:1,color:0xffffffff}];
      for(const flags of [0,1,2,3])renderer.mesh({vertices,primitive:5,addressFlags:flags,filter:flags%2,a:{name:'bullet',entry:0,blend:flags}});
      renderer.mesh({vertices,primitive:5,addressFlags:0,filter:1,a:{name:'text',entry:2,blend:0}});
      renderer.stageSurfaceCopied=renderer.stageSurfaceComposited=false;renderer.stageSurface(14);
      const line=vertices.map(v=>({x:v.x+100,y:v.y+120,color:[1,.2,.4,.7]}));
      renderer.solidLineStrip(line,0);renderer.solidLineStrip(line.map(v=>({...v,y:v.y+20})),1);
      renderer.trail(300,200,.5,60,0xffff0000,0xff00ffff,.8);renderer.beamTrail(400,300,.8,60,12,0xff00ff00,0xffff00ff);
      renderer.flush();renderer.capturePauseField();renderer.region('text',1,0,0,128,128,500,100,100,100);renderer.flush();
      g.disable(g.SCISSOR_TEST);
    };
    const scenarios=[...Object.entries(rendererWorkloads).map(([name,workload])=>[name,r=>drawRendererWorkload(r,workload)]),['mixed',mixed]];
    for(const [name,draw]of scenarios){
      const traces=[Before,After].map(Renderer=>{const fixture=createRendererFixture(Renderer,{record:true});draw(fixture.renderer);return fixture.draws;});
      if(JSON.stringify(traces[0])!==JSON.stringify(traces[1]))throw Error('Draw stream mismatch: '+name);
      const pixels=[before,after].map(renderer=>{
        draw(renderer);const gl=renderer.gl,bytes=new Uint8Array(640*480*4);
        gl.readPixels(0,0,640,480,gl.RGBA,gl.UNSIGNED_BYTE,bytes);
        const error=gl.getError();if(error!==gl.NO_ERROR)throw Error('WebGL error '+error+' in '+name);
        return bytes;
      });
      let differentBytes=0;for(let i=0;i<pixels[0].length;i++)if(pixels[0][i]!==pixels[1][i])differentBytes++;
      let nonBackgroundPixels=0;
      for(let i=4;i<pixels[0].length;i+=4)if(pixels[0][i]!==pixels[0][0]||pixels[0][i+1]!==pixels[0][1]||pixels[0][i+2]!==pixels[0][2])nonBackgroundPixels++;
      if(nonBackgroundPixels<100)throw Error('Empty rendered workload: '+name);
      rows.push({name,draws:traces[0].length,differentBytes,nonBackgroundPixels});
    }
    const gl=after.gl,debug=gl.getExtension('WEBGL_debug_renderer_info');
    const {SoundPlayer:BeforeSound}=await import('/baseline/src/sound-player.mjs');
    const {SoundPlayer:AfterSound}=await import('/current/src/sound-player.mjs');
    const audio=[];
    for(const channels of [1,2])for(const pan of [-3000,0,3000]){
      const outputs=[];
      for(const Player of [BeforeSound,AfterSound]){
        const context=new OfflineAudioContext(2,2048,48000),buffer=context.createBuffer(channels,2048,48000);
        for(let c=0;c<channels;c++)for(let i=0;i<2048;i++)buffer.getChannelData(c)[i]=Math.sin(i*.137+c*.73)*.5;
        const player=new Player({context,index:[{id:0,file:'tone',volume:-600,loop:false}],readBytes:()=>{throw Error('Unexpected audio load');}});
        player.buffers.set('tone',buffer);player.play({id:0,pan});
        const nodes=player.voices.get(0).nodes.length,rendered=await context.startRendering();
        outputs.push({nodes,pcm:[...rendered.getChannelData(0),...rendered.getChannelData(1)]});
      }
      const differentSamples=outputs[0].pcm.reduce((n,v,i)=>n+(v!==outputs[1].pcm[i]?1:0),0);
      if(differentSamples||!outputs[1].pcm.some(v=>Math.abs(v)>.001))throw Error('Audio PCM mismatch');
      if(pan===0&&outputs[1].nodes!==2)throw Error('Centered audio should use two nodes');
      audio.push({channels,pan,differentSamples,nodesBefore:outputs[0].nodes,nodesAfter:outputs[1].nodes});
    }
    return {renderer:debug?gl.getParameter(debug.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER),rows,audio};
  });
  assert.deepEqual(errors,[]);assert.ok(report.rows.every(row=>row.differentBytes===0),'rendered pixels must match');
  console.log(JSON.stringify({baseline:commit,browser:browser.version(),...report,errors},null,2));
}finally{
  if(browser)await browser.close();await new Promise(done=>server.close(done));
}
