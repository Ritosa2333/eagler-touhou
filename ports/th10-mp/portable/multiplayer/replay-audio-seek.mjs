// Diagnostic-only. Exercise the production seek mixer and native refill owner
// against the same immutable PCM played without seeking. No game state or
// Replay is changed; the fixture wrappers are absent from shipping binaries.
export function checkReplaySeekAudio(c,Module){
  if(!c.mp_fixture_audio_seek_output||!c.mp_fixture_audio_seek_tick)
    throw Error('Audio seek checks require the fixture-only wrappers');
  const allocated=[],alloc=n=>{const p=c.graphics_allocate(n);if(!p)throw Error('Audio test allocation failed');allocated.push(p);return p;};
  const text=s=>{const p=alloc(s.length+1);new Uint8Array(c.memory.buffer,p,s.length+1).set(new TextEncoder().encode(s+'\0'));return p;};
  const u32=p=>new DataView(c.memory.buffer).getUint32(p,true);
  const stats=()=>Array.from(new Uint32Array(c.memory.buffer,c.sdl_audio_stats(),12));
  const require=(value,reason)=>{if(!value)throw Error(reason);};
  const rate=44100,total=rate*16,intro=12348,source=new Int16Array(total*2);
  for(let i=0;i<total;i++){
    source[i*2]=Math.round(10000*Math.sin(i*.047)+3000*Math.sin(i*.0071));
    source[i*2+1]=Math.round(9000*Math.cos(i*.023));
  }
  Module.FS.writeFile('/game/replay-seek-probe.dat',new Uint8Array(source.buffer));
  const fmt=new Uint8Array(104),view=new DataView(fmt.buffer);fmt.set(new TextEncoder().encode('probe.wav'));
  for(const [offset,value]of [[16,0],[20,source.byteLength],[24,intro*4],[28,source.byteLength],[36,rate],[40,rate*4]])view.setUint32(offset,value,true);
  for(const [offset,value]of [[32,1],[34,2],[44,4],[46,16]])view.setUint16(offset,value,true);
  const fileName=text('th10.dat'),formatName=text('thbgm.fmt'),musicName=text('replay-seek-probe.dat'),trackName=text('probe.wav');
  const args=alloc(64),out=alloc(64),pcm=alloc(8192*8),format=alloc(18),desc=alloc(36);
  let files=0,audio=0,device=0,reference=0;
  const call=(id,op,values=[])=>{new Uint32Array(c.memory.buffer,args,16).fill(0);new Uint32Array(c.memory.buffer,args,values.length).set(values);return c.audio_call(id,op,args);};
  const scenarios=[];
  try{
    files=c.files_create();audio=c.audio_create(files);
    require(files&&audio&&c.files_attach(files,fileName)&&c.audio_initialize(audio,1)===0&&
      c.audio_formats(audio,formatName)===0,'Native BGM setup failed');
    const manager=c.audio_manager(audio);
    // Original AudioManager format table and BGM stream offsets; these are
    // local test observations, never a network checksum or portable snapshot.
    new Uint8Array(c.memory.buffer,u32(manager+0x1f84),fmt.length).set(fmt);
    const command=kind=>{
      c.audio_queue_music(audio,kind,-1,trackName);
      for(let i=0;u32(manager+0x1f88);i++){
        require(i<64,'Native BGM command did not drain');c.audio_update(audio);
      }
    };
    c.sdl_audio_pause(1);
    const maxTicks=1200,testFrames=6144+maxTicks*735+8192;
    const baseline=new Float32Array(testFrames*2);
    device=c.sdl_audio_device();
    new Uint8Array(c.memory.buffer,format,18).set(fmt.subarray(32,50));
    new Uint32Array(c.memory.buffer,desc,9).set([36,0x80c8,(testFrames+4096)*4,0,format,0,0,0,0]);
    call(device,2,[desc,out,0]);reference=u32(out);require(reference!==0,'Reference PCM buffer missing');
    call(reference,14,[0,(testFrames+4096)*4,out,out+4,out+8,out+12,0]);
    const raw=new Int16Array(c.memory.buffer,u32(out),(testFrames+4096)*2);
    for(let i=0;i<testFrames+4096;i++){
      const frame=i<total?i:intro+(i-total)%(total-intro);
      raw[i*2]=source[frame*2];raw[i*2+1]=source[frame*2+1];
    }
    call(reference,10,[0,0,0]);
    for(let frame=0;frame<testFrames;frame+=1024){
      const count=Math.min(1024,testFrames-frame);
      require(c.sdl_audio_render(pcm,count)===count,'Incomplete reference PCM');
      baseline.set(new Float32Array(c.memory.buffer,pcm,count*2),frame*2);
    }
    call(reference,11);call(reference,1);reference=0;call(device,1);device=0;

    for(const [name,ticks,suspended]of [['short-seek',37,false],['across-track-loop',1200,false],['already-suspended',91,true]]){
      require(c.audio_start_file(audio,musicName)===0,'BGM archive start failed');command(2);
      // Match the original test's immediate native gain; preserve loop/worker.
      call(u32(u32(u32(manager+0x5208)+4)),8,[0]);
      c.mp_fixture_audio_seek_output(0);c.sdl_audio_pause(0);
      const before=stats();c.sdl_audio_pump();const prefilled=stats();
      const prefix=prefilled[4]-before[4];
      require(prefix===6144&&prefilled[5]>0,'Expected one bounded native output prefill');
      if(suspended)c.sdl_audio_pause(1);
      c.mp_fixture_audio_seek_output(1);
      require(stats()[5]===0&&stats()[9]===1,name+': Seek did not clear and suppress queued device output: '+JSON.stringify(stats()));
      let elapsed=0;
      for(let tick=0;tick<ticks;tick++){
        const next=Math.floor((tick+1)*1000/60);c.audio_advance(audio,next-elapsed);elapsed=next;
        require(c.mp_fixture_audio_seek_tick()===1,'Seek did not consume exactly one native PCM interval');
        c.sdl_audio_pump();
        if(tick%31===0){
          const current=stats();require(current[5]===0&&current[9]===1&&!current[8],'Seek leaked output or lost its owner');
        }
      }
      c.mp_fixture_audio_seek_output(0);
      const resumed=stats();
      require(resumed[5]===0&&!resumed[8],'Seek left stale audio queued at its boundary');
      require(resumed[9]===(suspended?1:0),'Seek overwrote an independent suspension request');
      // First samples after seeking must be the exact continuation of the
      // immutable reference, including native ring refill and track looping.
      let mismatch=0,maxError=0,first=null;
      require(c.sdl_audio_render(pcm,8192)===8192,'Incomplete resumed PCM');
      const samples=new Float32Array(c.memory.buffer,pcm,8192*2),offset=(prefix+ticks*735)*2;
      for(let i=0;i<samples.length;i++){
        const error=Math.abs(samples[i]-baseline[offset+i]);maxError=Math.max(maxError,error);
        if(error>1e-6){mismatch++;first??={sample:i,actual:samples[i],expected:baseline[offset+i]};}
      }
      scenarios.push({name,ticks,prefix,comparedFrames:8192,mismatch,maxError,first,queueAfterSeek:resumed[5],suspensionPreserved:true});
      require(mismatch===0,'Resumed native BGM samples differ: '+JSON.stringify(scenarios.at(-1)));
      command(4);c.sdl_audio_pause(1);
    }
    c.audio_destroy(audio);audio=0;c.files_destroy(files);files=0;
    const final=stats();require(!final[1]&&!final[2]&&!final[8],'Audio buffer/event ownership leaked');
    return {passed:true,scenarios,remainingBuffers:final[1],remainingEvents:final[2],
      scope:'native mixer samples, seek suppression and refill lifecycle',
      notClaimed:['physical speaker output on every device']};
  }finally{
    if(reference){call(reference,11);call(reference,1);}
    if(device)call(device,1);
    c.mp_fixture_audio_seek_output(0);
    if(audio)c.audio_destroy(audio);if(files)c.files_destroy(files);
    c.sdl_audio_shutdown();
    for(const pointer of allocated)c.graphics_free(pointer);
    Module.FS.unlink('/game/replay-seek-probe.dat');
  }
}
