// th09 exports replays with touch or charge extensions as .rpyx.
const signature=[84,72,77,79,84,73,79,78],limit=600000,velocityLimit=10000000;
const dictionaryLimit=80+15*limit+40*(1+5*127);
export function validateMotionReplay(bytes,game){
 if(!(bytes instanceof Uint8Array))throw Error('录像数据无效');
 const end=bytes.length-24;if(end<0||signature.some((v,i)=>bytes[end+i]!==v))return false;
 const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),length=view.getUint32(end+16,true),start=end-length;
 const version=view.getUint32(end+12,true);
 const fail=()=>{throw Error('触摸录像数据损坏或不属于这款游戏');};
 if(view.getUint32(end+8,true)!==game||(version!==2&&(game!==9||version<3||version>5))||length<6||length>80+limit*12||start<0)fail();
 let hash=2166136261;for(let i=start;i<end;i++)hash=Math.imul(hash^bytes[i],16777619)>>>0;
 if(hash!==view.getUint32(end+20,true))fail();
 if(version>=4){
  const plain=view.getUint32(start,true);
  if(plain<80)fail();
  if(version===4&&(plain>80+limit*12||(plain-80)%12||bytes[start+4]>1))fail();
  if(version===5&&(plain>dictionaryLimit||bytes[start+4]!==0))fail();
  return true; // The native decoder verifies the compressed data and every entry.
 }
 if(length<80)fail();
 let offset=start+80,total=0;
 const float=new DataView(new ArrayBuffer(4));
 const takeWord=width=>{
  let word=0;for(let i=0;i<width;i++)word|=bytes[offset++]<<(8*i);
  return word>>>0;
 };
 const asFloat=word=>{float.setUint32(0,word,true);return float.getFloat32(0,true);};
 for(let stage=0;stage<20;stage++){
  const count=view.getUint32(start+stage*4,true);
  if(count>limit-total||count>(end-offset)/(version===2?12:1))fail();
  total+=count;
  let previous=-1,xBits=0,yBits=0;
  for(let i=0;i<count;i++){
   let tick,x,y;
   if(version===2){
    tick=view.getUint32(offset,true);x=view.getFloat32(offset+4,true);y=view.getFloat32(offset+8,true);
    offset+=12;
   }else{
    if(offset===end)fail();
    const control=bytes[offset++],a=control&7,b=(control>>>3)&7,gap=control>>>6,t=gap===3?4:gap;
    if(a>4||b>4||end-offset<t+a+b)fail();
    tick=previous+1+takeWord(t);xBits^=takeWord(a);yBits^=takeWord(b);
    x=asFloat(xBits);y=asFloat(yBits);
   }
   if(tick<=previous||tick>0xffffffff||!Number.isFinite(x)||!Number.isFinite(y)||Math.abs(x)>velocityLimit||Math.abs(y)>velocityLimit)fail();
   previous=tick;
  }
 }
 if(offset!==end)fail();return true;
}
export function validateChargeReplay(bytes,game){
 if(!(bytes instanceof Uint8Array))throw Error('录像数据无效');
 return validateChargeTrailer(bytes,game,validateMotionReplay(bytes,game));
}
function validateChargeTrailer(bytes,game,motion){
 const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
 const size=motion?bytes.length-24-view.getUint32(bytes.length-8,true):bytes.length;
 const end=size-24,magic=[84,57,67,72,65,82,71,69];
 if(end<0||magic.some((v,i)=>bytes[end+i]!==v))return false;
 const length=view.getUint32(end+16,true),start=end-length;
 const fail=()=>{throw Error('本地蓄力录像数据损坏或不属于这款游戏');};
 const version=view.getUint32(end+12,true);
 if(game!==9||view.getUint32(end+8,true)!==9||version<1||version>3||length<5||start<0||length>32*1024*1024)fail();
 const plain=view.getUint32(start,true);if(plain<40||plain>40+600000*88||(plain-40)%88)fail();
 if(version===3){
  if(length<=8)fail();
  const decoded=view.getUint32(start+4,true),words=(plain-40)/4;
  if(decoded<40+words||decoded>40+5*words)fail();
 }
 let hash=2166136261;for(let i=start;i<end;i++)hash=Math.imul(hash^bytes[i],16777619)>>>0;
 if(hash!==view.getUint32(end+20,true))fail();return true;
}
const extendedReplay=(bytes,game)=>{const motion=validateMotionReplay(bytes,game),charge=validateChargeTrailer(bytes,game,motion);return motion||charge;};
export function exportReplayName(name,bytes,game){return name.endsWith('.rpy')&&extendedReplay(bytes,game)?name+'x':name;}
export function importReplayName(name,bytes,game){
 if(!/\.rpyx?$/.test(name))return name;
 const extended=extendedReplay(bytes,game);if(name.endsWith('.rpyx')&&!extended)throw Error('扩展录像缺少播放记录');
 return name.endsWith('.rpyx')?name.slice(0,-1):name;
}
