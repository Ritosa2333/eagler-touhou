import http from 'node:http';import fs from 'node:fs';import path from 'node:path';
const root=path.resolve(import.meta.dirname,'..'),site=path.join(root,'site'),port=Number(process.argv.find(a=>a.startsWith('--port='))?.slice(7)||8112);
const types={'.html':'text/html; charset=utf-8','.css':'text/css; charset=utf-8','.mjs':'text/javascript; charset=utf-8','.js':'text/javascript; charset=utf-8','.json':'application/json; charset=utf-8','.wasm':'application/wasm','.png':'image/png','.wav':'audio/wav','.ogg':'audio/ogg','.rpy':'application/octet-stream'};
http.createServer((req,res)=>{
  try{
    if(!['GET','HEAD'].includes(req.method)){res.writeHead(405).end();return;}
    const url=new URL(req.url,'http://localhost'),decoded=decodeURIComponent(url.pathname),prefix=decoded.startsWith('/src/')?path.join(root,'src'):site;
    const relative=decoded.startsWith('/src/')?decoded.slice(5):decoded==='/'?'index.html':decoded.slice(1),file=path.resolve(prefix,relative);
    if(!file.startsWith(prefix+path.sep)||!fs.existsSync(file)||!fs.statSync(file).isFile()){res.writeHead(404).end('Not found');return;}
    const stats=fs.statSync(file);res.setHeader('Content-Type',types[path.extname(file)]||'application/octet-stream');res.setHeader('X-Content-Type-Options','nosniff');res.setHeader('Cache-Control','no-cache');res.setHeader('Accept-Ranges','bytes');
    let start=0,end=stats.size-1;const range=req.headers.range?.match(/^bytes=(\d+)-(\d*)$/);
    if(range){start=Number(range[1]);end=range[2]?Math.min(end,Number(range[2])):end;if(start>end){res.writeHead(416).end();return;}res.statusCode=206;res.setHeader('Content-Range',`bytes ${start}-${end}/${stats.size}`);}
    res.setHeader('Content-Length',end-start+1);if(req.method==='HEAD'){res.end();return;}fs.createReadStream(file,{start,end}).pipe(res);
  }catch{res.writeHead(400).end('Bad request');}
}).listen(port,'127.0.0.1',()=>console.log(`TH12 development preview: http://127.0.0.1:${port}/`));
