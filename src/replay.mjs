// TH12 1.00b retail Replay format. Native decode: 0x43c424, 0x4639d0.
// Independent from the simulation. Bounds are validated before allocations.
export function decryptBytes(input,key,step,block,limit=input.length) {
  const out=new Uint8Array(input),untouched=(input.length%block<block/4?input.length%block:0)+(input.length&1);
  let cursor=0,remaining=input.length-untouched;
  while(remaining>0&&limit>0){const n=Math.min(block,remaining);let k=0;
    for(let parity=1;parity<=2;parity++)for(let pos=n-parity;pos>=0;pos-=2){out[cursor+pos]=input[cursor+k++]^key;key=(key+step)&255;}
    cursor+=n;remaining-=n;limit-=n;
  }return out;
}
export function inflateReplay(input,size) {
  if(size<112||size>64*1024*1024)throw Error('录像解压大小无效');
  const out=new Uint8Array(size),dict=new Uint8Array(8192);let cursor=0,mask=128,byte=0,written=0,head=1,zeroPadding=0;
  const bit=()=>{if(mask===128){if(cursor>=input.length){if(++zeroPadding>2)throw Error('录像压缩流不完整');byte=0;}else byte=input[cursor++];}const b=(byte&mask)?1:0;mask>>=1;if(!mask)mask=128;return b;};
  const bits=n=>{let x=0;while(n--)x=x*2+bit();return x;};
  const put=x=>{if(written>=size)throw Error('录像解压越界');out[written++]=x;dict[head]=x;head=(head+1)&8191;};
  for(;;){if(bit())put(bits(8));else{const offset=bits(13);if(!offset)break;const n=bits(4)+3;for(let k=0;k<n;k++)put(dict[(offset+k)&8191]);}}
  if(written!==size)throw Error(`录像解压不完整 ${written}/${size}`);return out;
}
export function parseReplay(source) {
  const b=source instanceof Uint8Array?source:new Uint8Array(source),v=new DataView(b.buffer,b.byteOffset,b.byteLength);
  if(b.length<36||v.getUint32(0,true)!==0x72323174||v.getUint16(4,true)!==4)throw Error('需要原版 TH12 1.00b .rpy 录像');
  const packed=v.getUint32(28,true),size=v.getUint32(32,true);
  if(!packed||packed>b.length-36||packed>64*1024*1024)throw Error('录像压缩长度无效');
  const data=inflateReplay(decryptBytes(decryptBytes(b.subarray(36,36+packed),0x5e,0xe1,0x800),0x7d,0x3a,0x40),size);
  const d=new DataView(data.buffer),n=d.getUint32(88,true),character=d.getUint32(92,true),shot=d.getUint32(96,true),difficulty=d.getUint32(100,true);
  if(n<1||n>7||character>2||shot>1||difficulty>4)throw Error('录像机体或难度无效');
  const stages=[];let offset=112;
  for(let i=0;i<n;i++){
    if(offset>size-160)throw Error('录像关卡头不完整');
    const number=d.getUint16(offset,true),seed=d.getUint16(offset+2,true),frames=d.getUint32(offset+4,true),payload=d.getUint32(offset+8,true);
    if(number<1||number>7||stages.some(s=>s.number===number)||frames<1||frames>1e7||payload>size-offset-160||payload<frames*6)throw Error('录像关卡流无效');
    const fpsCount=frames>1?1+Math.floor((frames-2)/30):0;
    if(payload-frames*6<fpsCount)throw Error('录像帧率数据不完整');
    stages.push({number,seed,frames,payload,offset,inputOffset:offset+160,
      initial:{score:d.getInt32(offset+12,true),power:d.getInt32(offset+16,true),pointValue:d.getInt32(offset+20,true),lives:d.getUint16(offset+24,true),lifeFragments:d.getUint16(offset+26,true),bombs:d.getUint16(offset+28,true),bombFragments:d.getUint16(offset+30,true),ufoColors:[32,36,40].map(at=>d.getInt32(offset+at,true)),rank:d.getInt32(offset+44,true),x:d.getInt32(offset+48,true),y:d.getInt32(offset+52,true),nativeGlobals:[56,60,64,68].map(at=>d.getInt32(offset+at,true))}});
    offset+=160+payload;
  }
  if(offset!==size)throw Error('录像包含未识别的解压尾部');
  return {name:new TextDecoder('shift-jis').decode(data.subarray(0,8)).trim(),character,shot,difficulty,stages,data,
    input(stage,frame){const s=stages.find(s=>s.number===stage);if(!s||frame<0||frame>=s.frames)return null;const p=s.inputOffset+frame*6;return {held:d.getUint16(p,true),pressed:d.getUint16(p+2,true),released:d.getUint16(p+4,true)};}};
}
