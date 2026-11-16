import {parseEclIncludes} from './ecl-includes.mjs';
import {readZipDirectory,readEntryData} from './zip-reader.mjs';
import {openTh128Dat} from './th128-dat.mjs';
const LAYOUT='sha256-58cd45fe7cac9af281d9de993a527ccfbbe6f27b89bcd0ca98a7031333fce200';
const MAX_ENTRY=128*1024*1024,MAX_PACKAGE=256*1024*1024;
const HASH=/^[0-9a-f]{64}$/,ID=/^[a-z0-9][a-z0-9._:-]{0,127}$/i;
function need(value,message){if(!value)throw new Error('TH128 DAT package: '+message);}
function safeName(name){return typeof name==='string'&&name.length>0&&!name.startsWith('/')&&!name.includes('\\')&&!name.includes(':')&&!name.includes('\0')&&name.split('/').every(p=>p&&p!=='.'&&p!=='..');}
function plain(o){return o!==null&&typeof o==='object'&&!Array.isArray(o);}
async function hash(bytes){return [...new Uint8Array(await globalThis.crypto.subtle.digest('SHA-256',bytes))].map(v=>v.toString(16).padStart(2,'0')).join('');}
function recordBytes(row){need(plain(row)&&Number.isSafeInteger(row.bytes)&&row.bytes>0&&row.bytes<=MAX_ENTRY&&HASH.test(row.sha256),'invalid file byte receipt');}
function descriptor(bytes){
 let d;try{d=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(bytes));}catch{throw new Error('TH128 DAT package: invalid package descriptor JSON');}
 need(d.schema==='eagler-touhou/package/1'&&d.game==='th128'&&ID.test(d.revision),'unsupported package descriptor');
 need(!d.runtime&&!d.runtimes,'Runtime scripts cannot be carried by a resource package');
 const r=d.runtimeRequirement;
 need(r?.protocol==='eagler-touhou/1'&&r.target==='th128'&&r.dataFile==='game-data'&&r.dataLayout===LAYOUT,'incompatible DAT mount layout');
 need(plain(d.files)&&Object.keys(d.files).length<=100,'invalid package file table');
 const sources=new Set(),targets=new Set(),kind=new Map();
 for(const[id,row]of Object.entries(d.files)){
  need(ID.test(id)&&plain(row),'invalid file identifier');recordBytes(row);
  need(safeName(row.source)&&typeof row.target==='string'&&row.target.startsWith('/'),'unsafe source/target');
  need(!sources.has(row.source)&&!targets.has(row.target),'duplicate package source/target');sources.add(row.source);targets.add(row.target);
  if(id==='game-data')need(row.source==='games/th128/th128.dat'&&row.target==='/th128.dat','missing original DAT mount'),kind.set(id,'data');
  else if(/^games\/th128\/music\/ogg\/th128_\d\d\.ogg$/.test(row.source)){
   need(row.target==='/music/'+row.source.split('/').at(-1),'invalid OGG mount');kind.set(id,'ogg');
  }else if(row.source==='games/th128/language/lang_zh-hans.zip'){
   need(row.target==='/__eagler/language/lang_zh-hans.zip','invalid language mount');kind.set(id,'language');
  }else if(/^shared\/[A-Za-z0-9_.-]+\.(?:ttc|otf|ttf)$/.test(row.source)){
   need(row.target==='/'+row.source.split('/').at(-1),'invalid shared font mount');kind.set(id,'font');
  }else need(false,'unsupported file layout');
 }
 need(kind.get('game-data')==='data','missing game-data');
 const base=d.base?.files;need(Array.isArray(base)&&new Set(base).size===base.length&&base.includes('game-data'),'invalid base file set');
 need(base.length===[...kind.values()].filter(k=>k==='data'||k==='font').length&&base.every(id=>['data','font'].includes(kind.get(id))),'base files are incomplete');
 const ogg=d.components?.ogg;
 need(ogg?.type==='ogg'&&Array.isArray(ogg.files)&&new Set(ogg.files).size===ogg.files.length,'invalid OGG component');
 need(ogg.files.length===10&&ogg.files.length===[...kind.values()].filter(k=>k==='ogg').length&&ogg.files.every(id=>kind.get(id)==='ogg'),'OGG component is incomplete');
 const language=d.components?.language;
 need(language?.type==='language'&&Array.isArray(language.entries)&&language.entries.length===1,'missing Chinese component');
 const entry=language.entries[0];
 need(entry.id==='lang_zh-hans'&&kind.get(entry.file)==='language'&&[...kind.values()].filter(k=>k==='language').length===1,'invalid Chinese component');
 return d;
}
function mime(target){
 if(target.endsWith('.ogg'))return'audio/ogg';
 if(target.endsWith('.wav'))return'audio/wav';
 if(target.endsWith('.png'))return'image/png';
 if(target.endsWith('.json'))return'application/json; charset=utf-8';
 if(target.endsWith('.txt'))return'text/plain; charset=utf-8';
 return'application/octet-stream';
}
export async function createDatOggProvider(input,options={}){
 need(input instanceof Uint8Array&&input.length>=22&&input.length<=MAX_PACKAGE,'ZIP package size budget exceeded');
 return createProvider({zip:input},options);
}
export async function createDatOggProviderFromFiles(descriptorBytes,filesById,options={}){
 need(descriptorBytes instanceof Uint8Array&&descriptorBytes.length>0&&descriptorBytes.length<=1024*1024,'invalid descriptor bytes');
 need(filesById instanceof Map,'file payloads must be a Map');
 return createProvider({descriptorBytes,filesById},options);
}
async function createProvider(source,{origin=globalThis.location?.origin,signal,isCurrent=()=>true,onProgress=()=>{}}={}){
 const page=new URL(origin);need(page.protocol==='http:'||page.protocol==='https:','unsupported page origin');
 const baseOrigin=page.origin;
 let closed=false,archive=null,index=null;
 const payloads=new Map(),resourceCache=new Map(),rawCache=new Map();
 function current(localSignal){
  if(closed||signal?.aborted||localSignal?.aborted||!isCurrent())throw new DOMException('DAT package owner was cancelled or disposed','AbortError');
 }
 current();
 async function inflate(compressed,{entry,signal:localSignal}){
  current(localSignal);need(typeof DecompressionStream==='function','DEFLATE decoder is unavailable');
  const reader=new Blob([compressed]).stream().pipeThrough(new DecompressionStream('deflate-raw')).getReader();
  const body=new Uint8Array(entry.uncompressedSize);let offset=0,complete=false;
  try{
   for(;;){const next=await reader.read();current(localSignal);if(next.done){complete=true;break;}need(next.value.length<=body.length-offset,'ZIP entry expanded beyond its declared size');body.set(next.value,offset);offset+=next.value.length;}
   need(offset===body.length,'ZIP entry size mismatch');return body;
  }finally{if(!complete)try{await reader.cancel();}catch{}reader.releaseLock();}
 }
 async function read(entry,localSignal){
  current(localSignal);need(entry&&entry.uncompressedSize<=MAX_ENTRY,'missing or excessive ZIP entry');
  try{const body=await readEntryData(entry,{inflate,signal:localSignal});current(localSignal);return body;}catch(error){current(localSignal);throw error;}
 }
 const entries=new Map();
 let d,readPayload;
 if(source.zip){
  const owned=new Uint8Array(source.zip);
  for(const entry of readZipDirectory(owned).entries){need(safeName(entry.name),'unsafe ZIP entry path');entries.set(entry.name,entry);}
  need(entries.has('package.json'),'package descriptor is missing');
  d=descriptor(await read(entries.get('package.json')));
  need(entries.size===Object.keys(d.files).length+1,'ZIP contains undeclared files');
  readPayload=async(row)=>{
   const entry=entries.get(row.source);need(entry&&entry.uncompressedSize===row.bytes,'declared package file is missing or has wrong size');
   return read(entry);
  };
 }else{
  d=descriptor(new Uint8Array(source.descriptorBytes));
  const ids=Object.keys(d.files);
  need(source.filesById.size===ids.length,'file payload table differs from descriptor');
  let total=source.descriptorBytes.length;
  // Detach the complete table synchronously. Caller-owned buffers may be
  // transferred or mutated as soon as the first digest yields.
  for(const[id,input]of source.filesById){
   need(Object.hasOwn(d.files,id),'undeclared file payload');
   const row=d.files[id];
   need(input instanceof Uint8Array&&input.length===row.bytes,'file payload size/type differs from descriptor');
   need(total<=MAX_PACKAGE-input.length,'file payload package size budget exceeded');total+=input.length;
   payloads.set(id,new Uint8Array(input));
  }
  need(payloads.size===ids.length&&ids.every(id=>payloads.has(id)),'declared file payload is missing');
  readPayload=async(_row,id)=>payloads.get(id);
 }
 let count=0;
 for(const[id,row]of Object.entries(d.files)){
  const body=await readPayload(row,id);current();
  need(body.length===row.bytes&&await hash(body)===row.sha256,'package SHA-256 mismatch: '+row.source);current();
  payloads.set(id,body);onProgress({completed:++count,total:Object.keys(d.files).length});current();
 }
 archive=openTh128Dat(payloads.get('game-data'));
 const languageId=d.components.language.entries[0].file,language=payloads.get(languageId);
 const inner=new Map();
 for(const entry of readZipDirectory(language).entries){need(safeName(entry.name),'unsafe language ZIP path');inner.set(entry.name,entry);}
 need(inner.has('runtime-assets.json'),'runtime resource index is missing');
 try{index=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(await read(inner.get('runtime-assets.json'))));}catch(error){current();throw new Error('TH128 DAT package: runtime resource index is invalid: '+error.message);}
 need(index.schema==='th128-dat-resource-map/1'&&index.variant==='zh'&&index.dataFile==='game-data'&&index.dataLayout===LAYOUT,'incompatible runtime resource index');
 need(plain(index.raw)&&Object.keys(index.raw).length<=4096&&plain(index.resources)&&Object.keys(index.resources).length<=20000,'runtime resource map exceeds budget');
 for(const[name,row]of Object.entries(index.raw)){
  need(/^[A-Za-z0-9][A-Za-z0-9_.-]*$/.test(name)&&archive.names.includes(name),'unknown DAT resource');recordBytes(row);
  if(row.patch)need(row.patch==='raw/'+name&&inner.has(row.patch),'missing translated raw resource');
 }
 for(const[target,row]of Object.entries(index.resources)){
  need(target.startsWith('/assets/')&&safeName(target.slice(1))&&plain(row),'unsafe runtime target');recordBytes(row);
  if(row.kind==='entry')need(row.entry===target.slice(1)&&inner.has(row.entry),'missing derived resource');
  else if(row.kind==='raw'){
   const wanted=index.raw[row.name];need(wanted&&wanted.bytes===row.bytes&&wanted.sha256===row.sha256,'raw resource receipt mismatch');
  }else if(row.kind==='music'){
   const wanted=d.files[row.fileId];need(wanted&&d.components.ogg.files.includes(row.fileId)&&wanted.bytes===row.bytes&&wanted.sha256===row.sha256,'music resource receipt mismatch');
  }else need(false,'unsupported resource binding');
 }
 const eclTargets=new Map();
 for(const[target,row]of Object.entries(index.resources)){
  if(row.kind!=='raw'||!row.name.endsWith('.ecl'))continue;
  const suffix='/runtime/'+row.name;
  if(!target.endsWith(suffix))continue;
  const prefix=target.slice(0,-suffix.length);
  eclTargets.set(prefix+'/ecl/'+row.name.slice(0,-4)+'.json',row.name);
 }
 async function checked(body,row,localSignal){need(body.length===row.bytes&&await hash(body)===row.sha256,'runtime asset SHA-256 mismatch');current(localSignal);return body;}
 async function raw(name,localSignal){
  if(!rawCache.has(name)){
   const row=index.raw[name],pending=(async()=>checked(row.patch?await read(inner.get(row.patch),localSignal):archive.read(name),row,localSignal))();
   rawCache.set(name,pending);pending.catch(()=>rawCache.delete(name));
  }
  const body=await rawCache.get(name);current(localSignal);return body;
 }
 async function load(target,localSignal){
  if(eclTargets.has(target)&&!index.resources[target]){
   if(!resourceCache.has(target)){
    const name=eclTargets.get(target),pending=(async()=>{
     const bytes=await raw(name,localSignal),metadata=parseEclIncludes(bytes);
     metadata.provenance={name,bytes:bytes.length,sha256:index.raw[name].sha256};
     current(localSignal);return new TextEncoder().encode(JSON.stringify(metadata)+'\n');
    })();
    resourceCache.set(target,pending);pending.catch(()=>resourceCache.delete(target));
   }
   const result=await resourceCache.get(target);current(localSignal);return result;
  }
  const row=index.resources[target];need(row,'missing resource target: '+target);
  if(!resourceCache.has(target)){
   const pending=(async()=>{
    const body=row.kind==='entry'?await read(inner.get(row.entry),localSignal):row.kind==='raw'?await raw(row.name,localSignal):payloads.get(row.fileId);
    return checked(body,row,localSignal);
   })();
   resourceCache.set(target,pending);pending.catch(()=>resourceCache.delete(target));
  }
  const body=await resourceCache.get(target);current(localSignal);return body;
 }
 const targets=Object.freeze([...new Set([...Object.keys(index.resources),...eclTargets.keys()])].sort());
 return Object.freeze({
  revision:d.revision,descriptor:structuredClone(d),targets,
  dispose(){closed=true;archive=null;index=null;payloads.clear();resourceCache.clear();rawCache.clear();inner.clear();entries.clear();},
  async fetch(input,init={}){
   const localSignal=init.signal??(input instanceof Request?input.signal:undefined);current(localSignal);
   const url=new URL(input instanceof Request?input.url:String(input),baseOrigin);
   need(url.origin===baseOrigin&&!url.search&&!url.hash,'resource URL origin/source rejected');
   const method=String(init.method??(input instanceof Request?input.method:'GET')).toUpperCase();
   need((method==='GET'||method==='HEAD')&&init.body==null,'resource method must be GET or HEAD');
   const body=await load(url.pathname,localSignal),headers=new Headers(input instanceof Request?input.headers:undefined);
   for(const[k,v]of new Headers(init.headers))headers.set(k,v);
   const responseHeaders={'Content-Type':mime(url.pathname),'Accept-Ranges':'bytes','Content-Length':String(body.length)};
   const range=headers.get('Range');let start=0,end=body.length-1,status=200;
   if(range){
    const match=/^bytes=(\d*)-(\d*)$/.exec(range);
    if(!match||(!match[1]&&!match[2]))return new Response(null,{status:416,headers:{'Content-Range':'bytes */'+body.length}});
    if(match[1]){start=Number(match[1]);if(match[2])end=Math.min(Number(match[2]),end);}
    else start=Math.max(0,body.length-Number(match[2]));
    if(!Number.isSafeInteger(start)||!Number.isSafeInteger(end)||start>=body.length||end<start)return new Response(null,{status:416,headers:{'Content-Range':'bytes */'+body.length}});
    status=206;responseHeaders['Content-Range']='bytes '+start+'-'+end+'/'+body.length;responseHeaders['Content-Length']=String(end-start+1);
   }
   current(localSignal);return new Response(method==='HEAD'?null:body.slice(start,end+1),{status,headers:responseHeaders});
  }
 });
}
