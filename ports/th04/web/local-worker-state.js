import {writeNativeTouch,touchPlayer} from './native-touch.js';
export function createLocalWorkerState(emulator,patch){
 const signature=new TextEncoder().encode('TH04COOPLABv001!');let mailbox=-1,cursor=0,candidates=[];
 function scan(){const h=emulator.module.HEAPU8,v=new DataView(h.buffer);
  candidates=candidates.filter(at=>signature.every((b,i)=>h[at+i]===b));
  const live=()=>candidates.filter(at=>h[at+25]===1&&v.getUint32(at+16,true)>0&&v.getUint16(at+20,true)>0);
  if(!live().length){if(cursor>=h.length)cursor=0;const end=Math.min(cursor+1048576,h.length),part=h.subarray(cursor,Math.min(end+15,h.length));
   for(let i=part.indexOf(signature[0]);i>=0&&cursor+i<end;i=part.indexOf(signature[0],i+1)){
    const at=cursor+i;if(at+675<h.length&&signature.every((b,j)=>h[at+j]===b)&&!candidates.includes(at))candidates.push(at);
   }cursor=end;
  }
  const active=live();if(active.length===1)mailbox=active[0];else if(!active.includes(mailbox))mailbox=-1;
  emulator.module.projectileArt?.bind(mailbox);
 }
 function read(){scan();if(mailbox<0)return null;const h=emulator.module.HEAPU8,v=new DataView(h.buffer),m=mailbox,cs=m-patch.mailbox_cs_offset;
  const players=[];if(h[m+605]===1)for(let i=0;i<Math.min(h[m+606],h[m+607]);i++){
   const at=m+609+i*h[m+608];players.push({id:i+1,power:h[at],level:h[at+1],lives:Math.max(0,h[at+4]-1),bombs:h[at+5],out:!!h[at+6]});
  }
  return {at:m,ticks:v.getUint32(m+16,true),ds:v.getUint16(m+20,true),stage:v.getUint16(m+26,true),players,
   shots:v.getUint32(m+28,true),hits:v.getUint32(m+32,true),shotHits:v.getUint32(m+85,true),items:v.getUint32(m+597,true),targets:v.getUint32(m+601,true),
   p1:[v.getInt16(m+36,true)/16,v.getInt16(m+38,true)/16],p2:[v.getInt16(m+46,true)/16,v.getInt16(m+48,true)/16],
   stageFrame:v.getUint16(m+40,true),p1Miss:h[m+42],p1Inv:h[m+43],p2Inv:h[m+66],p2Miss:h[m+72],input:v.getUint16(m+22,true),
   nativePaused:!!h[cs+patch.native_pause_cs_offset+3],touch:touchPlayer(h,m,patch,0)};
 }
 function input(value){if(mailbox<0)return;const h=emulator.module.HEAPU8,v=new DataView(h.buffer),cs=mailbox-patch.mailbox_cs_offset;
  if(h[mailbox+25]!==1||!signature.every((b,i)=>h[mailbox+i]===b))return;
  if(value.generation!==undefined&&value.generation!==v.getUint16(mailbox+26,true))return;
  v.setUint16(mailbox+22,value.bits&63,true);h[mailbox+24]=value.focus?1:0;h[cs+patch.focus_visible_mask_cs_offset]=value.points||0;
  writeNativeTouch(h,mailbox,patch,value.touch||[],!!value.blocked);
 }
 function clear(){input({bits:0,focus:0,blocked:true});}
 function hit(){const s=read();if(!s||s.p2Inv||s.p2Miss)throw Error('请等 P2 无敌 / 复活动画结束后再测试');
  const h=emulator.module.HEAPU8,v=new DataView(h.buffer),d=mailbox-patch.mailbox_cs_offset+(patch.data_segment-patch.code_segment)*16;
  if(v.getInt16(d+0x464e,true)/16!==s.p1[0])throw Error('内存布局校验失败');let slot=-1;
  for(let i=0;i<440;i++){const at=d+0x5a22+i*26;if(!h[at]){slot=at;break;}}if(slot<0)throw Error('没有空闲敌弹位置');
  h.fill(0,slot,slot+26);h[slot]=1;h[slot+1]=2;
  for(const [off,n]of [[2,s.p2[0]],[4,s.p2[1]],[6,s.p2[0]],[8,s.p2[1]]])v.setInt16(slot+off,n*16,true);
  h[slot+0x12]=2;h[slot+0x13]=2;
 }
 return {read,input,clear,hit};
}
