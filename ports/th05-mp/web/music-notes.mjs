export function decodeMusicNotes(bytes){
 const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);if(bytes.length<8)throw Error('Invalid music note metadata');
 const size=view.getUint32(0,true),base=size+4;if(base>bytes.length)throw Error('Invalid music note index');
 const index=JSON.parse(new TextDecoder().decode(bytes.subarray(4,base)));
 for(const [name,[offset,count]] of Object.entries(index))if(!/^[a-z0-9_]+$/.test(name)||!Number.isSafeInteger(offset)||offset<0||!Number.isSafeInteger(count)||count<1||base+offset+count*14>bytes.length)throw Error('Invalid music note range');
 return (name,seconds)=>{
  const entry=index[name.toLowerCase().replace(/\.(?:m2|m)$/,'')];if(!entry||!Number.isFinite(seconds)||seconds<0)return null;
  const [offset,count]=entry,start=base+offset,time=seconds*1000;let left=0,right=count;
  while(left<right){const middle=(left+right)>>>1;if(view.getUint32(start+middle*14,true)<=time)left=middle+1;else right=middle;}
  if(left===0)return null;const at=start+(left-1)*14;return {measure:view.getUint16(at+4,true),notes:Array.from(bytes.subarray(at+6,at+14))};
 };
}
