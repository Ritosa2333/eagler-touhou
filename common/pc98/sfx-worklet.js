class OriginalEffectsOutput extends AudioWorkletProcessor {
  constructor(){super();this.chunks=[];this.offset=0;this.samples=0;this.peak=0;this.reportFrames=0;this.port.onmessage=({data})=>{
    const samples=new Float32Array(data);this.chunks.push(samples);this.samples+=samples.length;
    while(this.samples>sampleRate*.12&&this.chunks.length>1){this.samples-=this.chunks.shift().length-this.offset;this.offset=0;}
  };}
  process(_inputs,outputs){
    const out=outputs[0][0];let at=0;
    while(at<out.length&&this.chunks.length){
      const head=this.chunks[0],count=Math.min(out.length-at,head.length-this.offset);
      out.set(head.subarray(this.offset,this.offset+count),at);this.offset+=count;this.samples-=count;at+=count;
      if(this.offset===head.length){this.chunks.shift();this.offset=0;}
    }
    for(const value of out)this.peak=Math.max(this.peak,Math.abs(value));
    this.reportFrames+=out.length;
    if(this.reportFrames>=sampleRate/4){this.port.postMessage({peak:this.peak});this.peak=0;this.reportFrames=0;}
    return true;
  }
}
registerProcessor('pc98-original-effects',OriginalEffectsOutput);
