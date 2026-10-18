export function decodeTimingRing(buffer,{base,capacity,count,next,stride}){
  // The WASM memory belongs to the runtime iframe realm, so instanceof against
  // the parent window's ArrayBuffer constructor is intentionally invalid.
  if(!buffer||typeof buffer.byteLength!=='number'||capacity!==512||stride!==128||!Number.isInteger(base)||base<0||base%8||count>capacity||count<0||next>=capacity||next<0||base+capacity*stride>buffer.byteLength)throw Error('Invalid presentation timing ring');
  const view=new DataView(buffer),rows=[];for(let n=0;n<count;n++){const index=(next+capacity-count+n)%capacity,offset=base+index*stride,flags=view.getUint32(offset+16,true),camera=[];
    for(let i=0;i<26;i++)camera.push(view.getFloat32(offset+24+i*4,true));
    rows.push({timestampMs:view.getFloat64(offset,true),deltaMs:view.getFloat32(offset+8,true),alpha:view.getFloat32(offset+12,true),flags:{ready:!!(flags&1),fast:!!(flags&2),highRefresh:!!(flags&4),tickDue:!!(flags&8),interpolate:!!(flags&16),presented:!!(flags&32),cameraValid:!!(flags&64),cameraRebased:!!(flags&128)},simulationFrame:view.getUint32(offset+20,true),camera:flags&64?{previous:camera.slice(0,13),current:camera.slice(13)}:null});
  }return {schema:'th08/presentation-timing-ring/1',capacity,count,durationMs:rows.length>1?rows.at(-1).timestampMs-rows[0].timestampMs:0,rows};
}
