import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import {unzipSync} from 'fflate';
export async function multiplayerFixture(count=3){
 const repo=resolve(import.meta.dirname,'../../..'),root=resolve(repo,'ports/th05-mp');
 const runtime=resolve(process.env.TH05_TEST_RUNTIME??resolve(root,'artifacts/test-runtime'));
 const assets=resolve(process.env.TH05_TEST_ASSETS??resolve(repo,'ports/th05/artifacts'));
 const font=resolve(process.env.TH05_TEST_FONT??resolve(repo,'dependencies/unifont-15.1.05/unifont-15.1.05.otf'));
 const fixture=`<!doctype html><body><script>
 window.events=[];window.responses=new Map();window.serial=0;
 window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
 addEventListener('message',e=>{events.push(e.data);if(e.data.request){responses.get(e.data.request)?.(e.data);responses.delete(e.data.request);}});
 window.command=(seat,command,payload={})=>new Promise(resolve=>{const request='r'+(++serial);responses.set(request,resolve);document.querySelectorAll('iframe')[seat].contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th05',epoch:7+seat,command,request,...payload},location.origin);});
 </script>${Array.from({length:count},(_,seat)=>`<iframe width="640" height="480" src="/runtime/th05.html?runtimeEpoch=${7+seat}&managedData=1&multiplayer=1"></iframe>`).join('')}`;
 const server=createServer(async(req,res)=>{try{
  const path=new URL(req.url,'http://test').pathname;let data,type;
  if(path==='/'){data=fixture;type='text/html';}
  else if(path==='/language.json'){
   const zip=unzipSync(await readFile(resolve(assets,'languages/language/lang_zh-hans.zip')));
   data=JSON.stringify({language:'lang_zh-hans',url:'/language.zip',manifest:JSON.parse(new TextDecoder().decode(zip['manifest.json'])),files:[{path:'/thcrap/th05/chs.dat',bytes:Array.from(zip['thcrap/th05/chs.dat'])}]});type='application/json';
  }
  else if(path==='/data')data=await readFile(resolve(assets,'content/th05.data'));
  else if(path==='/font')data=await readFile(font);
  else{const name=path.replace(/^\/runtime\//,'');if(name.includes('..')||name.startsWith('/'))throw Error('path');data=await readFile(resolve(runtime,name));type=({'.mjs':'text/javascript','.html':'text/html','.wasm':'application/wasm'})[extname(name)];}
  res.writeHead(200,{'Content-Type':type??'application/octet-stream'});res.end(data);
 }catch(error){res.writeHead(404);res.end(error.message);}});
 await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
 return {url:`http://127.0.0.1:${server.address().port}`,close:()=>new Promise(resolve=>server.close(resolve))};
}
