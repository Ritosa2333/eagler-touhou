// Isolated local Launcher preview using the normal eagler-touhou/1 shell.
import {createServer} from 'node:http';
import {createHash} from 'node:crypto';
import {readFile,stat} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
const workspace=resolve(import.meta.dirname,'../../..');
const launcher=resolve(workspace,'eagler-touhou');
process.env.EAGLER_WORKSPACE_ROOT=workspace;
const load=p=>import(pathToFileURL(resolve(launcher,p)).href);
const {FRONTEND_PACKAGE_FILES,resolveFrontendPackageSource}=await load('lib/frontend-manifest.mjs');
const {createDevelopmentHostManifestFromContent}=await load('lib/development-host-manifest.mjs');
const {PRODUCT_CONTENT}=await load('lib/content-definition.mjs');
const root=resolve(import.meta.dirname,'..'),runtime=resolve(root,'build-eagler'),assets=resolve(root,'th15_web/artifacts/thprac-private-assets');
const data=await readFile(resolve(assets,'th15.dat'));
const host=await createDevelopmentHostManifestFromContent({shared:{unicodeFont:'./shared/unifont.otf',vanillaFont:''},games:{th15:{runtime:'./runtime/th15/th15.html?hosted=1',data:{source:'./games/th15/th15.dat',identity:{bytes:data.length,sha256:createHash('sha256').update(data).digest('hex'),layout:PRODUCT_CONTENT.th15.dataLayout}},music:{}}}},{games:['th15']});
const files=new Map(FRONTEND_PACKAGE_FILES.map(p=>['/'+p,resolveFrontendPackageSource(p)]));
files.set('/shared/unifont.otf',resolve(assets,'unifont.otf'));files.set('/games/th15/th15.dat',resolve(assets,'th15.dat'));
const inventory=JSON.parse(await readFile(resolve(runtime,'runtime-files.json'),'utf8'));
for(const p of Object.keys(inventory.files))files.set('/runtime/th15/'+p,resolve(runtime,p));
for(const p of ['assets/th15-card.webp','th06.ico','pwa/icon-192.png','pwa/icon-512.png','pwa/icon-maskable-512.png','pwa/apple-touch-icon.png']){
 for(const candidate of [resolve(workspace,'eagler-touhou/private-assets',p),resolve(workspace,'_scratch/test-th15-20261006/payload',p),resolve(launcher,'public',p)]){try{if((await stat(candidate)).isFile()){files.set('/'+p,candidate);break;}}catch{}}
}
const json=new Map([['/host-manifest.json',host],['/release-catalog.json',{schema:'eagler-touhou/release-catalog/1',games:{}}]]);
const server=createServer(async(req,res)=>{try{let p=new URL(req.url,'http://localhost').pathname;if(p==='/')p='/index.html';res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');res.setHeader('Cross-Origin-Resource-Policy','same-origin');res.setHeader('Cache-Control','no-store');if(json.has(p)){res.setHeader('Content-Type','application/json');res.end(JSON.stringify(json.get(p)));return;}const file=files.get(p);if(!file){res.writeHead(404).end();return;}res.setHeader('Content-Type',p.endsWith('.html')?'text/html; charset=utf-8':/\.(mjs|js)$/.test(p)?'text/javascript':p.endsWith('.css')?'text/css':p.endsWith('.json')?'application/json':p.endsWith('.wasm')?'application/wasm':p.endsWith('.webp')?'image/webp':p.endsWith('.woff2')?'font/woff2':'application/octet-stream');res.end(await readFile(file));}catch(e){res.writeHead(500).end(String(e));}});
server.listen(8148,'127.0.0.1',()=>console.log('TH15 Launcher protocol preview: http://127.0.0.1:8148/?game=th15'));
