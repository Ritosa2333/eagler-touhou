import {createServer} from 'node:http';
import {readFileSync,existsSync,createReadStream} from 'node:fs';
import {resolve,extname} from 'node:path';
const root=resolve(import.meta.dirname,'../../..'),runtime=resolve(process.env.EAGLER_TH075_MP_RUNTIME||resolve(root,'build-eagler-multiplayer'));
const port=Number(process.env.EAGLER_TH075_MP_TEST_PORT||18375);
const html=`<!doctype html><meta charset="utf-8"><style>body{margin:0;background:#111}iframe{width:800px;height:600px;border:0}</style><iframe id="game"></iframe><script>
const protocol='eagler-touhou/1',game='th075',events=[],pending=new Map();let serial=0;
const iframe=document.querySelector('iframe');
window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
window.addEventListener('message',e=>{if(e.source!==iframe.contentWindow||e.origin!==location.origin)return;const m=e.data;if(m.protocol!==protocol)return;if(m.event)events.push(m);if(pending.has(m.request)){const p=pending.get(m.request);pending.delete(m.request);m.ok?p.resolve(m):p.reject(Error(m.error));}});
window.host={events,request(command,fields={}){return new Promise((resolve,reject)=>{const request='t'+(++serial);pending.set(request,{resolve,reject});iframe.contentWindow.postMessage({protocol,game,epoch:1,request,command,...fields},location.origin);});},state(){const c=iframe.contentWindow.Module;if(!c)return null;const p=c._th075_mp_status();return Array.from(c.HEAPU32.subarray(p/4,p/4+64));},error(){return iframe.contentWindow.__th075LastError||null;},key(code,down){return this.request('keyboard',{code,down});}};
host.history=new Map();function observe(){const s=host.state();if(s&&s[2]>0)host.history.set(s[2],[s[7],...s.slice(8,48)]);requestAnimationFrame(observe);}observe();
iframe.src='/runtime/th075.html?hosted=1&managedData=1&gameGeneration=mp-test&runtimeEpoch=1';
</script>`;
createServer((req,res)=>{const path=new URL(req.url,'http://localhost').pathname;if(path==='/'){res.setHeader('Content-Type','text/html');return res.end(html);}const f=path==='/data'?resolve(process.env.EAGLER_TH075_DATA||resolve(root,'assets-ogg/th075.data')):path.startsWith('/runtime/')&&!path.includes('..')?resolve(runtime,path.slice(9)):null;if(!f||!existsSync(f)){res.statusCode=404;return res.end('not found');}res.setHeader('Content-Type',({'.mjs':'text/javascript','.wasm':'application/wasm','.html':'text/html','.json':'application/json','.css':'text/css'})[extname(f)]||'application/octet-stream');createReadStream(f).pipe(res);}).listen(port,'127.0.0.1',()=>console.log('TH075 MP runtime host '+port));
