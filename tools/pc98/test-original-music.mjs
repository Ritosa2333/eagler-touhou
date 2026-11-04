import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createServer} from 'node:http';
import {resolve,sep} from 'node:path';
import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../launcher/lib/chromium-executable.mjs';
const root=resolve(import.meta.dirname,'../..'),workspace=resolve(root,'..');
const disks={th02:'th02-coop-lab/web/th02-solo-jp.hdi.gz',th03:'th03-netplay-lab/web/disks/3-jp.hdi.gz',th04:'th04-coop-lab/web/th04-solo-jp.hdi.gz'};
const server=createServer(async(req,res)=>{
  const match=/^\/(th0[234])\/(.+)$/.exec(decodeURIComponent(req.url));if(!match)return res.writeHead(404).end();
  const [,game,name]=match,base=resolve(root,'ports',game,'build-eagler');
  const file=name==='disk.gz'?resolve(workspace,disks[game]):resolve(base,name);
  if(name!=='disk.gz'&&!file.startsWith(base+sep))return res.writeHead(400).end();
  try{const bytes=await readFile(file);res.writeHead(200,{'Content-Type':name.endsWith('.wasm')?'application/wasm':/\.(m?js)$/.test(name)?'text/javascript':name.endsWith('.json')?'application/json':'application/octet-stream'});res.end(bytes);}catch{res.writeHead(404).end();}
});
await new Promise(done=>server.listen(0,'127.0.0.1',done));
const require=createRequire(new URL('../../launcher/package.json',import.meta.url)),puppeteer=require('puppeteer-core');
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--autoplay-policy=no-user-gesture-required']});
try{
 for(const game of Object.keys(disks)){
  const page=await browser.newPage();await page.goto(`http://127.0.0.1:${server.address().port}/${game}/pmd-player.mjs`);
  const result=await page.evaluate(async game=>{
   const {Fat12}=await import(game==='th03'?'./disk.js':'./solo-fat12.js');
   const {extractOriginalPmd}=await import('./pmd-archives.mjs');const {PmdPlayer}=await import('./pmd-player.mjs');
   const {LocalBgm}=await import(game==='th03'?'./local-bgm.js':'./netplay/local-bgm.js');
   const response=await fetch('./disk.gz'),disk=new Uint8Array(await new Response(response.body.pipeThrough(new DecompressionStream('gzip'))).arrayBuffer());
   const context=new AudioContext({sampleRate:44100}),player=new LocalBgm(context,()=>{});await context.resume();
   player.pmd={tracks:extractOriginalPmd(game,new Fat12(disk)),engine:await PmdPlayer.prepare(context,location.href)};
   await player.load({warmOpening:false});const analyser=context.createAnalyser();player.volume.connect(analyser);
   const hash=Number(Object.keys(player.manifest.headers)[0]);player.play(hash);
   const samples=new Float32Array(analyser.fftSize);let peak=0;
   for(let i=0;i<80;i++){await new Promise(r=>setTimeout(r,50));analyser.getFloatTimeDomainData(samples);for(const value of samples)peak=Math.max(peak,Math.abs(value));if(peak>.001)break;}
   const status=player.status;player.dispose();await context.close();return {peak,status};
  },game);
  assert(result.peak>.001&&result.status.startsWith('原版 FM'),`${game}: ${JSON.stringify(result)}`);
  console.log(`${game}: original game music PASS (peak ${result.peak.toFixed(4)})`);await page.close();
 }
}finally{await browser.close();await new Promise(done=>server.close(done));}
