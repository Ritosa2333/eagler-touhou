import {createChallengeCounters} from './pc98-challenge-counters.mjs';

// A unit of spare life is reserved in the native engine so that its original
// hit/respawn path remains active. Game-owned stocks are normalized at the
// synchronous emulation boundary; signed challenge counts are checkpointed
// separately with the rest of the lockstep host state.
export function createChallengeStocks(players, baseline=2){
  if(!Number.isInteger(baseline)||baseline<1||baseline>4)throw Error('挑战模式残机基准无效');
  const counters=createChallengeCounters(players);
  let pending=null;
  function validateStocks(stocks){
    if(!Array.isArray(stocks)||stocks.length!==players||stocks.some(s=>!s||!Number.isInteger(s.lives)||s.lives<0||s.lives>255||!Number.isInteger(s.bombs)||s.bombs<0||s.bombs>255))throw Error('挑战模式资源状态无效');
  }
  return {
    // Every player starts the native step at the same safe, non-terminal
    // stock. An offline seat is never credited with an item or a miss.
    begin(offlineMask=0){if(pending)throw Error('挑战模式帧尚未结束');pending={offlineMask};return Array.from({length:players},()=>({lives:baseline,bombs:0}));},
    finish(stocks){
      if(!pending)throw Error('挑战模式帧尚未开始');validateStocks(stocks);
      const changes=[];
      for(let i=0;i<players;i++){
        if(pending.offlineMask&(1<<i))continue;
        const n=baseline-stocks[i].lives-stocks[i].bombs;
        for(let j=0;j<Math.abs(n);j++)changes.push({kind:n>0?'miss':'life',slots:[i]});
      }
      counters.applyFrame(changes);pending=null;
      return Array.from({length:players},()=>({lives:baseline,bombs:0}));
    },
    abort(){pending=null;},
    reset(){pending=null;counters.reset();},
    snapshot(){if(pending)throw Error('挑战模式帧尚未结束');return counters.snapshot();},
    restore(value){pending=null;counters.restore(value);},
  };
}
