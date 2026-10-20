// Installed data-only TH12c overlay. Network/package installation belongs to
// Launcher. This module never imports code or fetches a translation URL.
export const LOCALIZATION_PREFIX='/thcrap/th12/';
export const LOCALIZATION_SCHEMA='th12-localization-overlay/1';
export const LOCALIZATION_SOURCE='9854c455fecb4c3c3e34b9a11a10700ec8fccfec13877a5bb96c4560d634e73a';
const ORIGINAL_EXE='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417';
const TEXTURES=new Set([
  ...['e08-4','e10-4','e11-4','front-0','front-3','staff-0','staff-3','staff-4','title_v-0'],
  ...Array.from({length:7},(_,i)=>String(i+1).padStart(2,'0')).flatMap(s=>['st'+s+'logo-0','st'+s+'logo-1']),
  ...['stgenm01-14','stgenm02-16','stgenm03-14','stgenm04-15','stgenm05-14','stgenm06-14','stgenm07-14'],
  ...[0,6,7,...Array.from({length:14},(_,i)=>i+12)].map(i=>'title-'+i)
].map(name=>name+'.png'));
export function isLocalizationResourcePath(path){
  return typeof path==='string'&&(path===LOCALIZATION_PREFIX+'overlay.json'||
    (path.startsWith(LOCALIZATION_PREFIX+'textures/')&&TEXTURES.has(path.slice((LOCALIZATION_PREFIX+'textures/').length))));
}
let active=null;
const object=value=>value!==null&&typeof value==='object'&&!Array.isArray(value);
const has=(value,key)=>Object.hasOwn(value,key);
const hex=value=>typeof value==='string'&&value.length<=8192&&value.length%2===0&&/^[a-f0-9]*$/i.test(value);
const text=value=>typeof value==='string'&&value.length<=8192&&!value.includes('\0');
const integer=(value,min,max)=>Number.isInteger(value)&&value>=min&&value<=max;
const sha=value=>typeof value==='string'&&/^[a-f0-9]{64}$/.test(value);
const hash=async value=>[...new Uint8Array(await crypto.subtle.digest('SHA-256',value))].map(v=>v.toString(16).padStart(2,'0')).join('');
const bytes=value=>{
  if(value instanceof Uint8Array)return value;
  if(ArrayBuffer.isView(value))return new Uint8Array(value.buffer,value.byteOffset,value.byteLength);
  if(Object.prototype.toString.call(value)==='[object ArrayBuffer]')return new Uint8Array(value);
  throw Error('语言资源尚未安装为字节数据');
};
function normalHex(value){
  if(!hex(value))return null;
  value=value.toLowerCase();
  for(let i=0;i<value.length;i+=2)if(value.slice(i,i+2)==='00')return value.slice(0,i);
  return value;
}
function japanese(value){return new TextDecoder('shift-jis',{fatal:true}).decode(Uint8Array.from(value.match(/../g)??[],v=>parseInt(v,16)));}
function checkTextRecord(value){
  if(!object(value)||!text(value.text)||!hex(value.sourceHex)||!integer(value.byteLength,0,4096)||
    (value.offset!==undefined&&!integer(value.offset,-8192,8192))||
    (value.spacing!==undefined&&!integer(value.spacing,-8192,8192)))throw Error('语言包文字记录无效');
  if(value.byteWidths!==undefined&&(!Array.isArray(value.byteWidths)||value.byteWidths.length!==[...value.text].length||
    !value.byteWidths.every(v=>v===1||v===2)||value.byteWidths.reduce((a,b)=>a+b,0)!==value.byteLength))throw Error('语言包字距记录无效');
}
function recordTable(table,namePattern,keyPattern){
  if(!object(table)||Object.keys(table).length>256)throw Error('语言包文字表无效');
  for(const [name,records]of Object.entries(table)){
    if(!namePattern.test(name)||!object(records)||Object.keys(records).length>4096)throw Error('语言包资源名称无效');
    for(const [key,value]of Object.entries(records)){
      if(!keyPattern.test(key))throw Error('语言包文字索引无效');
      checkTextRecord(value);
      if(key.includes(':')&&key.slice(key.indexOf(':')+1)!==value.sourceHex)throw Error('语言包原文索引不匹配');
    }
  }
}
function validateDialoguePatches(overlay){
  const table=overlay.dialoguePatches;
  if(!object(table)||Object.keys(table).length>42||Object.keys(table).length!==Object.keys(overlay.dialogue).length)throw Error('语言包对话文字补丁表无效');
  for(const [name,patch]of Object.entries(table)){
    if(!/^st0[1-7]_0[012][ab]\.msg$/.test(name)||!has(overlay.dialogue,name)||!object(patch)||
      !sha(patch.sourceSha256)||!Array.isArray(patch.entries)||!patch.entries.length||patch.entries.length>256)throw Error('语言包对话来源无效');
    for(let entry=0;entry<patch.entries.length;entry++){
      const anchors=patch.entries[entry]?.anchors;
      if(!Array.isArray(anchors)||!anchors.length||anchors.length>4096)throw Error('语言包对话控制锚点无效');
      let previous=-1;
      for(const anchor of anchors){
        if(!object(anchor)||!integer(anchor.offset,0,1024*1024)||anchor.offset<=previous||
          !Array.isArray(anchor.texts)||anchor.texts.length>64)throw Error('语言包对话锚点范围无效');
        previous=anchor.offset;
        for(const record of anchor.texts){
          checkTextRecord(record);
          if(![15,16,17].includes(record.op)||!integer(record.time,0,65535)||!hex(record.utf8Hex)||record.utf8Hex.length>502)throw Error('语言包只能修改对话文字命令');
          const raw=new TextDecoder('utf-8',{fatal:true}).decode(Uint8Array.from(record.utf8Hex.match(/../g)??[],v=>parseInt(v,16)));
          const plain=record.op===17&&raw.startsWith('|')?raw.match(/^\|(-?\d+),(-?\d+),([\s\S]*)$/):null;
          if((plain?(Number(plain[1])!==record.offset||Number(plain[2])!==record.spacing||plain[3]!==record.text):raw!==record.text)||raw.includes('\0'))throw Error('语言包对话文字编码不匹配');
          const visible=[...new TextEncoder().encode(record.text)].map(v=>v.toString(16).padStart(2,'0')).join('');
          const lookup=overlay.dialogue[name][entry+':'+visible];
          if(visible!==record.sourceHex||!lookup||lookup.text!==record.text||lookup.byteLength!==record.byteLength||
            lookup.offset!==record.offset||lookup.spacing!==record.spacing||JSON.stringify(lookup.byteWidths)!==JSON.stringify(record.byteWidths))throw Error('语言包对话显示记录不匹配');
        }
      }
    }
  }
}
function validateOverlay(value,language){
  if(!object(value)||value.schema!==LOCALIZATION_SCHEMA||value.game!=='th12'||value.language!==language||
    value.sourceRevision!==LOCALIZATION_SOURCE||value.sourceExeSha256!==ORIGINAL_EXE)throw Error('星莲船语言包源版本无效');
  recordTable(value.dialogue,/^st0[1-7]_0[012][ab]\.msg$/,/^(?:0|[1-9]\d{0,2}):[a-f0-9]*$/);
  validateDialoguePatches(value);
  recordTable(value.endings,/^e(?:0\d|1[01])\.msg$/,/^(?:0|[1-9]\d{0,7})$/);
  if(!object(value.spells)||Object.keys(value.spells).length>113)throw Error('语言包符卡表无效');
  for(const [id,record]of Object.entries(value.spells)){
    if(!/^(?:0|[1-9]\d{0,2})$/.test(id)||Number(id)>112)throw Error('语言包符卡编号无效');
    checkTextRecord(record);
  }
  if(!object(value.textures)||Object.keys(value.textures).length>TEXTURES.size)throw Error('语言包贴图表无效');
  for(const [name,item]of Object.entries(value.textures)){
    if(!TEXTURES.has(name)||!object(item)||item.path!==LOCALIZATION_PREFIX+'textures/'+name||item.semantics!=='replace'||
      !integer(item.width,1,8192)||!integer(item.height,1,8192))throw Error('语言包贴图引用无效');
  }
  if(!object(value.musicComments)||Object.keys(value.musicComments).length>17)throw Error('语言包音乐表无效');
  for(const [id,item]of Object.entries(value.musicComments))if(!/^th12_\d\d$/.test(id)||!object(item)||
    !text(item.label)||!text(item.title)||!text(item.comment)||!Array.isArray(item.lines)||
    item.lines.length!==8||!item.lines.every(text)||item.comment!==item.lines.join('\n'))throw Error('语言包音乐说明无效');
  if(value.musicWarning!==undefined&&(!object(value.musicWarning)||
    !['lines','sourceLines'].every(key=>Array.isArray(value.musicWarning[key])&&value.musicWarning[key].length===8&&value.musicWarning[key].every(text))))throw Error('语言包音乐提醒无效');
  return value;
}
function uniqueIndex(records){
  const result=new Map();
  for(const record of records){
    const prior=result.get(record.sourceHex);
    if(!result.has(record.sourceHex))result.set(record.sourceHex,record);
    else if(prior&&JSON.stringify(prior)!==JSON.stringify(record))result.set(record.sourceHex,null);
  }
  return result;
}
function checkPng(data,item){
  const signature=[137,80,78,71,13,10,26,10];
  if(data.length<33||!signature.every((v,i)=>data[i]===v)||
    ![73,72,68,82].every((v,i)=>data[i+12]===v))throw Error('语言包贴图不是 PNG');
  const view=new DataView(data.buffer,data.byteOffset,data.byteLength);
  if(view.getUint32(8)!==13||view.getUint32(16)!==item.width||view.getUint32(20)!==item.height)throw Error('语言包贴图尺寸不匹配');
}
function combine(parts){
  const size=parts.reduce((sum,part)=>sum+part.length,0);
  if(size>16*1024*1024)throw Error('语言对话资源过大');
  const result=new Uint8Array(size);let cursor=0;
  for(const part of parts){result.set(part,cursor);cursor+=part.length;}
  return result;
}
function textCommand(record){
  const raw=Uint8Array.from(record.utf8Hex.match(/../g)??[],v=>parseInt(v,16));
  const length=(raw.length+4)&~3;
  if(length>252)throw Error('语言对话文字过长');
  const output=new Uint8Array(4+length),view=new DataView(output.buffer);
  view.setUint16(0,record.time,true);output[2]=record.op;output[3]=length;
  let key=0x77,step=7;
  for(let i=0;i<length;i++){output[i+4]=(raw[i]??0)^key;key=(key+step)&255;step=(step+16)&255;}
  return output;
}
function rebuildMessage(data,patch){
  if(data.length<12||data.length>1024*1024)throw Error('原版对话文件大小无效');
  const source=new DataView(data.buffer,data.byteOffset,data.byteLength),count=source.getUint32(0,true);
  if(count!==patch.entries.length||count<1||4+8*count>data.length)throw Error('原版对话入口表不匹配');
  const entries=[];
  for(let entry=0;entry<count;entry++){
    let cursor=source.getUint32(4+8*entry,true),anchor=0,ended=false,
      originalInstructions=0,translatedInstructions=0,sourceTextCount=0,sourceTextTime=null;
    const end=entry+1<count?source.getUint32(12+8*entry,true):data.length,parts=[],anchors=patch.entries[entry].anchors;
    if(cursor<4+8*count||cursor>=end||end>data.length)throw Error('原版对话入口范围无效');
    while(cursor+4<=end){
      const op=data[cursor+2],length=4+data[cursor+3];
      if(cursor+length>end)throw Error('原版对话指令范围无效');
      if(++originalInstructions>=4096)throw Error('原版对话入口指令数必须小于4096');
      if([15,16,17].includes(op)){
        const time=source.getUint16(cursor,true);
        if(sourceTextCount&&time!==sourceTextTime)throw Error('原版对话文字组包含多个时间点');
        sourceTextTime=time;sourceTextCount++;
      }else{
        const target=anchors[anchor++];
        if(!target||target.offset!==cursor)throw Error('语言对话控制锚点与原版不匹配');
        // A whole-group insertion/removal can change Ctrl skip's next time
        // even when the following non-text instruction is copied unchanged.
        if(Boolean(sourceTextCount)!==Boolean(target.texts.length))throw Error('语言对话不能增删整个文字组');
        translatedInstructions+=target.texts.length+1;
        if(translatedInstructions>=4096)throw Error('语言对话入口指令数必须小于4096');
        for(const record of target.texts){
          if(record.time!==sourceTextTime)throw Error('语言对话文字时间与原版文字组不匹配');
          parts.push(textCommand(record));
        }
        sourceTextCount=0;sourceTextTime=null;
        // Every non-text instruction, including its time and payload, is
        // copied from the verified Japanese resource, never from the pack.
        parts.push(data.slice(cursor,cursor+length));
      }
      cursor+=length;
      if(op===0){ended=true;break;}
    }
    if(!ended||anchor!==anchors.length)throw Error('语言对话结束标记不匹配');
    parts.push(data.slice(cursor,end));entries.push(combine(parts));
  }
  const header=new Uint8Array(4+8*count),view=new DataView(header.buffer);
  view.setUint32(0,count,true);let offset=header.length;
  for(let entry=0;entry<count;entry++){
    view.setUint32(4+8*entry,offset,true);view.setUint32(8+8*entry,source.getUint32(8+8*entry,true),true);
    offset+=entries[entry].length;
  }
  return combine([header,...entries]);
}

