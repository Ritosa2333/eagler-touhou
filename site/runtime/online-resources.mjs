import {installDatPackage,reopenDatPackage} from './dat-package-install.mjs';
const MAX_ARCHIVE=256*1024*1024,MAX_MANIFEST=8192,HASH=/^[0-9a-f]{64}$/;
function current(signal,isCurrent){if(signal?.aborted||!isCurrent())throw new DOMException('Online game resource owner was cancelled','AbortError');}
function verifiedDeployment(value,pageURL){
 if(value?.schema!=='th128-online-deployment/1'||value.game!=='th128')throw new Error('Invalid TH128 online deployment');
 const page=new URL(pageURL),r=value.resource;
 if(!r||!Number.isSafeInteger(r.bytes)||r.bytes<1||r.bytes>MAX_ARCHIVE||!HASH.test(r.sha256)
  ||typeof r.revision!=='string'||!/^[a-z0-9][a-z0-9._-]{0,127}$/i.test(r.revision)||typeof r.url!=='string'||r.url.length>2048)
  throw new Error('Invalid online resource bytes, hash or revision');
 const url=new URL(r.url,page);
 if(!['http:','https:'].includes(page.protocol)||url.origin!==page.origin||url.username||url.password||url.search||url.hash||!url.pathname.endsWith('.zip'))
  throw new Error('Online resource must use this website origin and a ZIP path');
 let parts;
 if(Object.hasOwn(r,'parts')){
  if(!Array.isArray(r.parts)||r.parts.length<1||r.parts.length>64)throw new Error('Invalid online resource part count');
  const seen=new Set();let size=0;
  parts=r.parts.map(part=>{
   if(!part||typeof part.url!=='string'||part.url.length>2048||!Number.isSafeInteger(part.bytes)||part.bytes<1||part.bytes>25*1024*1024||!HASH.test(part.sha256))
    throw new Error('Invalid online resource part bytes or hash');
   const target=new URL(part.url,page);
   if(target.origin!==page.origin||target.username||target.password||target.search||target.hash||!target.pathname.endsWith('.bin')||seen.has(target.href))
    throw new Error('Online resource part must have a unique same-origin binary path');
   seen.add(target.href);size+=part.bytes;return Object.freeze({url:target.href,bytes:part.bytes,sha256:part.sha256});
  });
  if(size!==r.bytes)throw new Error('Online resource part byte sum mismatch');
  Object.freeze(parts);
 }
 return Object.freeze({schema:value.schema,game:'th128',resource:Object.freeze({url:url.href,revision:r.revision,bytes:r.bytes,sha256:r.sha256,...(parts?{parts}:{})})});
}
async function bodyBytes(response,limit,{signal,isCurrent,onChunk=()=>{}}){
 const declared=response.headers?.get('Content-Length');
 if(declared!==null&&declared!==undefined&&(!/^\d+$/.test(declared)||Number(declared)>limit))throw new Error('Download length exceeds the declared byte limit');
 if(!response.body?.getReader){
  const bytes=new Uint8Array(await response.arrayBuffer());current(signal,isCurrent);
  if(bytes.length>limit)throw new Error('Download bytes exceed the byte limit');
  onChunk(bytes.length);return bytes;
 }
 const reader=response.body.getReader(),parts=[];let size=0,complete=false;
 try{
  while(true){
   current(signal,isCurrent);const {done,value}=await reader.read();current(signal,isCurrent);
   if(done){complete=true;break;}
   if(!(value instanceof Uint8Array)||size+value.length>limit)throw new Error('Download bytes exceed the byte limit');
   parts.push(value);size+=value.length;onChunk(size);
  }
  const bytes=new Uint8Array(size);let offset=0;for(const part of parts){bytes.set(part,offset);offset+=part.length;}return bytes;
 }finally{
  if(!complete)await reader.cancel().catch(()=>{});
  reader.releaseLock();
 }
}
export async function readOnlineDeployment({pageURL=globalThis.location?.href,fetchImpl=globalThis.fetch,signal,isCurrent=()=>true}={}){
 current(signal,isCurrent);
 const url=new URL('/deployment.json',pageURL),response=await fetchImpl(url.href,{signal,cache:'no-store',credentials:'same-origin',redirect:'error'});
 current(signal,isCurrent);if(response.status===404)return null;
 if(response.status!==200||response.redirected)throw new Error('Online deployment request failed: '+response.status);
 const bytes=await bodyBytes(response,MAX_MANIFEST,{signal,isCurrent});current(signal,isCurrent);
 let value;try{value=JSON.parse(new TextDecoder('utf8',{fatal:true}).decode(bytes));}catch{throw new Error('Invalid online deployment JSON');}
 return verifiedDeployment(value,pageURL);
}
export async function downloadOnlineArchive(deployment,{pageURL=globalThis.location?.href,fetchImpl=globalThis.fetch,signal,isCurrent=()=>true,onProgress=()=>{}}={}){
 const {resource}=verifiedDeployment(deployment,pageURL);current(signal,isCurrent);
 onProgress({phase:'download',loaded:0,total:resource.bytes});
 const get=async(url,size,label,onChunk)=>{
  current(signal,isCurrent);
  const response=await fetchImpl(url,{signal,credentials:'same-origin',redirect:'error'});current(signal,isCurrent);
  if(response.status!==200||response.redirected||response.url&&new URL(response.url).href!==url)
   throw new Error('Online resource '+label+' status or redirect rejected: '+response.status);
  const declared=response.headers?.get('Content-Length');
  if(declared!==null&&declared!==undefined&&Number(declared)!==size)throw new Error('Online resource '+label+' length mismatch');
  const bytes=await bodyBytes(response,size,{signal,isCurrent,onChunk});current(signal,isCurrent);
  if(bytes.length!==size)throw new Error('Online resource '+label+' bytes are truncated');
  return bytes;
 };
 const hash=async bytes=>{
  const value=[...new Uint8Array(await globalThis.crypto.subtle.digest('SHA-256',bytes))].map(x=>x.toString(16).padStart(2,'0')).join('');
  current(signal,isCurrent);return value;
 };
 let bytes;
 if(resource.parts){
  bytes=new Uint8Array(resource.bytes);let offset=0;
  for(const part of resource.parts){
   const piece=await get(part.url,part.bytes,'part',loaded=>onProgress({phase:'download',loaded:offset+loaded,total:resource.bytes}));
   if(await hash(piece)!==part.sha256)throw new Error('Online resource part SHA-256 hash mismatch');
   bytes.set(piece,offset);offset+=piece.length;
  }
 }else bytes=await get(resource.url,resource.bytes,'download',loaded=>onProgress({phase:'download',loaded,total:resource.bytes}));
 current(signal,isCurrent);onProgress({phase:'hash',loaded:bytes.length,total:resource.bytes});
 if(await hash(bytes)!==resource.sha256)throw new Error('Online resource complete SHA-256 hash mismatch');
 return bytes;
}
function bestEffortStore(store,{signal,isCurrent}){
 let session=null,pending=null,persistent=!!store,cacheError=store?null:'Browser storage is unavailable; resources are kept for this page only';
 const denied=error=>{if(error?.name==='AbortError')throw error;persistent=false;cacheError=error?.message??String(error);};
 return {
  get cached(){return persistent;},get cacheError(){return cacheError;},
  async readCurrent(){
   current(signal,isCurrent);if(session)return session;if(!store)return null;
   try{const result=await store.readCurrent();current(signal,isCurrent);return result;}
   catch(error){current(signal,isCurrent);denied(error);if(pending)session=pending;return session;}
  },
  async commit(value){
   current(signal,isCurrent);pending=value;
   if(!store||!persistent){session=value;return;}
   try{await store.commit(value);current(signal,isCurrent);}
   catch(error){current(signal,isCurrent);denied(error);session=value;}
  }
 };
}
export async function loadOnlineResources(deployment,{pageURL=globalThis.location?.href,store,signal,isCurrent=()=>true,onProgress=()=>{},
 fetchImpl=globalThis.fetch,install=installDatPackage,reopen=reopenDatPackage}={}){
 const expected=verifiedDeployment(deployment,pageURL),origin=new URL(pageURL).origin,cache=bestEffortStore(store,{signal,isCurrent});
 const options={store:cache,origin,signal,isCurrent,onProgress},saved=await cache.readCurrent();current(signal,isCurrent);
 if(saved&&saved.bytes===expected.resource.bytes&&saved.sha256===expected.resource.sha256&&saved.revision===expected.resource.revision){
  let provider=null;
  try{
   onProgress({phase:'cache',loaded:saved.bytes,total:saved.bytes});
   provider=await reopen(options);current(signal,isCurrent);
   if(!provider||provider.revision!==expected.resource.revision)throw new Error('Installed online resource revision mismatch');
   return {provider,source:'cache',cached:cache.cached,cacheError:cache.cacheError};
  }catch(error){provider?.dispose();current(signal,isCurrent);if(error?.name==='AbortError')throw error;}
 }
 const bytes=await downloadOnlineArchive(expected,{pageURL,fetchImpl,signal,isCurrent,onProgress});current(signal,isCurrent);
 let provider=null;
 try{
  provider=await install(bytes,options);current(signal,isCurrent);
  if(provider?.revision!==expected.resource.revision)throw new Error('Downloaded online resource revision mismatch');
  return {provider,source:'download',cached:cache.cached,cacheError:cache.cacheError};
 }catch(error){provider?.dispose();throw error;}
}
