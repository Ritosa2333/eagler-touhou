import http from 'node:http';
import {createReadStream} from 'node:fs';
import {stat} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {spawn} from 'node:child_process';
import {networkInterfaces} from 'node:os';

const mime={'.html':'text/html; charset=utf-8','.mjs':'text/javascript; charset=utf-8','.js':'text/javascript; charset=utf-8','.css':'text/css; charset=utf-8','.json':'application/json; charset=utf-8','.wasm':'application/wasm','.png':'image/png','.webp':'image/webp','.svg':'image/svg+xml','.ico':'image/x-icon','.ogg':'audio/ogg','.ttf':'font/ttf','.woff2':'font/woff2','.webmanifest':'application/manifest+json'};

export function startServer({routes,port=Number(process.argv[2]||8130),host='0.0.0.0'}){
 if(!Number.isInteger(port)||port<1||port>65535)throw Error('Invalid port');
 const mounts=routes.map(r=>({...r,directory:resolve(r.directory)}));
 const server=http.createServer(async(req,res)=>{
  try{
   if(req.method!=='GET'&&req.method!=='HEAD'){res.writeHead(405);return res.end();}
   const url=new URL(req.url,'http://localhost'),pathname=decodeURIComponent(url.pathname);
   if(pathname.includes('\0')||pathname.split('/').some(p=>p.startsWith('.'))){res.writeHead(403);return res.end();}
   for(const mount of mounts)if(mount.prefix!=='/'&&pathname===mount.prefix.slice(0,-1)){res.writeHead(302,{Location:mount.prefix+url.search});return res.end();}
   const mount=mounts.find(r=>pathname.startsWith(r.prefix));
   if(!mount){res.writeHead(404);return res.end();}
   const suffix=pathname.slice(mount.prefix.length),path=resolve(mount.directory,suffix||'index.html');
   if(!path.startsWith(mount.directory+sep)){res.writeHead(403);return res.end();}
   const info=await stat(path);if(!info.isFile()){res.writeHead(404);return res.end();}
   let first=0,last=info.size-1,status=200;
   const headers={'Content-Type':mime[extname(path)]||'application/octet-stream','Cache-Control':'no-cache','Accept-Ranges':'bytes','Cross-Origin-Opener-Policy':'same-origin','Cross-Origin-Embedder-Policy':'require-corp'};
   if(req.headers.range){const m=/^bytes=(\d+)-(\d*)$/.exec(req.headers.range);if(!m||Number(m[1])>=info.size){res.writeHead(416,{'Content-Range':`bytes */${info.size}`});return res.end();}first=Number(m[1]);last=m[2]?Math.min(last,Number(m[2])):last;if(last<first){res.writeHead(416);return res.end();}status=206;headers['Content-Range']=`bytes ${first}-${last}/${info.size}`;}
   headers['Content-Length']=Math.max(0,last-first+1);res.writeHead(status,headers);
   if(req.method==='HEAD')return res.end();createReadStream(path,{start:first,end:last}).on('error',e=>res.destroy(e)).pipe(res);
  }catch{if(!res.headersSent)res.writeHead(404);res.end('Not found');}
 });
 server.listen(port,host,()=>{
  const url=`http://127.0.0.1:${port}/`;console.log(`TH19: ${url}`);
  console.log(`Listening on ${host}:${port}`);
  if(host==='0.0.0.0')for(const list of Object.values(networkInterfaces()))for(const address of list||[])if(address.family==='IPv4'&&!address.internal)console.log(`LAN: http://${address.address}:${port}/`);
  if(process.argv.includes('--open')&&process.platform==='win32'){const child=spawn('rundll32.exe',['url.dll,FileProtocolHandler',url],{windowsHide:true,detached:true,stdio:'ignore'});child.on('error',e=>console.error(e.message));child.unref();}
 }).on('error',e=>{console.error(e.message);process.exitCode=1;});
 return server;
}
