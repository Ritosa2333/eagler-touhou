// Actual host module, synthetic resources, controlled fetch/FS/adoption edges.
import {readFileSync,writeFileSync} from 'node:fs';
import {pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';
import {gzipSync} from 'node:zlib';
const [sourcePath,out]=process.argv.slice(2),host=await import(pathToFileURL(sourcePath));
const rows=[],data=Uint8Array.from({length:16},(_,i)=>i+1),base='http://localhost/runtime/th14/th14.html';
const resource=(n=0,compressed=false)=>({path:compressed?'/fonts/font0.bin':'/fonts/test'+n+'.bin',url:compressed?'./fonts/font0.bin.gz':'./font'+n+'.bin',bytes:data.length,...(compressed?{encoding:'gzip',sha256:createHash('sha256').update(data).digest('hex')}: {})});
async function test(name,{mount=false,legacy=false,game='th14',stage='',compressed=false,expected={}}={}){
 const controller=new AbortController(),calls={fetch:0,parents:0,malloc:0,free:0,adopt:0,writes:[],signals:[]};let error=null;
 const Module={HEAPU8:new Uint8Array(64),_malloc(){calls.malloc++;return 32;},_free(){calls.free++;},FS:{mkdirTree(){},writeFile(p){calls.writes.push(p);}}};
 if(!legacy)Module._th14_adopt_managed_archive=()=>{calls.adopt++;if(stage==='adopt')controller.abort();return 1;};
 const fetcher=async(url,options)=>{
  calls.fetch++;calls.signals.push(options?.signal===controller.signal);
  if(String(url).endsWith('resources.json'))return {ok:true,async json(){if(stage==='manifest-body')controller.abort();return {schema:'eagler-sdl-resources/1',game,resources:[resource()]};}};
  if(stage==='fetch')controller.abort();
  if(compressed){const packed=gzipSync(data);return {ok:true,body:new ReadableStream({start(c){c.enqueue(packed);if(stage==='body')controller.abort();c.close();}})};}
  return {ok:true,async arrayBuffer(){if(stage==='body')controller.abort();return data.buffer;}};
 };
 if(stage==='before')controller.abort();
 try{
  if(mount)await host.mountManagedData(Module,{game,query:new URLSearchParams({managedData:'1',runtimeEpoch:'1',gameGeneration:'fixture'}),parentWindow:{async __eaglerPrepareManagedRuntimeDataV1(){calls.parents++;if(stage==='parent')controller.abort();return {buffer:data.buffer};}},base,fetcher,signal:controller.signal});
  else await host.installResources(Module,[resource(0,compressed),...(!compressed?[resource(1)]:[])],{game,base,fetcher,signal:controller.signal,emit(){if(stage==='between')controller.abort();}});
 }catch(e){error={name:e.name,message:String(e.message)};}
 const failures=[];
 for(const [k,v]of Object.entries(expected)){const actual=k==='aborted'?error?.name==='AbortError':k==='writes'?calls.writes.length:k==='signals'?calls.signals.every(Boolean):calls[k];if(actual!==v)failures.push({field:k,expected:v,actual:actual??null});}
 rows.push({name,passed:!failures.length,failures,calls,error});
}
await test('install-already-aborted',{stage:'before',expected:{aborted:true,fetch:0,writes:0}});
await test('install-abort-after-fetch',{stage:'fetch',expected:{aborted:true,fetch:1,writes:0,signals:true}});
await test('install-abort-after-body',{stage:'body',expected:{aborted:true,fetch:1,writes:0,signals:true}});
await test('install-abort-between-resources',{stage:'between',expected:{aborted:true,fetch:1,writes:1,signals:true}});
await test('compressed-abort-during-body',{stage:'body',compressed:true,expected:{aborted:true,fetch:1,writes:0,signals:true}});
await test('install-completes',{expected:{aborted:false,fetch:2,writes:2}});
await test('compressed-completes',{compressed:true,expected:{aborted:false,fetch:1,writes:1}});
await test('mount-aborted-before-parent',{mount:true,stage:'before',expected:{aborted:true,parents:0,malloc:0,fetch:0,writes:0}});
await test('mount-abort-after-parent',{mount:true,stage:'parent',expected:{aborted:true,parents:1,malloc:0,fetch:0,writes:0}});
await test('legacy-abort-after-parent',{mount:true,legacy:true,stage:'parent',expected:{aborted:true,parents:1,fetch:0,writes:0}});
await test('mount-abort-after-adoption',{mount:true,stage:'adopt',expected:{aborted:true,malloc:1,adopt:1,free:0,fetch:0,writes:0}});
await test('mount-abort-during-manifest-body',{mount:true,stage:'manifest-body',expected:{aborted:true,adopt:1,free:0,fetch:1,writes:0}});
await test('mount-abort-during-font-fetch',{mount:true,stage:'fetch',expected:{aborted:true,adopt:1,free:0,fetch:2,writes:0,signals:true}});
await test('mount-completes',{mount:true,expected:{aborted:false,adopt:1,free:0,fetch:2,writes:1}});
const q={passed:rows.every(r=>r.passed),cases:rows.length,failedCases:rows.filter(r=>!r.passed).length,rows,sourceSha256:createHash('sha256').update(readFileSync(sourcePath)).digest('hex'),actualHostModule:true,syntheticAssets:true,fetchFilesystemAndNativeOwnershipBoundariesControlled:true,actualBrowserCancelAccepted:false,completeGame:false};writeFileSync(out,JSON.stringify(q,null,2)+'\n');console.log(JSON.stringify({passed:q.passed,cases:q.cases,failedCases:q.failedCases}));process.exitCode=q.passed?0:1;
