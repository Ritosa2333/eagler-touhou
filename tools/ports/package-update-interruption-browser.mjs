import {installPackageFromRemote} from '../../launcher/package/package-installer.mjs';
import {readCurrentPackageGeneration,readPackageGeneration,readPackageObject} from '../../launcher/package/package-store.mjs';

/** Run the real installer and HTTP transport; abort the second new object after staging the first. */
export async function run({descriptor,descriptorUrl,expectedGenerationId,interruptAtBytes}){
  const game=descriptor.game,controller=new AbortController(),hash=async bytes=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),byte=>byte.toString(16).padStart(2,'0')).join('');
  const record={schema:'eagler-touhou/interrupted-package-download/1',game,descriptor,descriptorUrl,progress:[],requests:[],before:await readCurrentPackageGeneration(game)};
  try{
    await installPackageFromRemote(descriptor,{descriptorUrl,desiredFileIds:Object.keys(descriptor.files),expectedGenerationId,signal:controller.signal,
      onProgress:progress=>record.progress.push({...progress}),
      fetchImpl:async(url,options)=>{
        const request={url:String(url),signalBefore:controller.signal.aborted};record.requests.push(request);
        const completed=record.progress.find(row=>row.found===true&&row.reused!==true);
        if(completed){
          const current=await readCurrentPackageGeneration(game),pending=await readPackageGeneration(game,current.installation.pendingGeneration),reference=pending.files[completed.fileId];
          const object=await readPackageObject(reference.objectId),blob=object.blob??new Blob([object.data]),bytes=await blob.arrayBuffer();
          record.staged={current,pending,fileId:completed.fileId,objectId:reference.objectId,bytes:bytes.byteLength,sha256:await hash(bytes)};
        }
        const response=await fetch(url,options);Object.assign(request,{status:response.status,contentLength:Number(response.headers.get('content-length'))});
        if(completed){
          const reader=response.body.getReader(),chunks=[];let received=0;
          while(received<interruptAtBytes){const chunk=await reader.read();if(chunk.done)throw Error('Response completed before its interruption boundary');chunks.push(chunk.value);received+=chunk.value.length;}
          const prefix=new Uint8Array(received);let offset=0;for(const chunk of chunks){prefix.set(chunk,offset);offset+=chunk.length;}
          request.prefix={bytes:received,sha256:await hash(prefix)};
          const body=reader.read();controller.abort();request.signalAfter=controller.signal.aborted;
          try{await body;request.bodyCompleted=true;}catch(error){request.bodyError={name:error.name,message:error.message};}
          if(request.bodyError?.name!=='AbortError')throw Error('Actual HTTP body did not abort');
          record.abortedRequest=request;throw Object.assign(new Error(request.bodyError.message),{name:request.bodyError.name});
        }
        return response;
      }});
    record.unexpectedCommit=true;
  }catch(error){record.error={name:error.name,message:error.message};}
  record.after=await readCurrentPackageGeneration(game);
  if(record.staged){
    record.pendingAfter=await readPackageGeneration(game,record.staged.pending.id);
    record.stagedObjectAfter=await readPackageObject(record.staged.objectId);
  }
  return record;
}
