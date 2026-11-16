// Transactional in-memory IndexedDB boundary for store tests, not browser durability.
import {encodeReplay} from '../../site/runtime/replay.mjs';
export function replayFile(score=1234){
 const header=new Uint8Array(36),metadata=new Uint8Array(112),stage=new Uint8Array(144);
 header.set([49,50,56,114]);new DataView(header.buffer).setUint32(4,2,true);
 metadata.set(new TextEncoder().encode('TEST    '));const m=new DataView(metadata.buffer);
 m.setBigUint64(12,1281268800n,true);m.setUint32(20,score,true);m.setUint32(92,0,true);m.setUint32(100,1,true);m.setUint32(104,1,true);
 const s=new DataView(stage.buffer);s.setUint16(0,1,true);s.setUint16(2,34772,true);
 s.setInt32(24,0,true);s.setInt32(28,0,true);s.setUint32(128,10000,true);
 return encodeReplay({header:{raw:header},metadata:{raw:metadata},stages:[{rawHeader:stage,inputRaw:new Uint8Array(12),fpsRaw:new Uint8Array()}],payloadTailRaw:new Uint8Array(),dataGapRaw:new Uint8Array(),fileTailRaw:new Uint8Array(),userBlocks:[]},{recompress:true});
}
export function createReplayIDB({strict=true,failWrite=()=>false,alterRead=null,beforeCommit=null,beforeGet=null}={}){
 const dbs=new Map(),counts={opens:0,reads:0,writes:0,deletes:0,commits:0,aborts:0},later=fn=>setImmediate(fn);
 class DB{
  constructor(name){this.name=name;this.stores=new Map();this.queue=[];this.running=false;this.objectStoreNames={contains:name=>this.stores.has(name)};}
  createObjectStore(name){this.stores.set(name,new Map());}
  close(){}
  transaction(names,mode,options={}){
   names=Array.isArray(names)?names:[names];const db=this;
   const tx={durability:strict?options.durability??'default':'relaxed',mode,oncomplete:null,onabort:null,onerror:null,error:null,requests:[],active:0,started:false,finished:false,pending:new Map(),
    objectStore(name){
     if(!names.includes(name)||!db.stores.has(name))throw new Error('Missing synthetic store');
     const request=(kind,key,value)=>{
      const r={result:undefined,error:null,onsuccess:null,onerror:null};tx.active++;
      const run=()=>later(()=>{
       if(tx.finished)return;
       try{
        const rows=tx.pending.get(name);
        if(kind==='get'){
         counts.reads++;beforeGet?.(key,counts,db);
         const found=rows.get(key);r.result=found===undefined?undefined:structuredClone(found);
         if(alterRead)r.result=alterRead(r.result,key,counts);
        }else{
         if(mode!=='readwrite')throw new Error('readonly mutation');
         if(failWrite(kind,key,value,counts))throw new Error('Injected synthetic write failure');
         if(kind==='put'){counts.writes++;rows.set(key,structuredClone(value));r.result=key;}
         else{counts.deletes++;rows.delete(key);}
        }
        tx.active--;r.onsuccess?.();tx.settle();
       }catch(error){r.error=error;tx.error=error;tx.active--;r.onerror?.();tx.abort();}
      });
      if(tx.started)run();else tx.requests.push(run);return r;
     };
     return{get:key=>request('get',key),put:(value,key)=>request('put',key,value),delete:key=>request('delete',key)};
    },
    settle(){
     if(tx.finished||tx.active)return;
     later(()=>{if(tx.finished||tx.active)return;
      if(mode==='readwrite'){beforeCommit?.(tx);if(tx.finished)return;for(const[name,rows]of tx.pending)db.stores.set(name,rows);}
      tx.finished=true;counts.commits++;tx.oncomplete?.();db.running=false;db.pump();
     });
    },
    abort(){if(tx.finished)return;tx.finished=true;counts.aborts++;later(()=>{tx.onabort?.();db.running=false;db.pump();});}
   };
   db.queue.push(()=>{if(tx.finished)return;tx.started=true;for(const name of names)tx.pending.set(name,new Map([...db.stores.get(name)].map(([key,value])=>[key,structuredClone(value)])));for(const run of tx.requests)run();tx.settle();});db.pump();return tx;
  }
  pump(){if(this.running||!this.queue.length)return;this.running=true;later(this.queue.shift());}
 }
 return{
  counts,_dbs:dbs,
  async databases(){return[...dbs.keys()].map(name=>({name,version:1}));},
  open(name){
   counts.opens++;const request={result:null,error:null,onsuccess:null,onerror:null,onupgradeneeded:null};
   later(()=>{let db=dbs.get(name);const fresh=!db;if(!db){db=new DB(name);dbs.set(name,db);}request.result=db;if(fresh)request.onupgradeneeded?.();request.onsuccess?.();});return request;
  },
  seed(slot,record){let db=dbs.get('th128-owned-replays-v1');if(!db){db=new DB('th128-owned-replays-v1');db.createObjectStore('slots');dbs.set(db.name,db);}db.stores.get('slots').set(slot,structuredClone(record));},
  peek(slot){return structuredClone(dbs.get('th128-owned-replays-v1')?.stores.get('slots').get(slot));}
 };
}
