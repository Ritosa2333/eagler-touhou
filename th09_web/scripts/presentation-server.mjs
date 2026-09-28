// Private development server: explicit file allowlist, loopback only.
import http from 'node:http';
import {attachNetplay} from './netplay-relay.mjs';
import {createHash} from 'node:crypto';
import {createReadStream,statSync,readdirSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const root=fileURLToPath(new URL('../',import.meta.url)),workspace=resolve(root,'..');
export async function presentationServer(port=0,{release=false,artifactDirectory}={}){
 const compiled=artifactDirectory?resolve(artifactDirectory):resolve(root,release?'artifacts/sdl-release':'artifacts/sdl3');
 const files=new Map([
  ['/',resolve(root,'tests/browser/presentation.html')],
  ['/probe.mjs',resolve(compiled,release?'th09.mjs':'th09-presentation.mjs')],
  [release?'/th09.wasm':'/th09-presentation.wasm',resolve(compiled,release?'th09.wasm':'th09-presentation.wasm')],
  ['/th09.dat',resolve(workspace,'[th09] 东方花映塚 (日文版)/th09.dat')],
 ]);
 for(const name of ['th09.html','style.css','shell.mjs','keyboard.mjs','netplay.mjs','shared-netplay.mjs','motion-replay.mjs'])files.set('/app/'+name,resolve(root,'sdl-runtime',name));
 for(const name of ['cp932.bin','blend.bin','msgothic.ttc'])files.set('/'+name,resolve(root,'assets/sdl-native',name));
 const music=readdirSync(resolve(root,'assets/sdl-native/music')).filter(n=>n.endsWith('.ogg')).sort();
 for(const name of music)files.set('/music/'+name,resolve(root,'assets/sdl-native/music',name));
 const resources=[...files].filter(([url])=>url==='/th09.dat'||url.startsWith('/music/')||['/cp932.bin','/blend.bin','/msgothic.ttc'].includes(url)).map(([url,path])=>({url,path:url.startsWith('/music/')||url==='/th09.dat'?url:'/fonts'+url,bytes:statSync(path).size,sha256:createHash('sha256').update(readFileSync(path)).digest('hex')}));
 const server=http.createServer((req,res)=>{
  const path=new URL(req.url,'http://127.0.0.1').pathname;
  res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');res.setHeader('Cache-Control','no-store');
  if(req.method!=='GET'&&req.method!=='HEAD'){res.writeHead(405).end();return;}
  if(path==='/version.json'||path==='/app/version.json'){res.setHeader('Content-Type','application/json');res.end(JSON.stringify({game:'th09',build:JSON.parse(readFileSync(resolve(compiled,'build.json'),'utf8')).sha256.slice(0,24)}));return;}
  if(path==='/'&&release){const html=readFileSync(files.get('/'),'utf8').replace('core._th09_title_open()','core._th09_game_open(12345)').replaceAll('core._th09_probe_error()','core._th09_error()').replace(/window.probeStatus=.*?;/,'window.probeStatus=()=>[];');res.setHeader('Content-Type','text/html; charset=utf-8');res.end(html);return;}
  if(path==='/resources.json'){res.setHeader('Content-Type','application/json');res.end(JSON.stringify(resources));return;}
  if(path==='/music.json'){res.setHeader('Content-Type','application/json');res.end(JSON.stringify(music));return;}
  const file=files.get(path);if(!file){res.writeHead(404).end();return;}
  const type=path.endsWith('.mjs')?'text/javascript':path.endsWith('.wasm')?'application/wasm':path.endsWith('.css')?'text/css':path==='/'||path.endsWith('.html')?'text/html; charset=utf-8':'application/octet-stream';
  res.setHeader('Content-Type',type);res.setHeader('Content-Length',statSync(file).size);if(req.method==='HEAD'){res.end();return;}createReadStream(file).pipe(res);
 });
 const netplay=attachNetplay(server,{build:JSON.parse(readFileSync(resolve(compiled,'build.json'),'utf8')).sha256.slice(0,24)});
 await new Promise((accept,reject)=>{server.once('error',reject);server.listen(port,'127.0.0.1',accept);});
 return {server,netplay,url:'http://127.0.0.1:'+server.address().port};
}
if(process.argv[1]&&resolve(process.argv[1])===fileURLToPath(import.meta.url)){const {url}=await presentationServer(Number(process.argv[2]??8096));console.log(url);}
