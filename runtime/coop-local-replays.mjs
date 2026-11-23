import {readTh128ReplayFile} from './replay-file.mjs';

const schema='th128-multiplayer-replay-shelf-v1',limit=25,maxFile=8*1024*1024;
const recent=(a,b)=>b.savedAt-a.savedAt||a.id.localeCompare(b.id);
const hash=/^[0-9a-f]{64}$/;
export function coopReplaySlotKey(build,slot){
 if(!Number.isInteger(slot)||slot<0||slot>=limit||!hash.test(build.coreSHA256??'')||!hash.test(build.resourceSHA256??''))throw Error('Invalid numbered multiplayer replay slot');
 return `${build.coreSHA256}:${build.resourceSHA256}:${slot}`;
}
const shelfSort=(a,b)=>Number.isInteger(a.slot)?Number.isInteger(b.slot)?a.slot-b.slot:-1:Number.isInteger(b.slot)?1:recent(a,b);
export function pruneCoopReplaySlots(existing,next){return [...existing.filter(row=>row.id!==next.id),next].sort(recent).slice(limit).map(row=>row.id);}
const request=r=>new Promise((resolve,reject)=>{r.onsuccess=()=>resolve(r.result);r.onerror=()=>reject(r.error);});
const completed=t=>new Promise((resolve,reject)=>{t.oncomplete=resolve;t.onabort=()=>reject(t.error??Error('Multiplayer replay storage transaction aborted'));t.onerror=()=>reject(t.error);});
export function createCoopLocalReplays({build,signal,indexedDB=globalThis.indexedDB}){
 let opening=null,closed=false;
 function open(){signal.throwIfAborted();if(closed)throw Error('Multiplayer replay storage closed');
  opening??=new Promise((resolve,reject)=>{
   if(!indexedDB){reject(Error('Multiplayer replay storage unavailable'));return;}
   const r=indexedDB.open(schema,1);
   r.onupgradeneeded=()=>{r.result.createObjectStore('slots',{keyPath:'id'});r.result.createObjectStore('tapes');};
   r.onsuccess=()=>{if(closed){r.result.close();reject(Error('Multiplayer replay storage closed'));}else resolve(r.result);};
   r.onerror=()=>reject(r.error);r.onblocked=()=>reject(Error('Multiplayer replay database is blocked'));
  });return opening;
 }
 signal.addEventListener('abort',()=>{closed=true;void opening?.then(db=>db.close(),()=>{});},{once:true});
 const matches=row=>row&&row.core===build.coreSHA256&&row.resource===build.resourceSHA256;
 return Object.freeze({async save(file,placement=null){
   if(!(file instanceof File)||file.size<1||file.size>maxFile||!file.name.endsWith('.th128mp'))throw Error('Invalid multiplayer replay slot');
   const operation=placement?.signal;operation?.throwIfAborted();
   let id,details={};
   if(placement){
     const {slot,playerName}=placement;id=coopReplaySlotKey(build,slot);
     if(typeof playerName!=='string'||!/^[ -~]{0,8}$/.test(playerName))throw Error('Invalid multiplayer replay name');
     const {tape}=await readTh128ReplayFile(file,build),{rules}=tape.header;
     details={slot,playerName,frames:tape.status().nextFrame,difficulty:rules.difficulty,route:rules.route,players:rules.players};
   }else{
     const digest=await crypto.subtle.digest('SHA-256',await file.arrayBuffer());
     id=Array.from(new Uint8Array(digest),x=>x.toString(16).padStart(2,'0')).join('');
   }
   const row={id,name:file.name,savedAt:Date.now(),core:build.coreSHA256,resource:build.resourceSHA256,...details};const db=await open();signal.throwIfAborted();operation?.throwIfAborted();
   const tx=db.transaction(['slots','tapes'],'readwrite'),done=completed(tx),slots=tx.objectStore('slots'),tapes=tx.objectStore('tapes');
   const cancel=()=>{try{tx.abort();}catch{}};operation?.addEventListener('abort',cancel,{once:true});
   try{
     if(placement){slots.put(row);tapes.put(file,id);}
     else{
       // Legacy saves do not evict the user-selected numbered archives.
       const list=slots.getAll();list.onsuccess=()=>{slots.put(row);tapes.put(file,id);
         for(const key of pruneCoopReplaySlots(list.result.filter(r=>!Number.isInteger(r.slot)),row)){slots.delete(key);tapes.delete(key);}};
     }
     await done;return row;
   }finally{operation?.removeEventListener('abort',cancel);}
  },async list(){const db=await open();signal.throwIfAborted();const rows=await request(db.transaction('slots').objectStore('slots').getAll());return rows.filter(matches).sort(shelfSort);},
  async get(id){if(typeof id!=='string'||(!hash.test(id)&&!(/^[a-f0-9]{64}:[a-f0-9]{64}:(?:[0-9]|1[0-9]|2[0-4])$/).test(id)))return null;
   const db=await open();signal.throwIfAborted();const tx=db.transaction(['slots','tapes']);
   const [row,blob]=await Promise.all([request(tx.objectStore('slots').get(id)),request(tx.objectStore('tapes').get(id))]);
   return matches(row)&&blob instanceof Blob?new File([blob],row.name,{type:'application/octet-stream',lastModified:row.savedAt}):null;
 }});
}
