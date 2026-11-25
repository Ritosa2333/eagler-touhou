import {playFull} from './native/full.mjs';

const virtualKeys = ['ArrowUp','ArrowDown','ArrowLeft','ArrowRight','KeyX','KeyZ','ShiftLeft','Escape','Enter'];
const coopBindings = [
  ['KeyW','KeyS','KeyA','KeyD','KeyJ','KeyL','KeyK'],
  ['Numpad8','Numpad2','Numpad4','Numpad6','Numpad0','NumpadDecimal','NumpadEnter'],
];
const coopBits = [1,2,4,8,16,32,1024];
function padBits(pad) {
  if (!(pad>=0)) return 0;
  return (pad>>4&15)|(pad&1?16:0)|(pad&2?32:0)|(pad&4?1024:0);
}

export function createNativeSolo({canvas,input,files,localCoop,exit,fail}) {
  let player, virtual = 0, saved;
  const physical = new Set();
  const coopHeld = [0,0], coopPulse = [0,0];
  const clearCoopKeys = () => {coopHeld.fill(0);coopPulse.fill(0);};
  const sync = () => {
    if (!saved && player) saved=player.sync().finally(()=>{saved=undefined;});
    return saved;
  };
  return {
    async start() {
      player = await playFull(canvas,undefined,{
        hosted:true,localCoop,
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
          if(localCoop){
            const bridge=globalThis.__eaglerGamepadInputBridge;
            const extra=coopHeld.map((held,i)=>i+1<localCoop[0]?held|coopPulse[i]|padBits(bridge?.sample(i+1)):0);
            host.coopInputs(0,...extra);
            coopPulse.fill(0);
          }
        },
        onExit() { void exit(sync()).catch(fail); },
      });
      canvas.focus();
    },
    key(code,down,event) {
      if (!player) return;
      if (down) {physical.add(code);player.unlock();}
      else physical.delete(code);
      if(localCoop)for(const [i,keys] of coopBindings.entries()){
        const index=keys.indexOf(code);
        if(index<0)continue;
        if(i+1<localCoop[0]){
          const bit=coopBits[index];
          if(down){if(!(coopHeld[i]&bit))coopPulse[i]|=bit;coopHeld[i]|=bit;}
          else coopHeld[i]&=~bit;
        }
        return;
      }
      const index=virtualKeys.indexOf(code),virtualHeld=index>=0&&(virtual&(1<<index));
      if (!virtualHeld && !(code==='KeyV'&&event?.ctrlKey)) player.key(code,down,event);
    },
    clearCoopKeys,
    health:()=>player?.health()??{frames:0,gameFrame:0,audio:{state:'suspended'}},
    async files(message) {
      if (message.command==='sync') {await sync();return {};}
      return files(message);
    },
    stop() {player?.stop();player=undefined;physical.clear();clearCoopKeys();virtual=0;},
  };
}
