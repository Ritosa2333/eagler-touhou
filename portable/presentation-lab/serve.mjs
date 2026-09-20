import http from 'node:http';
import {readFileSync,writeFileSync,createReadStream,existsSync,statSync,mkdirSync} from 'node:fs';
import {resolve,extname} from 'node:path';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
const lab=import.meta.dirname,root=resolve(lab,'../..'),workspace=resolve(root,'../..');
const snapshot=process.env.TH08_LAB_SNAPSHOT||'';
if(snapshot&&!/^[a-z0-9-]{1,40}$/.test(snapshot))throw Error('Invalid snapshot');
const runtime=snapshot?resolve(root,'artifacts/presentation-lab/builds',snapshot):resolve(root,'build-eagler'),input=resolve(root,'artifacts/presentation-lab/input');
const build=JSON.parse(readFileSync(snapshot?resolve(runtime,'build.json'):resolve(root,'th08_web/artifacts/sdl3/build.json'),'utf8'));
const names=new Set(build.exports.map(x=>x.name));
for(const name of ['audit_draw','audit_reference','audit_state','audit_fault','audit_timing_records','audit_timing_capacity','audit_timing_count','audit_timing_next','audit_timing_stride'])if(!names.has(name))throw Error('Build with --th08 --presentation-lab first: missing '+name);
const sha=b=>createHash('sha256').update(b).digest('hex');
if(sha(readFileSync(resolve(runtime,'th08-sdl.wasm')))!==build.sha256)throw Error('Packaged lab WASM is stale');
const inventory=JSON.parse(readFileSync(resolve(runtime,'runtime-files.json'),'utf8')).files;
for(const [name,info] of Object.entries(inventory)){
 if(name.includes('..')||name.startsWith('/')||name.includes('\\'))throw Error('Invalid runtime inventory path');
 if(sha(readFileSync(resolve(runtime,name)))!==info.sha256)throw Error('Packaged Runtime file is stale: '+name);
}
if(!snapshot)for(const [name,digest] of Object.entries(build.sourceFiles))if(sha(readFileSync(resolve(root,name)))!==digest)throw Error('Rebuild modified instrumented source: '+name);
const files=new Map([['/','index.html'],['/index.html','index.html'],['/app.mjs','app.mjs'],['/controller.mjs','controller.mjs'],['/analyzer.mjs','analyzer.mjs'],['/style.css','style.css'],['/fullscreen.css','fullscreen.css']].map(([url,file])=>[url,resolve(lab,file)]));
for(const file of Object.keys(inventory))files.set('/runtime/'+file,resolve(runtime,file));
const font=process.env.TH08_LAB_FONT||resolve(workspace,'th06-eagler/assets/msgothic.ttc');
if(!existsSync(font))throw Error('Set TH08_LAB_FONT to your local MS Gothic font');
files.set('/msgothic.ttc',font);
const data=process.env.TH08_LAB_DATA||resolve(input,'th08.dat');
if(existsSync(data))files.set('/input/th08.dat',data);
for(let i=0;i<4;i++)if(existsSync(resolve(input,`demo${i}.rpy`)))files.set(`/input/demo${i}.rpy`,resolve(input,`demo${i}.rpy`));
const campaignManifest=resolve(input,'campaign/manifest.json');
if(existsSync(campaignManifest)){
 files.set('/campaign/manifest.json',campaignManifest);
 const corpus=JSON.parse(readFileSync(campaignManifest,'utf8'));
 for(const fixture of corpus.fixtures||[]){
  if(!/^[a-z0-9-]{1,40}$/.test(fixture.id))throw Error('Invalid campaign fixture ID');
  const path=resolve(input,'campaign',fixture.id+'.rpy');
  if(sha(readFileSync(path))!==fixture.sha256)throw Error('Replay fixture identity mismatch');
  files.set('/campaign/'+fixture.id+'.rpy',path);
 }
}
const identity={schema:'th08/presentation-lab-build/1',commit:execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),wasm:build.sha256,loader:build.loaderSha256,
  sourceDigest:sha(JSON.stringify(build.sourceFiles)),snapshot:snapshot||null,instrumented:true,dataAvailable:files.has('/input/th08.dat'),fixtureManifest:existsSync(resolve(input,'manifest.json'))?JSON.parse(readFileSync(resolve(input,'manifest.json'),'utf8')):{},
  evidence:'diagnostic runtime, not production deployment',sampling:'authoritative Draw endpoints + frozen alpha sweep'};
