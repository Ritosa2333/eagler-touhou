// Unlock the native SDL audio device during the Launcher user gesture. The
// same-origin child receives this context; there is one mixer and one device.
function resumeNativeAudio(){const context=window.__touhouAudioContext;if(context&&context.state!=='running')void context.resume().catch(error=>console.warn(error));}
function unlockNativeAudio(){try{if(!window.__touhouAudioContext)window.__touhouAudioContext=new (window.AudioContext||window.webkitAudioContext)({sampleRate:44100,latencyHint:'interactive'});resumeNativeAudio();}catch(error){console.warn(error);}}
window.addEventListener('pointerdown',unlockNativeAudio,{capture:true});
window.addEventListener('keydown',unlockNativeAudio,{capture:true});
window.addEventListener('focus',resumeNativeAudio);
window.addEventListener('pageshow',resumeNativeAudio);
document.addEventListener('visibilitychange',()=>{if(!document.hidden)resumeNativeAudio();});
