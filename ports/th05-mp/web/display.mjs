export const displayDefaults=Object.freeze({bossBar:'native',phaseMarkers:false,phaseStars:false,phaseTimeout:false,timeoutAsTime:false,numericDream:false,numericPower:false});
export function displayOptions(value={}){
 if(!value||typeof value!=='object'||Array.isArray(value))throw Error('Invalid TH05 multiplayer display');
 for(const key of Object.keys(value))if(!Object.hasOwn(displayDefaults,key))throw Error('Unknown TH05 display setting: '+key);
 for(const key of Object.keys(displayDefaults))if(key!=='bossBar'&&value[key]!==undefined&&typeof value[key]!=='boolean')throw Error('Invalid TH05 display setting: '+key);
 if(value.bossBar!==undefined&&!['native','eosd','ring'].includes(value.bossBar))throw Error('Invalid TH05 boss bar');
 const result={...displayDefaults,...value};
 result.phaseMarkers=result.bossBar==='native'&&result.phaseMarkers;
 result.phaseStars=result.bossBar==='eosd'&&result.phaseStars;
 result.phaseTimeout=result.bossBar!=='native'||result.phaseTimeout;
 return Object.freeze(result);
}
export function applyDisplay(module,value){
 const v=displayOptions(value),style=['native','eosd','ring'].indexOf(v.bossBar);
 const flags=['phaseMarkers','phaseStars','phaseTimeout','timeoutAsTime','numericDream','numericPower'].reduce((bits,key,i)=>bits|(+v[key]<<i),0);
 module._th05_coop_display(style,flags);
}
