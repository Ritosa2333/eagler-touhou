let api;
export const managedAPI={
 async start({options,language,music}){
  if(options.netplayPlayerCount!==2||![0,1].includes(options.netplayPlayer)||options.netplaySpectator)throw Error('梦时空需要双人对战房间');
  const url=new URL(options.netplayUrl),room=url.searchParams.get('room'),runId=Number(url.searchParams.get('run'));
  if(!room||!Number.isSafeInteger(runId)||runId<1)throw Error('开局身份无效');
  if(!Number.isInteger(options.netplayDifficulty)||options.netplayDifficulty<0||options.netplayDifficulty>3)throw Error('难度无效');
  globalThis.th03RoomHost={params:new URLSearchParams({net:'rtc',role:options.netplayPlayer?'guest':'host',room,token:'launcher-owned',runId:String(runId),netWorker:'0'}),
   state:{started:true,runId,mode:'rtc',settings:{language:language==='ja'?'jp':'cn',difficulty:options.netplayDifficulty,clock:32,rollback:options.netplayAdonisMode===0,focusEnabled:true,touchUnlimitedAllowed:options.touchMovementMode==='touch-unlimited',hostSeat:0}},
   notify(message){if(message.event==='room-menu')void api?.end();}};
  api=(await import('./app.js')).managedAPI;await api.start({music});
 },
 health(){return api?.health()??{frames:0,running:false,frame:0,confirmed:0};},
 files:message=>globalThis.__eaglerPC98FileCommands(message),
 playback(){throw Error('多人录像工作台尚未接入');},
 stop(){api?.stop();},
};
