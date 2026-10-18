import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync,openSync,readSync,closeSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const out='th10_web/artifacts/architecture-candidate/validation';mkdirSync(out,{recursive:true});
const browser=await launchBrowser(),context=await browser.newContext(),page=await context.newPage(),errors=[];
page.on('pageerror',e=>errors.push(e.stack));
const replay=readFileSync('th10_web/artifacts/sdl3/motion/recording.rpyx'),base='http://127.0.0.1:8093';
async function open(){await page.goto(base+'/?lang=chs&debug=1');await page.locator('.game[data-game="th10"]').click();await page.locator('#launch').click();await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.__th10Runtime?.app,null,{timeout:120000});return page.frames().find(f=>f.url().includes('/runtime/th10/'));}
try{
 await page.goto(base+'/?debug=1');await page.evaluate(async bytes=>{for(const language of ['jp','chs'])await new Promise((resolve,reject)=>{const request=indexedDB.open('th10-1.00a-'+language,1);request.onupgradeneeded=()=>request.result.createObjectStore('files');request.onsuccess=()=>{const db=request.result,tx=db.transaction('files','readwrite');tx.objectStore('files').put(new Uint8Array(bytes),'replay/th10_01.rpy');tx.oncomplete=()=>{db.close();resolve();};tx.onerror=()=>reject(tx.error);};});},Array.from(replay));
 let f=await open();const migrated=await f.evaluate(()=>__th10Runtime.command({command:'read',path:'replay/th10_01.rpyx'}));assert(Buffer.from(migrated.bytes).equals(replay));
 const lists=await f.evaluate(()=>__th10Runtime.command({command:'list'}));assert(lists.files.some(f=>f.path==='replay/th10_01.rpyx'));
 await f.evaluate(bytes=>__th10Runtime.command({command:'write',path:'replay/th10_02.rpyx',bytes}),Array.from(replay));
 // SDL_IO + miniaudio must preserve PCM across unaligned reads, end-of-track
 // boundaries, backward loop seeks and all 18 original music offsets.
 const entries=JSON.parse(readFileSync('th10_web/assets/sdl-native/music-layout.json','utf8')),fd=openSync('[th10] 东方风神录 (汉化版+日文版)/thbgm.dat','r');
 const tracks=Array.isArray(entries)?entries:entries.tracks;
 const ranges=[];for(const t of tracks){const start=t.offset??t.start,length=t.length??t.size;ranges.push([start,4096],[start+123,8191],[start+length-1027,1027]);}ranges.push(...ranges.slice(0,6).reverse());
 const expected=[];try{for(const [offset,length] of ranges){const b=Buffer.alloc(length);assert.equal(readSync(fd,b,0,length,offset),length);expected.push(createHash('sha256').update(b).digest('hex'));}}finally{closeSync(fd);}
 const actual=await f.evaluate(async ranges=>{const c=__th10Runtime.core,name=new TextEncoder().encode('thbgm.dat\0'),p=c.graphics_allocate(16384),hex=b=>Array.from(new Uint8Array(b),x=>x.toString(16).padStart(2,'0')).join('');new Uint8Array(c.memory.buffer,p,name.length).set(name);const file=c.sdl_file_open(p,0),hashes=[];try{for(const [offset,length] of ranges){if(c.sdl_file_seek(file,offset,0)!==offset||c.sdl_file_read(file,p,length)!==length)throw Error('Native music read failed');hashes.push(hex(await crypto.subtle.digest('SHA-256',new Uint8Array(c.memory.buffer,p,length))));}}finally{c.sdl_file_close(file);c.graphics_free(p);}return hashes;},ranges);assert.deepEqual(actual,expected);
 await f.evaluate(()=>__th10Runtime.stop());await page.waitForFunction(()=>!document.querySelector('#player')?.classList.contains('open'));
 await page.evaluate(async()=>{const registration=await navigator.serviceWorker.ready;if(registration.installing)await new Promise(resolve=>registration.installing.addEventListener('statechange',resolve));});
 await context.setOffline(true);f=await open();const offline=await f.evaluate(()=>__th10Runtime.command({command:'read',path:'replay/th10_02.rpyx'}));assert(Buffer.from(offline.bytes).equals(replay));
 await f.evaluate(()=>__th10Runtime.command({command:'remove',path:'replay/th10_02.rpyx'}));await f.evaluate(()=>__th10Runtime.stop());f=await open();const after=await f.evaluate(()=>__th10Runtime.command({command:'list'}));assert(!after.files.some(f=>f.path==='replay/th10_02.rpyx'));
 await f.evaluate(()=>__th10Runtime.stop());await context.setOffline(false);await page.evaluate(()=>localStorage.setItem('eagler-touhou-language-v1-th10','ja'));f=await open();const japanese=await f.evaluate(()=>__th10Runtime.command({command:'read',path:'replay/th10_01.rpyx'}));assert(Buffer.from(japanese.bytes).equals(replay));
 assert.equal(errors.length,0,errors.join('\n'));writeFileSync(out+'/storage-audio.json',JSON.stringify({passed:true,checks:['legacy JP/CHS migration','rpyx byte-for-byte persistence','offline reload and launch','durable removal','JP/CHS storage separation','native lossless PCM seek/read'],pcmRanges:ranges.length,errors},null,2));console.log('Storage, offline and native PCM checks passed');
}catch(e){writeFileSync(out+'/storage-failure.json',JSON.stringify({error:e.stack,errors},null,2));throw e;}finally{await browser.close();}
