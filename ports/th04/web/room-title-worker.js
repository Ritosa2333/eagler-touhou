// A disposable native instance: no input, recording, saves, or match state.
let emulator, timer, paused = false;
const send = (type, props = {}) => postMessage({type, ...props});
async function boot({game, canvas, disk, wasmBinary, module}) {
  if (game === '01') {
    const core = (await WebAssembly.instantiate(module, {})).exports;
    core.th_init(1,1,0x12345678,0);core.th_menu_open(1,0);core.th_render();
    const context=canvas.getContext('2d',{alpha:false});
    const image=new ImageData(new Uint8ClampedArray(core.memory.buffer,core.th_pixels(),640*400*4),640,400);
    context.putImageData(image,0,0);
    send('ready',{program:'TH01 native title',static:true});
    let frames=0;timer=setInterval(()=>{if(!paused){core.th_render();context.putImageData(image,0,0);send('frame',{frames:++frames,cycles:0,program:'TH01 native title'});}},1000);
    return;
  }
  const dispatch = globalThis.onmessage;
  const {workerEnvironment} = await import('./room-title-environment.js');
  workerEnvironment(canvas, 44100);
  if (game === '05') {
    emulator = await (await import('./room-title-th05.js')).createTitleCore({canvas, disk, wasmBinary});
  } else {
    const {DOSBoxPC98} = await import('./dosbox-adapter.js');
    emulator = await DOSBoxPC98.create({canvas, wasmBinary, lockstep: true,
      rollbackEnabled: false, workerRuntime: true, externalEffects: true, directLaunch:false});
    emulator.addDiskImage('title', disk);
    await emulator.setHdd(0, 'title');
  }
  globalThis.onmessage = dispatch;
  emulator.run();
  const m = emulator.module;
  m.warmingBoot = true; // Suppress PCM collection as well as playback.
  const sample = () => {
    const surface=m.framePresenter.capture();
    if(!surface)return {colored:0,lit:0,hash:0};
    const {frame,frameWidth,frameHeight}=surface,h=m.HEAPU8;
    let colored=0,lit=0,samples=0,hash=2166136261;
    for(let at=frame;at<frame+frameWidth*frameHeight*3;at+=48){
      const r=h[at],g=h[at+1],b=h[at+2];
      if(Math.max(r,g,b)-Math.min(r,g,b)>32)colored++;
      if(Math.max(r,g,b)>64)lit++;
      hash=Math.imul(hash^r,16777619);hash=Math.imul(hash^g,16777619);hash=Math.imul(hash^b,16777619);samples++;
    }
    return {colored:colored/samples,lit:lit/samples,hash:hash>>>0};
  };
  let opFrames = 0, frames = 0, graphics = false, pixels = sample();
  const settle = ['02','04'].includes(game) ? 240 : 720;
  // Boot offscreen without exposing DOS, logo transitions, or a partial fade.
  while (opFrames < settle || pixels.colored < .08 || pixels.lit < .12) {
    for (let i = 0; i < 30; i++) {
      emulator.step(); frames++;
      const program = m.UTF8ToString(m._emGetRunningProgram());
      if (program.toUpperCase() === 'OP' && graphics) opFrames++;
      else opFrames = 0;
      if (frames > 18000) throw Error('标题启动超时');
    }
    pixels = sample();
    if (pixels.colored > .08) graphics = true;
    await new Promise(resolve => setTimeout(resolve, 0));
  }
  m.netPresent();
  let presented = 0;
  send('ready', {frames, pixels, program: m.UTF8ToString(m._emGetRunningProgram())});
  timer = setInterval(() => {
    if (paused) return;
    try {
      // Let the original title, fades and demo playback advance continuously.
      // This isolated worker is discarded before the match starts.
      for (let i = 0; i < 2; i++) emulator.step();
      pixels = sample();
      m.netPresent(); presented++;
      if (presented % 30 === 0) send('frame', {frames: emulator.tick, pixels,
        program:m.UTF8ToString(m._emGetRunningProgram())});
    } catch (error) { clearInterval(timer); send('error', {message: error.message}); }
  }, 1000 / 30);
}
onmessage = ({data}) => {
  if (data.type === 'boot') boot(data).catch(error => send('error', {message: error.message}));
  if (data.type === 'pause') paused = data.value;
};
