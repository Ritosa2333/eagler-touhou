// Appended to netplay/app.js. The existing NetSession owns simulation and hashes.
function sample(){return globalThis.__eaglerPC98Input.nativeSample();}
export const managedAPI={
 async start({options,music}){
  const count=options.netplayPlayerCount,slot=options.netplayPlayer;
  if(![2,3].includes(count)||!Number.isInteger(slot)||slot<0||slot>=count||options.netplaySpectator)throw Error('需要已落座的主项目房间');
  const url=new URL(options.netplayUrl),runId=Number(url.searchParams.get('run'));
  if(!url.searchParams.get('room')||!Number.isSafeInteger(runId)||runId<1)throw Error('开局身份无效');
  const ballCount=options.netplayBallCount ?? 2;
  if(![1,2,3,4,6,8].includes(ballCount))throw Error('球数无效');
  if(!Number.isInteger(options.netplayDifficulty)||options.netplayDifficulty<0||options.netplayDifficulty>3)throw Error('难度无效');
  identity={role:roles[slot],room:url.searchParams.get('room'),token:'launcher-owned'};
  room={room:identity.room,runId,generation:[identity.room,runId,url.searchParams.get('runtimeGeneration')].join(':'),transport:'ws',phase:'loading',
   seats:{host:0,guest:1,guest2:2},ready:{host:true,guest:true,guest2:true},present:{host:true,guest:true,guest2:count===3},
   settings:{players:count,rank:options.netplayDifficulty,count:ballCount,lifeCompensation:true,startStage:'normal',network:'lan',transport:'ws',ice:'all',delay:0,rollback:options.netplayAdonisMode!==1}};
  globalThis.__eaglerPC98RoomEnd=()=>managedAPI.end('本局结束');
  session=new NetSession(identity,room,{canvas:$('canvas'),api:post,status,buttons:sample,render,failure:message=>{managedAPI.stop();globalThis.__eaglerPC98Emit({event:'error',message});}});
  session.game.setMusicVolume(music==='none'?0:.7);
  window.th01Net={get session(){return session;},get room(){return room;}};
  await session.prepare();
 },
 end(reason){if(returningTask)return returningTask;returningTask=(async()=>{try{await session?.relay?.finish();session?.recorder.finish(reason);await session?.recorder.sealPublisher();await session?.recorder.seal(2000);managedAPI.stop();globalThis.__eaglerPC98Emit({event:'exit',code:0,reason});}catch(error){managedAPI.stop();globalThis.__eaglerPC98Emit({event:'error',message:error.message});}})();return returningTask;},
 health(){const frame=session?.queue?.frame??0;Object.assign(globalThis,{__eaglerNetplayLanActive:!!session?.running,__eaglerNetplayLanFrame:frame,__eaglerNetplayLanConfirmed:Math.min(frame,session?.queue?.confirmed??0),__eaglerNetplayLanRollback:session?.rollbackCount??0,__eaglerNetplayLanResimulated:session?.replayedFrames??0,__eaglerNetplayInputDelayFrames:session?.queue?.delay??0});return {frames:session?.game.displayFrames??0,frame,confirmed:Math.min(frame,session?.queue?.confirmed??0),rollbackEnabled:!!session?.queue?.rollback,rollbackCount:session?.rollbackCount??0,replayedFrames:session?.replayedFrames??0,running:!!session?.running,stopped:!!session?.closed,audio:{state:session?.running?'running':'loading'}};},
 files:message=>globalThis.__eaglerPC98FileCommands(message),
 playback(){throw Error('多人录像工作台尚未接入');},
 stop(){session?.close();clear();},
};
