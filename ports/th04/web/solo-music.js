import {Fat12,sha256} from './solo-fat12.js';
import {NativeMusic} from './solo-native-music.js';
import {LocalBgm} from './netplay/local-bgm.js';

// Only the PMD driver is wrapped. Original OP, MAIN and menu flow still run.
export class SoloMusic {
  constructor(report=()=>{}) {
    this.context=new AudioContext();
    this.context.resume().catch(()=>{});
    this.player=new LocalBgm(this.context,report);this.report=report;
    this.lastPoll=-Infinity;this.closed=false;
  }
  async install(data,folder) {
    const response=await fetch('solo-music.json');
    if(!response.ok)throw Error('独立音乐配置加载失败');
    const meta=await response.json(),asset=await fetch(meta.url);
    if(!asset.ok)throw Error('独立音乐驱动加载失败');
    const bytes=new Uint8Array(await asset.arrayBuffer());
    if(bytes.length!==meta.bytes||await sha256(bytes)!==meta.sha256)throw Error('独立音乐驱动校验失败');
    this.reader=new NativeMusic(meta);
    this.batchPath=folder+'/GAME.BAT';this.driverPath=folder+'/WEBMUSIC.COM';
    const fat=new Fat12(data),entry=fat.find(this.batchPath);
    if(fat.find(this.driverPath,{optional:true})!==undefined)throw Error('单机存档已包含音乐补丁，请使用原版存档');
    this.originalBatch=fat.read(entry);
    // Byte-preserving conversion keeps the Japanese DOS text unchanged.
    let batch=Array.from(this.originalBatch,b=>String.fromCharCode(b)).join('');
    let installs=0,removes=0;
    batch=batch.replace(/^(pmd(?:b2|86)?[^\r\n]*)(\r?\n)/gim,(line,command,eol)=>{
      if(/\/R\s*$/i.test(command)){removes++;return 'webmusic /R'+eol+line;}
      installs++;return line+'webmusic'+eol;
    });
    if(!installs||installs!==removes)throw Error('无法识别单机音乐启动流程');
    fat.put(this.driverPath,bytes);
    fat.replace(entry,Uint8Array.from(batch,c=>c.charCodeAt(0)));
  }
  async installCoop(data,patch,language,diskSha256) {
    // Cooperative MAIN already has the verified native BGM-OFF observer used
    // online. Reuse it without retaining another DOS interrupt driver.
    const response=await fetch('netplay/runtime.json',{cache:'no-cache'});
    if(!response.ok)throw Error('合作音乐配置加载失败');
    const version=await response.json(),sound=version.native_sound_by_language?.[language];
    if(sound?.disk_sha256!==diskSha256||sound.schema!==2||sound.bgm!==0||sound.se!==1||!sound.edits?.length)
      throw Error('合作音乐配置版本不匹配');
    const manifestResponse=await fetch('bgm/manifest.json');
    if(!manifestResponse.ok)throw Error('合作音乐曲目清单加载失败');
    const manifest=await manifestResponse.json(),trackHashes={};
    for(const [hash,id]of Object.entries(manifest.headers)){
      const match=/^(.+)-(m26|m86)$/.exec(id);
      if(match&&(match[2]==='m86'||trackHashes[match[1]]===undefined))trackHashes[match[1]]=Number(hash);
    }
    const seen=new Set();
    for(const edit of sound.edits)for(let i=0;i<edit.targets.length;i++){
      const at=edit.targets[i];
      if(!Number.isInteger(at)||at<0||at>=data.length||seen.has(at)||data[at]!==edit.expected[i])throw Error('合作音乐补丁校验失败');
      seen.add(at);
    }
    this.coopEdits=sound.edits;
    this.reader=new NativeMusic({kind:'th04-native',signature:'TH04COOPLABv001!',
      mailbox_cs_offset:patch.mailbox_cs_offset,data_segment:patch.data_segment,code_segment:patch.code_segment,
      observer:sound.observer,trackHashes});
    for(const edit of sound.edits)for(let i=0;i<edit.targets.length;i++)data[edit.targets[i]]=edit.values[i];
  }
  start() {
    if(this.closed||this.loading)return;
    // The driver can queue commands while the small manifest loads. Tracks
    // download/decode on demand, after the emulator has started.
    this.loading=this.player.load({warmOpening:false}).catch(error=>{
      if(this.closed)return;
      this.player.status='本地音乐错误：'+error.message;
      this.report(this.player.status+'；游戏继续，可重启选择原声模式');
    });
  }
  async attachReplay(){
    const response=await fetch('solo-music.json',{cache:'no-store'});if(!response.ok)throw Error('Replay 音乐桥读取失败');
    this.reader=new NativeMusic(await response.json());
  }
  exportDisk(data) {
    if(this.coopEdits){const copy=new Uint8Array(data);for(const edit of this.coopEdits)for(let i=0;i<edit.targets.length;i++)copy[edit.targets[i]]=edit.expected[i];return copy;}
    const copy=new Uint8Array(data),fat=new Fat12(copy);
    fat.replace(fat.find(this.batchPath),this.originalBatch);
    const entry=fat.find(this.driverPath);
    fat.replace(entry,new Uint8Array());copy[entry]=0xe5;
    return copy;
  }
  poll(heap,now,boundary=false) {
    if(this.closed||!this.reader||!boundary&&now-this.lastPoll<50)return;
    this.lastPoll=now;
    try {
      for(const [ax,hash] of this.reader.read(heap))this.player.command(ax,hash);
      if(this.player.status.includes('错误')||this.player.status.includes('未识别')) {
        if(this.lastError!==this.player.status){this.lastError=this.player.status;this.report(this.lastError+'；可重启选择原声模式');}
      }
    }catch(error){this.report('独立音乐：'+error.message);}
  }
  resume(){if(!this.closed&&!document.hidden)this.context.resume().catch(()=>{});}
  suspend(){if(!this.closed)this.context.suspend().catch(()=>{});}
  reset(){this.player.stop();this.reader?.reset();this.lastPoll=-Infinity;}
  dispose(){if(this.closed)return;this.closed=true;this.player.dispose();this.context.close().catch(()=>{});}
}
