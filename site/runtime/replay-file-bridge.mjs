import {OwnedReplayStore} from './sound.mjs';
const ROOT='replay/',DB='th128-owned-replays-v1',MAX=16*1024*1024;
function fail(message,errno='invalid-field'){return Object.assign(new Error(message),{errno});}
function pathFor(slot){return ROOT+'th128_'+String(slot).padStart(2,'0')+'.rpy';}
function slotFor(path){const match=/^replay\/th128_(\d{2})\.rpy$/.exec(path);if(!match||Number(match[1])>=25)throw fail('Unsupported Replay slot path');return Number(match[1]);}
export function createReplayFileBridge({indexedDB=globalThis.indexedDB,crypto=globalThis.crypto,signal,isCurrent=()=>true}={}){
 const controller=new AbortController(),abort=()=>controller.abort();let store=null,closed=false,busy=false;
 signal?.addEventListener('abort',abort,{once:true});
 function current(){if(closed||signal?.aborted||controller.signal.aborted||!isCurrent())throw new DOMException('Replay file owner cancelled','AbortError');}
 function actualStore(){if(!store)store=new OwnedReplayStore(indexedDB,crypto);return store;}
 async function snapshot(){
  current();if(typeof indexedDB?.databases!=='function')throw fail('Replay storage enumeration is unavailable','unavailable');
  const databases=await indexedDB.databases();current();if(!databases.some(row=>row.name===DB))return Array(25).fill(null);
  const rows=await actualStore().observe();current();return rows;
 }
 return Object.freeze({
  capabilities:Object.freeze({commands:['list','read','write','remove','sync'],root:ROOT,slots:25,mutableBeforeLaunchOnly:true,completeFileBridge:false}),
  dispose(){if(closed)return;closed=true;controller.abort();signal?.removeEventListener('abort',abort);store?.close();},
  async execute(command,data={}){
   current();if(busy)throw fail('Replay file operation is already pending','invalid-state');
   busy=true;
   try{
    if(!['list','read','write','remove','sync'].includes(command))throw fail('File command unsupported','unsupported-command');
    const slot=['read','write','remove'].includes(command)?slotFor(data.path):null;
    const rows=await snapshot();current();
    if(command==='list')return{files:rows.flatMap((row,i)=>row?[{path:pathFor(i),size:row.bytes.length,sha256:row.digest}]:[])};
    if(command==='sync')return{synced:true,storeOnly:true,appliesToRunningGame:false,pendingWrites:0,verifiedFiles:rows.filter(Boolean).length,
     files:rows.flatMap((row,i)=>row?[{path:pathFor(i),size:row.bytes.length,sha256:row.digest}]:[])};
    if(command==='read'){
     const row=rows[slot];if(!row)throw fail('Replay slot is empty',44);
     return{path:data.path,bytes:Array.from(row.bytes),sha256:row.digest};
    }
    if(data.replayMatch!==undefined)throw fail('TH128 Replay match metadata is unavailable','unsupported-command');
    if(command==='write'){
     if(!Array.isArray(data.bytes)||data.bytes.length<36||data.bytes.length>MAX
      ||!data.bytes.every(value=>Number.isInteger(value)&&value>=0&&value<=255))throw fail('Replay bytes are invalid or exceed the byte limit');
     const receipt=await actualStore().importAtSlot(slot,Uint8Array.from(data.bytes),{expectedDigest:rows[slot]?.digest??null,fileName:data.path.slice(ROOT.length),isCurrent:()=>{current();return true;},signal:controller.signal});current();
     return{path:data.path,size:receipt.bytes.length,sha256:receipt.digest,strictFullReadback:receipt.strictFullReadback,strictDurability:receipt.strictDurability};
    }
    if(!rows[slot])throw fail('Replay slot is empty',44);
    const receipt=await actualStore().removeSlot(slot,{expectedDigest:rows[slot].digest,isCurrent:()=>{current();return true;},signal:controller.signal});current();
    return{path:data.path,removed:receipt.removed,strictFullReadback:receipt.strictFullReadback,strictDurability:receipt.strictDurability};
   }finally{busy=false;}
  }
 });
}
