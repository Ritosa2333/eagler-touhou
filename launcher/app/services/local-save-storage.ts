import {replayStorageLocation} from '../../src/launcher/replay-storage-policy.mts';
import type {ProductId} from '../../src/contracts/product-catalog.mts';
import {gameIdForProduct, isMultiplayerProductId} from '../../src/contracts/product-catalog.mts';
import type {RuntimeService} from './runtime';
import {idbfsBytes, openIdbfsDatabase, type IdBfsEntry} from './idbfs';

export type LocalSaveReader=(product:ProductId,names:readonly string[])=>Promise<File[]>;
export function validLocalSaveNames(names:readonly string[]) {
  if(!names.length||names.length>12||new Set(names).size!==names.length||names.some(name=>name.length>128||name.startsWith('/')||name.includes('\\')||name.split('/').some(part=>!part||part==='.'||part==='..')))throw new Error('Invalid save filenames');
}
export function validateLocalSaveFile(bytes:Uint8Array,total:number) {
  if(!bytes.length||bytes.length>4*1024*1024||total>8*1024*1024)throw new Error('Local save exceeds the upload limit');
}
export function localSaveFiles(files:{name:string;bytes:Uint8Array}[],product:ProductId,language:string) {
  const preferred=language==='lang_zh-hans'?'scoreth10c.dat':'scoreth10.dat';
  if((product==='th10'||product==='th10mp')&&files.some(file=>file.name===preferred))files=files.filter(file=>!/^scoreth10c?\.dat$/.test(file.name)||file.name===preferred);
  return files.map(file=>new File([new Uint8Array(file.bytes)],file.name,{type:'application/octet-stream'}));
}
/** Reads one committed transaction. Runtime remains the only IDBFS writer. */
export async function readLocalSaveFiles(product:ProductId,names:readonly string[],language:string,factory:IDBFactory) {
  validLocalSaveNames(names);
  const {database:databaseName,root}=replayStorageLocation(product,language),database=await openIdbfsDatabase(factory,databaseName);
  if(!database)return [];
  try {
    const files=await new Promise<{name:string;bytes:Uint8Array}[]>((resolve,reject)=>{
      const transaction=database.transaction('FILE_DATA','readonly'),store=transaction.objectStore('FILE_DATA'),files:{name:string;bytes:Uint8Array}[]=[];
      let total=0,problem:unknown;
      for(const name of names) {
        const request=store.get(root+'/'+name);
        request.onsuccess=()=>{
          try {
            const entry=request.result as IdBfsEntry|undefined;if(!entry)return;
            if((entry.mode&0xf000)!==0x8000)throw new Error('Invalid local save entry');
            const bytes=idbfsBytes(entry.contents);if(!bytes)throw new Error('Invalid local save contents');
            total+=bytes.length;validateLocalSaveFile(bytes,total);files.push({name,bytes});
          }catch(error){problem=error;transaction.abort();}
        };
      }
      transaction.onerror=transaction.onabort=()=>reject(problem??transaction.error??new Error('Could not read local saves'));
      transaction.oncomplete=()=>resolve(files);
    });
    return localSaveFiles(files,product,language);
  } finally {database.close();}
}

export async function collectLocalSaveFiles({product,names,language,indexedDB,runtime}: {
  product:ProductId;names:readonly string[];language:string;indexedDB:IDBFactory;runtime:RuntimeService|null;
}) {
  validLocalSaveNames(names);
  const current=runtime?.getSnapshot(),variant=isMultiplayerProductId(product)?'multiplayer':'normal';
  if(!runtime||!current?.ready||current.game!==gameIdForProduct(product)||current.runtimeVariant!==variant)return readLocalSaveFiles(product,names,language,indexedDB);
  return runtime.withFileSession(gameIdForProduct(product),async access=>{
    await access.sync();const files:{name:string;bytes:Uint8Array}[]=[];let total=0;
    for(const name of names) {
      try {
        const response=await access.send('read',{path:name});
        if(!Array.isArray(response.bytes)||response.bytes.length>4*1024*1024)throw new Error('Invalid Runtime save contents');
        for(let index=0;index<response.bytes.length;index++)if(!Object.prototype.hasOwnProperty.call(response.bytes,index)||!Number.isInteger(response.bytes[index])||response.bytes[index]<0||response.bytes[index]>255)throw new Error('Invalid Runtime save contents');
        const bytes=new Uint8Array(response.bytes);total+=bytes.length;validateLocalSaveFile(bytes,total);files.push({name,bytes});
      }catch(error){if(!(error&&typeof error==='object'&&'errno'in error&&error.errno===44))throw error;}
    }
    return localSaveFiles(files,product,language);
  },{readOnly:true,runtimeVariant:variant,epoch:current.epoch??undefined});
}
