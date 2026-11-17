import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';

const content=process.env.TH03_NATIVE_CONTENT;
if(!content)throw Error('Set TH03_NATIVE_CONTENT to the native game content directory');
const web=resolve(import.meta.dirname,'../web');
const server=createServer(async(req,res)=>{
 try{
  const path=new URL(req.url,'http://localhost').pathname;
  if(path==='/')return res.end('<!doctype html><title>TH03 music continuity</title>');
  const file=path==='/track.m'?resolve(content,'YUME1.DAT/SELECT.M'):resolve(web,'.'+path);
  if(path!=='/track.m'&&!file.startsWith(web+'\\')&&!file.startsWith(web+'/'))throw Error('Invalid path');
  const type={'.wasm':'application/wasm','.mjs':'text/javascript','.js':'text/javascript','.json':'application/json','.opus':'audio/ogg'}[extname(file)]||'application/octet-stream';
  const bytes=await readFile(file);res.writeHead(200,{'Content-Type':type});res.end(bytes);
 }catch{res.writeHead(404).end();}
});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url));
const browser=await require('puppeteer-core').launch({executablePath:await findChromiumExecutable(),headless:true,args:['--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling']});
try{
 const page=await browser.newPage();await page.goto(`http://127.0.0.1:${server.address().port}/`);
 const results=await page.evaluate(async()=>{
  const {createNativeMusic}=await import('/native-music-player.mjs');
  const context=new AudioContext({sampleRate:48000});await context.resume();
  const probe=`class Probe extends AudioWorkletProcessor{constructor(){super();this.n=0;this.sum=0;this.blocks=0}process(inputs,outputs){const input=inputs[0],output=outputs[0];for(let c=0;c<output.length;c++){const source=input[c]||input[0];if(source){output[c].set(source);for(const v of source)this.sum+=v*v;this.n+=source.length}}if(++this.blocks===Math.round(sampleRate/128/10)){this.port.postMessage({time:currentTime,rms:Math.sqrt(this.sum/this.n)});this.blocks=0;this.sum=0;this.n=0}return true}}registerProcessor('probe',Probe);`;
  await context.audioWorklet.addModule(URL.createObjectURL(new Blob([probe],{type:'text/javascript'})));
  const measure=new AudioWorkletNode(context,'probe',{numberOfInputs:1,numberOfOutputs:1,outputChannelCount:[2]});measure.connect(context.destination);
  const samples=[];measure.port.onmessage=e=>samples.push(e.data);
  const originalConnect=AudioNode.prototype.connect,OriginalAudio=window.Audio;
  AudioNode.prototype.connect=function(target,...args){return originalConnect.call(this,target===context.destination&&this!==measure?measure:target,...args);};
  window.Audio=function(){const element=new OriginalAudio();context.createMediaElementSource(element).connect(measure);return element;};
  const bytes=new Uint8Array(await (await fetch('/track.m')).arrayBuffer());
  const output={};
  for(const source of ['original','akyu']){
   const music=createNativeMusic({source,readTrack:()=>bytes});music.unlock(context);music.commands(Uint8Array.from([0,2,0,0,1,2,0,0]));
   const until=performance.now()+10000;while(!music.playing&&performance.now()<until)await new Promise(r=>setTimeout(r,50));
   if(!music.playing)throw Error(source+' did not start');
   await new Promise(r=>setTimeout(r,1200));const start=context.currentTime;
   const end=performance.now()+1200;while(performance.now()<end){}
   const finish=context.currentTime;await new Promise(r=>setTimeout(r,400));
   output[source]={duration:finish-start,windows:samples.filter(s=>s.time>=start&&s.time<finish).map(s=>s.rms)};music.stop();await new Promise(r=>setTimeout(r,200));
  }
  AudioNode.prototype.connect=originalConnect;window.Audio=OriginalAudio;await context.close();return output;
 });
 for(const [source,result] of Object.entries(results)){
  assert.ok(result.duration>1,`${source}: audio clock continued during the stall`);
  assert.ok(result.windows.length>=9,`${source}: audio worklet rendered during the stall`);
  assert.ok(result.windows.filter(value=>value>0.00001).length>=result.windows.length-1,`${source}: music must remain audible throughout the stall: ${JSON.stringify(result)}`);
  console.log(source,'continued through main thread stall',result);
 }
}finally{await browser.close();await new Promise(resolve=>server.close(resolve));}
