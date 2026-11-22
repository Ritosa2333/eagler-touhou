import {encodeReplay} from './native-replay.mjs';
import {writeRoomReplay} from './managed-storage.mjs';

// The current .th03r reader replays from room boot. Keep its input prefix here,
// outside the game thread, and snapshot only the confirmed length of each save.
const rows=[];
let writing=Promise.resolve();
globalThis.onmessage=({data})=>{
  if(data.type==='append'){rows.push(...data.rows);return;}
  if(data.type!=='save')return;
  const {id,frames,native,focus,difficulty,seed}=data;
  writing=writing.then(async()=>{
    const bytes=await encodeReplay(rows.slice(0,frames),focus,native,{difficulty,seed});
    const path=`replay/native-${Date.now()}-${crypto.randomUUID()}.th03r`;
    await writeRoomReplay(path,bytes);
    globalThis.postMessage({id,path,bytes},[bytes.buffer]);
  }).catch(error=>globalThis.postMessage({id,error:String(error?.message||error)}));
};
