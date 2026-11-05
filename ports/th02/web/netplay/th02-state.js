import {unpackTouch} from '../touch-input.js';
export const signature=new TextEncoder().encode('TH02COOPv0002!!\0');
export const mailboxActive=(heap,patch,at)=>at>=0&&at+patch.players.mailboxSize<=heap.length&&heap[at+22]===1&&signature.every((b,i)=>heap[at+i]===b);
// Network/player UI bits stay common to the other games; only this boundary
// translates them into TH02's native shot/bomb/focus/menu convention.
export const nativeBits=value=>(value&15)|((value&32)>>1)|((value&16)<<1)|((value&64)<<2)|((value&128)>>1)|((value&256)>>1);
export function locateMailbox(heap,patch,previous=-1){
 const valid=at=>at>=0&&at+patch.players.mailboxSize<=heap.length&&signature.every((b,i)=>heap[at+i]===b);
 if(valid(previous)&&heap[previous+22]===1)return previous;
 const view=new DataView(heap.buffer),found=[];
 for(let at=heap.indexOf(signature[0]);at>=0;at=heap.indexOf(signature[0],at+1))
  if(valid(at)&&heap[at+22]===1&&view.getUint16(at+20,true)>0)found.push(at);
 return found.length===1?found[0]:-1;
}
export function guestBase(heap,mailbox,patch){
 const data=mailbox-patch.entries.mailbox+(patch.dataSegment-patch.codeSegment)*16;
 const base=data-new DataView(heap.buffer).getUint16(mailbox+20,true)*16;
 if(base<0||base+1048576>heap.length)throw Error('TH02 客体内存布局不匹配');
 return base;
}
export function nativePaused(heap,mailbox,patch){return mailboxActive(heap,patch,mailbox)&&!!heap[mailbox-patch.entries.mailbox+patch.entries.touch_paused];}
export function writeInputs(heap,mailbox,patch,inputs,players,settings={}){
 if(!mailboxActive(heap,patch,mailbox))return;
 const v=new DataView(heap.buffer),cb=mailbox-patch.entries.mailbox,blocked=!heap[mailbox+22]||nativePaused(heap,mailbox,patch);
 for(let slot=0;slot<3;slot++){
  const offline=!!(heap[cb+patch.entries.offline_mask]&(1<<slot));
  const value=slot<players&&!offline?inputs[slot]:0;
  v.setUint16(mailbox+patch.players.inputOffsets[slot],nativeBits(value),true);
  heap[mailbox+patch.focus.percentOffsets[slot]]=50;
  const motion=unpackTouch(value),at=cb+patch.entries.touch_state+slot*patch.touch.stride;
  const allowed=!blocked&&motion.active;
  if(motion.begin){v.setInt32(at+2,0,true);v.setInt32(cb+patch.entries.touch_remainder+slot*4,0,true);}
  v.setUint16(at,allowed?(motion.unlimited?3:1):0,true);
  const clamp=x=>Math.max(-8192,Math.min(8191,x));
  v.setInt16(at+2,allowed?clamp(v.getInt16(at+2,true)+motion.x):0,true);
  v.setInt16(at+4,allowed?clamp(v.getInt16(at+4,true)+motion.y):0,true);
 }
 heap[mailbox+patch.focus.enabledOffset]=settings.focusEnabled===false?0:(1<<players)-1;
 // Display preferences never write different native state on different peers.
 heap[mailbox+patch.focus.pointOffset]=0;
 heap[mailbox+patch.targeting.enabledOffset]=Number(settings.teamTargeting!==false);
 heap[mailbox+patch.debug.invincibilityMailboxOffset]=0;
}
export function readState(heap,mailbox,patch){
 if(!mailboxActive(heap,patch,mailbox))return null;
 const v=new DataView(heap.buffer),cb=mailbox-patch.entries.mailbox,b=patch.balance;
 const count=heap[mailbox+patch.players.countOffset];
 // The stage hook publishes its address before its first complete snapshot.
 // Keep that short initialization interval in confirmed-only boot mode.
 if(![2,3].includes(count)||v.getUint32(mailbox+16,true)===0)return null;
 return {gameFrame:v.getUint32(mailbox+16,true),generation:v.getUint16(mailbox+28,true),stage:heap[mailbox+30],
  score:v.getUint32(cb+patch.entries.team_score,true),players:patch.players.stateOffsets.slice(0,count).map((offset,i)=>{
   const p=mailbox+offset,stats=cb+patch.entries[b.statsEntry];
   return {id:i+1,x:v.getInt16(p,true),y:v.getInt16(p+2,true),lives:heap[p+4],bombs:heap[p+5],power:heap[p+6],
    hit:heap[p+7],invincibility:heap[p+8],shot:heap[p+9],out:!!(heap[mailbox+23]&(1<<i)),offline:!!(heap[cb+patch.entries.offline_mask]&(1<<i)),
    score:v.getUint32(stats+b.scoreOffset,true),nextExtend:v.getUint32(stats+b.nextExtendOffset,true)||null};
  })};
}
