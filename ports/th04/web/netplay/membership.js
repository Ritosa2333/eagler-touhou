// Pure construction of a common boundary; no wall clock enters simulation.
import {MAX_INPUT} from '../touch-input.js';
export function mergeDeparture(reports,slots,players){
  if(!reports.length)throw Error('缺少在线玩家状态');
  for(const r of reports){
    if(!Number.isSafeInteger(r.frame)||!Number.isSafeInteger(r.prefix)||r.prefix<0||r.prefix>r.frame||
       !Array.isArray(r.inputs)||r.inputs.length!==players)throw Error('无效离线状态报告');
  }
  const start=Math.min(...reports.map(r=>r.prefix)),frame=Math.max(...reports.map(r=>r.frame));
  if(frame-start>32)throw Error('离线同步超出可修复窗口');
  const lanes=Array.from({length:players},()=>new Map());
  for(const r of reports)for(let slot=0;slot<players;slot++){
    if(!Array.isArray(r.inputs[slot])||r.inputs[slot].length>200)throw Error('离线报告输入过多');
    for(const entry of r.inputs[slot]){
      if(!Array.isArray(entry)||entry.length!==2)throw Error('无效离线输入');
      const [at,bits]=entry;
      if(!Number.isSafeInteger(at)||at<0||at>r.frame+120||!Number.isSafeInteger(bits)||bits<0||bits>MAX_INPUT)throw Error('离线输入越界');
      if(at<start)continue;
      const lane=lanes[slot];
      if(lane.has(at)&&lane.get(at)!==bits)throw Error('在线玩家保存的输入互相冲突');
      lane.set(at,bits);
    }
  }
  for(const slot of slots){
    for(let at=start;at<frame;at++)if(!lanes[slot].has(at))lanes[slot].set(at,0);
    for(const at of lanes[slot].keys())if(at>=frame)lanes[slot].delete(at);
  }
  return {frame,slots,inputs:lanes.map(lane=>[...lane].sort((a,b)=>a[0]-b[0]))};
}
