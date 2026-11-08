// Install a game-native signed-number routine in the private cooperative MAIN.
// The twelve leading bytes are three signed 32-bit counters in the guest CS.
const code02='000000000000000000000000e8b5e1666006b800a08ec031ed69fde00181c7b606b90b00b8200551e82fe159e2f983ef16e80c00452e3b2e288572dd076661c3665066536651665289ebc1e3022e668b87a88a6685c0790d66f7d86650b82de1e8f7e0665831c966bb0a0000006631d266f7f380c23052416685c075f058b4e151e8d6e059e2f6665a6659665b6658c3';
const code04='000000000000000000000000e8f6d7666006b800a08ec031ed69fd400181c7ea04b90b00b8200551e87ad759e2f983ef16e80f00452e0fb606a29539c572da076661c3665066536651665289ebc1e3022e668b8720b96685c0790d66f7d86650b82de1e83fd7665831c966bb0a0000006631d266f7f380c23052416685c075f058b4e151e81ed759e2f6665a6659665b6658c3';
const fromHex=value=>Uint8Array.from(value.match(/../g),b=>parseInt(b,16));
function check(bytes,at,expected){return at>=0&&at+expected.length<=bytes.length&&expected.every((value,i)=>bytes[at+i]===value);}
function insertImage(source,at,gap,code){
  const v=new DataView(source.buffer,source.byteOffset,source.byteLength),hs=v.getUint16(8,true)*16,table=v.getUint16(24,true),count=v.getUint16(6,true);
  if(at%16||gap%16||code.length>gap||at>source.length-hs)throw Error('挑战模式程序扩展无效');
  const image=source.slice(hs),iv=new DataView(image.buffer),rel=[];
  for(let i=0;i<count;i++){
    const pos=table+i*4,loc=v.getUint16(pos,true)+v.getUint16(pos+2,true)*16;
    if(loc+2>image.length)throw Error('挑战模式程序重定位无效');
    const segment=iv.getUint16(loc,true);if(segment*16>=at)iv.setUint16(loc,segment+gap/16,true);
    rel.push(loc>=at?loc+gap:loc);
  }
  const expanded=new Uint8Array(image.length+gap);expanded.set(image.subarray(0,at));expanded.set(code,at);expanded.set(image.subarray(at),at+gap);
  const header=source.slice(0,hs),hv=new DataView(header.buffer);
  for(const field of [14,22]){const segment=hv.getUint16(field,true);if(segment*16>=at)hv.setUint16(field,segment+gap/16,true);}
  rel.forEach((loc,i)=>{hv.setUint16(table+i*4,loc%16,true);hv.setUint16(table+i*4+2,Math.floor(loc/16),true);});
  const result=new Uint8Array(hs+expanded.length);hv.setUint16(2,result.length%512,true);hv.setUint16(4,Math.ceil(result.length/512),true);result.set(header);result.set(expanded,hs);return result;
}
export function installChallengeHud(source,game,patch){
  if(!(source instanceof Uint8Array)||source.length<32)throw Error('挑战模式程序无效');
  const hs=new DataView(source.buffer,source.byteOffset,source.byteLength).getUint16(8,true)*16;
  let result,cs,offset,call;
  if(game==='02'){
    cs=patch.codeSegment;offset=0x8aa8;call=0x6b33;const base=hs+cs*16,code=fromHex(code02);
    if(!check(source,base+call,fromHex('e83601')))throw Error('TH02 挑战模式 HUD 调用不匹配');if(!check(source,base+0x6b00,fromHex('83c7066a46e83d0083c70231c02e80be2a85007503a02d0be85e06')))throw Error('TH02 挑战模式 HUD Bomb 绘制不匹配');if(!check(source,base+offset,new Uint8Array(code.length))){const n=source.subarray(base+offset,base+offset+code.length).findIndex(value=>value!==0);throw Error('TH02 挑战模式 HUD 空间已占用：'+n+'/'+source[base+offset+n]);}
    result=source.slice();result.set(code,base+offset);result.fill(0x90,base+0x6b00,base+0x6b1b);
    // In challenge mode pickups belong to the collector, not the whole team.
    // Raise native per-step caps so several simultaneous pickups still count.
    for(const [at,expected,replacement] of [
      [0x7f0b,'2e803e147d02','e98400909090'],
      [0x2595,'3d0500','3d6400'],[0x289e,'3d0500','3d6400'],
      [0x2940,'3d0500','3d6400'],[0x2cb8,'3d0500','3d6400'],
      [0x2d31,'3d0500','3d6400'],
    ]){if(!check(result,base+at,fromHex(expected)))throw Error('TH02 挑战模式奖励补丁不匹配');result.set(fromHex(replacement),base+at);}
  }else if(game==='04'){
    cs=patch.code_segment;offset=0xb920;call=0x8fe3;const base=hs+cs*16,insert=patch.insert_image_offset+patch.insert_size,guard=fromHex(patch.near_code_guard.original),code=fromHex(code04);
    if(insert!==cs*16+offset||!check(source,base+call,fromHex('e83f01'))||!check(source,hs+insert,guard))throw Error('TH04 挑战模式 HUD 原程序不匹配');
    result=insertImage(source,insert,160,code);
    if(!check(result,base+0x8ce0,fromHex('e81200')))throw Error('TH04 挑战模式奖励补丁不匹配');
    result.fill(0x90,base+0x8ce0,base+0x8ce3);
  }else throw Error('挑战模式作品无效');
  const header=new DataView(result.buffer).getUint16(8,true)*16,base=header+cs*16,view=new DataView(result.buffer);
  result[base+call]=0xe8;view.setUint16(base+call+1,(offset+12-call-3)&65535,true);
  return {bytes:result,countsOffset:offset,addedParagraphs:game==='04'?10:0};
}
