import {createDirectDrag} from './direct-drag.js';
// 32-bit, nonnegative wire value: existing commands 0..14; focus 15;
// signed 14-bit horizontal delta 16..29; active 30; gesture begin 31.
export const TOUCH_ACTIVE=2**30,TOUCH_BEGIN=2**31,TOUCH_FOCUS=2**15;
export function mountHorizontalTouch(canvas,options){
  let sensitivity=150,doubleTap=false,pulses=0;
  try{sensitivity=Math.max(100,Math.min(300,Number(localStorage.getItem('th01.touch.sensitivity'))||150));}catch{}
  try{doubleTap=localStorage.getItem('th01.touch.double-tap')==='on';}catch{}
  const drag=createDirectDrag(canvas,{...options,sensitivity:()=>sensitivity,
    doubleTap:()=>doubleTap,pulse:bit=>{if(bit===16&&options.gameplay())pulses|=options.bombBit??16;else options.pulse?.(bit);options.change?.();},
    geometry:()=>{const r=canvas.getBoundingClientRect();return {width:r.width,height:r.height,scale:Math.min(r.width/640,r.height/400)};},
  });
  canvas.style.touchAction='none';
  return {
    reset:()=>{pulses=0;drag.reset();},consume:()=>{pulses=0;drag.consume();},
    get menuBits(){return drag.menuBits;},
    pack(buttons=0){
      if(!options.allowed())return buttons;
      buttons|=pulses;
      if(!options.gameplay())return buttons|({1:16,2:32,4:1,8:2}[drag.menuBits]||0);
      const motion=drag.sample();
      if(drag.focused)buttons|=TOUCH_FOCUS;
      if(!motion.active)return buttons;
      return (buttons&~3)+TOUCH_ACTIVE+(drag.beginning?TOUCH_BEGIN:0)+(motion.x&16383)*65536;
    },
    setSensitivity(value){sensitivity=Math.max(100,Math.min(300,Number(value)||150));drag.reset();try{localStorage.setItem('th01.touch.sensitivity',String(sensitivity));}catch{}},
    get sensitivity(){return sensitivity;},
    get doubleTap(){return doubleTap;},
    setDoubleTap(value){doubleTap=!!value;drag.clearTap();try{localStorage.setItem('th01.touch.double-tap',doubleTap?'on':'off');}catch{}},
  };
}
export function mountTouchPreferences(anchor,touch){
  const box=document.createElement('div');box.className='small';
  box.innerHTML='<label>滑动灵敏度 <input type="range" min="100" max="300" step="10" aria-label="滑动灵敏度"><output></output></label> <label><input type="checkbox"> 双击同一位置使用 Bomb</label> · 左右拖动移动，第二指按住低速，四指暂停';
  anchor.after(box);const range=box.querySelector('input[type=range]'),check=box.querySelector('input[type=checkbox]'),output=box.querySelector('output');
  range.value=String(touch.sensitivity);output.textContent=touch.sensitivity+'%';check.checked=touch.doubleTap;
  range.oninput=()=>{touch.setSensitivity(range.value);output.textContent=touch.sensitivity+'%';};check.onchange=()=>touch.setDoubleTap(check.checked);
}
