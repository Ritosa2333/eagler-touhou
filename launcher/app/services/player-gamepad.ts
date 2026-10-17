import {gamepadMappingSchema, type GamepadMappingModel} from '../../src/launcher/gamepad-mapping.mts';
import type {RuntimeService} from './runtime';
import type {Th09LocalInput} from '../../src/launcher/th09-local-input.mts';
/** Bind one synchronous policy port to the current Runtime identity. */
export function createPlayerGamepad({runtime,model,window,document,playerOpen,th09Local}: {
  runtime:Pick<RuntimeService,'getInputContext'|'subscribe'>;model:GamepadMappingModel;
  window:EventTarget;document:EventTarget&{visibilityState:string;hasFocus():boolean};playerOpen():boolean;
  th09Local?:Th09LocalInput;
}) {
  let unbind:(()=>void)|null=null;let identity='';
  let boundTarget:unknown=null,boundBridge:unknown=null;
  function sync(){
    const context=runtime.getInputContext(),key=context.game+':'+context.epoch;
    const target=context.target;
    let currentBridge:unknown;try{currentBridge=target?.__eaglerGamepadInputBridge;}catch{}
    if(identity===key&&unbind&&target===boundTarget&&currentBridge===boundBridge)return;
    unbind?.();unbind=null;identity=key;boundTarget=target;boundBridge=currentBridge;model.suspend();if(!target)return;
    try {const bridge=target.__eaglerGamepadInputBridge as {schema?:string;epoch?:number;protocol?:string;game?:string;origin?:string}|undefined;
      if(bridge?.schema!==gamepadMappingSchema||bridge.protocol!==context.protocol||bridge.game!==context.game||bridge.epoch!==context.epoch||bridge.origin!==context.targetOrigin)return;
      const localMode=()=>context.game==='th09'&&(target.__eaglerKeyboardInputBridge as {profile?():string}|undefined)?.profile?.()==='th09-local';
      const port={schema:gamepadMappingSchema,protocol:context.protocol,game:context.game,epoch:context.epoch,origin:context.targetOrigin,
        enabled:()=>localMode()?th09Local?.getSnapshot().enabled===true:model.getSnapshot().enabled,
        sample(seat:number){const next=runtime.getInputContext(),allowed=next.target===target&&next.epoch===context.epoch&&next.game===context.game&&next.launched&&!next.spectator&&playerOpen()&&document.visibilityState!=='hidden'&&document.hasFocus();
          if(localMode()){const state=th09Local?.getSnapshot();if(!state?.enabled)return -1;const player=state.players[seat];return player?.device==='gamepad'?model.sampleDevice(seat,player.gamepad,allowed):0;}return model.sample(seat,allowed);},
        clear:()=>model.suspend()};
      target.__eaglerGamepadMapping=port;unbind=()=>{try{if(target.__eaglerGamepadMapping===port)delete target.__eaglerGamepadMapping;}catch{}};
    }catch { /* Cross-origin and legacy Runtime input remains native. */ }
  }
  const clear=()=>model.suspend();const visibility=()=>{if(document.visibilityState==='hidden')clear();};
  window.addEventListener('blur',clear);window.addEventListener('pagehide',clear);window.addEventListener('gamepadconnected',clear);window.addEventListener('gamepaddisconnected',clear);document.addEventListener('visibilitychange',visibility);
  const unsubscribe=runtime.subscribe(sync),unsubscribeLocal=th09Local?.subscribe(clear);sync();
  return {dispose(){unsubscribe();unsubscribeLocal?.();unbind?.();clear();window.removeEventListener('blur',clear);window.removeEventListener('pagehide',clear);window.removeEventListener('gamepadconnected',clear);window.removeEventListener('gamepaddisconnected',clear);document.removeEventListener('visibilitychange',visibility);}};
}
