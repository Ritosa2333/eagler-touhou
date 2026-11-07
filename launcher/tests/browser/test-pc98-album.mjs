import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {createHash} from 'node:crypto';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
for(const game of ['th02','th04']){
 const root=resolve('ports',game,'build-eagler'),music=resolve('ports',game,'content/music'),data=await readFile(resolve('ports',game,'content',`${game}.data`));
 const album=JSON.parse(await readFile(resolve(root,'bgm/akyu/manifest.json'))),tracks=[...new Set(Object.values(album.tracks).map(t=>t.url.split('/').at(-1)))];
 const html=`<iframe src="/runtime/${game}.html?hosted=1&managedData=1&runtimeEpoch=1"></iframe><script>window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});addEventListener('message',e=>events.push(e.data));window.rpc=(command,fields={})=>new Promise((ok,no)=>{const f=document.querySelector('iframe'),request=crypto.randomUUID(),timer=setTimeout(()=>no(Error('timeout '+command)),90000);function recv(e){if(e.source!==f.contentWindow||e.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',recv);e.data.ok?ok(e.data):no(Error(e.data.error));}addEventListener('message',recv);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:1,request,command,...fields},location.origin)});</script>`;
 const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;if(path==='/')return res.end(html);if(path==='/data')return res.end(data);let file;if(path.startsWith('/music/'))file=resolve(music,path.slice(7));else{file=resolve(root,path.slice(9));if(!path.startsWith('/runtime/')||!file.startsWith(root+sep))throw Error('path');}
  let bytes=await readFile(file);if(path==='/runtime/netplay/local-bgm.js')bytes=Buffer.from(bytes.toString().replace('const track=this.track(id);','const track=this.track(id);globalThis.__testBgm={id,url:track?.url,player:this};'));
  res.setHeader('content-type',({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript','.json':'application/json','.wasm':'application/wasm','.css':'text/css','.ogg':'audio/ogg'})[extname(file)]||'application/octet-stream');res.end(bytes);
 }catch{res.writeHead(404).end();}});await new Promise(r=>server.listen(0,'127.0.0.1',r));
 const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
 try{const page=await browser.newPage();await page.goto(`http://127.0.0.1:${server.address().port}/`);await page.waitForFunction(()=>events.some(e=>e.event==='ready'),{timeout:120000});
 await page.evaluate(()=>rpc('configure',{music:'ogg',language:'ja',options:{musicSource:'akyu'},resources:[],runtimeResources:[],sharedResources:[]}));
 const resources=[];for(const name of tracks){const b=await readFile(resolve(music,name));resources.push({path:'/bgm/akyu/'+name,bytes:b.length,sha256:createHash('sha256').update(b).digest('hex'),name});}
 await page.evaluate(async items=>{for(const item of items){const b=await(await fetch('/music/'+item.name)).blob();item.url=URL.createObjectURL(b);delete item.name;}await rpc('resources',{resources:items});},resources);
 await page.evaluate(()=>rpc('launch',{launchSerial:1}));await page.waitForFunction(()=>events.some(e=>e.event==='first-frame'),{timeout:120000});
 const frame=page.frames().find(f=>f.url().includes('runtimeEpoch=1'));await frame.waitForFunction(()=>globalThis.__testBgm?.url?.startsWith('akyu/')&&globalThis.__testBgm.player.active?.source,{timeout:60000});
 const played=await frame.evaluate(()=>({id:__testBgm.id,url:__testBgm.url,status:__testBgm.player.status}));assert(played.url.endsWith('.ogg'));console.log(game,'album selected',played.id,played.url);
 assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
 }finally{await browser.close();await new Promise(r=>server.close(r));}
}
