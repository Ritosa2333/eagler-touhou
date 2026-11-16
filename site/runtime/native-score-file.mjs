// Bounded native score container codec. File order is CR[8], CD, ST;
// the native memory profile is CR[8], ST, CD. No defaults or storage writes.
const SIZE=52232,ROUTE=1644,ST=13152,STSIZE=1068,CD=14220,CDSIZE=38012,MAX=2*1024*1024;
function need(ok,message){if(!ok)throw new Error('Native score file: '+message);}
const view=bytes=>new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
function bytes(input,size,label){
 need(input instanceof Uint8Array&&input.length===size,label+' profile size/type mismatch');
 return new Uint8Array(input);
}
function crypt(input,encrypt){
 const output=input.slice(),tail=input.length%16,count=Math.max(0,input.length-(tail<4?tail:0)-(input.length&1));
 let cursor=0,mask=0xac;
 for(let base=0;base<count;base+=16){
  const length=Math.min(16,count-base);
  for(let parity=0;parity<2;parity++)for(let p=length-1-parity;p>=0;p-=2){
   if(encrypt)output[cursor]=input[base+p]^mask;else output[base+p]=input[cursor]^mask;
   cursor++;mask=(mask+0x35)&255;
  }
 }
 return output;
}
function checksum(input,offset,size){let total=0;for(let i=offset+8;i<offset+size;i++)total=(total+input[i])>>>0;return total;}
function block(input,offset,tag,size,checked){
 const v=view(input);
 need(v.getUint32(offset,true)===tag&&v.getUint32(offset+8,true)===size,'block identity/size mismatch');
 if(checked)need(v.getUint32(offset+4,true)===checksum(input,offset,size),'block checksum mismatch');
}
function memoryProfile(input,checked=false){
 const output=bytes(input,SIZE,'memory'),v=view(output);
 for(let i=0;i<8;i++){
  const p=i*ROUTE;block(output,p,0x00035243,ROUTE,checked);
  if(checked)need(v.getUint32(p+12,true)===i,'route index/order mismatch');
  else{v.setUint32(p+12,i,true);v.setUint32(p+4,checksum(output,p,ROUTE),true);}
 }
 block(output,ST,0x00025453,STSIZE,checked);block(output,CD,0x00014443,CDSIZE,checked);
 if(!checked){v.setUint32(ST+4,checksum(output,ST,STSIZE),true);v.setUint32(CD+4,checksum(output,CD,CDSIZE),true);}
 return output;
}
function inflate(input){
 need(input.length>0&&input.length<=MAX,'compressed size budget exceeded');
 const output=new Uint8Array(SIZE),ring=new Uint8Array(8192);let bit=0,written=0,head=1;
 const read=width=>{
  need(bit+width<=input.length*8+21,'missing bounded LZSS terminator');
  let value=0;for(let i=0;i<width;i++,bit++)value=(value<<1)|(bit<input.length*8?(input[bit>>>3]>>>(7-(bit&7)))&1:0);return value;
 };
 const put=value=>{need(written<SIZE,'LZSS output exceeds profile size');output[written++]=value;ring[head]=value;head=(head+1)&8191;};
 for(;;){
  if(read(1))put(read(8));
  else{
   const source=read(13);if(source===0)break;
   const count=read(4)+3;need(count<=SIZE-written,'LZSS match exceeds profile size');
   for(let i=0;i<count;i++)put(ring[(source+i)&8191]);
  }
 }
 need(written===SIZE,'LZSS output size differs from profile');
 need(bit>=input.length*8-7,'excess compressed tail bytes');
 while(bit<input.length*8)need(read(1)===0,'nonzero compressed tail padding');
 return output;
}
function literalStream(input){
 const output=new Uint8Array(Math.ceil((input.length*9+14)/8));let bit=0;
 const put=(value,width)=>{for(let i=width-1;i>=0;i--,bit++)if(value>>>i&1)output[bit>>>3]|=1<<(7-(bit&7));};
 for(const value of input){put(1,1);put(value,8);}put(0,14);return output;
}
export function encodeNativeScoreFile(input){
 const profile=memoryProfile(input),assembled=new Uint8Array(SIZE);
 assembled.set(profile.subarray(0,ST));assembled.set(profile.subarray(CD),ST);assembled.set(profile.subarray(ST,CD),ST+CDSIZE);
 const compressed=literalStream(assembled),output=new Uint8Array(24+compressed.length),v=view(output);
 output.set([84,56,50,49]);v.setUint32(4,output.length,true);v.setUint32(12,0x100,true);
 v.setUint32(16,compressed.length,true);v.setUint32(20,SIZE,true);output.set(crypt(compressed,true),24);return output;
}
export function decodeNativeScoreFile(input){
 need(input instanceof Uint8Array&&input.length>=25&&input.length<=MAX+24,'container byte type/size budget mismatch');
 const owned=new Uint8Array(input),v=view(owned);
 need(v.getUint32(0,true)===0x31323854,'container magic mismatch');
 need(v.getUint32(4,true)===owned.length&&v.getUint32(16,true)===owned.length-24,'container size mismatch');
 need(v.getUint32(12,true)===0x100,'container version mismatch');need(v.getUint32(20,true)===SIZE,'inflated profile size mismatch');
 const assembled=inflate(crypt(owned.subarray(24),false)),profile=new Uint8Array(SIZE);
 profile.set(assembled.subarray(0,ST));profile.set(assembled.subarray(ST+CDSIZE),ST);profile.set(assembled.subarray(ST,ST+CDSIZE),CD);
 return memoryProfile(profile,true);
}
