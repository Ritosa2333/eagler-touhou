import fs from 'node:fs';
import assert from 'node:assert/strict';
import {mountWebInput} from '../../site/runtime/web-input.mjs';
import {environment} from './surface.mjs';
export const source=fs.readFileSync(new URL('../../site/play.mjs',import.meta.url),'utf8');
export function functionSource(name){
 const start=source.indexOf('function '+name+'(');assert(start>=0,'Missing production function '+name);
 const open=source.indexOf('{',start);let depth=0,quote=null,escape=false,line=false,block=false;
 for(let i=open;i<source.length;i++){const c=source[i],n=source[i+1];
  if(line){if(c==='\n')line=false;continue;}if(block){if(c==='*'&&n==='/'){block=false;i++;}continue;}
  if(quote){if(escape){escape=false;continue;}if(c==='\\'){escape=true;continue;}if(c===quote)quote=null;continue;}
  if(c==='/'&&n==='/'){line=true;i++;continue;}if(c==='/'&&n==='*'){block=true;i++;continue;}
  if(c==='"'||c==="'"||c==='\x60'){quote=c;continue;}
  if(c==='{')depth++;else if(c==='}'&&--depth===0)return source.slice(start,i+1);
 }
 throw Error('Unterminated '+name);
}
export function bootPlay({worldMode=true,capability=true}={}){
 const env=environment(),receipts=[],menus=[],postrun=[],pause=[];
 const state={worldMode,shotMode:true,active:true,paused:false,handoff:false,pressed:new Set(),action:1,stageSelection:{stage:1},controller:null,
  module:{_th128_world_input(held,pressed,released){receipts.push([held,pressed,released]);return 1;},UTF8ToString:x=>x},audio:null};
 const menu={page:'main',launchId:1,pauseOpen:false,fault:false,launching:false,audioUnlocked:true},suppressedKeys=new Set();
 const movementKeys=new Map([['ArrowUp',16],['ArrowDown',32],['ArrowLeft',64],['ArrowRight',128],['KeyZ',1],['ShiftLeft',8],['ShiftRight',8]]);
 const find=s=>s==='#game'?env.canvas:{hidden:true};
 const openPauseMenu=()=>{pause.push('pause');menu.pauseOpen=true;state.paused=true;state.nativePause={held:0,pressed:0,keyUp:code=>pause.push(['up',code]),suspend:async()=>{}};};
 const context={...env,state,menu,suppressedKeys,movementKeys,perfectFreezeXBit:2,mountWebInput,find,HTMLSelectElement:class {},HTMLInputElement:class {},
  getPageLaunchContext:()=>null,flowCapabilities:()=>({perfectFreezeNormalEmptyLasers:capability}),status(){},clock:{reset(){}},resetHighFrameratePresentation(){},openPauseMenu,
  error(){},handleFaultKey:code=>menus.push(['fault',code]),handlePauseKey:code=>menus.push(['pause',code]),handleMenuKey:code=>menus.push(['menu',code]),
  unlockMenuAudio(){},nativePageNames:['main','difficulty','route','replay','options'],handleNativeTitleKey:code=>menus.push(['native',code]),
  resumeTitlePause(){state.paused=false;},resumeNormalPostrun(){},pauseNormalPostrun(){},pauseOriginalEnding(){},cancelLaunch(){},
  stopOwnedReplayToList(){},cancelOriginalEnding(){},stopAudio(){},document:env.document,window:env.window};
 const start=source.indexOf("window.addEventListener('keydown',event=>{"),end=source.indexOf('\nasync function present(',start);
 assert(start>=0&&end>start);
 const functions=['worldMovementInput','supportsPerfectFreezeX','gameplayXHeld','gameplayXPress','gameplayXRelease','gameplayInputMask','installWebInput'];
 const code=functions.map(functionSource).join('\n')+'\n'+source.slice(start,end);
 const names=Object.keys(context),make=new Function(...names,code+'\nreturn {installWebInput,loseFocus};');
 const api=make(...names.map(n=>context[n]));api.installWebInput();
 const input=state.webInput;assert(input,'production install must own the touch layer');
 const button=action=>env.find(input.layer,'data-web-action',action),pad=env.find(input.layer,'data-web-drag','1');
 return {...env,state,menu,receipts,menus,postrun,pause,input,button,pad,loseFocus:api.loseFocus,suppressedKeys};
}
