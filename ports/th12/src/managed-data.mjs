// Prepared original content belongs to Package Store, never Runtime Release.
// This format contains extracted/decoded retail assets, not executable code.
export const DATA_SCHEMA='th12-prepared-content/1';
export const DATA_MAGIC='TH12PK01';
export const DATA_LAYOUT_SPEC=Object.freeze({schema:DATA_SCHEMA,magic:DATA_MAGIC,header:'magic8+manifestBytesLE32+utf8JSON',files:'contiguous-relative-offsets',mount:'/assets',music:'/music'});
const decoder=new TextDecoder('utf-8',{fatal:true});
const resourceName=/^assets\/[a-zA-Z0-9_-]+\.(?:json|png|anm|ecl|std|sht|msg|wav|rpy)$/;
const musicName=/^\/music\/(?:th12_\d\d|th10_17)\.ogg$/; // Retail credits retain the TH10 filename.
const mime=path=>path.endsWith('.json')?'application/json':path.endsWith('.png')?'image/png':path.endsWith('.wav')?'audio/wav':path.endsWith('.ogg')?'audio/ogg':'application/octet-stream';
const digest=async bytes=>[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(n=>n.toString(16).padStart(2,'0')).join('');

export class PreparedContent {
  static async open(value){
    // Blob keeps large Package Store objects readable without one giant copy.
    // Cross-frame ArrayBuffers deliberately do not use instanceof checks.
    const blob=new Blob([value]);
    if(blob.size<14||blob.size>1024*1024*1024)throw Error('星莲船资源包大小无效');
    const header=new Uint8Array(await blob.slice(0,12).arrayBuffer());
    if(decoder.decode(header.subarray(0,8))!==DATA_MAGIC)throw Error('不是星莲船适配资源包');
    const length=new DataView(header.buffer).getUint32(8,true),base=12+length;
    if(length<2||length>4*1024*1024||base>blob.size)throw Error('星莲船资源包清单无效');
    const manifest=JSON.parse(decoder.decode(await blob.slice(12,base).arrayBuffer()));
    const layout='sha256-'+await digest(new TextEncoder().encode(JSON.stringify(DATA_LAYOUT_SPEC)));
    if(manifest.schema!==DATA_SCHEMA||manifest.game!=='th12'||manifest.layout!==layout||!Array.isArray(manifest.files)||!manifest.files.length||manifest.files.length>10000)throw Error('星莲船资源包格式无效');
    const files=new Map();let end=0;
    for(const item of manifest.files){
      if(!item||!resourceName.test(item.path)||files.has(item.path)||!Number.isSafeInteger(item.offset)||item.offset!==end||!Number.isSafeInteger(item.bytes)||item.bytes<=0||item.bytes>128*1024*1024||!/^([a-f0-9]{64})$/.test(item.sha256)||base+end+item.bytes>blob.size)throw Error('星莲船资源条目无效');
      files.set(item.path,item);end+=item.bytes;
    }
    if(base+end!==blob.size)throw Error('星莲船资源包不完整');
    for(const name of ['loading-resources.json','menu-resources.json','atlases.json','sound-index.json','music-index.json','demo0.rpy','demo1.rpy','demo2.rpy'])if(!files.has('assets/'+name))throw Error('星莲船资源缺失：'+name);
    return new PreparedContent(blob,base,files);
  }
  constructor(blob,base,files){this.blob=blob;this.base=base;this.files=files;this.verified=new Set();}
  async fetch(path){
    const item=this.files.get(path);if(!item)throw Error('资源包中不存在 '+path);
    const blob=this.blob.slice(this.base+item.offset,this.base+item.offset+item.bytes,mime(path));
    if(!this.verified.has(path)){
      if(await digest(await blob.arrayBuffer())!==item.sha256)throw Error('资源校验失败：'+path);
      this.verified.add(path);
    }
    return new Response(blob,{headers:{'Content-Type':mime(path)}});
  }
}

export class RuntimeAssets {
  constructor(environment=window){
    this.environment=environment;this.base=environment.location.href;
    this.query=new URL(this.base).searchParams;this.managed=this.query.get('managedData')==='1';
    this.epoch=Number(this.query.get('runtimeEpoch'));this.music=new Map();this.fonts=new Map();this.waitingMusic=new Map();this.content=null;this.localization=null;
    this.fetch=this.fetch.bind(this);
  }
  async prepare(){
    if(!this.managed)return;
    const environment=this.environment,parentWindow=environment.parent,epoch=this.epoch;
    if(parentWindow===environment||!Number.isSafeInteger(epoch)||epoch<=0)throw Error('请从 Eagler Touhou 启动此运行时');
    const provider=parentWindow.__eaglerPrepareManagedRuntimeDataV1;
    if(typeof provider!=='function')throw Error('启动器未提供受管理游戏资源');
    const result=await provider.call(parentWindow,{game:'th12',generation:this.query.get('gameGeneration'),epoch});
    if(!result?.buffer)throw Error('启动器未返回游戏资源');
    this.content=await PreparedContent.open(result.buffer);
  }
  async fetch(path){
    if(typeof path!=='string'||!/^\/(?:assets|replays)\//.test(path)||path.includes('..')||path.includes('\\'))throw Error('资源路径无效');
    const overlay=this.localization?.fetchOverride(path);if(overlay)return overlay;
    const response=await this.fetchBase(path);
    if(response.ok&&this.localization?.language!=='ja'&&/^\/assets\/st0[1-7]_0[012][ab]\.msg$/.test(path)){
      const original=new Uint8Array(await response.arrayBuffer()),patched=await this.localization?.patchMessage(path,original);
      return new Response(patched??original,{headers:response.headers,status:response.status});
    }
    return response;
  }
  async fetchBase(path){
    if(!this.managed)return this.environment.fetch(new URL('.'+path,new URL('.',this.base)));
    if(!path.startsWith('/assets/'))throw Error('正式运行时仅读取浏览器内已安装的录像');
    if(path.startsWith('/assets/music/')){
      const name='/music/'+path.slice('/assets/music/'.length),blob=this.music.get(name)??await this.waitForMusic(name);
      return new Response(blob,{headers:{'Content-Type':'audio/ogg'}});
    }
    if(!this.content)throw Error('游戏资源尚未准备');
    return this.content.fetch(path.slice(1));
  }
  waitForMusic(name){
    if(!musicName.test(name))return Promise.reject(Error('音乐路径无效'));
    const existing=this.waitingMusic.get(name);if(existing)return existing.promise;
    let resolve,reject;const promise=new Promise((yes,no)=>{resolve=yes;reject=no;});
    const timer=setTimeout(()=>{this.waitingMusic.delete(name);reject(Error('等待启动器安装音乐超时：'+name));},60000);
    this.waitingMusic.set(name,{promise,resolve,reject,timer});return promise;
  }
  async install(resources=[],emit=()=>{}){
    if(!Array.isArray(resources)||resources.length>128)throw Error('附加资源列表无效');
    const pending=new Map();
    for(const resource of resources){
      if(!resource||!musicName.test(resource.path)&&!['/msgothic.ttc','/unifont.otf'].includes(resource.path)||pending.has(resource.path))throw Error('未支持的附加资源路径');
      const url=new URL(resource.url,this.base),origin=new URL(this.base).origin;
      if(url.origin!==origin||!['http:','https:','blob:'].includes(url.protocol))throw Error('附加资源来源无效');
      const response=await this.environment.fetch(url);if(!response.ok)throw Error('附加资源加载失败：'+resource.path);
      const blob=await response.blob(),expected=resource.bytes??resource.size;
      if(!blob.size||blob.size>64*1024*1024||(expected!==undefined&&blob.size!==expected))throw Error('附加资源大小不匹配');
      if(resource.sha256!==undefined&&(!/^[a-f0-9]{64}$/.test(resource.sha256)||await digest(await blob.arrayBuffer())!==resource.sha256))throw Error('附加资源校验失败');
      pending.set(resource.path,blob);emit({event:'transfer',mode:musicName.test(resource.path)?'ogg':'font',path:resource.path,loaded:blob.size,total:blob.size});
    }
    for(const [path,blob]of pending){
      (musicName.test(path)?this.music:this.fonts).set(path,blob);
      const waiting=this.waitingMusic.get(path);if(waiting){this.waitingMusic.delete(path);clearTimeout(waiting.timer);waiting.resolve(blob);}
    }
  }
}
