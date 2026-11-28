// Explicit WebAudio graph seam; no physical audio/device or native World.
export function graph(){
 const nodes=[],sources=[],context={state:'running',currentTime:0};
 const node=kind=>{const n={kind,context,targets:[],connect(to){this.targets.push(to);return to;},disconnect(){this.targets=[];this.disconnected=true;}};nodes.push(n);return n;};
 context.createGain=()=>{const n=node('gain');n.gain={value:1,cancelScheduledValues(){},setValueAtTime(v){this.value=v;},exponentialRampToValueAtTime(v){this.value=v;}};return n;};
 context.createBufferSource=()=>{const n=node('source');Object.assign(n,{start(){this.started=true;},stop(at){this.stopAt=at;this.stopped=true;}});sources.push(n);return n;};
 context.createChannelMerger=()=>node('merger');context.createChannelSplitter=()=>node('splitter');
 context.destination=node('destination');context.resume=async()=>{context.state='running';};context.close=async()=>{context.state='closed';};
 const master=context.createGain();master.gain.value=.95;master.connect(context.destination);
 function level(n,seen=new Set()){if(n===context.destination)return 1;if(seen.has(n))return 0;const next=new Set(seen);next.add(n);return(n.kind==='gain'?n.gain.value:1)*n.targets.reduce((sum,t)=>sum+level(t,next),0);}
 return{context,master,nodes,sources,level};
}
export const track={name:'th128_07.wav',file:'music/ogg/th128_07.ogg',loopStart:0,loopEnd:1,sampleRate:44100,channels:2};
export const buffer={duration:1,sampleRate:44100,numberOfChannels:1,length:44100};
