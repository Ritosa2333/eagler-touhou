import {unpackTouch} from './touch-input.js';
// The browser only queues relative motion. Native TH02 applies the movement
// in the correct player bank, with speed limits, bounds and real collision.
export function writeNativeTouch(heap,mailbox,patch,slot,input,blocked=false){
  if(mailbox<0)return;
  const view=new DataView(heap.buffer),base=mailbox-patch.entries.mailbox+patch.entries.touch_state;
  const motion=unpackTouch(input),clamp=value=>Math.max(-8192,Math.min(8191,value));
  for(let seat=0;seat<3;seat++){
    const at=base+seat*patch.touch.stride;
    if(blocked||seat!==slot||!motion.active){view.setUint16(at,0,true);view.setInt32(at+2,0,true);continue;}
    if(motion.begin){view.setInt32(at+2,0,true);view.setInt32(mailbox-patch.entries.mailbox+patch.entries.touch_remainder+seat*4,0,true);}
    view.setUint16(at,motion.unlimited?3:1,true);
    view.setInt16(at+2,clamp(view.getInt16(at+2,true)+motion.x),true);
    view.setInt16(at+4,clamp(view.getInt16(at+4,true)+motion.y),true);
  }
}
