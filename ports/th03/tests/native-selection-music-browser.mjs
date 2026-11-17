import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';

const root=resolve(import.meta.dirname,'../build-eagler'),data=resolve(import.meta.dirname,'../content/th03.data');
const server=createServer(async(req,res)=>{
 try {
  const path=new URL(req.url,'http://localhost').pathname;
  if(path==='/')return res.end('<!doctype html><script src="/native-classic.js"></script>');
  const file=path==='/th03.data'?data:resolve(root,'.'+path);
  if(file!==data&&!file.startsWith(root+sep))throw Error('invalid path');
  const type={'.wasm':'application/wasm','.mjs':'text/javascript','.js':'text/javascript','.json':'application/json','.opus':'audio/ogg'}[extname(file)]||'application/octet-stream';
  const bytes=await readFile(file);res.writeHead(200,{'Content-Type':type});res.end(bytes);
 }catch{res.writeHead(404).end();}
});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url));
const browser=await require('puppeteer-core').launch({executablePath:await findChromiumExecutable(),headless:true,args:['--autoplay-policy=no-user-gesture-required']});
try {
 const page=await browser.newPage();await page.goto(`http://127.0.0.1:${server.address().port}/`);
 const results=await page.evaluate(async()=>{
  const {ManagedContent}=await import('/managed-data.mjs');
  const {bootToSelection}=await import('/managed-native-room.mjs');
  const {createNativeMusic}=await import('/native-music-player.mjs');
  const content=await ManagedContent.open(await (await fetch('/th03.data')).arrayBuffer());
  const context=new AudioContext({sampleRate:48000});await context.resume();const result=[];
  for(const source of ['original','akyu']){
   const engine=await createTh03Classic({locateFile:name=>'/native-'+name});
   engine.FS.mkdir('/content');engine.FS.mkdir('/user');engine.FS.mkdir('/user/REPLAY');
   for(const path of content.files.keys()){
    if(!path.startsWith('native-th03/'))continue;
    const name=path.slice('native-th03/'.length),parts=name.split('/');let dir='/content';
    for(const part of parts.slice(0,-1)){dir+='/'+part;if(!engine.FS.analyzePath(dir).exists)engine.FS.mkdir(dir);}
    engine.FS.writeFile('/content/'+name,new Uint8Array(await (await content.fetch(path)).arrayBuffer()));
   }
   const errors=[],music=createNativeMusic({source,readTrack:id=>engine.FS.readFile('/content/YUME1.DAT/'+id.toUpperCase()+'.M'),onError:e=>errors.push(e.message)});
   music.unlock(context);engine._th03_classic_external_music(1);
   if(engine._th03_classic_create_versus()!==1)throw Error('native versus create failed');
   const before=Array.from(engine.HEAPU8.slice(engine._th03_classic_audio_commands(),engine._th03_classic_audio_commands()+engine._th03_classic_audio_command_count()*4));
   bootToSelection(engine,music);
   const until=performance.now()+10000;while(!music.playing&&performance.now()<until&&!errors.length)await new Promise(r=>setTimeout(r,50));
   result.push({source,before,scene:engine._th03_classic_scene(),track:music.track,playing:music.playing,errors});
   music.stop();engine._th03_classic_destroy();
  }
  await context.close();return result;
 });
 for(const item of results){assert.equal(item.scene,3);assert.equal(item.track,'select');assert.equal(item.playing,true,JSON.stringify(item));assert.deepEqual(item.errors,[]);assert(item.before.length>0);console.log('PASS native selection music',item.source,item.before);}
}finally{await browser.close();await new Promise(resolve=>server.close(resolve));}
