// Exact integers below 2^45 survive JSON, history comparison and retransmission.
// Low 12 bits retain the existing buttons; two signed 14-bit axes use 1/16 px.
export const ALWAYS_POINT=2**42;
export const MAX_INPUT=2**45-1;
export const TOUCH_BEGIN=2**44;
export function packTouch(buttons,{x=0,y=0,active=false,unlimited=false,alwaysPoint=false,begin=false}={}){
  const axis=v=>Math.max(-8192,Math.min(8191,Math.round(v)))&16383;
  return (active&&begin?TOUCH_BEGIN:0)+(alwaysPoint?ALWAYS_POINT:0)+(buttons&4095)+(active?axis(x)*4096+axis(y)*2**26+2**40+(unlimited?2**41:0):0);
}
export function unpackTouch(value){
  const signed=v=>v>=8192?v-16384:v;
  return {x:signed(Math.floor(value/4096)%16384),y:signed(Math.floor(value/2**26)%16384),
    ...(Math.floor(value/TOUCH_BEGIN)%2===1?{begin:true}:{}),active:Math.floor(value/2**40)%2===1,unlimited:Math.floor(value/2**41)%2===1};
}
