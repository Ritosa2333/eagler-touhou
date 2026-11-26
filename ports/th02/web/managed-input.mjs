import {createBrowserKeyboard} from './directory-keyboard.mjs';
import {createHostedTouch} from './hosted-touch.mjs';
const buttons={ArrowUp:1,ArrowDown:2,ArrowLeft:4,ArrowRight:8,KeyX:16,KeyZ:32,ShiftLeft:64,ShiftRight:64,Escape:128,Enter:256};
const padBits=[[1,32],[2,16],[4,64],[8,128],[16,1],[32,2],[64,4],[128,8],[512,256]];
export function managedInput({game,epoch,active,options,onKey,allowNativeKey,scene,panelPointer}){
  let rawKey,keyPulse=0;
  const held=new Set();
  const touch=createHostedTouch({options,scene,panelPointer});
  const keyboard=createBrowserKeyboard({send:(code,down)=>{
    if(down){
      held.add(code);
      const bit=buttons[code]??0,axis=bit&3?3:bit&12?12:0;
      keyPulse=(keyPulse&~axis)|bit;
    }else held.delete(code);
    onKey(code,down,rawKey);
  },hostContext:{target:globalThis,protocol:'eagler-touhou/1',game,epoch,origin:location.origin,ready:active}});
  const input={
    key(message){
      rawKey=message;
      try{keyboard.event(message,message.down,'hosted');}finally{rawKey=undefined;}
    },
    clearKeyboard(){keyboard.clear();held.clear();keyPulse=0;},
    clear(){input.clearKeyboard();touch.cancel();},
    cancelTouch:touch.cancel,
    touch(message){
      if(typeof message.fireEnabled!=='boolean'||typeof message.focusEnabled!=='boolean'||
        !['bombSerial','escapeSerial'].every(k=>Number.isSafeInteger(message[k])&&message[k]>=0)||
        !['joystickX','joystickY'].every(k=>Number.isFinite(message[k])&&Math.abs(message[k])<=32767))throw Error('触控输入无效');
      touch.controls(message);
    },
    pointer(message){
      if(!['down','move','up'].includes(message.type)||!Number.isSafeInteger(message.id)||
        !Number.isFinite(message.x)||!Number.isFinite(message.y))throw Error('触控坐标无效');
      const rect=document.querySelector('#game canvas')?.getBoundingClientRect();
      if(rect?.width>0&&rect.height>0)touch.pointer({...message,
        x:(message.x*innerWidth-rect.left)*640/rect.width,y:(message.y*innerHeight-rect.top)*400/rect.height});
    },
    sample({keyboard:includeKeyboard=true}={}){
      if(!active())return 0;
      let bits=touch.sample();
      if(includeKeyboard){
        bits|=keyPulse;keyPulse=0;
        for(const code of held)bits|=buttons[code]??0;
      }
      const pad=globalThis.__eaglerGamepadInputBridge?.sample(options().netplayPlayer??0);
      if(pad>=0)for(const [mask,out] of padBits)if(pad&mask)bits|=out;
      return bits;
    },
    motion:touch.motion,
  };
  for(const type of ['keydown','keyup'])addEventListener(type,event=>{
    if(event.__eaglerPC98Synthetic||allowNativeKey?.(event)===false)return;
    if(type==='keydown'&&event.target?.closest?.('input,select,textarea,[contenteditable="true"]'))return;
    rawKey=event;
    try{
      if(keyboard.mapNative(event,type==='keydown')||!globalThis.__eaglerKeyboardInputBridge?.active()&&keyboard.event(event,type==='keydown')){
        event.preventDefault();event.stopImmediatePropagation();
      }
    }finally{rawKey=undefined;}
  },true);
  // The Launcher touch surface lives outside the iframe. A blur releases
  // physical keys without cancelling gestures on that surface.
  addEventListener('blur',input.clearKeyboard);
  addEventListener('pagehide',input.clear);
  document.addEventListener('visibilitychange',()=>{if(document.hidden)input.clear();});
  return input;
}
