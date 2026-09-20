import http from 'node:http';
import {createReadStream,existsSync,mkdirSync,readFileSync,statSync,writeFileSync} from 'node:fs';
import {extname,resolve} from 'node:path';
import {createHash} from 'node:crypto';

export const sha256=value=>createHash('sha256').update(value).digest('hex');

export function verifyRuntimeInventory(runtimeRoot,inventory){
  for(const [name,info] of Object.entries(inventory)){
    if(name.includes('..')||name.startsWith('/')||name.includes('\\'))throw Error('Invalid runtime inventory path');
    const path=resolve(runtimeRoot,name);if(!existsSync(path)||sha256(readFileSync(path))!==info.sha256)throw Error('Packaged Runtime file is stale: '+name);
  }
}

const types={'.html':'text/html; charset=utf-8','.mjs':'text/javascript; charset=utf-8','.css':'text/css; charset=utf-8','.json':'application/json','.wasm':'application/wasm','.ttc':'font/collection','.ttf':'font/ttf','.bin':'application/octet-stream'};

export function createPresentationLabServer({port,files,identity,incident}){
  if(!Number.isInteger(port)||port<1024||port>65535)throw Error('Invalid port');
  if(!(files instanceof Map))throw Error('Presentation Lab server requires an explicit file map');
  const headers={'Cache-Control':'no-store','X-Content-Type-Options':'nosniff','Cross-Origin-Resource-Policy':'same-origin'};
  const server=http.createServer((request,response)=>{
    const host=request.headers.host||'';
    if(![`127.0.0.1:${port}`,`localhost:${port}`].includes(host)){response.writeHead(403).end('Loopback host only');return;}
    let path;try{path=decodeURIComponent(new URL(request.url,`http://${host}`).pathname);}catch{response.writeHead(400).end();return;}
    if(request.method==='POST'&&path==='/incident'){
      if(!incident){response.writeHead(404,headers).end('Not found');return;}
      if(request.headers.origin!==`http://${host}`){response.writeHead(403,headers).end('Same-origin only');return;}
      const limit=incident.maxBytes??24*1024*1024,declared=Number(request.headers['content-length']||0);
      if(!Number.isInteger(declared)||declared<=0||declared>limit){response.writeHead(413,headers).end('Invalid report size');return;}
      const chunks=[];let received=0,overflow=false;
      request.on('data',chunk=>{received+=chunk.length;if(received>limit){overflow=true;request.destroy();}else chunks.push(chunk);});
      request.on('end',()=>{if(overflow)return;try{
        const report=JSON.parse(Buffer.concat(chunks).toString('utf8')),key=incident.validate(report);
        if(typeof key!=='string'||!/^[a-z0-9-]{1,80}$/.test(key))throw Error('Invalid incident key');
        mkdirSync(incident.directory,{recursive:true});const name=`latest-${key}.json`,body=JSON.stringify(report,null,2)+'\n';writeFileSync(resolve(incident.directory,name),body);writeFileSync(resolve(incident.directory,'latest.json'),body);
        response.writeHead(201,{...headers,'Content-Type':'application/json'}).end(JSON.stringify({saved:name}));
      }catch(error){response.writeHead(400,headers).end(String(error.message||error));}});return;
    }
    if(!['GET','HEAD'].includes(request.method)){response.writeHead(405).end();return;}
    if(path==='/build.json'){const value=typeof identity==='function'?identity():identity;response.writeHead(200,{...headers,'Content-Type':'application/json'}).end(JSON.stringify(value));return;}
    const file=files.get(path);if(!file||!existsSync(file)){response.writeHead(404,headers).end('Not found');return;}
    const size=statSync(file).size;response.writeHead(200,{...headers,'Content-Type':types[extname(file)]||'application/octet-stream','Content-Length':size});
    if(request.method==='HEAD')response.end();else createReadStream(file).on('error',()=>response.destroy()).pipe(response);
  });
  return {server,start(){server.listen(port,'127.0.0.1');return server;}};
}
