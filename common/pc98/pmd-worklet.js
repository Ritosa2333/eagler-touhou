class Pc98PmdProcessor extends AudioWorkletProcessor {
  constructor(options){
    super();
    this.wasm=new WebAssembly.Instance(options.processorOptions.module,{}).exports;
    this.player=this.wasm.pmd_create(sampleRate);
    this.loaded=false;
    this.port.onmessage=({data})=>{
      if(data.type==='load'){
        const bytes=new Uint8Array(data.bytes),at=this.wasm.pmd_input(this.player,bytes.length);
        new Uint8Array(this.wasm.memory.buffer,at,bytes.length).set(bytes);
        this.loaded=this.wasm.pmd_load(this.player);
        this.port.postMessage({type:'loaded',ok:this.loaded});
      }else if(data.type==='stop'){
        this.wasm.pmd_destroy(this.player);this.player=0;this.loaded=false;
      }
    };
  }
  process(_inputs,outputs){
    if(!this.player)return false;
    const channels=outputs[0];
    if(!this.loaded)return true;
    const frames=channels[0].length,at=this.wasm.pmd_render(this.player,frames);
    const samples=new Float32Array(this.wasm.memory.buffer,at,frames*2);
    for(let i=0;i<frames;i++){channels[0][i]=samples[2*i];channels[1][i]=samples[2*i+1];}
    return true;
  }
}
registerProcessor('pc98-pmd',Pc98PmdProcessor);
