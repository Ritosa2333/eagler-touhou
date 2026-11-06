export class PmdPlayer {
  static async prepare(context,base){
    const [module]=await Promise.all([
      WebAssembly.compileStreaming(fetch(new URL('pmd-player.wasm',base))),
      context.audioWorklet.addModule(new URL('pmd-worklet.js',base)),
    ]);
    return new PmdPlayer(context,module);
  }
  constructor(context,module){this.context=context;this.module=module;this.prepared=new Map();}
  prewarm(bytes){
    if(this.prepared.has(bytes))return;
    const pending=this.loadSource(bytes);this.prepared.set(bytes,pending);
    pending.catch(()=>{if(this.prepared.get(bytes)===pending)this.prepared.delete(bytes);});
    while(this.prepared.size>2){const [key,old]=this.prepared.entries().next().value;this.prepared.delete(key);old.then(source=>source.stop(this.context.currentTime),()=>{});}
  }
  createSource(bytes){
    const pending=this.prepared.get(bytes);
    if(pending){this.prepared.delete(bytes);return pending;}
    return this.loadSource(bytes);
  }
  dispose(){for(const pending of this.prepared.values())pending.then(source=>source.stop(this.context.currentTime),()=>{});this.prepared.clear();}
  async loadSource(bytes){
    const node=new AudioWorkletNode(this.context,'pc98-pmd',{numberOfInputs:0,numberOfOutputs:1,outputChannelCount:[2],processorOptions:{module:this.module}});
    await new Promise((resolve,reject)=>{
      node.port.onmessage=({data})=>{if(data.type==='loaded')data.ok?resolve():reject(Error('原始 PMD 曲目无法播放'));};
      node.port.postMessage({type:'load',bytes});
    });
    let onended;
    return {
      connect(target){node.connect(target);},
      disconnect(){node.disconnect();},
      start(){node.port.postMessage({type:'start'});},
      stop(at){const delay=Math.max(0,(at-node.context.currentTime)*1000);setTimeout(()=>{node.port.postMessage({type:'stop'});node.disconnect();onended?.();},delay);},
      set onended(callback){onended=callback;node.onprocessorerror=callback;},
    };
  }
}
