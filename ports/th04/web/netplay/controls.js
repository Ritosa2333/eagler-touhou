// Each browser owns one seat. Physical devices never address P1/P2 directly.
import {padBits} from '../control-settings.js';
export {keyboardBits,isBound} from '../control-settings.js';
export function normalize(bits){
  if((bits&3)===3)bits&=~3;
  if((bits&12)===12)bits&=~12;
  return bits;
}
export let gamepadStatus='手柄：连接后按一下按钮';
export function gamepadBits(){
  if(document.hidden||!document.hasFocus())return 0;
  if(!window.isSecureContext||!navigator.getGamepads){
    gamepadStatus='手柄不可用：浏览器需要安全上下文；局域网 HTTP 的设置方法见 LAN-试玩说明。键盘仍可使用。';return 0;
  }
  let pads;
  try{pads=Array.from(navigator.getGamepads()).filter(p=>p?.connected);}
  catch{gamepadStatus='手柄访问被浏览器阻止，请检查此页面的手柄权限。键盘仍可使用。';return 0;}
  const pad=pads.find(p=>p.mapping==='standard');
  if(!pad){gamepadStatus=pads.length?'手柄未提供标准映射，请切换至 XInput 模式':'未检测到手柄：连接后按一下按钮';return 0;}
  gamepadStatus=`手柄：${pad.id}`;
  return padBits(pad);
}
