import { parseMeasuredNetplayTiming } from '../../../common/contracts/netplay-timing.mjs';
import { multiplayerConfigForProduct, multiplayerInputTimingPolicy } from '../../../common/contracts/product-catalog.mjs';
import { COOPERATIVE_SPECTATOR_MARKERS as cooperativeProtocolMarkers } from './spectator-stages.mjs';
// Wire-format identities, not product feature switches.
// Only the room's declared game protocol may enter its spectator backlog.
export function isSpectatorFrameForRoom(roomId, payload, playerCount) {
  const product = roomId.split('-')[0], policy = multiplayerConfigForProduct(product);
  const protocol = product === 'th075mp' ? 0x50 : cooperativeProtocolMarkers[product];
  const timingPolicy = multiplayerInputTimingPolicy(policy);
  if(product==='th075mp'&&playerCount===2&&payload?.length===40&&
     payload[0]===69&&payload[1]===0x50&&payload[2]===84&&payload[3]===77&&payload[4]===1){
    const v=new DataView(payload.buffer,payload.byteOffset,payload.byteLength);
    const mode=payload[5],delay=payload[6],reserve=payload[7],rtt=v.getUint32(24,true);
    const full=v.getUint32(8,true),lost=v.getUint32(28,true),automatic=v.getUint32(32,true),samples=v.getUint32(36,true);
    // th075 native DelaySession uses 0=rollback, 1=pure delay and up to eight
    // prediction frames; preserve that wire contract independently of Adonis.
    return mode<=1&&delay<=9&&(mode===1?reserve===0:delay===0&&reserve>=1&&reserve<=8)&&
      rtt>=1&&rtt<=1000000&&lost<=72&&automatic<=1&&samples>=96&&samples<=120&&
      full===Math.max(1,Math.ceil(Math.floor(rtt/2)*60/1000000))&&(!automatic||mode===1&&delay===full);
  }
  if(protocol!==undefined&&timingPolicy.measuredStartup&&policy.playerCounts.includes(playerCount)&&payload?.length===40&&
     payload[0]===69&&payload[1]===protocol&&payload[2]===84&&payload[3]===77&&payload[4]===1){
    if(product==='th15mp'&&payload[5]===0){
      const v=new DataView(payload.buffer,payload.byteOffset,payload.byteLength);
      return timingPolicy.rollback&&payload[6]===0&&payload[7]===0&&v.getBigUint64(12,true)!==0n&&v.getUint32(20,true)!==0&&[8,24,28,32,36].every(at=>v.getUint32(at,true)===0);
    }
    if (!timingPolicy.rollback && (payload[5] !== 1 || payload[7] !== 0)) return false;
    const v=new DataView(payload.buffer,payload.byteOffset,payload.byteLength),automatic=v.getUint32(32,true);
    return automatic<=1&&!!parseMeasuredNetplayTiming({phase:'ready',automatic:!!automatic,adonisMode:payload[5],
      inputDelay:payload[6],predictionReserve:payload[7],fullDelay:v.getUint32(8,true),rttP95Us:v.getUint32(24,true),
      lost:v.getUint32(28,true),samples:v.getUint32(36,true),route:'spectator'});
  }
  if(roomId.startsWith('th09mp-') && playerCount===2 && payload?.length===40 &&
     payload[0]===84 && payload[1]===57 && payload[2]===84 && payload[3]===77 && payload[4]===1) {
    const v=new DataView(payload.buffer,payload.byteOffset,payload.byteLength);
    const mode=payload[5],d=payload[6],reserve=payload[7],full=v.getUint32(8,true);
    const rtt=v.getUint32(24,true),lost=v.getUint32(28,true),automatic=v.getUint32(32,true),samples=v.getUint32(36,true);
    return automatic<=1 && !!parseMeasuredNetplayTiming({phase:'ready',automatic:!!automatic,
      adonisMode:mode,inputDelay:d,predictionReserve:reserve,fullDelay:full,
      rttP95Us:rtt,lost,samples,route:'spectator'});
  }
  if (!payload || payload[6] !== playerCount) return false;
  if (roomId.startsWith('th09mp-')) {
    if(playerCount!==2||payload[0]!==0x54||payload[1]!==0x39||payload[2]!==0x53||payload[3]!==0x50||payload[5]!==3||payload[7]!==0)return false;
    if(payload[4]===1)return payload.length===46;
    if(payload[4]!==2||payload.length!==126)return false;
    const data=new DataView(payload.buffer,payload.byteOffset,payload.byteLength);
    for(const at of [46,86]){
      const charge=data.getFloat32(at,true),released=data.getFloat32(at+28,true),events=data.getUint32(at+32,true);
      if(!Number.isFinite(charge)||charge<0||charge>400||!Number.isFinite(released)||
         !(released===-1||released>=0&&released<=400)||(events&~7)||data.getUint32(at+36,true)>1||!!(events&2)!==(released>=0))return false;
      for(const timer of [at+4,at+16]){
        const previous=data.getInt32(timer,true),time=data.getFloat32(timer+4,true),current=data.getInt32(timer+8,true);
        if(previous< -999999||previous>1e8||!Number.isFinite(time)||time< -1||time>1e8||current< -1||current>1e8)return false;
      }
    }
    return true;
  }
  return protocol !== undefined && policy?.playerCounts.includes(playerCount) &&
    payload.length === 24 + playerCount * 12 &&
    payload[0] === 0x45 && payload[1] === protocol &&
    payload[2] === 0x4e && payload[3] === 0x50 && payload[4] === 4 &&
    payload[5] === 3 && payload[7] === 0;
}
