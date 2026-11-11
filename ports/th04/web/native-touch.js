import {unpackTouch} from './touch-input.js';
export function touchPlayer(heap,mailbox,patch,slot){
  const code=mailbox-patch.mailbox_cs_offset;
  const data=code+(patch.data_segment-patch.code_segment)*16;
  const motion=slot===0?data+0x464c:slot===1?mailbox+44:code+patch.p3_motion_cs_offset;
  const status=code+patch.touch_status_cs_offset;
  const flags=heap[status+slot],ready=heap[status+3]===1;
  return {motion,miss:!ready||!!(flags&1),entry:!ready||!!(flags&2),out:!!(flags&4)};
}
export function writeNativeTouch(heap,mailbox,patch,inputs,blocked=false){
  if(mailbox<0)return;
  const view=new DataView(heap.buffer),base=mailbox-patch.mailbox_cs_offset+patch.touch_cs_offset;
  for(let slot=0;slot<3;slot++){
    const at=base+slot*patch.touch_stride,touch=unpackTouch(inputs[slot]||0);
    if(blocked||!touch.active){view.setUint16(at,0,true);view.setInt32(at+2,0,true);continue;}
    // Accumulate only until a native player update consumes the movement.
    // Bounds are applied by native code with the correct player bank active.
    const clamp=(v,lo,hi)=>Math.max(lo,Math.min(hi,v));
    if(touch.begin)view.setInt32(at+2,0,true);
    view.setUint16(at,touch.unlimited?3:1,true);
    view.setInt16(at+2,clamp(view.getInt16(at+2,true)+touch.x,-8192,8191),true);
    view.setInt16(at+4,clamp(view.getInt16(at+4,true)+touch.y,-8192,8191),true);
  }
}
