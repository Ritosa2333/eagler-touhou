import {parseReplay} from './replay.mjs';

// The Launcher owns the controls; this owner runs only recorded logical ticks.
// Seeking restarts the selected native stage, never patches clocks or RNG.
export class ReplayWorkspace {
  constructor(api){
    this.api=api;this.replay=null;this.path='';this.parts=[];this.startIndex=0;
    this.paused=true;this.rate=1;this.target=null;this.preparing=false;
    this.ended=false;this.credit=0;this.reportedAt=-Infinity;this.pending=null;this.pulseQueued=false;this.channel=null;
  }
  async inspect(path){
    const bytes=await this.api.read(path);if(!bytes)throw Error('录像文件不存在');
    const replay=parseReplay(bytes);return {replay,description:this.describe(replay)};
  }
  describe(replay){
    const timestamp=replay.timestamp,date=timestamp>0&&Number.isFinite(new Date(timestamp*1000).getTime())?new Date(timestamp*1000).toISOString():null;
    return {name:replay.name,character:replay.character,shot:replay.shot,difficulty:replay.difficulty,
      ...(date?{date,timestamp}:{}),score:replay.scoreStored*10+replay.continues,
      slowdown:replay.slowdown,lastStage:replay.finalStage,mode:replay.practice?'practice':'story',
      stages:replay.stages.map((part,index)=>({id:part.number,label:part.number===7?'Extra':`Stage ${part.number}`,
        score:(replay.stages[index+1]?.initial.score??replay.scoreStored)*10,frames:this.length(replay,part)}))};
  }
  length(replay,part){
    const view=new DataView(replay.data.buffer,replay.data.byteOffset,replay.data.byteLength);
    for(let frame=0;frame<part.frames;frame++){
      const p=part.inputOffset+frame*6;
      if(view.getUint16(p,true)===0xffff&&view.getUint16(p+2,true)===0xffff&&view.getUint16(p+4,true)===0xffff)return frame;
    }
    return part.frames;
  }
  get active(){return !!this.replay;}
  get seeking(){return this.target!==null;}
  get audible(){return this.active&&!this.paused&&!this.seeking&&!this.preparing&&!this.ended&&this.rate===1;}
  position(){
    const native=this.api.state(),part=this.parts.find(part=>part.stage===native.stage);
    return part?part.start+Math.min(part.frames,native.cursor):this.startFrame;
  }
  snapshot(){return {kind:'replay',path:this.path,frame:this.position(),total:this.total,
    stage:this.api.state().stage,startFrame:this.startFrame,startStage:this.parts[this.startIndex].stage,
    difficulty:this.replay.difficulty,paused:this.paused,rate:this.rate,seeking:this.seeking,
    target:this.target,preparing:this.preparing,ended:this.ended,description:this.description};}
  report(){if(this.active)this.api.report(this.snapshot());}
  async restart(){
    this.preparing=true;this.ended=false;this.credit=0;this.api.silence();
    try{await this.api.open(this.replay,this.parts[this.startIndex].stage);}
    finally{this.preparing=false;}
  }
  async command(message){
    if(this.pending)await this.pending;
    const {action,value}=message;
    if(action==='inspect')return {description:(await this.inspect(message.path)).description};
    if(action==='open'){
      const {replay,description}=await this.inspect(message.path),stage=message.stage??replay.stages[0].number;
      const index=replay.stages.findIndex(part=>part.number===stage);
      if(index<0)throw Error('录像没有此关卡');
      this.path=message.path.replace(/^\/savesth12\//,'');this.replay=replay;this.description=description;this.startIndex=index;
      let start=0;this.parts=replay.stages.map(part=>{const frames=this.length(replay,part),result={stage:part.number,start,frames};start+=frames;return result;});
      this.startFrame=this.parts[index].start;this.total=start;this.target=null;this.paused=false;
      try{await this.restart();}catch(error){this.replay=null;this.paused=true;this.api.report(null);throw error;}
    }else{
      if(!this.active)throw Error('请先选择录像关卡');
      if(action==='pause')this.paused=true;
      else if(action==='play'){
        if(this.ended){this.target=null;await this.restart();}
        this.paused=false;
      }else if(action==='rate'){
        if(![0,.5,1,2,4,8].includes(value))throw Error('播放速度无效');this.rate=value;
      }else if(action==='seek'){
        if(!Number.isSafeInteger(value)||value<this.startFrame||value>this.total)throw Error('录像位置无效');
        const restart=value<this.position()||this.ended;
        this.target=value;this.api.silence();if(restart)await this.restart();
      }else throw Error('不支持的录像操作');
    }
    this.credit=0;this.api.resetTime();this.api.transport();this.report();return {};
  }
  background(){if(this.active){this.paused=true;this.credit=0;this.api.silence();this.report();}}
  pulse(){
    if(this.pulseQueued||this.preparing||this.ended||this.api.busy()||!this.seeking&&(this.paused||this.rate!==0))return;
    if(!this.channel){this.channel=new MessageChannel();this.channel.port1.onmessage=()=>{
      this.pulseQueued=false;if(this.api.busy())return;
      try{this.update(0,performance.now());}catch(error){this.paused=true;this.target=null;this.api.silence();this.report();this.api.error(error);}
    };}
    this.pulseQueued=true;this.channel.port2.postMessage(null);
  }
  boundary(){
    const stage=this.api.state().stage,index=this.parts.findIndex(part=>part.stage===stage),part=this.parts[index];
    if(!part||this.api.state().cursor<part.frames)return false;
    if(index+1===this.parts.length){this.ended=true;this.paused=true;this.target=null;this.api.silence();this.report();return true;}
    this.preparing=true;this.credit=0;
    this.pending=this.api.transition(this.parts[index+1].stage).then(()=>{
      this.preparing=false;this.api.resetTime();this.api.transport();this.report();
    }).catch(error=>{this.preparing=false;this.paused=true;this.target=null;this.api.silence();this.report();this.api.error(error);throw error;});
    // The command lane awaits the same transition; the animation lane reports
    // its failure without creating an unhandled rejected promise.
    this.pending.catch(()=>{});
    return true;
  }
  update(elapsed,now){
    if(!this.active||this.preparing||this.api.busy())return;
    const unlimited=this.seeking||this.rate===0;
    if(!unlimited)this.credit=Math.min(240,this.credit+elapsed/1000*60*this.rate);
    const allowed=this.paused&&!this.seeking?0:unlimited?4096:Math.floor(this.credit+1e-9),began=performance.now();
    let count=0;
    while(count<allowed&&!this.ended&&!this.api.busy()){
      if(this.seeking&&this.position()>=this.target)break;
      if(this.boundary())break;
      this.api.step();count++;
      if(performance.now()-began>=8)break;
    }
    if(!unlimited)this.credit=Math.max(0,this.credit-count);
    // Resolve EOF even when the final input exhausted this slice's credit.
    if(count&&!this.preparing&&!this.ended)this.boundary();
    if(this.seeking&&!this.preparing&&(this.position()>=this.target||this.ended)){
      this.target=null;this.credit=0;this.api.resetTime();this.api.transport();this.report();
    }
    if(now-this.reportedAt>=100){this.reportedAt=now;this.report();}
    // Replay catch-up is independent from display refresh. Yield every 8 ms
    // to browser input/painting; RAF remains the sole presentation owner.
    this.pulse();
  }
}
