import {readMultiplayerReplay} from './confirmed-history.mjs';

const STORE='rounds',dbName='th128-mp-preview-replays-v1',maxBytes=8*1024*1024;
const credentials=/^[a-zA-Z0-9_-]{1,64}$/;
const digest=async bytes=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),b=>b.toString(16).padStart(2,'0')).join('');
const value=request=>new Promise((resolve,reject)=>{
  request.onsuccess=()=>resolve(request.result);request.onerror=()=>reject(request.error);
});

export async function openPreviewReplayStore({indexedDB,build,matchId,isCurrent}){
  if(typeof indexedDB?.open!=='function'||!credentials.test(matchId)||typeof isCurrent!=='function')
    throw new TypeError('A local TH128 multiplayer Replay owner is required');
  const open=indexedDB.open(dbName,1);
  open.onupgradeneeded=()=>{open.result.createObjectStore(STORE);};
  const db=await value(open);let closed=false;
  function check(){if(closed||!isCurrent())throw new Error('Multiplayer Replay owner retired');}
  try{check();}catch(error){db.close();throw error;}
  db.onversionchange=()=>close();
  async function access(mode,callback){
    check();const tx=db.transaction(STORE,mode,...(mode==='readwrite'?[{durability:'strict'}]:[]));
    const end=new Promise((resolve,reject)=>{tx.oncomplete=resolve;tx.onabort=tx.onerror=()=>reject(tx.error??new Error('Replay transaction aborted'));});
    let output;
    try{[output]=await Promise.all([value(callback(tx.objectStore(STORE))),end]);}
    catch(error){try{tx.abort();}catch{}throw error;}
    check();return output;
  }
  async function decode(bytes){
    if(!(bytes instanceof Uint8Array)||bytes.byteLength===0||bytes.byteLength>maxBytes)throw new Error('Invalid preview Replay bytes');
    const tape=await readMultiplayerReplay(bytes,build);
    if(tape.header.rules.matchId!==matchId)throw new Error('Replay belongs to a different preview match');
    return tape;
  }
  async function read(){
    const row=await access('readonly',store=>store.get(matchId));if(!row)return null;
    if(row.bytes?.byteLength!==row.byteLength||await digest(row.bytes)!==row.sha256)throw new Error('Stored multiplayer Replay checksum differs');
    const tape=await decode(row.bytes);check();return {tape,bytes:row.bytes,sha256:row.sha256};
  }
  async function save(original){
    check();const bytes=original?.slice();await decode(bytes);const hash=await digest(bytes);
    await access('readwrite',store=>store.put({bytes,sha256:hash,byteLength:bytes.byteLength},matchId));
    const stored=await read();
    if(!stored||stored.sha256!==hash||!stored.bytes.every((byte,index)=>byte===bytes[index]))
      throw new Error('Multiplayer Replay write failed its full readback');
    return {sha256:hash,bytes:bytes.byteLength};
  }
  function close(){if(!closed){closed=true;db.close();}}
  return Object.freeze({save,read,close});
}
