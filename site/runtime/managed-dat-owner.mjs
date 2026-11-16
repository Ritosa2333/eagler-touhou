// Managed DATA augments an independently verified, complete resource package.
// It does not implement arbitrary mounts, mutable files or a replacement package.
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
export async function bindManagedDatProvider(provider,{window=globalThis.window,document=window?.document,location=window?.location,
 signal,isCurrent=()=>true,fetchImpl=globalThis.fetch}={}){
 const query=new URLSearchParams(location?.search??'');
 if(!query.has('managedData'))return null;
 const one=name=>{const values=query.getAll(name);need(values.length===1,'expected one '+name);return values[0];};
 need(one('managedData')==='1','unsupported managedData mode');
 const generation=one('gameGeneration'),epochText=one('runtimeEpoch'),epoch=Number(epochText);
 need(/^[A-Za-z0-9_.:-]{1,128}$/.test(generation),'invalid package generation');
 need(/^[1-9][0-9]*$/.test(epochText)&&Number.isSafeInteger(epoch),'invalid runtime epoch');
 need(window&&window.parent&&window.parent!==window,'managed DATA needs a parent');
 const parent=window.parent,page=new URL(location.href),referrer=new URL(document.referrer),href=location.href,search=location.search;
 need(['http:','https:'].includes(page.protocol)&&page.origin===location.origin&&referrer.origin===page.origin,'parent DATA must share the runtime origin');
 const callback=parent.__eaglerPrepareManagedRuntimeDataV1;
 need(typeof callback==='function','parent DATA callback is unavailable');
 need(provider?.descriptor?.runtimeRequirement?.dataFile==='game-data'&&provider.descriptor.files?.['game-data']
  &&typeof provider.adoptManagedData==='function'&&typeof provider.verifyMountedFile==='function','authenticated package consumer is unavailable');
 const descriptor=structuredClone(provider.descriptor),data=descriptor.files['game-data'];
 const controller=new AbortController(),onAbort=()=>controller.abort();let disposed=false;
 signal?.addEventListener('abort',onAbort,{once:true});
 function current(){
  if(disposed||signal?.aborted||controller.signal.aborted||!isCurrent()||window.parent!==parent||window.document!==document
   ||location.href!==href||location.search!==search||location.origin!==page.origin||document.referrer!==referrer.href
   ||parent.__eaglerPrepareManagedRuntimeDataV1!==callback)throw abort();
 }
 function dispose(){if(disposed)return;disposed=true;controller.abort();signal?.removeEventListener('abort',onAbort);}
 function waitOwned(promise){
  return new Promise((resolve,reject)=>{
   let timer;const cancelled=()=>finish(reject,abort());
   const finish=(fn,value)=>{clearTimeout(timer);controller.signal.removeEventListener('abort',cancelled);fn(value);};
   if(controller.signal.aborted){reject(abort());return;}
   controller.signal.addEventListener('abort',cancelled,{once:true});
   timer=setTimeout(()=>finish(reject,new Error('Managed DATA callback timed out')),30000);
   Promise.resolve(promise).then(value=>finish(resolve,value),error=>finish(reject,error));
  });
 }
 try{
  current();const request=Object.freeze({game:'th128',generation,epoch});
  const result=await waitOwned(callback.call(parent,request));current();
  need(result&&result.fileId==='game-data'&&Number.isSafeInteger(result.bytes)&&result.bytes===data.bytes,'parent DATA identity/byte receipt mismatch');
  // Intrinsic ArrayBuffer access accepts a same-origin parent's realm and
  // rejects array-like objects, detached buffers and SharedArrayBuffer.
  let copied;
  try{copied=ArrayBuffer.prototype.slice.call(result.buffer,0);}catch{need(false,'parent DATA buffer is not an owned ArrayBuffer');}
  need(copied.byteLength===result.bytes,'parent DATA buffer length mismatch');
  const receipt=await provider.adoptManagedData(new Uint8Array(copied));current();
  return Object.freeze({
   identity:Object.freeze({game:'th128',generation,epoch}),dataReceipt:receipt,
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
 }catch(error){dispose();throw error;}
}
