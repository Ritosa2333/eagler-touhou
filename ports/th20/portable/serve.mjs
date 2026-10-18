import http from 'node:http';
import {createReadStream,readFileSync,statSync,existsSync} from 'node:fs';
import {resolve,dirname,extname,sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawn} from 'node:child_process';
const args=process.argv.slice(2),value=(k,f)=>args.includes(k)?args[args.indexOf(k)+1]:f;
const site=resolve(value('--site',resolve(dirname(fileURLToPath(import.meta.url)),'../site'))),manifest=JSON.parse(readFileSync(resolve(site,'manifest.json')));
// TH20 packages use the runtime-directory schema (runtime-files.json) instead
// of the older site files.json; accept either as the serve whitelist.
const filesListing=(()=>{try{return Object.keys(JSON.parse(readFileSync(resolve(site,'files.json')))).concat('files.json');}catch{return Object.keys(JSON.parse(readFileSync(resolve(site,'runtime-files.json'))).files).concat('runtime-files.json');}})();
const allowed=new Set(filesListing),th20=manifest.game==='th20';
if(th20)for(const name of ['game-data/th20.dat','game-data/thbgm.dat','game-data/msgothic.ttc','game-data/unifont.otf'])allowed.add(name);
if(th20)for(let n=1;n<=18;n++)allowed.add(`bgm-ogg/th20_${String(n).padStart(2,'0')}.ogg`);
if(th20)allowed.add('bgm-ogg/th128_08.ogg');
const port=Number(value('--port',manifest.game==='th08'?'8092':th20?'8094':'8090'));
if(!Number.isInteger(port)||port<1||port>65535)throw Error('Invalid port');
function open(){if(args.includes('--open')){const url=th20?`http://127.0.0.1:${port}/th20.html?managedData=1&runtimeEpoch=1&gameGeneration=local`:`http://127.0.0.1:${port}`;if(process.platform==='win32')spawn('explorer.exe',[url],{windowsHide:true,stdio:'ignore'}).on('error',()=>console.log(url));}}
// Standalone TH20 smoke stations have no launcher iframe. Inject a tiny
// provider that hands the managed runtime the local primary archive; the BGM
// archive and shared font arrive through resources.json like any resource.
const localProvider='<script>window.__eaglerPrepareManagedRuntimeDataV1=async()=>{const r=await fetch("./game-data/th20.dat");if(!r.ok)throw Error("Missing local game data: place game-data/th20.dat next to the runtime (and game-data/thbgm.dat for music)");return {buffer:await r.arrayBuffer(),bytes:0};};'
 +'window.__th20Auto=setInterval(()=>{if(!window.__th20Runtime)return;clearInterval(window.__th20Auto);'
 +'const send=(command,extra)=>window.postMessage({protocol:"eagler-touhou/1",game:"th20",epoch:1,command,...extra},location.origin);'
 +'send("configure",{music:"ogg",options:{}});send("launch");},100);</script>';
