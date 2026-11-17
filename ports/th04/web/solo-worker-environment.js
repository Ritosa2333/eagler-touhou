// Minimal SDL browser adapter for a dedicated worker. No DOM is simulated beyond
// the event targets and canvas geometry used by this NP21 build.
export function workerEnvironment(canvas,sampleRate) {
  const listeners=new Map();
  function target(object,name){
    object.addEventListener=(type,fn)=>{const key=name+':'+type;if(!listeners.has(key))listeners.set(key,new Set());listeners.get(key).add(fn);};
    object.removeEventListener=(type,fn)=>listeners.get(name+':'+type)?.delete(fn);
    return object;
  }
  const geometry=()=>({left:0,top:0,width:canvas.width,height:canvas.height});
  Object.assign(canvas,{style:{},id:'canvas',nodeName:'CANVAS',getBoundingClientRect:geometry});
  target(canvas,'canvas');
  const body=target({style:{},appendChild(){},getBoundingClientRect:geometry},'body');
  const document=target({body,documentElement:{style:{}},hidden:false,visibilityState:'visible',activeElement:canvas,
    querySelector:selector=>selector==='#canvas'?canvas:null,
    getElementById:id=>id==='canvas'?canvas:null,
    createElement:tag=>{if(tag==='canvas')return new OffscreenCanvas(1,1);throw Error('Unsupported worker element: '+tag);}},'document');
  globalThis.document=document;
  globalThis.window=target({innerWidth:640,innerHeight:400,scrollX:0,scrollY:0,devicePixelRatio:1},'window');
  globalThis.screen={width:640,height:400};
  let processor;
  globalThis.AudioContext=class {
    sampleRate=sampleRate;state='running';destination={};
    createBuffer(channels,frames){const data=Array.from({length:channels},()=>new Float32Array(frames));return {numberOfChannels:channels,getChannelData:i=>data[i],data};}
    createScriptProcessor(frames,input,channels){
      const node={frames,channels,connect(){processor=node;},disconnect(){if(processor===node)processor=null;}};return node;
    }
    resume(){this.state='running';return Promise.resolve();}
    suspend(){this.state='suspended';return Promise.resolve();}
    close(){processor=null;return Promise.resolve();}
    addEventListener(){} removeEventListener(){}
  };
  return {
    key(message){
      const event={...message,target:canvas,preventDefault(){},stopPropagation(){},timeStamp:performance.now()};
      // SDL registers keyboard callbacks on window in this build. Match the DOM
      // bubbling route, preserving keydown/keyup order across the message port.
      for(const name of ['canvas','document','window'])for(const fn of listeners.get(name+':'+message.type)||[])fn(event);
    },
    audio(){
      if(!processor?.onaudioprocess)return null;
      const buffer=new AudioContext().createBuffer(processor.channels,processor.frames);
      processor.onaudioprocess({outputBuffer:buffer});return buffer.data;
    }
  };
}
