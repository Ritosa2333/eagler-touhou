import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import assert from 'node:assert/strict';
const b=await puppeteer.launch({executablePath:process.env.TH19_BROWSER||'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
try{const p=await b.newPage();await p.goto('http://127.0.0.1:8148/tests/harness.html?autolaunch=0');await p.waitForFunction(()=>__TH19_HARNESS?.configured,{timeout:180000});
const result=await p.evaluate(async()=>{await __TH19_HARNESS.command('launch');const c=__TH19_HARNESS.frame.contentWindow.__th19Runtime.core;c._th19_loop_stop();
// Silent test fixture verifies the requested music path without retail audio.
const wav=new Uint8Array(44+44100*4),v=new DataView(wav.buffer),text=(o,t)=>[...t].forEach((x,i)=>wav[o+i]=x.charCodeAt(0));text(0,'RIFF');v.setUint32(4,wav.length-8,true);text(8,'WAVE');text(12,'fmt ');v.setUint32(16,16,true);v.setUint16(20,1,true);v.setUint16(22,2,true);v.setUint32(24,44100,true);v.setUint32(28,176400,true);v.setUint16(32,4,true);v.setUint16(34,16,true);text(36,'data');v.setUint32(40,wav.length-44,true);c.FS.mkdirTree('/music');c.FS.writeFile('/music/th19_01.wav',wav);c._th19_music_enabled(1);
c._th19_network_select_begin(19019,2,0,1);let ptr=c._th19_audio_path(),end=ptr;while(c.HEAPU8[end])end++;const music=new TextDecoder().decode(c.HEAPU8.subarray(ptr,end));c._th19_network_end();c._th19_music_enabled(0);
c._th19_network_begin(19019,2,0,1);for(let i=0;i<400;i++)c._th19_network_step(i,0,0,0,0,0,0,0,0,0);
c._th19_touch_side(1);c._th19_touch_options(1,1,1,0,0);const read=()=>{const p=c._th19_test_cpu_input(-1)/4;return Array.from(c.HEAP32.subarray(p+4,p+6));};const before=read();
c._th19_touch(0,1,.5,.5);c._th19_touch(1,1,.55,.52);const inputs=[];
for(let i=0;i<8;i++){const p=c._th19_touch_sample(1);inputs.push({direct:c.HEAP32[p/4+1],...Object.fromEntries(['x','y'].map((k,j)=>[k,new DataView(c.HEAPU8.buffer).getFloat32(p+8+j*4,true)]))});}
const cp=c._th19_checkpoint_save();const run=()=>{for(let i=0;i<8;i++){const a=inputs[i];c._th19_network_step(400+i,0,0,0,0,a.x,a.y,0,a.direct,0);}return read();};const after=run(),hash=c._th19_network_hash();c._th19_checkpoint_load(cp);const replayed=run(),replayHash=c._th19_network_hash();c._th19_checkpoint_release(cp);return {music,before,inputs,after,replayed,hash,replayHash};});
assert.equal(result.music,'/music/th19_01');assert(Math.abs(result.inputs[0].x-32)<.001);assert(Math.abs(result.inputs[0].y-9.6)<.001);for(const a of result.inputs.slice(1)){assert.equal(a.x,0);assert.equal(a.y,0);}assert(Math.abs(result.after[0]-result.before[0]-32000)<2);assert(Math.abs(result.after[1]-result.before[1]-9600)<2);assert.deepEqual(result.after,result.replayed);assert.equal(result.hash,result.replayHash);console.log(JSON.stringify(result));
}finally{await b.close();}
