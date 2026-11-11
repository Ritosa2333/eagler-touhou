import {loadVerifiedBytes} from './startup-assets.js';

// Confirmed commands trigger complete original waveforms on the audio clock.
// Network waiting and emulator.pause()/PCM flushing never cut these sources.
export class LocalSfx {
  constructor(context){
    this.context=context;this.buffers=new Map();this.active=null;this.closed=false;
    this.played=0;this.interrupted=0;this.silent=0;this.status='本地音效：准备中';
    this.volume=context.createGain();this.volume.gain.value=1;this.volume.connect(context.destination);
  }
  async load(verifiedAssets){
    let manifest;
    const savedManifest=verifiedAssets?.get('sfx/manifest.json');
    if(savedManifest)manifest=JSON.parse(new TextDecoder().decode(savedManifest.bytes));
    else{
      const response=await fetch(new URL('./sfx/manifest.json',import.meta.url),{cache:'no-cache'});
      if(!response.ok)throw Error('本地音效清单加载失败');
      manifest=await response.json();
    }
    if(manifest.schema!==1||manifest.game!=='TH03'||Object.keys(manifest.effects).length!==22)throw Error('本地音效清单版本不匹配');
    const entries=Object.entries(manifest.effects);let next=0;
    await Promise.all(Array.from({length:4},async()=>{
      while(next<entries.length){
        const [id,asset]=entries[next++];
        const saved=verifiedAssets?.get('sfx/'+asset.url);
        // Preloaded entries were already verified against the room identity.
        // Also require that identity to agree with the inner sound manifest.
        if(saved&&saved.sha256!==asset.sha256)throw Error('本地音效资源版本不匹配');
        const bytes=saved?.bytes??await loadVerifiedBytes(new URL('./sfx/'+asset.url,import.meta.url).href,asset.sha256);
        if(bytes.length!==asset.bytes)throw Error('本地音效资源长度不匹配');
        const buffer=await this.context.decodeAudioData(bytes.slice().buffer);
        if(this.closed)return;
        this.buffers.set(Number(id),buffer);
      }
    }));
    if(!this.closed)this.status='本地音效：已就绪';
  }
  command(ax){
    if(this.closed||(ax>>>8)!==0x0c)return;
    const id=ax&255,buffer=this.buffers.get(id);
    // Empty EFC entries end the FM effect part without inventing a waveform.
    if(!buffer){this.silent++;this.stop();return;}
    if(this.context.state!=='running')return;
    // PMD has one FM effect part. Keep its replacement policy; do not stack
    // dozens of overlapping shots when several confirmed frames catch up.
    if(this.active){this.interrupted++;this.stop();}
    const source=this.context.createBufferSource();source.buffer=buffer;
    source.connect(this.volume);this.active=source;
    source.onended=()=>{source.disconnect();if(this.active===source)this.active=null;};
    source.start();this.played++;this.status=`本地音效：${id}`;
  }
  stop(){if(this.active){const source=this.active;this.active=null;try{source.stop();}catch{}source.disconnect();}}
  info(){return {mode:'confirmed-local-effects',ready:this.buffers.size,played:this.played,
    interrupted:this.interrupted,silent:this.silent,active:!!this.active,status:this.status};}
  dispose(){this.closed=true;this.stop();this.buffers.clear();this.volume.disconnect();}
}
