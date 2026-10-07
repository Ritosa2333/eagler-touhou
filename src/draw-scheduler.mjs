// TH12 1.00b draw-chain priorities installed by45dfa0. An ANM layer is
// only one kind of draw callback; embedded manager VMs bypass that chain.
const layerPriorities=[4,6,8,9,11,13,16,17,18,19,20,23,25,26,28,30,34,36,38,41,42,50,51,52,63,64,-1,-1,14,39,66,65];
export const animationLayerPriority=layer=>layerPriorities[layer]??-1;
export function readCoreDrawSchedule(core){
  if(!core._th12_anm_schedule_count||!core._th12_anm_schedule_ptr)return new Map();
  const count=core._th12_anm_schedule_count(),ptr=core._th12_anm_schedule_ptr(),stride=core._th12_anm_schedule_stride?.(),heap=core.HEAPU8;
  if(!Number.isSafeInteger(count)||count<0||count>65536||!Number.isSafeInteger(ptr)||ptr<0||stride!==16||!heap||ptr+count*stride>heap.byteLength)throw Error('动画绘制顺序记录无效');
  const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength),schedule=new Map();
  for(let i=0;i<count;i++){
    const at=ptr+i*stride,id=view.getUint32(at,true),priority=view.getInt32(at+4,true),order=view.getUint32(at+8,true),clip=view.getUint32(at+12,true);
    if(priority< -1||priority>100||clip>2||schedule.has(id))throw Error('动画绘制顺序字段无效');
    schedule.set(id,{drawPriority:priority,order,clip,clipped:!!clip});
  }
  return schedule;
}
export function orderPresentation(poses,schedule=new Map()){
  return poses.map((pose,index)=>{
    const metadata=pose.logical||pose.mesh&&pose.renderPass===0?schedule.get(pose.id):null;
    const drawPriority=metadata?.drawPriority??(pose.mesh&&pose.renderPass===0?animationLayerPriority(pose.a?.layer):pose.drawPriority??animationLayerPriority(pose.a?.layer));
    const clip=metadata?.clip??pose.clip??(metadata?.clipped!==undefined?metadata.clipped?2:0:pose.clipped!==undefined?pose.clipped?2:0:drawPriority<43?2:0);
    return {...pose,...metadata,drawPriority,order:metadata?.order??pose.order??index,index,clip,clipped:clip!==0};
  }).filter(p=>p.drawPriority>=0).sort((a,b)=>a.drawPriority-b.drawPriority||a.order-b.order||a.index-b.index);
}