function currentIdentity(){
 const toolSources=Object.fromEntries(['index.html','style.css','fullscreen.css','app.mjs','controller.mjs','analyzer.mjs'].map(name=>[name,sha(readFileSync(resolve(lab,name)))]));
 return {...identity,toolSources,toolDigest:sha(JSON.stringify(toolSources))};
}
const types={'.html':'text/html; charset=utf-8','.mjs':'text/javascript; charset=utf-8','.css':'text/css; charset=utf-8','.json':'application/json','.wasm':'application/wasm','.ttc':'font/collection'};
const port=Number(process.env.PORT||8132);
if(!Number.isInteger(port)||port<1024||port>65535)throw Error('Invalid port');
const server=http.createServer((req,res)=>{
  const host=req.headers.host||'';
  if(![`127.0.0.1:${port}`,`localhost:${port}`].includes(host)){res.writeHead(403).end('Loopback host only');return;}
  let path;try{path=decodeURIComponent(new URL(req.url,`http://${host}`).pathname);}catch{res.writeHead(400).end();return;}
  const headers={'Cache-Control':'no-store','X-Content-Type-Options':'nosniff','Cross-Origin-Resource-Policy':'same-origin'};
  if(req.method==='POST'&&path==='/incident'){
    if(req.headers.origin!==`http://${host}`){res.writeHead(403,headers).end('Same-origin only');return;}
    const declared=Number(req.headers['content-length']||0);if(!Number.isInteger(declared)||declared<=0||declared>24*1024*1024){res.writeHead(413,headers).end('Invalid report size');return;}
    const chunks=[];let received=0;req.on('data',chunk=>{received+=chunk.length;if(received>24*1024*1024)req.destroy();else chunks.push(chunk);});req.on('end',()=>{try{
      const report=JSON.parse(Buffer.concat(chunks).toString('utf8'));
      if(report?.schema!=='th08/presentation-audit/1'||report?.timing?.schema!=='th08/presentation-timing-ring/1'||!Array.isArray(report.timing.rows)||report.timing.rows.length>512||!Number.isInteger(report.tick))throw Error('Invalid incident report');
      const directory=resolve(root,'artifacts/presentation-lab/incidents');mkdirSync(directory,{recursive:true});const name=`latest-${report.tick}.json`;const body=JSON.stringify(report,null,2)+'\n';writeFileSync(resolve(directory,name),body);writeFileSync(resolve(directory,'latest.json'),body);
      res.writeHead(201,{...headers,'Content-Type':'application/json'}).end(JSON.stringify({saved:`artifacts/presentation-lab/incidents/${name}`}));
    }catch(error){res.writeHead(400,headers).end(String(error.message||error));}});return;
  }
  if(!['GET','HEAD'].includes(req.method)){res.writeHead(405).end();return;}
  if(path==='/build.json'){res.writeHead(200,{...headers,'Content-Type':'application/json'}).end(JSON.stringify(currentIdentity()));return;}
  const file=files.get(path);
  if(!file||!existsSync(file)){res.writeHead(404,headers).end('Not found');return;}
  const size=statSync(file).size;
  res.writeHead(200,{...headers,'Content-Type':types[extname(file)]||'application/octet-stream','Content-Length':size});
  if(req.method==='HEAD')res.end();else createReadStream(file).on('error',()=>res.destroy()).pipe(res);
});
server.listen(port,'127.0.0.1',()=>console.log(JSON.stringify({url:`http://127.0.0.1:${port}/`,wasm:identity.wasm,data:identity.dataAvailable,scope:'loopback reads / bounded same-origin incident write'})));
