import {createBrowserKeyboard} from '../../../common/browser/directory-keyboard.mjs';
const scans={Escape:1,Digit1:2,Digit2:3,Digit3:4,Digit4:5,Digit5:6,Digit6:7,Digit7:8,Digit8:9,Digit9:10,Digit0:11,Minus:12,Equal:13,Backspace:14,Tab:15,
KeyQ:16,KeyW:17,KeyE:18,KeyR:19,KeyT:20,KeyY:21,KeyU:22,KeyI:23,KeyO:24,KeyP:25,Enter:28,ControlLeft:29,ControlRight:29,KeyA:30,KeyS:31,KeyD:32,KeyF:33,KeyG:34,KeyH:35,KeyJ:36,KeyK:37,KeyL:38,ShiftLeft:42,ShiftRight:42,KeyZ:44,KeyX:45,KeyC:46,KeyV:47,KeyB:48,KeyN:49,KeyM:50,Space:57,ArrowUp:200,ArrowLeft:203,ArrowRight:205,ArrowDown:208};
const defaultBindings=[1,2,3,0,5,4,11,13,6];
export class Input {
  constructor(identity,ready){
    this.owners=new Map();this.pulses=new Map();this.options={};this.controls={};this.pointers=new Map();this.ready=ready;this.target=null;this.player=null;this.recordedMotion=new Map();this.extended=false;this.unlimitedUsed=false;
    this.keyboard=createBrowserKeyboard({hostContext:{...identity,ready,reset:()=>this.clear()},accept:code=>Object.hasOwn(scans,code),send:(code,down)=>this.key(code,down,'keyboard')});
  }
  key(code,down,source){let set=this.owners.get(code);if(down){if(!set)this.owners.set(code,set=new Set());set.add(source);}else if(set){set.delete(source);if(!set.size)this.owners.delete(code);}}
  pulse(code,ticks=2){this.pulses.set(code,code==='KeyZ'&&!this.isGameplay?.()?-ticks:ticks);}
  clearTouch(){for(const [code,set]of this.owners)for(const source of [...set])if(source==='touch'||source==='joystick'||source==='two-finger')this.key(code,false,source);this.pulses.clear();this.pointers.clear();this.controls={};this.target=null;}
  clear(){this.keyboard.clear();this.owners.clear();this.clearTouch();this.padBits=0;this.confirmRelease=false;this.confirmHeld=false;this.dialogueAdvance=false;this.dialogueRouting=false;}
  resetRecording(){this.recordedMotion.clear();this.extended=false;this.unlimitedUsed=false;this.motionCursor=undefined;this.restartDown=false;this.player=null;}
  configure(options){this.options=options;this.clearTouch();}
  syncTouchOwners(){
    const enabled=this.options.touchEnabled===true,digital=enabled&&this.options.touchMovementMode!=='joystick-free';
    this.key('KeyZ',enabled&&!!this.controls.fireEnabled&&this.isGameplay?.()===true,'touch');
    this.key('ShiftLeft',enabled&&!!this.controls.focusEnabled,'touch');
    for(const [code,held]of [['ArrowLeft',digital&&this.controls.joystickX<-.22],['ArrowRight',digital&&this.controls.joystickX>.22],['ArrowUp',digital&&this.controls.joystickY<-.22],['ArrowDown',digital&&this.controls.joystickY>.22]])this.key(code,held,'joystick');
  }
  command(m){
    switch(m.command){
    case 'keyboard':this.keyboard.event(m,!!m.down,'hosted');return true;
    case 'keyboard-clear':this.keyboard.clear();return true;
    case 'touch-cancel':this.clearTouch();return true;
    case 'touch-controls':{
      if(!Number.isFinite(m.touchSensitivity)||m.touchSensitivity<100||m.touchSensitivity>300)throw Error('触控灵敏度应为 100–300%');
      if(this.options.touchEnabled===true){
        if(this.controls.bombSerial!==undefined&&m.bombSerial!==this.controls.bombSerial)this.pulse('KeyX');
        if(this.controls.escapeSerial!==undefined&&m.escapeSerial!==this.controls.escapeSerial)this.pulse('Escape');
      }
      this.controls={...m};this.syncTouchOwners();return true;
    }
    case 'direct-touch':{
      if(!['down','move','up'].includes(m.type)||!Number.isFinite(m.x)||!Number.isFinite(m.y)||!Number.isSafeInteger(m.id))throw Error('触控坐标无效');
      if(this.options.touchEnabled!==true)return true;
      const old=this.pointers.get(m.id);
      if(m.type==='down'){
        if(!this.pointers.size)this.menuBackSent=false;
        this.pointers.set(m.id,{x:m.x,y:m.y,startX:m.x,startY:m.y,time:performance.now(),gesture:false});
        if(this.pointers.size>1)for(const pointer of this.pointers.values())pointer.multi=true;
        if(this.pointers.size===1&&this.player)this.target={x:this.player.x,y:this.player.y};
        if(this.options.doubleTapBombEnabled&&this.lastTap&&performance.now()-this.lastTap.time<280&&Math.hypot(m.x-this.lastTap.x,m.y-this.lastTap.y)<.08)this.pulse('KeyX');
      }else if(m.type==='move'&&old){
        if(this.isGameplay?.()&&this.pointers.keys().next().value===m.id&&['touch','touch-unlimited',undefined].includes(this.options.touchMovementMode)&&this.player){
          if(!this.target)this.target={x:this.player.x,y:this.player.y};const gain=(this.controls.touchSensitivity||this.options.touchSensitivity||100)/100;
          this.target.x+=(m.x-old.x)*640*gain;this.target.y+=(m.y-old.y)*480*gain;
        }else if(!old.gesture&&!old.multi){const dx=m.x-old.startX,dy=m.y-old.startY;if(Math.hypot(dx,dy)>.075){this.pulse(Math.abs(dx)>Math.abs(dy)?dx>0?'ArrowRight':'ArrowLeft':dy>0?'ArrowDown':'ArrowUp');old.gesture=true;}}
        old.x=m.x;old.y=m.y;
      }else if(m.type==='up'&&old){
        if(!this.isGameplay?.()&&!old.gesture&&Math.hypot(m.x-old.startX,m.y-old.startY)<.075){if(old.multi){if(!this.menuBackSent){this.pulse('KeyX');this.menuBackSent=true;}}else this.pulse('KeyZ');}
        this.lastTap={x:m.x,y:m.y,time:performance.now()};this.pointers.delete(m.id);if(!this.pointers.size)this.target=null;
      }
      if(this.options.touchFocusMode==='two-finger')this.key('ShiftLeft',this.pointers.size>=2,'two-finger');return true;
    }
    default:return false;
    }
  }
  motion(state){
    this.player={x:state.x,y:state.y};
    if(state.replay)return this.replayMotion?.get(state.cursor)||null;
    if(this.options.touchEnabled!==true)return null;
    if(this.motionCursor!==undefined&&state.cursor<this.motionCursor){this.recordedMotion.clear();this.extended=false;this.unlimitedUsed=false;}
    this.motionCursor=state.cursor;
    if(state.blocked){if(this.target)this.target={x:state.x,y:state.y};return null;}
    let dx=0,dy=0,active=false;
    if(this.target&&this.pointers.size){dx=this.target.x-state.oldX;dy=this.target.y-state.oldY;active=true;}
    else if(this.options.touchMovementMode==='joystick-free'){
      const x=Number(this.controls.joystickX)||0,y=Number(this.controls.joystickY)||0,magnitude=Math.min(1,Math.hypot(x,y));
      if(magnitude>.12){dx=x/Math.hypot(x,y)*state.speed*magnitude;dy=y/Math.hypot(x,y)*state.speed*magnitude;active=true;}
    }
    if(!active)return null;
    dx+=state.x-state.oldX;dy+=state.y-state.oldY;
    const unlimited=this.options.touchMovementMode==='touch-unlimited',distance=Math.hypot(dx,dy);
    if(!unlimited&&distance>state.speed){dx*=state.speed/distance;dy*=state.speed/distance;}
    const result={x:Math.min(376,Math.max(8,state.oldX+dx)),y:Math.min(432,Math.max(16,state.oldY+dy))};
    if(unlimited&&(result.x!==state.oldX||result.y!==state.oldY))this.unlimitedUsed=true;
    return result.x===state.x&&result.y===state.y?null:result;
  }
  recordMotion(state){if(state.replay)return;this.extended=true;this.recordedMotion.set(state.cursor,{x:state.x,y:state.y});}
  padState(){
    if(this.replaying)return 0;
    const bindings=this.bindings?.()||defaultBindings,indices=[4,5,6,7,2,3,0,1,8,4];let bitmap=0;
    indices.forEach((binding,index)=>{const bit=bindings[binding];if(index===9&&this.isGameplay?.())return;if((this.padBits&(1<<index))&&Number.isInteger(bit)&&bit>=0&&bit<16)bitmap|=1<<bit;});if(this.confirmRelease)bitmap&=~(1<<bindings[4]);if(this.dialogueRouting){bitmap&=~(1<<bindings[8]);if(this.dialogueAdvance)bitmap|=1<<bindings[8];}return bitmap;
  }
  sample(output){
    if(this.replaying){this.padBits=0;this.dialogueRouting=false;return;}
    this.syncTouchOwners();
    for(const code of this.owners.keys())if(scans[code]!==undefined)output[scans[code]]=1;
    let bits=globalThis.__eaglerGamepadInputBridge?.sample(0)??-1;
    if(bits<0){
      bits=0;
      const pads=navigator.getGamepads?.()||[];
      for(const pad of pads)if(pad?.connected&&this.ready()&&(typeof document==='undefined'||document.hasFocus())){
        const held=(i)=>!!pad.buttons[i]?.pressed;
        [held(0),held(1),held(4)||held(6),held(9),held(12)||pad.axes[1]<-.35,held(13)||pad.axes[1]>.35,held(14)||pad.axes[0]<-.35,held(15)||pad.axes[0]>.35,held(5)].forEach((yes,index)=>{if(yes)bits|=1<<index;});
      }
    }
    if(!this.isGameplay?.()&&(bits&(1<<9))&&!(this.padBits&(1<<9)))this.pulse('KeyZ');
    this.padBits=bits;if(bits&(1<<10))output[46]=1;if(bits&(1<<11))output[19]=1;
    if(!this.isGameplay?.()&&output[28])output[44]=1;
    this.confirmRelease=false;
    for(const [code,ticks]of this.pulses){if(ticks<0){output[scans[code]]=0;if(code==='KeyZ')this.confirmRelease=true;this.pulses.set(code,-ticks);}else{output[scans[code]]=1;if(ticks<=1)this.pulses.delete(code);else this.pulses.set(code,ticks-1);}}
    if(!this.isGameplay?.()&&[...this.pointers.values()].some(pointer=>!pointer.gesture&&performance.now()-pointer.time>350))output[29]=1;
    const confirm=!!output[44]||!this.confirmRelease&&!!(bits&((1<<0)|(1<<9)));
    this.dialogueRouting=this.isDialogue?.()===true;
    if(this.dialogueRouting){const skip=!!output[29]||!!(bits&(1<<8));output[29]=0;this.dialogueAdvance=confirm&&!this.confirmHeld||skip&&!this.dialogueAdvance;}
    else this.dialogueAdvance=false;
    this.confirmHeld=confirm;
    if(output[19]&&!this.restartDown)this.onRestart?.();this.restartDown=!!output[19];
  }
}