/**
 * manifest is the platform static pack manifest, NOT overlay.json.
 * readFile(path) reads Launcher-installed bytes only. Await this function and
 * the caller's installed-font promise before activate()/Runtime launch.
 * Missing language / ja returns an inert Japanese provider.
 */
export async function prepareLocalization({language='ja',manifest=null,readFile,fontFamily='Eagler Unicode',fontReady}={}){
  if(language==='ja'||language==='lang_ja')return new Localization(null,new Map(),fontFamily);
  if(language!=='lang_zh-hant'||!object(manifest)||manifest.schema!=='eagler-touhou/thcrap-static-pack/1'||
    manifest.game!=='th12'||manifest.language!==language||manifest.runtimeVersion!=='independent'||
    manifest.sourceRevision!==LOCALIZATION_SOURCE||!Array.isArray(manifest.files)||
    !manifest.files.length||manifest.files.length>256||typeof readFile!=='function')throw Error('星莲船静态语言包清单无效');
  if(typeof fontFamily!=='string'||!/^[A-Za-z0-9 _-]{1,80}$/.test(fontFamily))throw Error('已安装语言字体名称无效');
  const installed=new Map();let total=0;
  for(const file of manifest.files){
    const png=file?.path?.startsWith(LOCALIZATION_PREFIX+'textures/')?file.path.slice((LOCALIZATION_PREFIX+'textures/').length):null;
    if(!object(file)||(file.path!==LOCALIZATION_PREFIX+'overlay.json'&&(!png||!TEXTURES.has(png)))||
      installed.has(file.path)||!integer(file.bytes,1,32*1024*1024)||!sha(file.sha256))throw Error('语言包包含不支持的资源');
    total+=file.bytes;if(total>96*1024*1024)throw Error('语言包过大');
    const data=bytes(await readFile(file.path));
    if(data.byteLength!==file.bytes||await hash(data)!==file.sha256)throw Error('语言资源大小或校验不匹配');
    installed.set(file.path,data.slice());
  }
  const raw=installed.get(LOCALIZATION_PREFIX+'overlay.json');
  if(!raw||raw.length>8*1024*1024)throw Error('语言包缺少文字覆盖表');
  const overlay=validateOverlay(JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(raw)),language);
  const declared=new Set([LOCALIZATION_PREFIX+'overlay.json']);
  for(const item of Object.values(overlay.textures)){
    const data=installed.get(item.path);if(!data)throw Error('语言包贴图尚未安装');
    checkPng(data,item);declared.add(item.path);
  }
  if(declared.size!==installed.size)throw Error('语言包存在未声明的资源');
  if(fontReady!==undefined)await fontReady;
  return new Localization(overlay,installed,fontFamily);
}

