import {Fat12} from './disk.js';
import {unlockDiskScores} from './scores.js';

// Upgrade original, saved and imported disks without replacing leaderboard data.
export function prepareSoloDisk(data){
  const fat=new Fat12(data),entry=fat.find('YUMEZIKU/YUME.CFG');
  const cfg=fat.read(entry);
  if(cfg.length!==8)throw Error('梦时空配置长度不匹配');
  unlockDiskScores(fat);
  const cluster=fat.view.getUint16(entry+26,true);
  fat.data[fat.offset(cluster)+2]=3; // RANK_LUNATIC; other configuration stays intact.
  return data;
}
