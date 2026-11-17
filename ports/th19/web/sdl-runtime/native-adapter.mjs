// Adapter for the explicit native checkpoint ABI. No copying WASM linear memory.
export function nativeRollbackCore(core,{present=()=>false}={}){
 return {
  save:()=>core._th19_checkpoint_save(),
  load:id=>{if(!id||!core._th19_checkpoint_load(id))throw Error('Rollback checkpoint expired');},
  release:id=>{if(id)core._th19_checkpoint_release(id);},
  canPredict:(a,b)=>!!core._th19_rollback_can_predict((a?.keys||0)|(b?.keys||0)),
  step(frame,a,b,{resimulating,speculative}){
   const safe=!!core._th19_rollback_can_predict(a.keys|b.keys);
   if(speculative&&!safe)throw Error('Unconfirmed transition');
   if(safe)core._th19_rollback_effect_begin();
   let result,count=0;
   try{result=core._th19_network_step(frame,a.keys,b.keys,a.x,a.y,b.x,b.y,a.direct,b.direct,+(!resimulating&&present()));}
   finally{if(safe)count=core._th19_rollback_effect_end();}
   if(!result)throw Error('Rollback input sequence invalid');
   const offset=safe?core._th19_rollback_effect_data()/4:0;
   return {winner:core._th19_network_winner?.()??-1,sounds:Array.from(core.HEAP32.subarray(offset,offset+count)),result,left:{...a},right:{...b},played:!!core._th19_played_frames(),
    hash:(frame+1)%120===0?core._th19_network_hash()>>>0:null};
  },
  commit(effects){for(const sound of effects.sounds)core._th19_rollback_effect_commit(sound);}
 };
}
