// Pure reader: presentation never clears or advances the logical queue.
export function readCoreSoundEvents(core) {
  if(!core._th12_sound_events)return null;
  const count=core._th12_sound_events(),pointer=core._th12_sound_events_ptr(),stride=core._th12_sound_events_stride();
  if(!Number.isInteger(count)||count<0||count>12||stride!==16||!Number.isInteger(pointer)||pointer<0||pointer+count*stride>core.HEAPU8.byteLength)throw Error('音效记录无效');
  const view=new DataView(core.HEAPU8.buffer),events=[];
  for(let i=0;i<count;i++){const at=pointer+i*stride,id=view.getInt32(at,true),pan=view.getInt32(at+4,true),requests=view.getInt32(at+8,true),stop=view.getInt32(at+12,true);if(id<0||id>=60||requests< -1||requests>128||![0,1].includes(stop))throw Error('音效记录范围无效');events.push({id,pan,requests,stop:!!stop});}
  return events;
}
