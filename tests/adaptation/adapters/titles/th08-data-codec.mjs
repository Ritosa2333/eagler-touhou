/** TH08's 13-bit dictionary stream; used by the independent Replay and score readers. */
export function decodeTh08Lzss(input,payloadSize){
  if(!Number.isSafeInteger(payloadSize)||payloadSize<0||payloadSize>8*1024*1024)throw Error('Invalid TH08 payload bound');
  const output=Buffer.alloc(payloadSize),dictionary=Buffer.alloc(8192);
  let cursor=0,mask=0x80,current=0,head=1,written=0,operations=0;
  const bit=()=>{
    if(mask===0x80)current=cursor<input.length?input[cursor++]:0;
    const value=(current&mask)?1:0;mask>>=1;if(!mask)mask=0x80;return value;
  };
  const bits=count=>{let value=0;while(count--)value=(value<<1)|bit();return value;};
  const put=value=>{if(written>=payloadSize)throw Error('TH08 LZSS exceeds declared payload');
    output[written++]=value;dictionary[head]=value;head=(head+1)&8191;};
  let terminated=false;
  while(operations++<payloadSize+input.length+1024){
    if(bit())put(bits(8));else{
      const offset=bits(13);if(offset===0){terminated=true;break;}
      const count=bits(4)+3;
      for(let index=0;index<count;index++)put(dictionary[(offset+index)&8191]);
    }
  }
  if(!terminated||written!==payloadSize)throw Error('TH08 LZSS did not end at its declared payload size');
  return output;
}

export function decryptTh08ScoreEnvelope(input){
  const raw=Buffer.from(input),output=Buffer.from(raw);
  const untouched=(raw.length%256<64?raw.length%256:0)+(raw.length&1);
  let remaining=raw.length-untouched,cursor=0,limit=0xc00,key=0x59;
  while(remaining>0&&limit>0){
    const block=Math.min(256,remaining);let linear=0;
    for(let parity=1;parity<=2;parity++)for(let pos=block-parity;pos>=0;pos-=2){
      output[cursor+pos]=raw[cursor+linear++]^key;key=(key+0x79)&255;
    }
    cursor+=block;remaining-=block;limit-=block;
  }
  key=0;
  for(let index=2;index<output.length;index++){
    const sum=(key+output[index-1])&255;key=((sum<<3)|(sum>>>5))&255;output[index]^=key;
  }
  return output;
}
