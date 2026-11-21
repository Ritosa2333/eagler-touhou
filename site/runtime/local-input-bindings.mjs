import {analogGamepadDirections} from './gamepad-input.mjs';

export const localInputActions=Object.freeze([
 ['ArrowUp','上',12],['ArrowDown','下',13],['ArrowLeft','左',14],['ArrowRight','右',15],['KeyZ','射击／确认',0],['KeyX','完美冻结／返回',1],['KeyC','连射',2],['ShiftLeft','低速',4],['Escape','暂停',9],['KeyR','重开',3]
]);
const allowed=/^(?:Key[A-Z]|Digit[0-9]|Numpad[0-9]|Arrow(?:Up|Down|Left|Right)|Space|Enter|Escape|Shift(?:Left|Right)|Bracket(?:Left|Right)|Semicolon|Comma|Period)$/;
export function readInputBindings(keyboard,pad){
 const keys={},buttons={},used=new Set();
 for(const [code,,button]of localInputActions){const n=keyboard?.[code],p=pad?.[code];
  const physical=typeof n==='string'&&allowed.test(n)&&!used.has(n)?n:code;
  if(!used.has(physical)){keys[code]=physical;used.add(physical);}
  buttons[code]=Number.isInteger(p)&&p>=0&&p<=31?p:button;
 }
 return {keyboard:keys,pad:buttons};
}
export function mountLocalBindings({window,document,input,bindings,getActive}){
 const id=Symbol('local-keyboard-remap'),padId=Symbol('local-gamepad'),held=new Set();
 const remapped=Object.entries(bindings.keyboard).filter(([logical,actual])=>logical!==actual);
 const physical=new Map(remapped.map(([a,b])=>[b,a])),displaced=new Set(remapped.map(([logical])=>logical));
 const interactive=event=>event.target?.closest?.('button,input,textarea,select,summary,[contenteditable=true]');
 const apply=()=>input.setKeySource(id,[...held].map(code=>physical.get(code)));
 const down=event=>{
  if(!event.isTrusted||event.metaKey||event.altKey||event.ctrlKey||!getActive()||document.hidden||interactive(event))return;
  if(!physical.has(event.code)&&!displaced.has(event.code))return;
  event.preventDefault();event.stopImmediatePropagation();if(physical.has(event.code)){held.add(event.code);apply();}
 };
 const up=event=>{if(!event.isTrusted||!held.delete(event.code))return;event.preventDefault();event.stopImmediatePropagation();apply();};
 const clear=()=>{held.clear();input.clearKeySource(id);input.clearKeySource(padId);};
 window.addEventListener('keydown',down,true);window.addEventListener('keyup',up,true);window.addEventListener('blur',clear);
 document.addEventListener('visibilitychange',()=>{if(document.hidden)clear();});
 window.addEventListener('pagehide',clear,{once:true});
 return function poll(){
  const codes=new Set();
  if(!getActive()||document.hidden){input.clearKeySource(padId);return;}
  const pads=window.navigator?.getGamepads?.()??[];
  for(const pad of pads){if(!pad?.connected||pad.mapping!=='standard')continue;
   for(const [logical,button]of Object.entries(bindings.pad))if(pad.buttons[button]?.pressed)codes.add(logical);
   const [x=0,y=0]=pad.axes,directions=analogGamepadDirections(x,y);
   for(let i=0;i<4;i++)if(directions&(16<<i))codes.add(localInputActions[i][0]);
  }
  if(codes.size)input.setKeySource(padId,[...codes]);else input.clearKeySource(padId);
 };
}
