import {createDatOggProviderFromFiles,readDatOggDescriptor} from './dat-ogg-provider.mjs';
import {launcherParentOrigin} from './launcher-session.mjs';

const PACKAGE_VERSION='th128-dat-ogg/1';
function need(value,message){if(!value)throw Object.assign(new Error('Managed DAT owner: '+message),{errno:'invalid-field'});}
function abort(){return new DOMException('Managed DAT owner was superseded or closed','AbortError');}
async function limitedBody(response,size,current){
 const declared=response.headers?.get('Content-Length');
 need(declared==null||/^\d+$/.test(declared)&&Number(declared)===size,'resource response length mismatch');
 const reader=response.body?.getReader?.();
 if(!reader){const bytes=new Uint8Array(await response.arrayBuffer());current();need(bytes.length===size,'resource bytes mismatch');return bytes;}
 const result=new Uint8Array(size);let offset=0,complete=false;
 try{
  for(;;){const part=await reader.read();current();if(part.done){complete=true;break;}
   need(part.value instanceof Uint8Array&&part.value.length<=size-offset,'resource exceeded its receipt');
   result.set(part.value,offset);offset+=part.value.length;
  }
  need(offset===size,'resource bytes are truncated');return result;
 }finally{if(!complete)try{await reader.cancel();}catch{}reader.releaseLock();}
}
function managedLease({window=globalThis.window,document=window?.document,location=window?.location,
 signal,isCurrent=()=>true,fetchImpl=globalThis.fetch}={}){
 const query=new URLSearchParams(location?.search??'');
 if(!query.has('managedData'))return null;
 const one=name=>{const values=query.getAll(name);need(values.length===1,'expected one '+name);return values[0];};
 need(one('managedData')==='1','unsupported managedData mode');
 const generation=one('gameGeneration'),epochText=one('runtimeEpoch'),epoch=Number(epochText);
 need(/^[A-Za-z0-9_.:-]{1,128}$/.test(generation),'invalid package generation');
 need(/^[1-9][0-9]*$/.test(epochText)&&Number.isSafeInteger(epoch),'invalid runtime epoch');
 need(window&&window.parent&&window.parent!==window,'managed DATA needs a parent');
 const parent=window.parent,page=new URL(location.href),referrer=document.referrer,href=location.href,search=location.search;
 need(['http:','https:'].includes(page.protocol)&&page.origin===location.origin&&launcherParentOrigin(window,document,location)===page.origin,'parent DATA must share the runtime origin');
 const callback=parent.__eaglerPrepareManagedRuntimeDataV1;
 need(typeof callback==='function','parent DATA callback is unavailable');
 const controller=new AbortController(),onAbort=()=>controller.abort();let disposed=false;
 const identity=Object.freeze({game:'th128',generation,epoch}),deadline=Date.now()+30000;
 signal?.addEventListener('abort',onAbort,{once:true});
 function current(){
  if(disposed||signal?.aborted||controller.signal.aborted||!isCurrent()||window.parent!==parent||window.document!==document
   ||location.href!==href||location.search!==search||location.origin!==page.origin||document.referrer!==referrer
   ||launcherParentOrigin(window,document,location)!==page.origin
   ||parent.__eaglerPrepareManagedRuntimeDataV1!==callback)throw abort();
 }
 function dispose(){if(disposed)return;disposed=true;controller.abort();signal?.removeEventListener('abort',onAbort);}
 function waitOwned(promise){
  return new Promise((resolve,reject)=>{
   let timer;const cancelled=()=>finish(reject,abort());
   const finish=(fn,value)=>{clearTimeout(timer);controller.signal.removeEventListener('abort',cancelled);fn(value);};
   if(controller.signal.aborted){reject(abort());return;}
   controller.signal.addEventListener('abort',cancelled,{once:true});
   timer=setTimeout(()=>finish(reject,new Error('Managed DATA callback timed out')),Math.max(0,deadline-Date.now()));
   Promise.resolve(promise).then(value=>finish(resolve,value),error=>finish(reject,error));
  });
 }
 return {query,page,identity,controller,current,dispose,fetchImpl,waitOwned,request(value){current();return waitOwned(callback.call(parent,value));}};
}
const arrayBufferLength=Object.getOwnPropertyDescriptor(ArrayBuffer.prototype,'byteLength').get;
function ownedBytes(buffer,expected,max){
 let length;
 try{length=arrayBufferLength.call(buffer);}catch{need(false,'parent buffer is not an owned ArrayBuffer');}
 need(length>0&&length<=max&&(expected===null||length===expected),'parent buffer length mismatch');
 return new Uint8Array(buffer).slice();
}
function fileBytes(result,id,row,target){
 need(result&&result.fileId===id&&result.bytes===row.bytes&&(!target||result.path===row.target),'parent file identity/byte receipt mismatch');
 return ownedBytes(result.buffer,row.bytes,row.bytes);
}
function resourceOwner(lease,provider,descriptor,dataReceipt){
 const {page,identity,controller,current,dispose,fetchImpl}=lease;
 return Object.freeze({
   identity,dataReceipt,
   dispose,
   async resources(input){
    current();need(Array.isArray(input)&&input.length<=Object.keys(descriptor.files).length,'resource list exceeds declared package');
    const rows=structuredClone(input),seen=new Set(),receipts=[];
    for(const row of rows){
     need(row&&typeof row.path==='string'&&typeof row.url==='string'&&row.url.length<=2048&&!seen.has(row.path),'invalid or duplicate resource');
     seen.add(row.path);
     const entry=Object.entries(descriptor.files).find(([,value])=>value.target===row.path);
     need(entry,'resource mount is not in the verified package');
     const expected=entry[1],url=new URL(row.url,page);
     need(['http:','https:','blob:'].includes(url.protocol)&&url.origin===page.origin&&!url.username&&!url.password,'resource URL must share the runtime origin');
     if(row.bytes!==undefined)need(Number.isSafeInteger(row.bytes)&&row.bytes===expected.bytes,'resource byte receipt mismatch');
     if(row.size!==undefined)need(Number.isSafeInteger(row.size)&&row.size===expected.bytes,'resource size receipt mismatch');
     if(row.sha256!==undefined)need(typeof row.sha256==='string'&&row.sha256.toLowerCase()===expected.sha256,'resource SHA receipt mismatch');
     current();const response=await fetchImpl(url.href,{signal:controller.signal,credentials:'same-origin',redirect:'error'});current();
     need(response.status===200&&!response.redirected&&(!response.url||new URL(response.url).href===url.href),'resource status/redirect mismatch');
     const bytes=await limitedBody(response,expected.bytes,current);current();
     receipts.push(await provider.verifyMountedFile(row.path,bytes,controller.signal));current();
    }
    return receipts;
   }
 });
}
export async function bindManagedDatProvider(provider,options={}){
 const lease=managedLease(options);if(!lease)return null;
 try{
  need(provider?.descriptor?.runtimeRequirement?.dataFile==='game-data'&&provider.descriptor.files?.['game-data']
   &&typeof provider.adoptManagedData==='function'&&typeof provider.verifyMountedFile==='function','authenticated package consumer is unavailable');
  const descriptor=structuredClone(provider.descriptor),id='game-data';
  const result=await lease.request(lease.identity);lease.current();
  const receipt=await provider.adoptManagedData(fileBytes(result,id,descriptor.files[id],false));lease.current();
  return resourceOwner(lease,provider,descriptor,receipt);
 }catch(error){lease.dispose();throw error;}
}
export async function openManagedDatPackage(options={}){
 const lease=managedLease(options);need(lease,'Package Store mode requires managed DATA');
 let provider;
 try{
  const {query,identity,current,controller}=lease;
  need(query.getAll('managedPackage').length===1&&query.get('managedPackage')==='1','invalid Package Store mode');
  const result=await lease.request(Object.freeze({...identity,packageVersion:PACKAGE_VERSION}));current();
  const bundle=result?.managedPackage;
  need(bundle?.version===PACKAGE_VERSION&&bundle.generation===identity.generation&&bundle.epoch===identity.epoch&&typeof bundle.readFile==='function','parent package identity mismatch');
  const descriptorBytes=ownedBytes(bundle.descriptor,null,1024*1024),descriptor=readDatOggDescriptor(descriptorBytes);
  const id='game-data',data=descriptor.files[id],files=new Map([[id,fileBytes(result,id,data,false)]]),reader=bundle.readFile;
  for(const [fileId,row] of Object.entries(descriptor.files)){
   if(fileId===id)continue;
   current();const receipt=await lease.waitOwned(reader.call(bundle,fileId));current();
   files.set(fileId,fileBytes(receipt,fileId,row,true));
  }
  provider=await createDatOggProviderFromFiles(descriptorBytes,files,{origin:lease.page.origin,signal:controller.signal,
   isCurrent:()=>{current();return true;},onProgress:options.onProgress});current();
  const dataReceipt=Object.freeze({fileId:id,bytes:data.bytes,sha256:data.sha256});
  return Object.freeze({provider,owner:resourceOwner(lease,provider,descriptor,dataReceipt)});
 }catch(error){try{provider?.dispose();}finally{lease.dispose();}throw error;}
}
