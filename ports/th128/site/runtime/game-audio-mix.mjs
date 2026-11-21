const mixes=new WeakMap();
export function bindGameAudioMix(context,effects,{musicVolume,effectsVolume}){
 if(effects.context&&effects.context!==context)throw Error('Game audio output has a foreign owner');
 if(mixes.has(effects))throw Error('Game audio output is already configured');
 const bgm=context.createGain();bgm.connect(context.destination);
 const mix={context,bgm,effects};mixes.set(effects,mix);applyGameAudioMix(effects,{musicVolume,effectsVolume});return mix;
}
export function applyGameAudioMix(effects,{musicVolume,effectsVolume}){
 const found=mixes.get(effects);if(!found)return false;
 if(![musicVolume,effectsVolume].every(n=>Number.isFinite(n)&&n>=0&&n<=100))throw Error('Invalid game audio gain');
 found.bgm.gain.value=musicVolume/100;effects.gain.value=effectsVolume/100;return true;
}
export function musicMixDestination(context,effects){const mix=mixes.get(effects);return mix?.context===context?mix.bgm:effects;}
