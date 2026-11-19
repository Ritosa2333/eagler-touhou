// Only the SDL event targets needed during native startup. No user input route.
export function workerEnvironment(canvas, sampleRate = 44100) {
  const target = value => Object.assign(value, {addEventListener(){}, removeEventListener(){}});
  const geometry = () => ({left:0, top:0, width:640, height:400});
  Object.assign(canvas, {style:{}, id:'canvas', nodeName:'CANVAS', getBoundingClientRect:geometry});
  target(canvas);
  const body = target({style:{}, appendChild(){}, getBoundingClientRect:geometry});
  globalThis.document = target({body, documentElement:{style:{}}, hidden:false,
    visibilityState:'visible', activeElement:canvas,
    querySelector:selector => selector === '#canvas' ? canvas : null,
    getElementById:id => id === 'canvas' ? canvas : null,
    createElement:tag => { if(tag === 'canvas') return new OffscreenCanvas(1,1); throw Error('Unexpected title element'); }});
  globalThis.window = target({innerWidth:640, innerHeight:400, scrollX:0, scrollY:0, devicePixelRatio:1});
  globalThis.screen = {width:640, height:400};
  globalThis.AudioContext = class {
    sampleRate = sampleRate; state = 'running'; destination = {};
    createBuffer(channels, frames) { const data = Array.from({length:channels}, () => new Float32Array(frames)); return {numberOfChannels:channels, getChannelData:i=>data[i]}; }
    createScriptProcessor() { return {connect(){}, disconnect(){}}; }
    resume(){this.state='running';return Promise.resolve();}
    suspend(){this.state='suspended';return Promise.resolve();}
    close(){this.state='closed';return Promise.resolve();}
    addEventListener(){} removeEventListener(){}
  };
}
