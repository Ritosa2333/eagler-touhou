import {getPageLaunchContext} from './page-launch-context.mjs';
import {localPlayerLaunch} from './local-player-settings.mjs';
import {musicMixDestination} from './game-audio-mix.mjs';
// Session BGM output policy; SFX and original native timing stay independent.
const mutedDestinations=new WeakMap();
const selectedModes=new WeakMap();
function selectedMode(){
 const owner=getPageLaunchContext();if(!owner)return localPlayerLaunch()?.music??'ogg';
 if(!selectedModes.has(owner))selectedModes.set(owner,owner.config.music??'ogg');
 return selectedModes.get(owner);
}
export function musicOutputEnabled(mode=selectedMode()){
 if(!['ogg','none'].includes(mode))throw new Error('Unsupported music output mode');
 return mode==='ogg';
}
export function musicOutput(context,destination,mode=selectedMode()){
 if(musicOutputEnabled(mode))return musicMixDestination(context,destination);
 if(!context||typeof context.createGain!=='function'||!destination||typeof destination!=='object'
  ||destination.context&&destination.context!==context)throw new Error('Music output context/destination unavailable');
 const cached=mutedDestinations.get(destination);
 if(cached){if(cached.context!==context)throw new Error('Music output context changed');return cached.output;}
 const output=context.createGain();output.gain.value=0;output.connect(destination);
 mutedDestinations.set(destination,{context,output});return output;
}
