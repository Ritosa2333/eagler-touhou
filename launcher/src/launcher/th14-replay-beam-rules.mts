// Explicit compatibility annotation for user-identified TH14 recordings.
// The shared t13r header does not identify TH14 by itself. Never infer a rule.
const USER=0x52455355, T14B=0x42343154;
export function annotateTh14ReplayBeamRule(input: Uint8Array,rule: 0 | 1){
  if(!(input instanceof Uint8Array))throw new TypeError('Expected replay bytes');
  if(rule!==0&&rule!==1)throw new Error('Choose native (0) or bounded (1) explicitly');
  if(input.length<36||input.length>64*1024*1024)throw new Error('Invalid replay size');
  const v=new DataView(input.buffer,input.byteOffset,input.byteLength);
  if(v.getUint32(0,true)!==0x72333174||v.getUint16(4,true)!==2)throw new Error('Not a supported TH14 replay container');
  const packed=v.getUint32(28,true),unpacked=v.getUint32(32,true);
  if(!packed||packed>input.length-36||unpacked<112||unpacked>64*1024*1024)throw new Error('Invalid replay payload sizes');
  // Appending after unrecognized bytes would hide the rule from Replay::open.
  for(let offset=36+packed;offset<input.length;){
    if(input.length-offset<12||v.getUint32(offset,true)!==USER)throw new Error('Unrecognized replay trailer; refusing annotation');
    const length=v.getUint32(offset+4,true);
    if(length<12||length>input.length-offset)throw new Error('Invalid replay USER block');
    if(v.getUint32(offset+8,true)===T14B)throw new Error('Replay already declares beam rules; refusing replacement');
    offset+=length;
  }
  const result=new Uint8Array(input.length+20);result.set(input);
  const tail=new DataView(result.buffer,input.length,20);
  [USER,20,T14B,1,rule].forEach((value,index)=>tail.setUint32(index*4,value,true));
  return result;
}
