// Input transport for stage previews. Gameplay consumes one sample per tick.
export const KEY_BITS = Object.freeze({ArrowUp:16,ArrowDown:32,ArrowLeft:64,ArrowRight:128,KeyZ:1,KeyX:2,ShiftLeft:8,ShiftRight:8});

export function supportedReplay(replay,number) {
  return !!replay && Number.isInteger(replay.difficulty) && replay.difficulty >= 0 && replay.difficulty <= 4 && replay.stages?.some(stage=>Number.isInteger(stage.number)&&(number===undefined||stage.number===number)&&(replay.difficulty===4?stage.number===7:stage.number>=1&&stage.number<=6));
}

export function validateTouchSnapshot(value) {
  if(typeof value.fireEnabled!=='boolean'||typeof value.focusEnabled!=='boolean')throw Error('触控按键状态无效');
  for(const field of ['bombSerial','escapeSerial'])if(!Number.isSafeInteger(value[field])||value[field]<0)throw Error('触控动作序号无效');
  for(const field of ['joystickX','joystickY'])if(!Number.isFinite(value[field])||Math.abs(value[field])>1)throw Error('触控方向无效');
  if(value.touchSensitivity!==undefined&&(!Number.isFinite(value.touchSensitivity)||value.touchSensitivity<100||value.touchSensitivity>300))throw Error('触控灵敏度无效');
  return value;
}

export class PreviewInput {
  localKeys=new Set(); hostKeys=new Set(); touches=new Map(); hostTouch=0;
  localReplayKeys=new Set(); hostReplayKeys=new Set();
  bombSerial=0; escapeSerial=0; pendingBomb=false; blockedPadBomb=false;
  key(code,down,source='local') {
    if(code==='ControlLeft'||code==='ControlRight'){
      const keys=source==='host'?this.hostReplayKeys:this.localReplayKeys;down?keys.add(code):keys.delete(code);return;
    }
    const keys=source==='host'?this.hostKeys:this.localKeys;if(KEY_BITS[code])down?keys.add(code):keys.delete(code);
  }
  replayRate(active=false) {return active&&(this.localReplayKeys.size||this.hostReplayKeys.size)?4:1;}
  clearKeyboard() {this.localKeys.clear();this.hostKeys.clear();this.localReplayKeys.clear();this.hostReplayKeys.clear();}
  cancelTouch() {this.touches.clear();this.hostTouch=0;this.pendingBomb=false;}
  clear({resetSerials=false}={}) {this.clearKeyboard();this.cancelTouch();this.blockedPadBomb=true;if(resetSerials)this.bombSerial=this.escapeSerial=0;}
  touch(snapshot,active=true) {
    validateTouchSnapshot(snapshot);
    const bomb=snapshot.bombSerial>this.bombSerial,escape=snapshot.escapeSerial>this.escapeSerial;
    this.bombSerial=snapshot.bombSerial;this.escapeSerial=snapshot.escapeSerial;
    if(!active){this.hostTouch=0;this.pendingBomb=false;return escape;}
    this.hostTouch=(snapshot.fireEnabled?1:0)|(snapshot.focusEnabled?8:0)|
      (snapshot.joystickX<-.2?64:snapshot.joystickX>.2?128:0)|(snapshot.joystickY<-.2?16:snapshot.joystickY>.2?32:0);
    if(bomb)this.pendingBomb=true;
    return escape;
  }
  sample(previous,pad) {
    let held=this.hostTouch;
    for(const code of this.localKeys)held|=KEY_BITS[code];for(const code of this.hostKeys)held|=KEY_BITS[code];
    for(const value of this.touches.values())held|=value;
    if(pad?.connected){
      if(pad.axes[0]<-.3||pad.buttons[14]?.pressed)held|=64;if(pad.axes[0]>.3||pad.buttons[15]?.pressed)held|=128;
      if(pad.axes[1]<-.3||pad.buttons[12]?.pressed)held|=16;if(pad.axes[1]>.3||pad.buttons[13]?.pressed)held|=32;
      if(!pad.buttons[1]?.pressed)this.blockedPadBomb=false;
      if(pad.buttons[0]?.pressed)held|=1;if(pad.buttons[1]?.pressed&&!this.blockedPadBomb)held|=2;if(pad.buttons[2]?.pressed)held|=8;
    }
    const bomb=this.pendingBomb;this.pendingBomb=false;if(bomb)held|=2;
    return {held,pressed:(held&~previous)|(bomb?2:0)};
  }
}
