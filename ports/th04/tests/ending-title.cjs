const {createServer}=require('node:http'),fs=require('node:fs'),path=require('node:path');
const puppeteer=require('../../../launcher/node_modules/puppeteer-core');
const assert=require('node:assert/strict');
const root=path.resolve('ports/th04/build-eagler'),content=path.resolve('ports/th04/content/disks');
const pack=fs.readFileSync('ports/th04/content/th04.data'),packSize=pack.readUInt32LE(8),packFiles=JSON.parse(pack.subarray(12,12+packSize)).files;
const inject=`sync.qa=async function(action,arg){
 const h=emulator.module.HEAPU8,v=new DataView(h.buffer);
 if(action==='init'){
  const d=gameDataBase(),m=mailbox,base=guestBase(),load=d-patch.data_segment*16;
  const resident=residentAddress;globalThis.q={d,m,base,resident,tick:0,opens:[],history:[]};
  const open=emulator.module.FS.open;emulator.module.FS.open=function(...args){const s=open.apply(this,args);q.opens.push({tick:q.tick,path:s.path});return s;};
  h[resident+17]=arg==='extra'?6:5;h[resident+19]=48+h[resident+17];
  const entry=arg==='extra'?0xb80f:0xb7e4,at=load+0xacf7,segment=(load-base)/16+patch.code_segment;
  h[at]=0xea;v.setUint16(at+1,entry-patch.code_segment*16,true);v.setUint16(at+3,segment,true);emulator.module.thDirtyMark?.(base,0x100000);
  return {m,d,base,resident};
 }
 
 if(action==='advance'){
  for(let i=0;i<arg;i++){
   const n=q.tick++;const input=[q.opens.some(x=>/\\/OP\\.EXE$/i.test(x.path))?34:(n>=1500&&n%60<15?32:0),0,0];if(n===7702){capture(n);step(input,{frame:n});const expected=checksum();restore(n);step(input,{frame:n,replay:true});const actual=checksum();q.rollback={expected,actual};if(JSON.stringify(expected)!==JSON.stringify(actual))throw Error('title transition rollback differs');}else step(input,{frame:n});confirm(n+1);
   const program=emulator.module.UTF8ToString(emulator.module._emGetRunningProgram());
   const state=[program,h[q.resident+17],h[q.resident+19],h[q.resident+15],h[q.resident+48],mailbox];
   if(!q.last||state.some((a,i)=>a!==q.last[i])){q.history.push({tick:q.tick,state});q.last=state;}
  }
  return {tick:q.tick,history:q.history.slice(-20),opens:q.opens.filter(x=>/\\/(?:OP|MAIN|MAINE)\\.EXE$/i.test(x.path)),rollback:q.rollback,fault:sync.faultReport({cpu:false}).runtimeFailure};
 }
};\n`;
const server=createServer((req,res)=>{try{let n=decodeURIComponent(new URL(req.url,'http://a').pathname);if(n==='/'){res.setHeader('content-type','text/html');return res.end('<!doctype html><title>QA</title>');}let f=n.endsWith('.hdi.gz')?path.join(content,path.basename(n)):path.join(root,n);const entry=packFiles.find(x=>'/'+x.path===n);let b=entry?pack.subarray(12+packSize+entry.offset,12+packSize+entry.offset+entry.bytes):fs.readFileSync(f);if(n==='/netplay/runtime.js')b=Buffer.from(b.toString().replace('if(workerHost)workerHost.runtime=sync;',inject+'if(workerHost)workerHost.runtime=sync;'));res.setHeader('content-type',n.endsWith('.js')||n.endsWith('.mjs')?'text/javascript':n.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(b);}catch(e){res.statusCode=404;res.end(String(e));}});
(async()=>{await new Promise(r=>server.listen(0,'127.0.0.1',r));const {findChromiumExecutable}=await import('../../../launcher/lib/chromium-executable.mjs');const b=await puppeteer.launch({headless:true,executablePath:process.env.PC98_BROWSER||await findChromiumExecutable(),args:['--enable-unsafe-swiftshader']});try{const p=await b.newPage();p.on('pageerror',e=>console.log('error',e));await p.goto('http://127.0.0.1:'+server.address().port+'/');await p.evaluate(()=>{let id=0;const pending=new Map();window.w=new Worker('/netplay/runtime-worker.js',{type:'module'});w.onmessage=({data})=>{if(data.type==='error'){const a=pending.get(data.id);if(a){pending.delete(data.id);a.reject(Error(data.message));}else console.log('worker',data.message);}if(data.type==='result'){const a=pending.get(data.id);if(a){pending.delete(data.id);a.resolve(data.result);}}};window.call=(type,args=[],extra={})=>new Promise((resolve,reject)=>{const n=++id;pending.set(n,{resolve,reject});w.postMessage({type,id:n,args,...extra});});});await p.evaluate(()=>call('load',[],{configure:[0,0,3],settings:{players:3,rollback:true,language:'cn',p1:0,p2:2,p3:3,difficulty:1,lives:3,bombs:2}}));await p.evaluate(()=>call('start'));await p.evaluate(()=>call('qa',['init','good']));let v;
 for(let i=0;i<92;i++)v=await p.evaluate(()=>call('qa',['advance',100]));
 assert(!v.fault,JSON.stringify(v.fault));assert(v.opens.some(x=>/\/MAINE\.EXE$/i.test(x.path)));assert(v.opens.some(x=>/\/OP\.EXE$/i.test(x.path)));
 assert(!v.opens.some(x=>/\/MAIN\.EXE$/i.test(x.path)),'held ending controls must not start a new game at the title');
 assert.deepEqual(v.rollback?.actual,v.rollback?.expected,'title transition must replay identically');assert(v.rollback,'rollback must cover the title transition');
 console.log('TH04 ending returns to title without carrying held inputs; rollback passed');}finally{await b.close();server.close();}})().catch(e=>{console.error(e);process.exitCode=1;server.close()});








