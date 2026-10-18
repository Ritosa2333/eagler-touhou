// Thin host bridge only. Every thprac window, hotkey and state transition is
// rendered and owned by the native Dear ImGui path shared with TH06/TH07.
import {normalizePractice,fields,defaults} from './practice-config.mjs';

export function createPractice({core,getApp,canvas}){
 let options={},params={...defaults},keyboardBits=0;
 const thpracBit=code=>code==='Backspace'?1:code==='Tab'?1<<8:code==='F12'?1<<9:/^F[1-7]$/.test(code)?1<<Number(code.slice(1)):0;
 function publishKeys(){const module=window.Module;if(!module)return;(module.eaglerControls??={}).thpracKeyboardBits=keyboardBits;}
 function send(){
  const app=getApp();if(!app)return;const values=fields.map(key=>Number(params[key]));values.push(1);
  const pointer=core.graphics_allocate(values.length*8);
  try{new Float64Array(core.memory.buffer,pointer,values.length).set(values);core.practice_configure(app,pointer,values.length,false);}
  finally{core.graphics_free(pointer);}
 }
 return {
  configure(value){options=value||{};params=normalizePractice(options.thprac?.params||options.thpracSession?.params||params);const app=getApp();if(app){core.practice_enable(app,!!options.thpracEnabled);if(options.thpracEnabled)send();}},
  tick(){},
  key(code,down){const bit=thpracBit(String(code));if(!bit)return false;if(down)keyboardBits|=bit;else keyboardBits&=~bit;publishKeys();return true;},
  clear(){keyboardBits=0;publishKeys();},
  mouse(message){
   if(!options.thpracEnabled||!getApp()||typeof core.sdl_thprac_mouse!=='function')return;
   const rect=canvas.getBoundingClientRect(),scale=Math.min(rect.width/640,rect.height/480),renderWidth=640*scale,renderHeight=480*scale,left=rect.left+(rect.width-renderWidth)/2,top=rect.top+(rect.height-renderHeight)/2;
   const x=(Number(message.x)-left)/scale,y=(Number(message.y)-top)/scale,type=message.type==='down'?1:message.type==='up'?2:0;core.sdl_thprac_mouse(type,x,y);
  },
  close(){keyboardBits=0;publishKeys();window.dispatchEvent(new CustomEvent('eagler-thprac-menu',{detail:{open:false}}));}
 };
}
