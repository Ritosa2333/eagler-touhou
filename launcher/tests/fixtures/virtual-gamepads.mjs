// Synthetic browser layouts, not recordings or hardware compatibility claims.
export const virtualGamepadLayouts={
  xbox:{label:'Xbox-style standard',id:'Virtual XInput Controller',mapping:'standard',buttons:17,axes:[0,0,0,0]},
  dualsense:{label:'DualSense-style standard',id:'Virtual Wireless Controller',mapping:'standard',buttons:18,axes:[0,0,0,0]},
  pro:{label:'Pro-style standard',id:'Virtual Pro Controller',mapping:'standard',buttons:16,axes:[0,0,0,0]},
  arcade:{label:'Raw arcade stick',id:'Virtual USB Arcade Stick',mapping:'',buttons:8,axes:[0,0]},
  axisTriggers:{label:'Raw axes with resting-negative triggers',id:'Virtual Generic USB Gamepad',mapping:'',buttons:12,axes:[0,0,-1,0,0,-1]},
  buttonOnly:{label:'Raw button-only controller',id:'Virtual Digital Controller',mapping:'',buttons:16,axes:[]},
  compact:{label:'Raw compact controller',id:'Virtual Compact Gamepad',mapping:'',buttons:4,axes:[0,0]},
};
export function virtualGamepad(layout,index=0){
  const type=typeof layout==='string'?virtualGamepadLayouts[layout]:layout;
  return {id:type.id,index,connected:true,mapping:type.mapping,timestamp:0,buttons:Array.from({length:type.buttons},()=>({pressed:false,touched:false,value:0})),axes:[...type.axes],hapticActuators:[]};
}
export function virtualButton(pad,index,value){
  pad.buttons[index]={pressed:value>=.5,touched:value>0,value};pad.timestamp++;
}
export function virtualNeutral(pad,layout){
  const type=typeof layout==='string'?virtualGamepadLayouts[layout]:layout;
  pad.buttons.forEach((_,n)=>virtualButton(pad,n,0));pad.axes=[...type.axes];pad.timestamp++;
}
