import {makeTh128ReplayFile} from './replay-file.mjs';

/** The child keeps at most the latest validated, sealed Replay file. This slot
 * is independent of the native single-player file and Package owners. */
export function createManagedReplayExport({build,signal,onFile=()=>{},validate=bytes=>makeTh128ReplayFile(bytes,build)}){
  signal?.throwIfAborted();
  let current=null,generation=0,closed=false;
  function close(){
    if(closed)return;
    closed=true;generation++;signal?.removeEventListener('abort',close);
    if(current){current=null;onFile(null);}
  }
  async function remember(bytes){
    if(closed)return false;
    const serial=++generation;
    let next;
    try{next=await validate(bytes);}catch(error){if(!closed&&serial===generation)throw error;return false;}
    if(closed||serial!==generation)return false;
    current=next;onFile(current);return true;
  }
  signal?.addEventListener('abort',close,{once:true});
  return Object.freeze({file:()=>current,remember,close});
}
