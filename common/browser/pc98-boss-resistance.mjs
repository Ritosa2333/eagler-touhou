// The original native collision hooks retain a per-attack remainder. A fixed
// denominator keeps that remainder valid when a player becomes a ghost or is rescued.
const spec={
  '04':{scale:0xad7c, expected:[0xb9,0x0a,0x00,0xf7,0xe1,0x2e,0x03,0x04,0x83,0xd2,0x00,0xb9,0x0f,0x00,0x2e,0x80,0x3e,0xa2,0x95,0x03,0x75,0x03,0xb9,0x16,0x00,0xf7,0xf1], numerator:1,divisor:11},
};
export function installBossResistance(exe,game,patch){
  const s=spec[game], header=exe[8]*16+exe[9]*4096, base=header+patch.code_segment*16+s.scale;
  if(!s.expected.every((value,index)=>exe[base+index]===value))throw Error('Boss 伤害缩放原程序不匹配');
  exe[base+s.numerator]=5;
  exe.set([0xb9,5,0,...Array(11).fill(0x90)],base+s.divisor);
}
export function updateBossResistance(heap,game,mailbox,patch,players,mark=()=>{}){
  const code=mailbox-patch.mailbox_cs_offset;
  let alive=0;
  for(let seat=0;seat<players;seat++){
    const out=heap[mailbox+patch.resource_schema.offset+seat*patch.resource_schema.stride+6];
    const offline=heap[code+patch.offline_mask_cs_offset];
    if(!out&&!(offline&(1<<seat)))alive++;
  }
  const multiplier=alive>=3?2:alive===2?3:5,at=code+spec[game].scale+spec[game].numerator;
  if(heap[at]!==multiplier){mark(at,1);heap[at]=multiplier;}
}
