export function inspectReplay(bytes){
 if(!(bytes instanceof Uint8Array)||bytes.length<340||bytes.length>32*1024*1024||String.fromCharCode(...bytes.subarray(0,4))!=='RPY ')throw Error('Invalid uth05win Replay');
 const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),i32=at=>view.getInt32(at,true),u32=at=>view.getUint32(at,true),tag=at=>String.fromCharCode(...bytes.subarray(at,at+4));
 const version=i32(4),difficulty=i32(44),character=i32(48),practice=bytes[52]!==0;
 if(![1,2].includes(version)||difficulty<0||difficulty>4||character<0||character>3||bytes[52]>1||i32(36)<0||i32(36)>9||i32(40)<0||i32(40)>9)throw Error('Invalid Replay header');
 const stages=[];let at=340;
 for(let stage=0;stage<7;stage++){const flag=bytes[53+stage];if(flag>2)throw Error('Invalid Replay stage flag');if(!flag)continue;if(at+4>bytes.length)throw Error('Truncated Replay');const count=i32(at);at+=4;if(count<0||count>10000000||at+count*2>bytes.length)throw Error('Truncated Replay inputs');stages.push({id:stage+1,frames:count,score:i32(284+stage*4)*10,cleared:flag===2});at+=count*2;}
 if(!stages.length)throw Error('Replay has no recorded stage');
 if(version===2){if(at+12>bytes.length||tag(at)!=='TPC5')throw Error('Missing practice metadata');const v=u32(at+4),size=u32(at+8);if(![1,2,3].includes(v)||size!==[0,56,72,76][v]||at+12+size>bytes.length||!practice)throw Error('Invalid practice metadata');const mode=i32(at+12),stage=i32(at+16);if(mode!==1||stage<0||stage>6||stages.length!==1||stages[0].id!==stage+1||(stage===6)!==(difficulty===4))throw Error('Invalid practice stage');at+=12+size;}
 let speed=false,touch=false,originalPlayerShots=false;
 while(at<bytes.length){if(at+8>bytes.length)throw Error('Invalid Replay trailer');const kind=tag(at),revision=u32(at+4);at+=8;
  if(kind==='TSH5'){if(originalPlayerShots||speed||touch||![1,2,3].includes(revision))throw Error('Invalid shot rules trailer');originalPlayerShots=true;
   if(revision===3){if(at+7>bytes.length||bytes.subarray(at,at+7).some(progress=>progress>=32))throw Error('Invalid Rank progress');at+=7;}
  }
  else if(revision!==1)throw Error('Invalid Replay trailer');
  else if(kind==='TSP5'){if(speed||touch||at+4>bytes.length||u32(at)>1)throw Error('Invalid speed trailer');speed=true;at+=4;}
  else if(kind==='TCH5'){if(touch)throw Error('Duplicate touch trailer');touch=true;for(let stage=1;stage<=7;stage++){if(at+4>bytes.length)throw Error('Truncated touch trailer');const count=u32(at),frames=stages.find(s=>s.id===stage)?.frames??0;at+=4;if(count>frames||at+count*12>bytes.length)throw Error('Invalid touch count');let previous=-1;for(let n=0;n<count;n++){const frame=u32(at),x=view.getFloat32(at+4,true),y=view.getFloat32(at+8,true);if(frame<=previous||frame>=frames||!Number.isFinite(x)||!Number.isFinite(y)||Math.abs(x)>8192||Math.abs(y)>8192)throw Error('Invalid touch motion');previous=frame;at+=12;}}}
  else throw Error('Unknown Replay trailer');
 }
 return {format:'uth05win',version,name:new TextDecoder('shift_jis').decode(bytes.subarray(24,32)).replace(/\0.*$/,''),difficulty,character,practice,stages,touch};
}
