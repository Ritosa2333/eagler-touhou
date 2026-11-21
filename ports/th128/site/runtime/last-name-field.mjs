const NameKind='th128-original-stats-last-name-field9-v1';
const DefaultName=Uint8Array.from([32,32,32,32,32,32,32,32,0]);
function nameBytes(value){
  if(!(value instanceof Uint8Array)||value.length!==9||value[8]!==0
    ||value.slice(0,8).some(code=>code<32||code>126))throw new Error('Original last-name field must be eight native glyphs and NUL');
  return value.slice();
}
function sameName(first,second){return first instanceof Uint8Array&&second instanceof Uint8Array&&first.length===second.length&&first.every((value,index)=>value===second[index]);}
function revision(value){if(!Number.isSafeInteger(value)||value<0)throw new Error('Last-name field revision is unavailable');return value;}
async function digest(bytes){
  if(!globalThis.crypto?.subtle)throw new Error('Last-name full readback digest is unavailable');
  return Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),value=>value.toString(16).padStart(2,'0')).join('');
}
export function originalNameProfilePacket(record,serial=0){
  if(!record||record.strictRead!==true||!Number.isSafeInteger(serial)||serial<0||serial>0xffffffff
    ||(serial!==0&&record.strictFullReadback!==true))throw new Error('Last-name packet requires the actual owner readback receipt');
  const observedRevision=revision(record.revision),bytes=nameBytes(record.bytes);
  if(observedRevision===0&&!sameName(bytes,DefaultName))throw new Error('Absent last-name owner must retain original blank defaults');
  const output=new Uint8Array(32),view=new DataView(output.buffer),wide=BigInt(observedRevision);
  view.setUint32(0,0x314c4e50,true);view.setUint32(4,1,true);view.setUint32(8,Number(wide&0xffffffffn),true);
  view.setUint32(12,Number(wide>>32n),true);view.setUint32(16,serial,true);output.set(bytes,20);return output;
}
class IndexedNameFieldBackend{
  constructor(){if(!globalThis.indexedDB?.open)throw new Error('Original last-name owner requires IndexedDB');this.database=null;this.pending=null;}
  async open(){
    if(this.database)return this.database;if(this.pending)return this.pending;
    this.pending=new Promise((resolve,reject)=>{
      const request=indexedDB.open('th128-original-last-name-field-v1',1);let rejected=false;
      request.onupgradeneeded=()=>{if(!request.result.objectStoreNames.contains('name-field'))request.result.createObjectStore('name-field');};
      request.onerror=()=>{rejected=true;reject(request.error??new Error('Last-name owner open failed'));};
      request.onblocked=()=>{rejected=true;reject(new Error('Last-name owner is blocked by another connection'));};
      request.onsuccess=()=>{if(rejected){request.result.close();return;}this.database=request.result;this.database.onversionchange=()=>this.close();resolve(this.database);};
    });
    try{return await this.pending;}finally{this.pending=null;}
  }
  async read(){
    const database=await this.open();
    return new Promise((resolve,reject)=>{
      const transaction=database.transaction('name-field','readonly'),request=transaction.objectStore('name-field').get('original-stats-name9');let observed;
      request.onsuccess=()=>{observed=request.result;};
      transaction.oncomplete=()=>resolve(observed);
      transaction.onabort=()=>reject(transaction.error??new Error('Last-name owner read aborted'));
      transaction.onerror=()=>reject(transaction.error??new Error('Last-name owner read failed'));
    });
  }
  async compareAndSwap(expected,next,{strict=false}={}){
    const database=await this.open();
    return await new Promise((resolve,reject)=>{
      const transaction=database.transaction('name-field','readwrite',strict?{durability:'strict'}:undefined);let failure=null;
      transaction.oncomplete=()=>resolve({strictDurability:strict&&transaction.durability==='strict'});
      transaction.onabort=()=>reject(failure??transaction.error??new Error('Last-name owner write aborted'));
      transaction.onerror=()=>reject(failure??transaction.error??new Error('Last-name owner write failed'));
      if(strict&&transaction.durability!=='strict'){failure=new Error('Last-name strict transaction durability was not granted');transaction.abort();return;}
      const store=transaction.objectStore('name-field'),request=store.get('original-stats-name9');
      request.onsuccess=()=>{
        try{
          const actual=request.result;
          const matches=expected.revision===0?actual===undefined:
            actual?.kind===NameKind&&actual.revision===expected.revision&&actual.digest===expected.digest&&sameName(actual.bytes,expected.bytes);
          if(!matches)throw new Error('Last-name owner revision or complete prior field changed');
          store.put({...next,bytes:next.bytes.slice()},'original-stats-name9');
        }catch(error){failure=error;transaction.abort();}
      };
    });
  }
  close(){this.database?.close();this.database=null;}
}
export class OwnedLastNameFieldStore{
  constructor({backend}={}){this.backend=backend??new IndexedNameFieldBackend();}
  async observe(){
    const stored=await this.backend.read();
    if(stored===undefined)return{kind:NameKind,revision:0,bytes:DefaultName.slice(),digest:null,absent:true,strictRead:true};
    if(!stored||stored.kind!==NameKind||revision(stored.revision)===0||typeof stored.digest!=='string'||!/^[0-9a-f]{64}$/.test(stored.digest))
      throw new Error('Stored last-name field owner identity is invalid');
    const bytes=nameBytes(stored.bytes),actualDigest=await digest(bytes);
    if(actualDigest!==stored.digest)throw new Error('Stored last-name complete field digest differs');
    return{kind:NameKind,revision:stored.revision,bytes,digest:actualDigest,absent:false,strictRead:true};
  }
  async saveStrict(name,expected){return this.save(name,expected,{strict:true});}
  async save(name,expected,{strict=false}={}){
    if(!(name instanceof Uint8Array)||name.length!==8||!expected||expected.strictRead!==true||revision(expected.revision)===Number.MAX_SAFE_INTEGER)
      throw new Error('Last-name update requires its exact observed owner revision');
    const bytes=new Uint8Array(9);bytes.set(name);nameBytes(bytes);
    const prior={...expected,bytes:nameBytes(expected.bytes)};
    if(prior.revision===0){if(prior.digest!==null||!sameName(prior.bytes,DefaultName))throw new Error('Invalid absent last-name owner receipt');}
    else if(prior.kind!==NameKind||await digest(prior.bytes)!==prior.digest)throw new Error('Invalid observed last-name owner digest');
    const next={kind:NameKind,revision:prior.revision+1,bytes,digest:await digest(bytes)};
    const capability=await this.backend.compareAndSwap(prior,next,{strict});
    if(strict&&capability?.strictDurability!==true)throw new Error('Last-name backend did not attest granted strict durability');
    const actual=await this.observe();
    if(actual.revision!==next.revision||actual.digest!==next.digest||!sameName(actual.bytes,next.bytes))
      throw new Error('Last-name full durable readback differs from the intended field');
    return{...actual,strictFullReadback:true,strictDurability:strict&&capability.strictDurability===true,fullFieldBytes:9};
  }
  close(){this.backend.close?.();}
}