class Localization {
  constructor(overlay,installed,fontFamily){
    this.overlay=overlay;this.installed=installed;this.fontFamily=overlay?fontFamily:null;
    this.language=overlay?.language??'ja';this.context=null;
    this.dialogue=uniqueIndex(Object.values(overlay?.dialogue??{}).flatMap(records=>Object.values(records)));
    this.spells=uniqueIndex(Object.values(overlay?.spells??{}));
  }
  activate(){active=this;return this;}
  deactivate(){if(active===this)active=null;}
  setContext({stage,character,shot}={}){
    if(!integer(stage,1,7)||!integer(character,0,2)||!integer(shot,0,1)){
      this.context=null;return;
    }
    this.context='st'+String(stage).padStart(2,'0')+'_0'+character+(shot?'b':'a')+'.msg';
  }
  textForHex(kind,source,options={}){
    const key=normalHex(source);if(key===null||!this.overlay)return null;
    let record;
    if(kind==='dialogue'){
      const name=options.resource??this.context,table=this.overlay.dialogue[name];
      if(table)record=integer(options.entry,0,255)?table[options.entry+':'+key]:uniqueIndex(Object.values(table)).get(key);
      // A selected context must not borrow a different route's translation.
      if(!table)record=this.dialogue.get(key);
    }else if(kind==='spell'){
      const card=this.overlay.spells[String(options.cardId)];
      record=card?.sourceHex===key?card:this.spells.get(key);
    }
    return record?{...record,fontFamily:this.fontFamily,localized:true}:null;
  }
  fetchOverride(path){
    if(typeof path!=='string'||!path.startsWith('/assets/')||path.includes('..')||path.includes('\\'))return null;
    const name=path.slice('/assets/'.length);
    if(!TEXTURES.has(name)||!has(this.overlay?.textures??{},name))return null;
    const item=this.overlay.textures[name];
    if(!item)return null;
    return new Response(this.installed.get(item.path).slice(),{headers:{'Content-Type':'image/png'}});
  }
  async patchMessage(path,originalBytes){
    const name=typeof path==='string'?path.match(/^\/assets\/(st0[1-7]_0[012][ab]\.msg)$/)?.[1]:null;
    if(!name||!this.overlay||!has(this.overlay.dialoguePatches,name))return null;
    const data=bytes(originalBytes),patch=this.overlay.dialoguePatches[name];
    if(await hash(data)!==patch.sourceSha256)throw Error('语言补丁与原版对话源文件不匹配');
    return rebuildMessage(data,patch);
  }
  applyMenuResources(base){
    if(!this.overlay)return base;
    if(base?.originalExeSha256!==ORIGINAL_EXE)throw Error('语言覆盖的原版菜单版本不匹配');
    const overlay=this.overlay,translateEnding=ending=>{
      const table=overlay.endings[ending.file];if(!table)return ending;
      return {...ending,commands:ending.commands.map(command=>{
        const record=table[String(command.offset)];
        return command.op===3&&record&&japanese(record.sourceHex)===command.text?
          {...command,text:record.text,localized:true}:command;
      })};
    },spellNames={...base.spellNames};
    for(const [id,record]of Object.entries(overlay.spells))if(spellNames[id]?.title===japanese(record.sourceHex))
      spellNames[id]={...spellNames[id],title:record.text,byteLength:record.byteLength,localized:true};
    const warning=overlay.musicWarning;
    return {...base,localization:{language:this.language,fontFamily:this.fontFamily},spellNames,
      endings:base.endings.map(translateEnding),
      musicComments:base.musicComments.map(comment=>has(overlay.musicComments,comment.id)?
        {...comment,...overlay.musicComments[comment.id],localized:true}:comment),
      musicWarning:warning&&JSON.stringify(base.musicWarning?.lines)===JSON.stringify(warning.sourceLines)?
        {...base.musicWarning,lines:[...warning.lines],localized:true}:base.musicWarning};
  }
}

// Small presentation hooks. The native ECL and all non-text MSG commands stay
// original; only rebuilt dialogue text payloads use the overlay's known UTF-8.
export function localizedHex(kind,source,options){return active?.textForHex(kind,source,options)??null;}
export function localizedFontFamily(){return active?.fontFamily??null;}
