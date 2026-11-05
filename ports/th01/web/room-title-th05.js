// TH05's existing deterministic core, booted with the original solo OP batch.
// Display RGB buffers on a separate 2D canvas; the native GL canvas stays private.
export async function createTitleCore({canvas, disk, wasmBinary}) {
  const {workerEnvironment} = await import('./room-title-environment.js');
  const nativeCanvas = new OffscreenCanvas(640,400);
  workerEnvironment(nativeCanvas);
  const dispatch = globalThis.onmessage;
  await import('./vendor/dosbox-net/emulators.js');
  const {default:factory} = await import('./vendor/dosbox-net/dosbox-rollback.mjs');
  globalThis.onmessage = dispatch;
  const {gameFiles} = await import('./disk-files.js');
  const files = gameFiles(disk, {solo:true});
  const batch = files.find(file => file.path === 'game/KAIKI/FAST.BAT');
  batch.contents = new TextEncoder().encode(new TextDecoder().decode(batch.contents).replace('\r\nop\r\n', '\r\nz:\\system\\config.com -set "cpu cycles=30000"\r\nop\r\n'));
  const context = canvas.getContext('2d', {alpha:false});
  let surface, image, receiver, tick = 0;
  const messages = [], vendor = new URL('./vendor/dosbox-net/', import.meta.url);
  const m = {canvas:nativeCanvas, wasmBinary, virtualDate:Date, virtualNow:0, targetNow:0,
    deterministic:true, nativeScheduling:true, nativeCpu:true, nativeFramePresentation:false,
    SDL2:{audioContext:{sampleRate:44100}},
    locateFile:name=>new URL(name==='wdosbox-x-jspi.wasm'?'wdosbox-x-rollback.wasm':name, vendor).href,
    postMessage:(name,props)=>receiver?receiver(name,props):messages.push([name,props]),
    onAbort:error=>{throw Error(String(error));}};
  const present = () => {
    if (!surface) return false;
    const {frame, frameWidth:w, frameHeight:h} = surface;
    if (!image || image.width !== w || image.height !== h) { canvas.width=w;canvas.height=h;image=new ImageData(w,h); }
    const rgba=image.data, rgb=m.HEAPU8;
    for(let i=0,p=frame;i<rgba.length;i+=4,p+=3){rgba[i]=rgb[p];rgba[i+1]=rgb[p+1];rgba[i+2]=rgb[p+2];rgba[i+3]=255;}
    context.putImageData(image,0,0); return true;
  };
  m.installFramePresenter = () => {
    m.updateTexture = (frame,frameWidth,frameHeight) => {surface={frame,frameWidth,frameHeight};};
    m.framePresenter = {capture:()=>surface?{...surface}:null, restore:value=>{surface=value;},
      info:()=>({mode:'native-rgb'}), dispose(){}, flush:present};
  };
  const transport = {sessionId:'th05-room-title', module:m, net:null, exit(){},
    sendMessageToServer:(name,props)=>m.messageHandler({data:{name,props}}),
    initMessageHandler:handler=>{receiver=handler;for(const args of messages)handler(...args);messages.length=0;}};
  await factory(m); m.callMain([transport.sessionId]);
  const dosboxConf='[dosbox]\nmachine=pc98\nmemsize=16\n[cpu]\ncore=simple\ncputype=386\ncycles=1000\n[render]\nframeskip=0\nscaler=none\naspect=false\n[sblaster]\nsbtype=none\n[pc98]\npc-98 fm board=board86\n'+(files.some(f=>f.path==='game/CNFONT.BMP')?'pc-98 try font rom=false\npc-98 anex86 font=/home/web_user/game/CNFONT.BMP\n':'')+'[mixer]\nrate=44100\n[autoexec]\n@echo off\nmount a /home/web_user/game\na:\ncd KAIKI\nFAST.BAT\n';
  const ci=await globalThis.emulators.backend([...files,{dosboxConf,jsdosConf:{version:'8'}}],transport,{});
  ci.events().onSoundPush(()=>{});
  const emulator={module:m, ci, keys:new Set(), get tick(){return tick;},set tick(value){tick=value;},run(){},
    step(){m.targetNow=Math.max(m.virtualNow,Math.floor(++tick*1000/60));let safety=0;while(m.wakeAt<=m.targetNow){if(++safety>1000)throw Error('标题模拟未让出执行');m.virtualNow=Math.max(m.virtualNow,m.wakeAt);m.advanceNative();}m.virtualNow=m.targetNow;}};
  m.netPresent=present;
  return emulator;
}
