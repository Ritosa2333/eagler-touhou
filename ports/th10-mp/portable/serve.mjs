import http from 'node:http';
import {createReadStream,readFileSync,statSync,existsSync} from 'node:fs';
import {resolve,dirname,extname,sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawn} from 'node:child_process';
const args=process.argv.slice(2),value=(k,f)=>args.includes(k)?args[args.indexOf(k)+1]:f;
const site=resolve(value('--site',resolve(dirname(fileURLToPath(import.meta.url)),'../site'))),manifest=JSON.parse(readFileSync(resolve(site,'manifest.json'))),allowed=new Set(Object.keys(JSON.parse(readFileSync(resolve(site,'files.json')))).concat('files.json'));
const port=Number(value('--port',manifest.game==='th08'?'8092':'8090'));
if(!Number.isInteger(port)||port<1||port>65535)throw Error('Invalid port');
function open(){if(args.includes('--open')){const url=`http://127.0.0.1:${port}`;if(process.platform==='win32')spawn('explorer.exe',[url],{windowsHide:true,stdio:'ignore'}).on('error',()=>console.log(url));}}
const server=http.createServer((req,res)=>{
 if(!['GET','HEAD'].includes(req.method)){res.writeHead(405,{Allow:'GET, HEAD'}).end();return;}
 let name;try{name=decodeURIComponent(new URL(req.url,'http://localhost').pathname).slice(1)||'index.html';}catch{res.writeHead(400).end();return;}
 if(!allowed.has(name)){res.writeHead(name==='favicon.ico'?204:404).end();return;}
 let file=resolve(site,name);if(!file.startsWith(site+sep)){res.writeHead(403).end();return;}
 let stamp;try{stamp=statSync(file);if(!stamp.isFile())throw Error();}catch{res.writeHead(404).end();return;}
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
