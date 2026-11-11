import {Fat12,sha256} from './disk.js';
import {NativeMusic} from './native-music.js';
import {LocalBgm} from './local-bgm.js';

// Only the PMD driver is wrapped. Original OP, MAIN and menu flow still run.
export class SoloMusic {
  constructor(report=()=>{}) {
    this.context=new AudioContext();
    this.context.resume().catch(()=>{});
    this.player=new LocalBgm(this.context,report);this.report=report;
    this.lastPoll=-Infinity;this.closed=false;
  }
  prepare(folder) {
    return this.prepared ||= this.prepareAssets(folder);
  }
  async prepareAssets(folder) {
    const response=await fetch('solo-music.json');
    if(!response.ok)throw Error('独立音乐配置加载失败');
    const meta=await response.json(),asset=await fetch(meta.url);
    if(!asset.ok)throw Error('独立音乐驱动加载失败');
    const bytes=new Uint8Array(await asset.arrayBuffer());
    if(bytes.length!==meta.bytes||await sha256(bytes)!==meta.sha256)throw Error('独立音乐驱动校验失败');
    await this.player.load({warmOpening:false});
    // Decode the opening before muting native music, surfacing codec/download errors.
    const opening=folder==='YUMEZIKU'?'op-m26':'op-m86';
    await this.player.decode(opening);
    return {meta,bytes};
  }
  async install(data,folder) {
    const {meta,bytes}=await this.prepare(folder);
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
  exportDisk(data) {
    const copy=new Uint8Array(data),fat=new Fat12(copy);
    fat.replace(fat.find(this.batchPath),this.originalBatch);
    const entry=fat.find(this.driverPath);
    fat.replace(entry,new Uint8Array());copy[entry]=0xe5;
    return copy;
  }
  poll(heap,now) {
    if(this.closed||!this.reader||now-this.lastPoll<50)return;
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
