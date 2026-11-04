import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createServer} from 'node:http';
import {resolve,sep} from 'node:path';
import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../launcher/lib/chromium-executable.mjs';

const root=resolve(import.meta.dirname,'../..'),workspace=resolve(root,'..');
const require=createRequire(new URL('../../launcher/package.json',import.meta.url));
const puppeteer=require('puppeteer-core');
const disks={th02:'th02-coop-lab/web/th02-solo-jp.hdi.gz',th03:'th03-netplay-lab/web/disks/3-jp.hdi.gz',th04:'th04-coop-lab/web/th04-solo-jp.hdi.gz'};
const server=createServer(async(request,response)=>{
  const match=/^\/(th0[234])\/(.+)$/.exec(decodeURIComponent(request.url));
  if(!match){response.writeHead(404).end();return;}
  const [,game,name]=match,base=resolve(root,'ports',game,'build-eagler');
  const file=name==='disk.gz'?resolve(workspace,disks[game]):name==='original-sfx.mjs'?resolve(root,'common/pc98/original-sfx.mjs'):resolve(base,name);
  if(name!=='disk.gz'&&name!=='original-sfx.mjs'&&!file.startsWith(base+sep)){response.writeHead(400).end();return;}
  try{const data=await readFile(file);response.writeHead(200,{'Content-Type':name.endsWith('.wasm')?'application/wasm':/\.(m?js)$/.test(name)?'text/javascript':'application/octet-stream'});response.end(data);}
  catch{response.writeHead(404).end();}
});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
try{
  for(const game of process.argv.slice(2).length?process.argv.slice(2):Object.keys(disks)){
    const page=await browser.newPage();await page.goto(`http://127.0.0.1:${server.address().port}/${game}/original-sfx.mjs`);
    const result=await page.evaluate(async game=>{
      const {OriginalSfx}=await import('./original-sfx.mjs');
      const {Fat12}=await import(game==='th03'?'./disk.js':'./solo-fat12.js');
      const response=await fetch('./disk.gz');
      const disk=new Uint8Array(await new Response(response.body.pipeThrough(new DecompressionStream('gzip'))).arrayBuffer());
      const sfx=await OriginalSfx.create(game,new Fat12(disk));
      const analyser=sfx.context.createAnalyser();sfx.node.connect(analyser);
      const samples=new Float32Array(analyser.fftSize);let peak=0,latency=0;
      await new Promise(resolve=>setTimeout(resolve,120));
      const started=performance.now();sfx.play(1);
      while(performance.now()-started<1000&&peak<0.001){
        await new Promise(resolve=>setTimeout(resolve,10));analyser.getFloatTimeDomainData(samples);
        for(const value of samples)peak=Math.max(peak,Math.abs(value));latency=performance.now()-started;
      }
      if(game==='th02'){
        sfx.lastShot=sfx.context.currentTime;
        const before=sfx.played;sfx.play(1);sfx.play(1);sfx.play(2);
        if(sfx.played!==before+1)throw Error('shot sound throttle did not preserve other effects');
        await new Promise(resolve=>setTimeout(resolve,150));sfx.play(1);
        if(sfx.played!==before+2)throw Error('shot sound did not resume');
      }
      sfx.dispose();return {peak,latency};
    },game);
    assert(result.peak>0.001&&result.latency<500,`${game}: original sound effects ${JSON.stringify(result)}`);
    console.log(`${game}: original sound effects PASS (peak ${result.peak.toFixed(4)}, ${Math.round(result.latency)} ms)`);await page.close();
  }
}finally{await browser.close();await new Promise(resolve=>server.close(resolve));}
