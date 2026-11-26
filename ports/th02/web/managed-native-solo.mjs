import {playFull} from './native/full.mjs';

const virtualKeys = ['ArrowUp','ArrowDown','ArrowLeft','ArrowRight','KeyX','KeyZ','ShiftLeft','Escape','Enter'];
export function createNativeSolo({canvas,input,exit,fail}) {
  let player, virtual = 0, saved;
  const physical = new Set();
  const sync = () => {
    if (!saved && player) saved=player.sync().finally(()=>{saved=undefined;});
    return saved;
  };
  return {
    async start() {
      player = await playFull(canvas,undefined,{
        hosted:true,
        beforeFrame(host) {
          const next=input.sample({keyboard:false});
          for (const [index,code] of virtualKeys.entries()) {
            const bit=1<<index;
            if ((next^virtual)&bit && !physical.has(code)) {
              if (next&bit) host.unlock();
              host.key(code,Boolean(next&bit));
            }
          }
          virtual=next;
        },
        onExit() { void exit(sync()).catch(fail); },
      });
      canvas.focus();
    },
    key(code,down,event) {
      if (!player) return;
      if (down) {physical.add(code);player.unlock();}
      else physical.delete(code);
      const index=virtualKeys.indexOf(code),virtualHeld=index>=0&&(virtual&(1<<index));
      if (!virtualHeld && !(code==='KeyV'&&event?.ctrlKey)) player.key(code,down,event);
    },
    health:()=>player?.health()??{frames:0,gameFrame:0,audio:{state:'suspended'}},
    sync,
    stop() {player?.stop();player=undefined;physical.clear();virtual=0;},
  };
}
