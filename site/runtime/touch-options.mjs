export const TOUCH_OPTIONS_KEY='th128-owned-touch-options-v1';
export const TOUCH_DEFAULTS=Object.freeze({touchEnabled:false,touchMovementMode:'touch',touchSensitivity:150,touchFocusMode:'two-finger',doubleTapBombEnabled:false,restartButtonEnabled:true,thpracTouchControlsEnabled:true,touchControlOpacity:80});
export function touchOptions(value={},previous=TOUCH_DEFAULTS){
 if(!value||typeof value!=='object'||Array.isArray(value))throw TypeError('Invalid touch options');
 const next={...previous};for(const key of Object.keys(TOUCH_DEFAULTS))if(Object.hasOwn(value,key))next[key]=value[key];
 for(const key of ['touchEnabled','doubleTapBombEnabled','restartButtonEnabled','thpracTouchControlsEnabled'])if(typeof next[key]!=='boolean')throw TypeError('Invalid touch boolean '+key);
 if(!['touch','touch-unlimited','joystick','joystick-free'].includes(next.touchMovementMode)||!['hold-button','toggle-button','two-finger'].includes(next.touchFocusMode))throw TypeError('Invalid touch mode');
 for(const[key,min,max]of[['touchSensitivity',100,300],['touchControlOpacity',20,100]])if(!Number.isInteger(next[key])||next[key]<min||next[key]>max)throw TypeError('Invalid touch option '+key);
 return Object.freeze(next);
}
export function touchStorage(storage){return storage&&{getItem:key=>storage.getItem('th128-owned-'+key),setItem:(key,value)=>storage.setItem('th128-owned-'+key,value),removeItem:key=>storage.removeItem('th128-owned-'+key)};}
