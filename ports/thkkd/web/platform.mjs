import {Renderer,rgb} from './renderer.mjs';
import {PresentationStats} from './presentation-stats.mjs';
import {TextRenderer,defaultFont,fontStyle} from './text.mjs';
const normalized=name=>name.replaceAll('\\','/').replace(/^\.\//,'').replace(/^assets\//,'').toLowerCase();
const bits=new DataView(new ArrayBuffer(8));
const f=value=>{bits.setUint32(0,value,true);return bits.getFloat32(0,true);};
const d=(lo,hi)=>{bits.setUint32(0,lo,true);bits.setUint32(4,hi,true);return bits.getFloat64(0,true);};
const signed=value=>value|0;
export class Platform {
  constructor(canvas,input,emit){
    this.renderer=new Renderer(canvas);this.input=input;this.emit=emit;this.assets=new Map();this.graphs=new Map();this.sounds=new Map();this.fonts=new Map([[0,defaultFont()]]);this.textRenderer=new TextRenderer();
    this.presentationStats=new PresentationStats();this.fpsLabel=document.querySelector('#fps');
    this.fitFPS=()=>{const rect=canvas.getBoundingClientRect();if(this.fpsLabel)this.fpsLabel.style.transform=`translate(${rect.left+572*rect.width/640}px,${rect.top+465*rect.height/480}px) scale(${rect.width/640},${rect.height/480})`;};this.fitFPS();addEventListener('resize',this.fitFPS);
    this.renderer.onPresent=time=>{this.presentationStats.presented(time);if(this.fpsLabel){this.fpsLabel.hidden=!this.showFPS;if(!this.fpsTime||time-this.fpsTime>=100){this.fpsLabel.textContent=this.presentationStats.fps.toFixed(1)+'FPS';this.fpsTime=time;}}};
    this.textCache=new Map();this.next=1;this.defaultSize=16;this.divHandles=[];this.music=true;this.context=new AudioContext();
    this.bgmGain=this.context.createGain();this.sfxGain=this.context.createGain();this.bgmGain.connect(this.context.destination);this.sfxGain.connect(this.context.destination);
    this.bgmGain.__eaglerAudioRole='bgm';this.sfxGain.__eaglerAudioRole='sfx';
    this.urls=[];this.presented=0;this.seed=0x6d2b79f5;this.viewport=null;
  }
  async load(buffer,progress=()=>{}){
    const bytes=new Uint8Array(buffer),view=new DataView(buffer);
    if(new TextDecoder().decode(bytes.subarray(0,6))!=='THKKD1')throw Error('海惠堂 DATA 格式无效');
    const size=view.getUint32(8,true),header=JSON.parse(new TextDecoder().decode(bytes.subarray(12,12+size))),base=12+size;
    if(header.schema!=='thkkd/web-assets/1')throw Error('海惠堂资源版本无效');
    let done=0;
    for(const row of header.files){
      if(row.offset<0||row.bytes<=0||base+row.offset+row.bytes>bytes.length)throw Error('海惠堂资源范围无效');
      const slice=bytes.subarray(base+row.offset,base+row.offset+row.bytes);
      const ext=row.file.split('.').pop(),mime={png:'image/png',ogg:'audio/ogg',mp3:'audio/mpeg',wav:'audio/wav',webm:'video/webm'}[ext];
      const blob=new Blob([slice],{type:mime}),url=URL.createObjectURL(blob);this.urls.push(url);
      const item={...row,url};
      if(ext==='png'){
        let image;try{image=await createImageBitmap(blob,{premultiplyAlpha:'none',colorSpaceConversion:'none'});}catch(error){throw Error(`图像解码失败：${row.source} (${row.bytes} bytes)：${error.message}`);}item.graph={...this.renderer.texture(image),alpha:row.intrinsicAlpha};image.close();
      }else if(ext==='wav')item.buffer=await this.context.decodeAudioData(slice.slice().buffer);
      this.assets.set(normalized(row.source),item);progress(++done/header.files.length);
    }
  }
  keys(output){output.fill(0);this.input.sample(output);}
  asset(name){const a=this.assets.get(normalized(name));if(!a)throw Error('缺少资源：'+name);return a;}
  graph(handle){
    const graph=this.graphs.get(handle);
    if(graph?.video&&graph.video.readyState>=2&&graph.video.currentTime!==graph.uploadedTime){
      if(!graph.texture)Object.assign(graph,this.renderer.texture(graph.video));
      else{const gl=this.renderer.gl;gl.bindTexture(gl.TEXTURE_2D,graph.texture);gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,gl.RGBA,gl.UNSIGNED_BYTE,graph.video);}
      graph.uploadedTime=graph.video.currentTime;
    }
    return graph;
  }
  text(x,y,text,color,font=0,edge=0,vertical=false){
    if(x===572&&y===465&&/^(?:[\d.]+|nan|inf)FPS$/i.test(text)){this.showFPS=true;return 0;}
    const style=this.fonts.get(font)||this.fonts.get(0),size=font?style.size:this.defaultSize;
    const key=JSON.stringify([text,color,size,style,edge,vertical]);let graph=this.textCache.get(key);
    if(!graph){
      const {canvas,padding}=this.textRenderer.raster(text,style,size,color,edge,vertical);
      graph=this.renderer.texture(canvas);graph.text=text;graph.padding=padding;this.textCache.set(key,graph);
      if(this.textCache.size>1024){const [old,item]=this.textCache.entries().next().value;this.textCache.delete(old);this.renderer.releaseTexture(item);}
    }
    this.renderer.sprite(graph,x-graph.padding,y-graph.padding,1,0,false,false,true);return 0;
  }
  stopSound(sound){
    sound.playSerial=(sound.playSerial||0)+1;
    if(sound.node){const elapsed=Math.max(0,this.context.currentTime-(sound.started||0));sound.offset=(sound.offset||0)+elapsed;if(sound.node.loop)sound.offset%=sound.buffer.duration;try{sound.node.stop();}catch{}sound.node.disconnect();sound.node=null;}
    if(sound.audio)sound.audio.pause();
    sound.playing=false;
  }
  playSound(sound,mode,restart){
    if(!sound)return -1;
    if(sound.bgm){
      if(!this.music)return 0;
      sound.audio.loop=!!(mode&2);if(restart)sound.audio.currentTime=0;sound.playing=true;
      if(this.silent){if(restart)sound.seekTime=0;sound.playing=true;return 0;}
      this.playMedia(sound);return 0;
    }
    if(this.silent)return 0;
    if(sound.node)this.stopSound(sound);if(restart)sound.offset=0;
    const node=this.context.createBufferSource();node.buffer=sound.buffer;node.loop=!!(mode&2);node.connect(sound.gain);sound.offset=(sound.offset||0)%sound.buffer.duration;sound.started=this.context.currentTime;node.start(0,sound.offset);sound.node=node;sound.playing=true;node.onended=()=>{if(sound.node===node){sound.node=null;sound.playing=false;sound.offset=0;}node.disconnect();};return 0;
  }
  playMedia(media){
    if(this.silent||this.suspended||this.playbackPaused)return;
    const serial=media.playSerial=(media.playSerial||0)+1,element=media.audio||media.video;
    element.play().then(()=>{if(media.audio&&media.playSerial===serial&&media.failure){delete media.failure;this.emit('audio-health',this.audioHealth());}}).catch(error=>{if(media.playSerial!==serial||!media.playing||this.silent||this.suspended||this.playbackPaused)return;if(media.audio){media.failure={state:error.name==='NotAllowedError'?'blocked':'error',error:String(error)};this.emit('audio-health',this.audioHealth());}else this.emit('error',{error:'视频播放失败：'+error.message});});
  }
  audioHealth(){
    const failure=this.music&&([...this.sounds.values()].find(sound=>sound.bgm&&sound.failure)?.failure||this.missingMusic?.values().next().value);
    return failure||{state:this.context.state};
  }
  call(api,a,t,bytes){
    const r=this.renderer,g=h=>this.graph(h),n=i=>signed(a[i]);
    switch(api){
    case 0x43da40:return this.input.padState();
    case 0x41b8e0:r.clear();this.showFPS=false;return 0;
    case 0x40e6b0:{
      if(this.input.unlimitedUsed){const old=r.identity;r.identity=null;this.fonts.set(-1,{size:12,weight:400,edge:1,antialias:true,italic:false});this.text(8,2,'自由移动 · 处理落率 100%',0xffffff,-1,0);r.identity=old;}
      const marker=this.hitbox?.();
      if(marker){const old={mode:r.mode,alpha:r.alpha,brightness:r.brightness,area:r.area,identity:r.identity};r.mode=1;r.alpha=1;r.brightness=[1,1,1];r.area=[32,16,416,464];r.identity='always-hitbox:'+marker.life;r.sprite(this.graph(marker.graph),marker.x,marker.y);Object.assign(r,old);}
      r.commit();this.presented++;return 0;
    }
    case 0x404830:r.mode=n(0);r.alpha=Math.max(0,Math.min(255,n(1)))/255;return 0;
    case 0x4048a0:r.brightness=a.slice(0,3).map(v=>v/255);return 0;
    case 0x40dd90:r.area=a.slice(0,4).map(signed);return 0;
    case 0x43c090:{
      const asset=this.asset(t[0]),handle=this.next++;
      if(asset.graph)this.graphs.set(handle,{...asset.graph});else if(asset.file.endsWith('.webm')){
        const video=document.createElement('video');video.src=asset.url;video.muted=true;video.playsInline=true;video.preload='auto';video.load();this.graphs.set(handle,{video,width:640,height:480,alpha:false,playing:false,seekTime:0});
        video.addEventListener('seeked',()=>{if(this.graphs.has(handle))this.graph(handle);});
        video.addEventListener('error',()=>{if(video.getAttribute('src'))this.emit('error',{error:'视频资源解码失败：'+t[0]});});
      }else throw Error('图像格式无效：'+t[0]);return handle;
    }
    case 0x43c2c0:{const asset=this.asset(t[0]);this.divHandles=[];for(let i=0;i<a[0];i++){const h=this.next++;this.graphs.set(h,{...asset.graph,x:i%a[1]*a[3],y:Math.floor(i/a[1])*a[4],w:a[3],h:a[4]});this.divHandles.push(h);}return 0;}
    case 0x403e20:{this.releaseGraph(this.graphs.get(a[0]));this.graphs.delete(a[0]);return 0;}
    case 0x41bb70:{const graph=g(a[2]);if(graph?.video&&!graph.texture)return 0;r.sprite(graph,n(0),n(1),1,0,false,false,this.transparent(graph,a[3]));return 0;}
    case 0x41c3c0:case 0x41c910:{const graph=g(a[6]);r.sprite(graph,api===0x41c910?f(a[0]):n(0),api===0x41c910?f(a[1]):n(1),d(a[2],a[3]),d(a[4],a[5]),!!a[8],true,this.transparent(graph,a[7]));return 0;}
    case 0x421700:{const graph=g(a[8]);if(graph)r.add(graph,a.slice(0,8).map(f),undefined,this.transparent(graph,a[9]));return 0;}
    case 0x421760:r.box(n(0),n(1),n(2),n(3),rgb(a[4]),!!a[5]);return 0;
    case 0x41e7b0:r.box(n(0),n(1),n(0)+1,n(1)+1,rgb(a[2]));return 0;
    case 0x41e460:r.circle(n(0),n(1),n(2),rgb(a[3]),a.length<5||!!a[4]);return 0;
    case 0x41e5b0:if(a[7])r.triangle(a.slice(0,6).map(signed),rgb(a[6]));else{r.line(n(0),n(1),n(2),n(3),rgb(a[6]));r.line(n(2),n(3),n(4),n(5),rgb(a[6]));r.line(n(4),n(5),n(0),n(1),rgb(a[6]));}return 0;
    case 0x42d190:r.line(n(0),n(1),n(2),n(3),rgb(a[4]),a[5]||1);return 0;
    case 0x41dd00:{
      const graph=g(a[2]);if(!graph)return -1;const v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
      this.debug3d={args:a,viewport:this.viewport,vertices:Array.from({length:Math.min(18,bytes.length/4)},(_,i)=>v.getFloat32(i*4,true))};
      for(let i=0;i<a[1];i++){
        const points=[],uv=[],depth=[],colors=[];
        for(let j=0;j<3;j++){
          const k=(i*3+j)*24,projected=this.project(v.getFloat32(k,true),v.getFloat32(k+4,true),v.getFloat32(k+8,true));
          points.push(...projected.slice(0,2));depth.push(...projected.slice(2));uv.push(((graph.x||0)+v.getFloat32(k+16,true)*(graph.w??graph.width))/graph.width,((graph.y||0)+v.getFloat32(k+20,true)*(graph.h??graph.height))/graph.height);
          const color=v.getUint32(k+12,true);colors.push(...rgb(color).slice(0,3),(color>>>24)/255);
        }
        points.push(...points.slice(4,6));depth.push(...depth.slice(4,6));uv.push(...uv.slice(4,6));colors.push(...colors.slice(8,12));
        r.add(graph,points,uv,this.transparent(graph,a[3]),[1,1,1,1],depth,colors);
      }return 0;
    }
    case 0x40a9a0:this.viewport=a.map(f);return 0;
    case 0x432800:{const h=this.next++;this.fonts.set(h,fontStyle(a));return h;}
    case 0x432c20:return 0;
    case 0x4331b0:this.defaultSize=n(0);return 0;
    case 0x437bd0:case 0x437c20:{const font=(api===0x437c20&&a[0]!==0?this.fonts.get(a[0]):null)||this.fonts.get(0),size=api===0x437c20&&a[0]!==0?font.size:this.defaultSize;return this.textRenderer.width(t[0],font,size);}
    case 0x437c90:return this.text(n(0),n(1),t[0],a[2],a[3],a[4],a[5]);
    case 0x4434d0:{
      const name=normalized(t[0]),bgm=name.startsWith('bgm/'),asset=this.assets.get(name);
      if(!asset){
        if(!bgm)throw Error('缺少资源：'+t[0]);
        if(this.music){this.missingMusic??=new Map();this.missingMusic.set(name,{state:'error',error:'缺少音乐资源：'+t[0]});this.emit('audio-health',this.audioHealth());}
        return -1;
      }
      this.missingMusic?.delete(name);
      const h=this.next++,sound={buffer:asset.buffer,bgm,playing:false};sound.gain=this.context.createGain();sound.gain.connect(bgm?this.bgmGain:this.sfxGain);
      if(bgm&&this.music){
        sound.audio=new Audio(asset.url);sound.audio.preload='auto';
        sound.audio.addEventListener('error',()=>{if(sound.audio.getAttribute('src')){sound.failure={state:'error',error:'音乐资源解码失败：'+t[0]};this.emit('audio-health',this.audioHealth());}});
        sound.media=this.context.createMediaElementSource(sound.audio);sound.media.connect(sound.gain);
      }
      this.sounds.set(h,sound);return h;
    }
    case 0x43e0d0:return 0;
    case 0x43e010:{const s=this.sounds.get(a[0]);if(s){this.stopSound(s);s.media?.disconnect();s.gain.disconnect();if(s.audio){s.audio.removeAttribute('src');s.audio.load();}this.sounds.delete(a[0]);}return 0;}
    case 0x43fd40:{const s=this.sounds.get(a[1]);if(s)s.gain.gain.value=Math.min(255,a[0])/255;return 0;}
    case 0x440240:{const s=this.sounds.get(a[0]);if(s)this.stopSound(s);return 0;}
    case 0x441b20:{const s=this.sounds.get(a[0]);return s?.audio?+(this.silent?s.playing:!s.audio.paused&&!s.audio.ended):+!!s?.playing;}
    case 0x4425f0:return this.playSound(this.sounds.get(a[0]),a[1],a[2]);
    case 0x406770:{const graph=g(a[0]);if(graph?.video){graph.seekTime=a[1]/1000;if(!this.silent)graph.video.currentTime=graph.seekTime;}return 0;}
    case 0x406690:{const graph=g(a[0]),v=graph?.video;if(v){v.loop=!!(a[1]&2);graph.playing=true;this.playMedia(graph);}return 0;}
    case 0x406700:{const graph=g(a[0]);if(graph?.video){graph.playSerial=(graph.playSerial||0)+1;graph.playing=false;graph.video.pause();}return 0;}
    case 0x4067e0:{const graph=g(a[0]),v=graph?.video;return +(!!v&&(this.silent?graph.playing:!v.paused&&!v.ended));}
    case 0x466760:this.seed^=this.seed<<13;this.seed^=this.seed>>>17;this.seed^=this.seed<<5;return (this.seed>>>0)%(a[0]+1);
    case 0x443d80:case 0x40db50:case 0x41f450:case 0x42e940:case 0x4463a0:case 0x446510:case 0x4494c0:case 0x444500:case 0x444530:case 0x444540:return 0;
    default:throw Error('未实现的浏览器平台调用：0x'+api.toString(16));
    }
  }
  transparent(graph,requested){return !!requested||!!graph?.alpha&&[1,2,3].includes(this.renderer.mode);}
  project(x,y,z){
    // 使用 Windows 3.25a 后台实测的默认 view/projection，再应用游戏自有 viewport。
    const w=z+415.69219970703125,nx=(x-320)*1.299038052558899/w,ny=(y-240)*1.7320507764816284/w,nz=(w*1.00204-41.654)/w;
    const m=this.viewport||[320,0,0,0,0,-240,0,0,0,0,1,0,320,240,0,1];
    return [nx*m[0]+ny*m[4]+nz*m[8]+m[12],nx*m[1]+ny*m[5]+nz*m[9]+m[13],w,nz];
  }
  async suspend(){this.suspended=true;this.mediaEpoch=(this.mediaEpoch||0)+1;this.pausedAudio=[];for(const sound of this.sounds.values())if(sound.audio){sound.playSerial=(sound.playSerial||0)+1;sound.audio.pause();}for(const graph of this.graphs.values())if(graph.video){graph.playSerial=(graph.playSerial||0)+1;graph.video.pause();}if(this.context.state!=='closed')await this.context.suspend();}
  async resume(){this.suspended=false;const epoch=this.mediaEpoch=(this.mediaEpoch||0)+1;if(this.context.state==='closed')return;await this.context.resume();if(epoch!==this.mediaEpoch||this.suspended)return;for(const sound of this.sounds.values())if(sound.audio&&sound.playing)this.playMedia(sound);for(const graph of this.graphs.values())if(graph.video&&graph.playing)this.playMedia(graph);this.pausedAudio=[];}
  setSeeking(value){
    if(this.silent===value)return;this.silent=value;
    for(const sound of this.sounds.values()){
      if(sound.bgm){
        if(value){sound.playSerial=(sound.playSerial||0)+1;sound.seekTime=sound.audio.currentTime;if(!sound.audio.paused){sound.playing=true;sound.audio.pause();}}
        else if(sound.playing){const seek=()=>{sound.audio.currentTime=sound.seekTime||0;this.playMedia(sound);};if(sound.audio.readyState)seek();else sound.audio.addEventListener('loadedmetadata',seek,{once:true});}
      }else if(sound.node&&!sound.node.loop)this.stopSound(sound);
    }
    for(const graph of this.graphs.values())if(graph.video){
      if(value){graph.playSerial=(graph.playSerial||0)+1;graph.seekTime=graph.video.currentTime;if(!graph.video.paused){graph.playing=true;graph.video.pause();}}
      else{const seek=()=>{graph.video.currentTime=graph.seekTime||0;if(graph.playing)this.playMedia(graph);};if(graph.video.readyState)seek();else graph.video.addEventListener('loadedmetadata',seek,{once:true});}
    }
  }
  seekAudioTick(){
    if(!this.silent)return;
    for(const sound of this.sounds.values())if(sound.bgm&&sound.playing){sound.seekTime=(sound.seekTime||0)+1/60;if(Number.isFinite(sound.audio.duration)&&sound.seekTime>=sound.audio.duration){if(sound.audio.loop)sound.seekTime%=sound.audio.duration;else sound.playing=false;}}
    for(const graph of this.graphs.values())if(graph.video&&graph.playing){graph.seekTime=(graph.seekTime||0)+1/60;if(Number.isFinite(graph.video.duration)&&graph.seekTime>=graph.video.duration){if(graph.video.loop)graph.seekTime%=graph.video.duration;else graph.playing=false;}}
  }
  releaseGraph(graph){if(!graph?.video)return;graph.playSerial=(graph.playSerial||0)+1;graph.playing=false;graph.video.pause();graph.video.removeAttribute('src');graph.video.load();this.renderer.releaseTexture(graph);}
  resetGame(){for(const s of this.sounds.values()){this.stopSound(s);s.media?.disconnect();s.gain.disconnect();if(s.audio){s.audio.removeAttribute('src');s.audio.load();}}this.sounds.clear();this.missingMusic?.clear();for(const graph of this.graphs.values())this.releaseGraph(graph);this.graphs.clear();this.renderer.commands=[];this.renderer.current=[];this.renderer.previous.clear();this.renderer.identities.clear();this.renderer.committedScene=undefined;this.renderer.identity=null;this.viewport=null;this.fonts=new Map([[0,defaultFont()]]);this.defaultSize=16;this.pausedAudio=[];this.showFPS=false;if(this.fpsLabel)this.fpsLabel.hidden=true;}
  async destroy(){this.resetGame();removeEventListener('resize',this.fitFPS);if(this.fpsLabel)this.fpsLabel.hidden=true;await this.context.close();this.renderer.destroy();for(const url of this.urls)URL.revokeObjectURL(url);this.urls=[];}
}
