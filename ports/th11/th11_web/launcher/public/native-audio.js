// Unlock the native SDL audio device during the Launcher user gesture. The
// same-origin child receives this context; there is one mixer and one device.
function unlockNativeAudio(){try{if(!window.__touhouAudioContext||window.__touhouAudioContext.state==='closed')window.__touhouAudioContext=new (window.AudioContext||window.webkitAudioContext)({sampleRate:44100,latencyHint:'interactive'});void window.__touhouAudioContext.resume();}catch(error){console.warn(error);}}
window.addEventListener('pointerdown',unlockNativeAudio,{capture:true});
window.addEventListener('keydown',unlockNativeAudio,{capture:true});
