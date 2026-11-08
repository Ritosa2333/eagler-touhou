import http from 'node:http';
import {readFile,stat,realpath} from 'node:fs/promises';
import {createReadStream} from 'node:fs';
import {resolve,sep,extname,dirname} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {randomBytes} from 'node:crypto';
import {spawn} from 'node:child_process';
import {decodeTh19Replay} from '../../web/sdl-runtime/th19-recording.mjs';
const here=dirname(fileURLToPath(import.meta.url));
const mime={'.html':'text/html; charset=utf-8','.mjs':'text/javascript; charset=utf-8','.json':'application/json','.wasm':'application/wasm','.ogg':'audio/ogg','.css':'text/css','.ttf':'font/ttf'};
export async function startReplayViewer({root=resolve(here,'../..'),file,port=0}={}){
 root=await realpath(root);
 const config=JSON.parse(await readFile(resolve(root,'game-resources.json'),'utf8'));
 const manifest=JSON.parse(await readFile(resolve(root,'runtime/manifest.json'),'utf8'));
 let replay=null;
 if(file){
  if((await stat(file)).size>32*1024*1024)throw Error('录像超过32 MiB');
  replay=await readFile(file);
  decodeTh19Replay(replay);
  // Replay files from prior compatible web builds are accepted after full decode validation.
 }
 const token=randomBytes(24).toString('hex'),prefix=`/${token}/`;
 const server=http.createServer(async(req,res)=>{
  try{
   const origin=`http://127.0.0.1:${server.address().port}`;
   if(req.headers.host!==new URL(origin).host){res.writeHead(403).end();return;}
   if(!['GET','HEAD'].includes(req.method)){res.writeHead(405).end();return;}
   const url=new URL(req.url,origin);
   if(!url.pathname.startsWith(prefix)){res.writeHead(404).end();return;}
   const name=decodeURIComponent(url.pathname.slice(prefix.length));
   res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');res.setHeader('Cache-Control','no-store');
   let bytes,type='application/octet-stream',path;
   if(name===''){bytes=await readFile(resolve(here,'viewer.html'));type=mime['.html'];}
   else if(name==='viewer.mjs'){bytes=await readFile(resolve(here,'viewer.mjs'));type=mime['.mjs'];}
   else if(name==='session.json'){bytes=Buffer.from(JSON.stringify({build:manifest.buildId,config,hasReplay:!!replay}));type=mime['.json'];}
   else if(name==='recording.rpy'&&replay){bytes=replay;}
   else {
    if(!/^(runtime\/|content\/)/.test(name)||name.includes('\\')||name.split('/').includes('..')){res.writeHead(404).end();return;}
    path=await realpath(resolve(root,name));
    if(!path.startsWith(root+sep)){res.writeHead(403).end();return;}
    type=mime[extname(path)]||type;
   }
   res.setHeader('Content-Type',type);
   res.setHeader('Content-Length',bytes?bytes.length:(await stat(path)).size);
   res.writeHead(200);
   if(req.method==='HEAD')res.end();else if(bytes)res.end(bytes);else createReadStream(path).on('error',()=>res.destroy()).pipe(res);
  }catch(error){if(!res.headersSent)res.writeHead(error.code==='ENOENT'?404:400);res.end('无法读取资源');}
 });
 await new Promise((ok,no)=>{server.once('error',no);server.listen(port,'127.0.0.1',ok);});
 return {server,url:`http://127.0.0.1:${server.address().port}${prefix}`};
}
if(process.argv[1]&&import.meta.url===pathToFileURL(process.argv[1]).href){
 try{
  const args=process.argv.slice(2),rootIndex=args.indexOf('--root');
  const root=rootIndex<0?undefined:args.splice(rootIndex,2)[1];
  const noOpen=args.includes('--no-open');if(noOpen)args.splice(args.indexOf('--no-open'),1);
  if(args.length>1)throw Error('Usage: node viewer-server.mjs [FILE.rpy] [--root GAME_DIRECTORY] [--no-open]');
  const {url}=await startReplayViewer({root,file:args[0]});console.log(`录像播放器：${url}\n关闭此终端即可停止播放器服务。`);
  if(!noOpen){
   const child=process.platform==='win32'?spawn('rundll32.exe',['url.dll,FileProtocolHandler',url],{windowsHide:true,stdio:'ignore'}):spawn(process.platform==='darwin'?'open':'xdg-open',[url],{stdio:'ignore'});
   child.on('error',e=>console.error(`请手动打开上面的地址：${e.message}`));
  }
 }catch(error){console.error(error.message);process.exitCode=1;}
}
