// Initial-capture batching diagnostics. No reads inside the native callback loop.
const check=(value,message)=>{if(!value)throw Error('Initial capture batch: '+message);};
export const initialCaptureBatchStatusSchema=['schema','enabled','supported','scopeOpen',
 'pendingBlocks','pendingBytes','scopes','deferredBlocks','deferredBytes',
 'copiedBlocks','copiedBytes','flushes','batchImports','maxBlocksPerFlush'];

export function readInitialCaptureBatchStatus(core,app,required=false){
 const fn=core.multiplayer_initial_capture_batch_status;
 if(!fn){check(!required,'status export required');return null;}
 const pointer=fn(app)>>>0,buffer=core.memory.buffer;
 check(pointer>0&&pointer%8===0&&pointer+14*8<=buffer.byteLength,'invalid status range');
 const words=Array.from(new Float64Array(buffer,pointer,14));
 check(words.every(x=>Number.isSafeInteger(x)&&x>=0),'inexact counter');
 check(words[0]===1&&words[1]<=1&&words[2]<=1,'unsupported status schema');
 check(words[3]===0&&words[4]===0&&words[5]===0,'capture escaped its synchronous scope');
 check(words[7]===words[9]&&words[8]===words[10],'accepted payload not completely copied');
 check(words[12]<=words[11]&&words[11]<=words[7]&&words[13]<=words[7],
  'batch import/copy counters disagree');
 return words;
}

export function configureInitialCaptureBatch(core,app,enabled){
 if(enabled===null||enabled===undefined)return;
 readInitialCaptureBatchStatus(core,app,true);
 check(typeof core.multiplayer_initial_capture_batch_set_enabled==='function','control export required');
 check(core.multiplayer_initial_capture_batch_set_enabled(enabled?1:0)===Number(!!enabled),
  'requested option rejected');
 const status=readInitialCaptureBatchStatus(core,app,true);
 check(status[1]===Number(!!enabled),'option did not apply');
}
