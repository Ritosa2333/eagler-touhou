// Authored native score profiles and an independent literal-stream file fixture.
export const SIZE=52232,ST=13152,CD=14220;
export function profileFixture(){
 const p=new Uint8Array(SIZE),v=new DataView(p.buffer);
 for(let i=0;i<8;i++){const base=i*1644;v.setUint32(base,0x00035243,true);v.setUint32(base+8,1644,true);p[base+20]=10+i;}
 v.setUint32(ST,0x00025453,true);v.setUint32(ST+8,1068,true);p[ST+100]=17;
 v.setUint32(CD,0x00014443,true);v.setUint32(CD+8,38012,true);p[CD+200]=23;
 return p;
}
export function expectedCanonical(){
 const p=profileFixture(),v=new DataView(p.buffer);
 for(let i=0;i<8;i++){v.setUint32(i*1644+12,i,true);v.setUint32(i*1644+4,124+2*i,true);}
 v.setUint32(ST+4,65,true);v.setUint32(CD+4,295,true);return p;
}
export function referenceFile({blockChange=null,headerChange=null}={}){
 const p=expectedCanonical(),assembled=new Uint8Array(SIZE);
 assembled.set(p.subarray(0,ST));assembled.set(p.subarray(CD),ST);assembled.set(p.subarray(ST,CD),ST+38012);
 if(blockChange)blockChange(assembled);
 // Literal reference stream is written as a bit string, independent of the
 // production bit writer. A 14-bit end marker matches the native BST reader.
 let bitText='';for(const byte of assembled)bitText+='1'+byte.toString(2).padStart(8,'0');
 bitText+='0'.repeat(14);bitText=bitText.padEnd(Math.ceil(bitText.length/8)*8,'0');
 const clear=Uint8Array.from(bitText.match(/.{8}/g).map(row=>parseInt(row,2)));
 const ciphertext=clear.slice();let mask=0xac,offset=0;
 const excluded=(clear.length%16<4?clear.length%16:0)+(clear.length&1),count=Math.max(0,clear.length-excluded);
 for(let base=0;base<count;base+=16){
  const length=Math.min(16,count-base),order=[];
  for(let i=length-1;i>=0;i-=2)order.push(i);for(let i=length-2;i>=0;i-=2)order.push(i);
  for(const i of order){ciphertext[offset++]=clear[base+i]^mask;mask=(mask+0x35)&255;}
 }
 const file=new Uint8Array(24+ciphertext.length),v=new DataView(file.buffer);
 file.set([84,56,50,49]);v.setUint32(4,file.length,true);v.setUint32(12,0x100,true);
 v.setUint32(16,ciphertext.length,true);v.setUint32(20,SIZE,true);file.set(ciphertext,24);
 if(headerChange)headerChange(file);return file;
}
