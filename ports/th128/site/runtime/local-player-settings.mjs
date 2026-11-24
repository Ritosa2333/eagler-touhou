import {touchOptions,TOUCH_OPTIONS_KEY} from './touch-options.mjs';
import {readInputBindings} from './local-input-bindings.mjs';
const KEY='th128-local-player-settings-v1';
const unit=value=>Number.isInteger(value)&&value>=0&&value<=100;
const langs=['zh','ja','en'],music=['ogg','none'];
const old=(window,key)=>{try{return window.sessionStorage.getItem('th128.'+key);}catch{return null;}};
function hardwareTouch(window){return !!window.matchMedia?.('(pointer: coarse)').matches||Number(window.navigator?.maxTouchPoints)>0;}
export function readLocalPlayerSettings(window){
 const read=key=>{try{return JSON.parse(window.localStorage.getItem(key)||'{}')??{};}catch{return {};}};
 const saved=read(KEY),prior=read(TOUCH_OPTIONS_KEY),practice=read('th128.owned-thprac-options.v1');
 const raw=old(window,'volume'),base=raw!==null&&unit(Number(raw))?Number(raw):35;
 let touch;try{touch=touchOptions({touchEnabled:hardwareTouch(window),...prior,...saved.touch});}catch{touch=touchOptions({touchEnabled:hardwareTouch(window)});}
 const language=saved.language??old(window,'variant');
 return {language:langs.includes(language)?language:'zh',music:music.includes(saved.music)?saved.music:'ogg',
  musicVolume:unit(saved.musicVolume)?saved.musicVolume:base,effectsVolume:unit(saved.effectsVolume)?saved.effectsVolume:base,
  highRefresh:typeof saved.highRefresh==='boolean'?saved.highRefresh:null,
  thprac:typeof saved.thprac==='boolean'?saved.thprac:typeof practice.enabled==='boolean'?practice.enabled:null,touch,layoutZoom:saved.layoutZoom!==false,openLayout:saved.openLayout===true,
  pixelPerfect:saved.pixelPerfect===true,bindings:readInputBindings(saved.bindings?.keyboard,saved.bindings?.pad)};
}
export function writeLocalPlayerSettings(window,values){
 const settings={...readLocalPlayerSettings(window),...values};
 try{const {touch,...sheet}=settings;if(Object.hasOwn(values,'touch'))window.localStorage.setItem(TOUCH_OPTIONS_KEY,JSON.stringify(touch));window.localStorage.setItem(KEY,JSON.stringify(sheet));}catch{}
 return settings;
}
let bootstrap=null;
export function configureLocalPlayerLaunch(settings){if(bootstrap)throw Error('Already configured local runtime');bootstrap=Object.freeze({...settings,touch:Object.freeze({...settings.touch})});}
export function localPlayerLaunch(){return bootstrap;}
