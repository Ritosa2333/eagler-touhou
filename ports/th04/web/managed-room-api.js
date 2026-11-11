// Appended to netplay/room.js; the original rollback queue, boot, checksum,
// confirmed effects and departure protocol remain the simulation owner.
export const managedAPI={
  async end(reason){
    if(returningTask)return returningTask;
    returningTask=(async()=>{
      // A local confirmed exit must not close the fast/reliable channels while
      // another native engine is still consuming the final confirmed input.
      running=false;
      try{await relay?.finish();managedAPI.stop();globalThis.__eaglerPC98Emit({event:'exit',code:0,reason});}
      catch(error){managedAPI.stop();globalThis.__eaglerPC98Emit({event:'error',message:error.message});globalThis.__eaglerPC98Emit({event:'exit',code:1});}
    })();return returningTask;
  },
  async start({options,language,music}) {
    globalThis.Module??={};globalThis.Module.eaglerOptions=options;
    const count=options.netplayPlayerCount, local=options.netplayPlayer;
    if(![2,3].includes(count)||!Number.isInteger(local)||local<0||local>=count||options.netplaySpectator)throw Error('此入口需要已落座的主项目房间');
    const url=new URL(options.netplayUrl),runId=Number(url.searchParams.get('run'));
    if(!Number.isSafeInteger(runId)||runId<1||!url.searchParams.get('room'))throw Error('主项目房间开局身份无效');
    const loadouts=options.netplayLoadouts;
    if(!Array.isArray(loadouts)||loadouts.length!==count||loadouts.some(l=>![0,1].includes(l.character)||![0,1].includes(l.shot)))throw Error('机体选择无效');
    const difficulty=options.netplayDifficulty;
    if(!Number.isInteger(difficulty)||difficulty<0||difficulty>4)throw Error('难度无效');
    const settings={players:count,difficulty,lives:3,bombs:2,language:language==='ja'?'jp':'cn',rollback:options.netplayAdonisMode===0,
      p1:loadouts[0].character*2+loadouts[0].shot,p2:loadouts[1].character*2+loadouts[1].shot,p3:loadouts[2]?loadouts[2].character*2+loadouts[2].shot:0};
    session={role:roles[local],room:url.searchParams.get('room'),token:'launcher-owned'};
    state={phase:'loading',generation:[session.room,runId,url.searchParams.get('runtimeGeneration')].join(':'),runId,revision:1,settings,
      slots:{host:0,guest:1,guest2:2},ready:{host:true,guest:true,guest2:true},present:{host:true,guest:true,guest2:count===3}};
    stopped=false;preparing=true;runEpoch++;musicVolume.querySelector('input').value=music==='none'?'0':'70';
    player.enter();render();await beginGame();
  },
  health(){const render=runtime?.renderInfo()??{};
    Object.assign(globalThis,{__eaglerNetplayLanActive:running,__eaglerNetplayLanFrame:queue?.frame??0,__eaglerNetplayLanConfirmed:queue?.confirmed??0,
      __eaglerNetplayLanRollback:rollbackCount,__eaglerNetplayLanResimulated:resimulated,__eaglerNetplayInputDelayFrames:queue?.active?queue.inputDelay:queue?.lockstepDelay});
    return {frames:render.pageDrawn??render.drawn??0,frameMs:averageCost,audio:{state:running?'running':'loading'},running,stopped,frame:queue?.frame??0,confirmed:queue?.confirmed??0};},
  stop(){if(stopped&& !runtime)return;stopped=true;running=false;runEpoch++;clearInput();clearTimers();runtime?.stop();disconnect();replayRecorder.close();iframe?.remove();iframe=null;runtime=null;},
  files:message=>globalThis.__eaglerPC98FileCommands(message),
  playback(){throw Error('多人录像工作台尚未启用');},
};