const server=http.createServer((req,res)=>{
 console.log(new Date().toISOString().slice(11,19),req.url.slice(0,90));
 if(!['GET','HEAD'].includes(req.method)){res.writeHead(405,{Allow:'GET, HEAD'}).end();return;}
 let name;try{name=decodeURIComponent(new URL(req.url,'http://localhost').pathname).slice(1)||'index.html';}catch{res.writeHead(400).end();return;}
 if(th20&&(name==='index.html'||name==='th20.html')&&!req.url.includes('managedData=')){res.writeHead(302,{Location:`/th20.html?managedData=1&runtimeEpoch=1&gameGeneration=local`}).end();return;}
 if(th20&&name==='index.html')name='th20.html';
 // The local station extends the runtime resource manifest with whatever the
 // operator dropped into game-data/: the BGM archive and the shared font.
 if(th20&&name==='resources.json'){
  const base=JSON.parse(readFileSync(resolve(site,'resources.json'),'utf8')),extra=[];
  for(const [file,path] of [['thbgm.dat','/game/thbgm.dat'],['msgothic.ttc','/msgothic.ttc'],['unifont.otf','/unifont.otf']])
   if(existsSync(resolve(site,'game-data',file))){const bytes=statSync(resolve(site,'game-data',file)).size;extra.push({path,url:'./game-data/'+file,bytes});}
  for(const file of [...Array.from({length:18},(_,i)=>`th20_${String(i+1).padStart(2,'0')}.ogg`),'th128_08.ogg'])
   if(existsSync(resolve(site,'bgm-ogg',file))){const bytes=statSync(resolve(site,'bgm-ogg',file)).size;extra.push({path:'/bgm-ogg/'+file,url:'./bgm-ogg/'+file,bytes});}
  res.writeHead(200,{'Content-Type':'application/json','Cache-Control':'no-cache','X-Content-Type-Options':'nosniff'}).end(JSON.stringify({...base,resources:[...(base.resources||[]),...extra]},null,2));return;
 }
 if(!allowed.has(name)){res.writeHead(name==='favicon.ico'?204:404).end();return;}
 let file=resolve(site,name);if(!file.startsWith(site+sep)){res.writeHead(403).end();return;}
 let stamp;try{stamp=statSync(file);if(!stamp.isFile())throw Error();}catch{res.writeHead(404).end();return;}
 // The standalone TH20 page gets the local data provider injected before its
 // module script; the launcher-hosted package stays byte-identical on disk.
 if(th20&&name==='th20.html'){
  const html=readFileSync(file,'utf8').replace('<script type="module" src="shell.mjs">',localProvider+'<script type="module" src="shell.mjs">');
  res.writeHead(200,{'Content-Type':'text/html; charset=utf-8','Cache-Control':'no-cache','X-Content-Type-Options':'nosniff','Content-Length':Buffer.byteLength(html)}).end(req.method==='HEAD'?undefined:html);return;
 }
 const fullSize=stamp.size,type=({'.html':'text/html; charset=utf-8','.json':'application/json','.webmanifest':'application/manifest+json','.mjs':'text/javascript','.js':'text/javascript','.css':'text/css; charset=utf-8','.wasm':'application/wasm','.gz':'application/gzip','.txt':'text/plain; charset=utf-8','.png':'image/png','.svg':'image/svg+xml','.jpg':'image/jpeg','.woff2':'font/woff2','.flac':'audio/flac','.ogg':'audio/ogg'})[extname(file)]??'application/octet-stream';
 let start=0,end=fullSize-1,code=200,encoded=false;
 if(req.headers.range){const match=/^bytes=(\d+)-(\d*)$/.exec(req.headers.range);if(match){start=Number(match[1]);end=match[2]?Math.min(Number(match[2]),end):end;}if(!match||!Number.isSafeInteger(start)||!Number.isSafeInteger(end)||start>end||start>=fullSize){res.writeHead(416,{'Content-Range':`bytes */${fullSize}`}).end();return;}code=206;}
 else if(extname(file)==='.wasm'&&(req.headers['accept-encoding']??'').split(',').some(p=>{const[n,...q]=p.split(';');return n.trim()==='gzip'&&!q.some(v=>/^q\s*=\s*0(?:\.0*)?$/i.test(v.trim()));})&&allowed.has(name+'.gz')&&existsSync(file+'.gz')){file+='.gz';stamp=statSync(file);end=stamp.size-1;encoded=true;}
 const etag=`W/"${stamp.size.toString(16)}-${stamp.mtimeMs.toString(16)}-${start}-${end}"`,headers={'Content-Type':type,'Cache-Control':'no-cache','ETag':etag,'X-Content-Type-Options':'nosniff','Cross-Origin-Resource-Policy':'same-origin','Accept-Ranges':'bytes',...(type==='application/wasm'?{Vary:'Accept-Encoding'}:{}),...(encoded?{'Content-Encoding':'gzip'}:{})};
 if((req.headers['if-none-match']??'').split(',').some(t=>t.trim()===etag||t.trim()==='*')){res.writeHead(304,headers).end();return;}
 if(code===206)headers['Content-Range']=`bytes ${start}-${end}/${fullSize}`;
 res.writeHead(code,{...headers,'Content-Length':Math.max(0,end-start+1)});if(req.method==='HEAD'||!stamp.size)res.end();else createReadStream(file,{start,end}).on('error',()=>res.destroy()).pipe(res);
});
server.on('error',async error=>{if(error.code==='EADDRINUSE'){try{const current=await(await fetch(`http://127.0.0.1:${port}/manifest.json`,{signal:AbortSignal.timeout(2000)})).json();if(current.game===manifest.game&&current.execution?.sha256===manifest.execution.sha256&&current.execution?.loaderSha256===manifest.execution.loaderSha256){console.log(`游戏服务已运行：http://127.0.0.1:${port}`);open();return;}}catch{}}console.error(error.code==='EADDRINUSE'?`端口 ${port} 被其他服务或旧版本占用，请先关闭该服务。`:error);process.exitCode=1;});
server.listen(port,'127.0.0.1',()=>{console.log(`${manifest.game} / SDL ${manifest.execution.sdlVersion}: http://127.0.0.1:${port}`);console.log('游玩期间请保持窗口打开，Ctrl+C 停止服务。');open();});
