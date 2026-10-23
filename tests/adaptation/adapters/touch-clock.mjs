import {resolveAdaptationAdapter} from './registry.mjs';

const legacy={
  th10:{tickApp:true,startApp:true,source:'ports/th10/web/cpp/sdl/ApplicationHost.cpp | sdl_loop_tick'},
  th20:{tickApp:false,startApp:false,source:'ports/th20/web/cpp/sdl/ApplicationHost.cpp | sdl_loop_tick'},
};

export function touchClockRequest({game,variant='normal',action}){
  const adapter=resolveAdaptationAdapter(game,variant);
  const fallback=variant==='normal'?legacy[game]:null;
  const clock=adapter?.clock??(fallback?{...fallback,stopExport:'sdl_loop_stop',tickExport:'sdl_loop_tick',
    startExport:'sdl_loop_start',pauseExport:'sdl_loop_pause'}:null);
  return{game,variant,action,contract:clock?{...clock,runtime:adapter?.runtime??{
    global:`__${game}Runtime`,core:'core',app:'app',module:'Module',
  }}:null};
}

/** Browser-side clock adapter. Only explicit test waits advance the normal game loop. */
export function controlTouchMotionClock({game,variant,action,contract}){
  if(!['begin','tick','end'].includes(action))throw Error(`Unknown touch scene clock action ${String(action)}`);
  const key='__eaglerTouchMotionClock';
  const mapping=contract?.runtime,runtime=mapping?globalThis[mapping.global]:null,core=runtime?.[mapping?.core];
  const app=runtime?.[mapping?.app];
  if(action==='begin'){
    if(globalThis[key])throw Error('A touch scene already owns the game clock');
    if(!contract||!runtime||((contract.tickApp||contract.startApp)&&app==null)||
       ![contract.stopExport,contract.tickExport,contract.startExport].every(name=>typeof core?.[name]==='function'))
      return{mode:'realtime',reason:'This Runtime has no manual production-loop adapter'};
    core[contract.stopExport]();
    globalThis[key]={game,variant,runtime,core,contract,app,ticks:0};
    return{mode:'native-manual',ticks:0,source:contract.source};
  }
  const clock=globalThis[key];
  if(!clock)return{mode:'realtime'};
  if(clock.game!==game||clock.variant!==variant||clock.runtime!==runtime)throw Error('Touch scene clock belongs to another Runtime');
  if(action==='tick'){
    const args=clock.contract.tickApp?[clock.app,1/60,0]:[1/60,0];
    const result=clock.core[clock.contract.tickExport](...args);
    if(result!==0)throw Error(`Production loop ended during touch scene (${result})`);
    return{mode:'native-manual',ticks:++clock.ticks};
  }
  if(clock.contract.pauseExport)clock.core[clock.contract.pauseExport]?.(0);
  if(clock.contract.startApp)clock.core[clock.contract.startExport](clock.app);else clock.core[clock.contract.startExport]();
  delete globalThis[key];
  return{mode:'native-manual',ticks:clock.ticks,resumed:true};
}
