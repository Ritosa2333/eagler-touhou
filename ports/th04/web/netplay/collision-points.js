import {touchPlayer} from '../native-touch.js';

// Presentation only: never mutate emulator memory or send preferences to peers.
export function drawCollisionPoints(context,{heap,mailbox,patch,players=2,focus=true,always=false}){
  const points=collisionPoints({heap,mailbox,patch,players,focus,always});
  if(!points.length)return;
  context.save();
  context.beginPath();context.rect(32,16,384,368);context.clip();
  for(const [x,y]of points){
    context.fillStyle='#000';context.fillRect(x-2,y-2,5,5);
    context.fillStyle='#fff';context.fillRect(x-1,y-1,3,3);
  }
  context.restore();
}

export function collisionPoints({heap,mailbox,patch,players=2,focus=true,always=false}){
  const points=[];
  if(mailbox<0||!patch)return points;
  const view=new DataView(heap.buffer),code=mailbox-patch.mailbox_cs_offset;
  const data=code+(patch.data_segment-patch.code_segment)*16;
  for(let slot=0;slot<players;slot++){
    const player=touchPlayer(heap,mailbox,patch,slot);
    const focused=heap[slot===0?data+0x3976:slot===1?mailbox+24:code+patch.p3_input_cs_offset+2]!==0;
    if(player.miss||player.entry||player.out||!(always||focus&&focused))continue;
    // Match native 1/16-pixel collision coordinates and the 5x5 / 3x3 marker.
    const x=(view.getInt16(player.motion+2,true)>>4)+32;
    const y=(view.getInt16(player.motion+4,true)>>4)+16;
    points.push([x,y]);
  }
  return points;
}
