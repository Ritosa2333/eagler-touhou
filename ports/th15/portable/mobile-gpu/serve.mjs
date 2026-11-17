import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'../..');
const input=process.env.TH15_CAPTURE_INPUT,fonts=process.env.TH15_CAPTURE_FONTS;
if(!input||!fonts)throw Error('Set TH15_CAPTURE_INPUT and TH15_CAPTURE_FONTS (private operator resources)');
const build=JSON.parse(await readFile(resolve(root,'web/artifacts/sdl-application/build.json'),'utf8'));
if(build.kind!=='th15-sdl3-application-integration-development'||!build.features.thprac)throw Error('Build development --thprac first');
const wasm=await readFile(resolve(root,'web/artifacts/sdl-application/th15-application.wasm'));
if(createHash('sha256').update(wasm).digest('hex')!==build.sha256)throw Error('Development Wasm identity mismatch; rebuild first');
const exports=new Set(WebAssembly.Module.exports(new WebAssembly.Module(wasm)).map(e=>e.name));
for(const name of ['sdl_test_gpu_flags','sdl_test_capture_copies','application_test_capture_stage'])if(!exports.has(name))throw Error('Missing diagnostic export: '+name);
const fontNames=['cp932.bin'];
const paths=new Map([
 ['/',resolve(import.meta.dirname,'index.html')],['/probe.mjs',resolve(import.meta.dirname,'probe.mjs')],
 ['/module.mjs',resolve(root,'web/artifacts/sdl-application/th15-application.mjs')],
 ['/th15-application.wasm',resolve(root,'web/artifacts/sdl-application/th15-application.wasm')],
 ['/gpu-diagnostics.mjs',resolve(root,'web/sdl-runtime/gpu-diagnostics.mjs')],
 ['/th15.dat',resolve(input,'games/th15/th15.dat')],['/unifont.otf',resolve(input,'shared/unifont.otf')],
 ['/msgothic.ttc',resolve(input,'shared/msgothic.ttc')],
 ...fontNames.map(n=>['/fonts/'+n,resolve(fonts,n)])
]);
const server=createServer(async(req,res)=>{
 try{
  const path=new URL(req.url,'http://localhost').pathname;
  let bytes;if(path==='/identity.json')bytes=JSON.stringify({build,fontNames});
  else if(paths.has(path))bytes=await readFile(paths.get(path));
  else {res.writeHead(404).end();return;}
  res.writeHead(200,{'Content-Type':path==='/'?'text/html; charset=utf-8':path.endsWith('.mjs')?'text/javascript':path.endsWith('.wasm')?'application/wasm':path.endsWith('.json')?'application/json':'application/octet-stream','Cache-Control':'no-store','Cross-Origin-Opener-Policy':'same-origin','Cross-Origin-Embedder-Policy':'require-corp'}).end(bytes);
 }catch(error){console.error(error);res.writeHead(500).end('Diagnostic resource unavailable');}
});
const host=process.env.TH15_GPU_HOST||'127.0.0.1',port=Number(process.env.TH15_GPU_PORT||8165);
server.listen(port,host,()=>console.log(`TH15 private GPU diagnostic: http://${host}:${server.address().port}/ (no pixel polling; no uploads)`));
