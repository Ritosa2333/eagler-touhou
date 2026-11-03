import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createServer} from 'node:http';
import {resolve} from 'node:path';
import {createRequire} from 'node:module';
import {gunzipSync} from 'node:zlib';
import {Fat12} from '../../ports/th03/web/disk.js';
import {extractOriginalPmd} from '../../common/pc98/pmd-archives.mjs';
import {findChromiumExecutable} from '../../launcher/lib/chromium-executable.mjs';

const require=createRequire(new URL('../../launcher/package.json',import.meta.url));
const puppeteer=require('puppeteer-core');
const workspace=resolve(import.meta.dirname,'../../..');
const disk=new Uint8Array(gunzipSync(await readFile(resolve(workspace,'th03-netplay-lab/web/disks/3-jp.hdi.gz'))));
const songs=extractOriginalPmd('th03',new Fat12(disk));
const song=[...songs.values()].find(track=>track.name==='00mm.m').bytes;
const wasm=await readFile(process.argv[2]);
const files={
  '/pmd-player.wasm':[wasm,'application/wasm'],
  '/pmd-worklet.js':[await readFile(new URL('../../common/pc98/pmd-worklet.js',import.meta.url)),'text/javascript'],
  '/pmd-player.mjs':[await readFile(new URL('../../common/pc98/pmd-player.mjs',import.meta.url)),'text/javascript'],
  '/song':[song,'application/octet-stream'],
};
const server=createServer((request,response)=>{const entry=files[request.url];if(!entry){response.writeHead(404).end();return;}response.writeHead(200,{'Content-Type':entry[1]});response.end(entry[0]);});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--autoplay-policy=no-user-gesture-required']});
try{
  const page=await browser.newPage();await page.goto(`http://127.0.0.1:${server.address().port}/pmd-player.mjs`);
  const result=await page.evaluate(async()=>{
    const {PmdPlayer}=await import('/pmd-player.mjs'),context=new AudioContext({sampleRate:48000});await context.resume();
    const player=await PmdPlayer.prepare(context,location.href),source=await player.createSource(await(await fetch('/song')).arrayBuffer());
    const analyser=context.createAnalyser();source.connect(analyser);analyser.connect(context.destination);
    let peak=0;const samples=new Float32Array(analyser.fftSize);
    for(let i=0;i<50;i++){await new Promise(resolve=>setTimeout(resolve,20));analyser.getFloatTimeDomainData(samples);for(const value of samples)peak=Math.max(peak,Math.abs(value));}
    source.stop(context.currentTime);await context.close();return peak;
  });
  assert(result>0.001,`original PMD track must produce sound, peak=${result}`);
  console.log(`PMD AudioWorklet original track: PASS (peak ${result.toFixed(4)})`);
}finally{await browser.close();await new Promise(resolve=>server.close(resolve));}
