// The original replay payload stays intact; continuous touch adds a sparse
// movement track. Export it as .rpyx so it cannot be mistaken for an EXE replay.
const signature=[84,72,77,79,84,73,79,78],limit=600000,velocityLimit=10000000;
export function validateMotionReplay(bytes,game){
 if(!(bytes instanceof Uint8Array))throw Error('录像数据无效');
 const end=bytes.length-24;if(end<0||signature.some((v,i)=>bytes[end+i]!==v))return false;
 const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),length=view.getUint32(end+16,true),start=end-length;
 const fail=()=>{throw Error('触摸录像数据损坏或不属于这款游戏');};
 if(view.getUint32(end+8,true)!==game||view.getUint32(end+12,true)!==2||length<80||length>80+limit*12||start<0)fail();
 let hash=2166136261;for(let i=start;i<end;i++)hash=Math.imul(hash^bytes[i],16777619)>>>0;
 if(hash!==view.getUint32(end+20,true))fail();
 let offset=start+80,total=0;
 for(let stage=0;stage<20;stage++){
  const count=view.getUint32(start+stage*4,true);if(count>limit-total||count>(end-offset)/12)fail();total+=count;
  let previous=-1;for(let i=0;i<count;i++,offset+=12){const tick=view.getUint32(offset,true),x=view.getFloat32(offset+4,true),y=view.getFloat32(offset+8,true);if(tick<=previous||!Number.isFinite(x)||!Number.isFinite(y)||Math.abs(x)>velocityLimit||Math.abs(y)>velocityLimit)fail();previous=tick;}
 }
 if(offset!==end)fail();return true;
}
export function exportReplayName(name,bytes,game){return name.endsWith('.rpy')&&validateMotionReplay(bytes,game)?name+'x':name;}
export function importReplayName(name,bytes,game){
 if(!/\.rpyx?$/.test(name))return name;
 const extended=validateMotionReplay(bytes,game);if(name.endsWith('.rpyx')&&!extended)throw Error('触摸录像缺少移动记录');
 return name.endsWith('.rpyx')?name.slice(0,-1):name;
}
