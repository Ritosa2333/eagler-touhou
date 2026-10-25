import assert from 'node:assert/strict';
import {basename} from 'node:path';
import {HOST_PROTOCOL,PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';

/** Existing-only reads leave an uninstalled browser profile uninstalled. */
export async function readPackageImportBefore(game){
  try{const current=await window.__offlinePackageStoreObserver.readCurrentPackageGeneration(game);
    return{databaseAbsent:false,currentGeneration:current.generation?.id??null};}
  catch(error){if(String(error?.message)=== 'Offline Package Store is absent; observation cannot create it')return{databaseAbsent:true,currentGeneration:null};throw error;}
}

export async function readPackageImportState(input){
  const {game,generationId}=typeof input==='string'?{game:input}:input;
  const current=await window.__offlinePackageStoreObserver.readCurrentPackageGeneration(game),installation=current.installation,
    generation=generationId?await window.__offlinePackageStoreObserver.readPackageGeneration(game,generationId):current.generation;
  if(!installation||!generation)throw Error('Import route has no committed Package Store generation');
  const d=generation.descriptor,id=d.runtimeRequirement?.dataFile,declaration=d.files[id],reference=generation.files[id];
  const object=await window.__offlinePackageStoreObserver.readPackageObject(reference.objectId);
  if(!object?.blob)throw Error('Import route has no readable DATA object');const bytes=await object.blob.arrayBuffer();
  const sha256=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),value=>value.toString(16).padStart(2,'0')).join('');
  return{installation,generation:{id:generation.id,game:generation.game,descriptor:d,files:generation.files},data:{fileId:id,objectId:reference.objectId,
    target:declaration.target,declaredBytes:declaration.bytes,declaredSha256:declaration.sha256??null,bytes:bytes.byteLength,sha256}};
}

/** Recompute version, installation ownership and actual provider bytes for every supported source. */
export function assertPackageImportParity(e,{game,expectedData,expectedLayout,publishedRevision,runtimeGeneration,archiveIdentity,originalIdentity}={}){
  assert.equal(e.schema,'eagler-touhou/package-import-parity/1');assert.deepEqual(e.cleanupErrors,[]);
  const product=PRODUCT_GAMES[game],rawNames=product.package.rawDataImport?.fileNames??[],sources=['zip','online',...(rawNames.length?['original-file']:[])];
  assert.deepEqual(e.routes.map(row=>row.source),sources,'Every supported installation source needs its own real route');
  assert.equal(new Set(e.routes.map(row=>row.contextId)).size,sources.length);
  for(const row of e.routes){
    assert.equal(row.status,'passed');assert.equal(row.before.currentGeneration,null,'Source route reused an installed generation');
    assert.equal(row.ui.gameVersion,'original');
    if(row.source==='online'){assert.equal(row.ui.imported,false);assert.ok(row.responses.some(response=>response.expected?.sha256===expectedData.sha256&&
      response.actual?.sha256===expectedData.sha256&&response.actual.bytes===expectedData.bytes&&response.contentIdentity==='passed'),'Online route did not acquire actual DATA over HTTP');}
    else{
      assert.equal(row.ui.imported,true);assert.equal(row.ui.importClosed,true);
      const input=row.source==='zip'?archiveIdentity:originalIdentity;assert.deepEqual(row.input,input);
      if(row.source==='original-file'){assert.ok(rawNames.some(name=>name.toLowerCase()===basename(input.path).toLowerCase()),'Original import filename is not declared');
        assert.equal(input.bytes,expectedData.bytes);assert.equal(input.sha256,expectedData.sha256);}
    }
    for(const [current,state,runtime] of [[row.installed,row.pinned,row.runtime],[row.reopened.installed,row.reopened.pinned,row.reopened.runtime]]){
      const {installation,generation,data}=state,d=generation.descriptor,url=new URL(runtime.frameUrl),epoch=Number(url.searchParams.get('runtimeEpoch'));
      assert.equal(generation.game,game);assert.equal(d.game,game);assert.equal(d.gameVersion??'original','original');
      assert.equal(installation.game,game);assert.equal(current.installation.currentGeneration,current.generation.id);assert.equal(current.installation.pendingGeneration,null);
      assert.equal(current.installation.source,row.source==='online'?'remote':'local');
      assert.deepEqual(current.generation.descriptor,d,'Current and loaded DATA declarations diverged');
      assert.deepEqual(current.data,data,'Current and loaded DATA objects diverged');
      for(const id of d.base.files)assert.deepEqual(current.generation.files[id],generation.files[id],'A base object changed while the native instance was pinned');
      for(const [id,reference]of Object.entries(generation.files))assert.deepEqual(current.generation.files[id],reference,'Current generation lost a loaded file');
      for(const [id,reference]of Object.entries(current.generation.files)){assert.ok(Object.hasOwn(d.files,id),'Current generation added an undeclared file');assert.ok(typeof reference.objectId==='string'&&reference.objectId);}
      assert.equal(d.runtimeRequirement.protocol,HOST_PROTOCOL);assert.equal(d.runtimeRequirement.target,game);assert.equal(d.runtimeRequirement.dataLayout,expectedLayout);
      assert.equal(d.runtimeRequirement.dataFile,product.package.dataFileId);assert.equal(data.fileId,product.package.dataFileId);
      assert.equal(data.target,product.package.dataTarget);assert.equal(data.bytes,expectedData.bytes);assert.equal(data.declaredBytes,expectedData.bytes);assert.equal(data.sha256,expectedData.sha256);
      if(row.source==='original-file'){assert.equal(d.revision,'raw-'+expectedData.sha256.slice(0,16));assert.equal(data.declaredSha256,null);}
      else{assert.equal(d.revision,publishedRevision);assert.equal(data.declaredSha256,expectedData.sha256);}
      assert.equal(url.searchParams.get('gameGeneration'),generation.id);assert.equal(url.searchParams.get('runtimeVariant'),'normal');assert.equal(url.searchParams.get('managedData'),'1');
      assert.ok(url.pathname.startsWith(`/runtime/${game}/${runtimeGeneration}/`));assert.ok(Number.isSafeInteger(epoch)&&epoch>0);
      for(const event of [runtime.ready,runtime.first]){assert.equal(event.protocol,HOST_PROTOCOL);assert.equal(event.game,game);assert.equal(event.epoch,epoch);}
      assert.ok(Number.isSafeInteger(runtime.first.presentation.nativePresentCount)&&runtime.first.presentation.nativePresentCount>0);assert.equal(runtime.workerControlled,true);
      assert.equal(runtime.dataRead.game,game);assert.equal(runtime.dataRead.epoch,epoch);assert.equal(runtime.dataRead.generation,generation.id);
      assert.equal(runtime.dataRead.fileId,data.fileId);assert.equal(runtime.dataRead.bytes,expectedData.bytes);assert.equal(runtime.dataRead.sha256,expectedData.sha256);
    }
    assert.deepEqual(row.reopened.installed.generation.descriptor,row.installed.generation.descriptor,'Page reopen changed the committed source declaration');
    for(const id of row.installed.generation.descriptor.base.files)assert.deepEqual(row.reopened.installed.generation.files[id],row.installed.generation.files[id],'Page reopen replaced a committed base object');
    assert.equal(row.reopened.ui.gameVersion,'original');
  }
  return{complete:true,sources,gameVersion:'original',dataSha256:expectedData.sha256};
}
