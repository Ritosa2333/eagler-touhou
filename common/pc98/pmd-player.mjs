export class PmdPlayer {
  static async prepare(context,base){
    const [module]=await Promise.all([
      WebAssembly.compileStreaming(fetch(new URL('pmd-player.wasm',base))),
      context.audioWorklet.addModule(new URL('pmd-worklet.js',base)),
    ]);
    return new PmdPlayer(context,module);
  }
  constructor(context,module){this.context=context;this.module=module;}
  async createSource(bytes){
    const node=new AudioWorkletNode(this.context,'pc98-pmd',{numberOfInputs:0,numberOfOutputs:1,outputChannelCount:[2],processorOptions:{module:this.module}});
    await new Promise((resolve,reject)=>{
      node.port.onmessage=({data})=>{if(data.type==='loaded')data.ok?resolve():reject(Error('原始 PMD 曲目无法播放'));};
      node.port.postMessage({type:'load',bytes});
    });
    let onended;
    return {
      connect(target){node.connect(target);},
      disconnect(){node.disconnect();},
      start(){},
      stop(at){const delay=Math.max(0,(at-node.context.currentTime)*1000);setTimeout(()=>{node.port.postMessage({type:'stop'});node.disconnect();onended?.();},delay);},
      set onended(callback){onended=callback;node.onprocessorerror=callback;},
    };
  }
}
