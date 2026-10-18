// Loopback-only test harness: expose a fixed allowlist, never the workspace.
import {createServer} from 'node:http';
import {readFile,readdir} from 'node:fs/promises';
import {fileURLToPath} from 'node:url';
import {resolve,extname} from 'node:path';
const root=fileURLToPath(new URL('../',import.meta.url));
const files=new Map([
 ['/', 'sdl-runtime/th11.html'],['/app/style.css','sdl-runtime/style.css'],
 ['/app/shell.mjs','sdl-runtime/shell-harness.mjs'],['/app/frame-clock.mjs','sdl-runtime/frame-clock.mjs'],
 ['/th11-harness.mjs','artifacts/sdl3/th11-harness.mjs'],['/th11-harness.wasm','artifacts/sdl3/th11-harness.wasm'],
 ['/th11.dat','../[th11] 东方地灵�?(汉化�?日文�?/th11.dat']
]);
files.set('/music-index.json','assets/sdl-native/music-verification.json');
files.set('/fonts-index.json','assets/sdl-native/fonts/manifest.json');
for(const name of ['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'])files.set('/fonts/'+name,'assets/sdl-native/fonts/'+name);
for(const name of await readdir(resolve(root,'assets/sdl-native/music')))if(/^[a-z0-9_]+\.ogg$/.test(name))files.set('/music/'+name,'assets/sdl-native/music/'+name);
const types={'.html':'text/html; charset=utf-8','.mjs':'text/javascript','.css':'text/css','.wasm':'application/wasm'};
createServer(async(req,res)=>{
 try{
  if(!['GET','HEAD'].includes(req.method)){res.writeHead(405).end();return;}
  const name=files.get(new URL(req.url,'http://localhost').pathname);
  if(!name){res.writeHead(404).end();return;}
  const bytes=await readFile(resolve(root,name));
  res.writeHead(200,{'Content-Type':types[extname(name)]||'application/octet-stream','Content-Length':bytes.length,'Cache-Control':'no-store','Cross-Origin-Opener-Policy':'same-origin','Cross-Origin-Embedder-Policy':'require-corp'});
  res.end(req.method==='HEAD'?undefined:bytes);
 }catch(e){res.writeHead(500).end('Harness resource unavailable');console.error(e.message);}
}).listen(8112,'127.0.0.1',()=>console.log('TH11 SDL harness http://127.0.0.1:8112'));
