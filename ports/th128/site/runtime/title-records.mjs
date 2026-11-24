const PROFILE_BYTES=52232,ROUTE_BYTES=1644,STATS_OFFSET=13152,CLEAR_OFFSET=14220;
const DATABASE='th128-owned-native-title-records-v1',STORE='records',KEY='native-profile';
function nativeProfileView(profile){
  if(!(profile instanceof Uint8Array)||profile.byteLength!==PROFILE_BYTES)throw new Error('Original native title profile size differs');
  const view=new DataView(profile.buffer,profile.byteOffset,profile.byteLength);
  const block=(offset,tag,bytes)=>{if(view.getUint32(offset,true)!==tag||view.getUint32(offset+8,true)!==bytes)throw new Error('Original native title profile block differs');};
  for(let route=0;route<8;route++)block(route*ROUTE_BYTES,0x00035243,ROUTE_BYTES);
  block(STATS_OFFSET,0x00025453,1068);block(CLEAR_OFFSET,0x00014443,38012);
  return view;
}
export function nativeRouteScorePacket(profile){
  const source=profile===null?null:nativeProfileView(profile),packet=new Uint8Array(428),view=new DataView(packet.buffer);
  view.setUint32(0,0x31485254,true);view.setUint32(4,35,true);
  for(let route=0;route<7;route++)for(let rank=0;rank<5;rank++){
    const base=route*ROUTE_BYTES,record=base+16+rank*320,target=8+(route*5+rank)*12;
    const score=source?source.getUint32(record,true):100000,continues=source?source.getUint8(record+5):0,clears=source?source.getUint32(base+0x658+rank*4,true):0;
    if(score>999999999||continues>9)throw new Error('Original native route score or continue byte differs');
    view.setUint32(target,score,true);view.setUint32(target+4,continues,true);view.setUint32(target+8,clears,true);
  }
  return packet;
}
export async function validatedOwnedNativeTitleRecord(record){
  if(record===null||record===undefined)return null;
  if(record.schema!=='th128-owned-native-title-record/1'||!Number.isSafeInteger(record.revision)||record.revision<1
    ||record.provenance!=='ordinary-native-postrun'||record.tainted!==false||typeof record.sha256!=='string'||!(/^[a-f0-9]{64}$/).test(record.sha256))
    throw new Error('Owned native title record provenance differs');
  nativeRouteScorePacket(record.profile);
  const profile=record.profile.slice(),digest=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',profile)),b=>b.toString(16).padStart(2,'0')).join('');
  if(digest!==record.sha256)throw new Error('Owned native title record whole bytes differ');
  return {...record,profile};
}
export class OwnedNativeTitleRecordStore{
  constructor(indexedDB=globalThis.indexedDB){if(!indexedDB||typeof indexedDB.databases!=='function')throw new Error('Owned native title record observation unavailable');this.indexedDB=indexedDB;}
  async observe(){
    const databases=await this.indexedDB.databases();
    if(!databases.some(row=>row.name===DATABASE))return null;
    const database=await new Promise((resolve,reject)=>{
      const request=this.indexedDB.open(DATABASE);
      request.onupgradeneeded=()=>{request.transaction.abort();reject(new Error('Owned native title record disappeared before readonly open'));};
      request.onblocked=()=>reject(new Error('Owned native title record open blocked'));
      request.onerror=()=>reject(request.error||new Error('Owned native title record open failed'));
      request.onsuccess=()=>resolve(request.result);
    });
    try{
      if(!database.objectStoreNames.contains(STORE))throw new Error('Owned native title record store absent');
      const transaction=database.transaction(STORE,'readonly');
      const completed=new Promise((resolve,reject)=>{transaction.oncomplete=()=>resolve();transaction.onabort=()=>reject(transaction.error||new Error('Owned native title record observation aborted'));transaction.onerror=()=>reject(transaction.error||new Error('Owned native title record observation failed'));});
      const value=new Promise((resolve,reject)=>{const request=transaction.objectStore(STORE).get(KEY);request.onsuccess=()=>resolve(request.result);request.onerror=()=>reject(request.error);});
      const [record]=await Promise.all([value,completed]);return await validatedOwnedNativeTitleRecord(record);
    }finally{database.close();}
  }
}
