// Keep signed per-player counters independent of the game's stock fields.
export function createChallengeCounters(players, initial){
  if(!Number.isInteger(players)||players<2||players>3)throw Error('挑战模式人数无效');
  let counts=initial===undefined?Array(players).fill(0):validate(initial);
  function validate(value){
    if(!Array.isArray(value)||value.length!==players||value.some(n=>!Number.isSafeInteger(n)))throw Error('挑战模式计数无效');
    return value.slice();
  }
  function add(slot,delta){
    if(!Number.isInteger(slot)||slot<0||slot>=players||!Number.isSafeInteger(delta))throw Error('挑战模式事件无效');
    const next=counts[slot]+delta;if(!Number.isSafeInteger(next))throw Error('挑战模式计数溢出');counts[slot]=next;
  }
  function applyFrame(events){
    if(!Array.isArray(events))throw Error('挑战模式事件无效');
    const next=counts.slice();
    for(const event of events){
      if(!event||!['miss','bomb','life','extend'].includes(event.kind)||!Array.isArray(event.slots)||
        !event.slots.length||new Set(event.slots).size!==event.slots.length||event.slots.some(slot=>!Number.isInteger(slot)||slot<0||slot>=players))throw Error('挑战模式事件无效');
      for(const slot of event.slots){const value=next[slot]+(event.kind==='miss'?1:-1);if(!Number.isSafeInteger(value))throw Error('挑战模式计数溢出');next[slot]=value;}
    }
    counts=next;
  }
  return {
    applyFrame,
    miss(slot){add(slot,1);},
    item(slot){add(slot,-1);},
    extend(slots){
      if(!Array.isArray(slots)||new Set(slots).size!==slots.length||slots.some(slot=>!Number.isInteger(slot)||slot<0||slot>=players))throw Error('挑战模式奖残无效');
      if(slots.some(slot=>!Number.isSafeInteger(counts[slot]-1)))throw Error('挑战模式计数溢出');
      for(const slot of slots)add(slot,-1);
    },
    reset(){counts.fill(0);},
    snapshot(){return counts.slice();},
    restore(value){counts=validate(value);},
  };
}
