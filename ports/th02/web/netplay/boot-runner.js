// Bound wall-clock work without changing virtual ticks or guest CPU settings.
export async function runBoot({step,ready,tick,progress,cancelled=()=>false,now=()=>performance.now(),yieldTask=()=>new Promise(resolve=>setTimeout(resolve,0)),limit=10800}){
  let count=0,lastProgress=-Infinity;
  while(!ready()&&count<limit){
    const started=now();
    do{if(cancelled())return false;step();count++;}
    while(!ready()&&count<limit&&count%15!==0&&now()-started<25);
    const time=now();
    if(time-lastProgress>=500||ready()){
      await progress(`执行原 DOS / 游戏启动：模拟 ${(tick()/60).toFixed(1)} 秒`);lastProgress=now();
    }else await yieldTask();
  }
  if(cancelled())return false;
  if(!ready())throw Error('原游戏启动超过 180 模拟秒仍未检测到首帧');
  return true;
}
