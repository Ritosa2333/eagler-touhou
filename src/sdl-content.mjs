// Platform resource conversion only. The shared C++ host owns every game scene.
// Existing prepared DATA remains usable; raw ANM/ECL/MSG bytes pass through.
const utf8=new TextEncoder();
const sjis=new TextDecoder('shift-jis');
const hex=value=>Uint8Array.from(value?.match(/../g)??[],part=>parseInt(part,16));
const plain=value=>String(value??'').replace(/\{\{[^|{}]*\|([^{}]*)\}\}/g,'$1').replace(/<[^>]*>/g,'').replace(/\\[bBiIuU]/g,'');
class Writer {
  constructor(){this.parts=[];this.length=0;}
  bytes(value){const bytes=typeof value==='string'?utf8.encode(value):new Uint8Array(value);this.parts.push(bytes);this.length+=bytes.length;return this;}
  word(value){const data=new Uint8Array(4);new DataView(data.buffer).setUint32(0,value>>>0,true);return this.bytes(data);}
  real(value){const data=new Uint8Array(4);new DataView(data.buffer).setFloat32(0,value,true);return this.bytes(data);}
  text(value){const data=utf8.encode(plain(value));return this.word(data.length).bytes(data);}
  finish(){const bytes=new Uint8Array(this.length);let at=0;for(const part of this.parts){bytes.set(part,at);at+=part.length;}return bytes;}
}
function platformTables(atlases,sounds,menu,camera){
  const types=atlases.bullet?.types;if(!Array.isArray(types)||types.length!==31||sounds.length!==60)throw Error('SDL 原作数据表不完整');
  const w=new Writer().bytes('TH12META').word(1).real(camera?.near??30).real(camera?.far??1800).word(types.length);
  for(const type of types){w.word(type.script);if(type.colors?.length!==16)throw Error('弹型颜色表不完整');for(const c of type.colors)w.word(c.sprite).word(c.spawnSprite).word(c.cancelSprite);w.real(type.size).word(type.flags);}
  w.word(sounds.length);for(const sound of sounds)w.word(sound.id).word(sound.volume).word(Number(sound.loop)).text(sound.file);
  const warning=menu.musicWarning?.lines??[];w.word(warning.length);for(const line of warning)w.text(line);return w.finish();
}
function musicComments(menu){
  const tracks=menu.musicComments??[],w=new Writer().bytes('TH12MUS1').word(tracks.length);
  for(const track of tracks){w.text(track.id).text(track.label).text(track.title);w.word(track.lines.length);for(const line of track.lines)w.text(line);}return w.finish();
}
function musicFormat(tracks){
  const bytes=new Uint8Array(tracks.length*52),view=new DataView(bytes.buffer);let offset=0;
  tracks.forEach((track,i)=>{const at=i*52,name=utf8.encode(track.id+'.wav');if(name.length>15)throw Error('音乐名称无效');bytes.set(name,at);
    const rate=track.sampleRate??44100,channels=track.channels??2,align=channels*2,end=Math.round(track.loopEnd*rate)*align,start=Math.round(track.loopStart*rate)*align;
    if(!Number.isSafeInteger(end)||start<0||end<=start||channels!==2)throw Error('音乐循环点无效');
    view.setUint32(at+16,offset,true);view.setUint32(at+20,end,true);view.setUint32(at+24,start,true);view.setUint32(at+28,end,true);
    view.setUint16(at+32,1,true);view.setUint16(at+34,channels,true);view.setUint32(at+36,rate,true);view.setUint32(at+40,rate*align,true);view.setUint16(at+44,align,true);view.setUint16(at+46,16,true);offset+=end;
  });return bytes;
}
// Older prepared DATA omitted the original menu/ending ANM files. Rebuild only
// those presentation files from lossless command words; PNGs stay separate.
function presentationAnm(meta){
  const entries=[];
  for(let ei=0;ei<meta.entries.length;ei++){
    const info=meta.entries[ei],sprites=Object.values(meta.sprites).filter(s=>s.entry===ei),scripts=Object.values(meta.scripts).filter(s=>s.entry===ei);
    let at=64+sprites.length*4+scripts.length*8;const name=utf8.encode(info.name+'\0'),nameOffset=at;at=(at+name.length+3)&~3;
    const spriteOffsets=sprites.map(()=>{const offset=at;at+=20;return offset;});
    const scriptOffsets=scripts.map(script=>{const offset=at;at+=Math.max(0,...script.commands.map(c=>c.offset+8+c.ints.length*4))+8;return offset;});
    const bytes=new Uint8Array((at+3)&~3),v=new DataView(bytes.buffer);const u16=(p,n)=>v.setUint16(p,n,true),u32=(p,n)=>v.setUint32(p,n>>>0,true);
    u32(0,7);u16(4,sprites.length);u16(6,scripts.length);u16(10,info.width);u16(12,info.height);u16(14,info.textureFormat??1);u32(16,nameOffset);u16(20,info.xOffset??0);u16(22,info.yOffset??0);bytes.set(name,nameOffset);
    sprites.forEach((s,i)=>{const p=spriteOffsets[i];u32(64+i*4,p);u32(p,s.sourceId??i);[s.x,s.y,s.w,s.h].forEach((n,j)=>v.setFloat32(p+4+j*4,n,true));});
    scripts.forEach((s,i)=>{const p=64+sprites.length*4+i*8,base=scriptOffsets[i];u32(p,s.sourceId??i);u32(p+4,base);let end=base;
      for(const c of s.commands){const at=base+c.offset;u16(at,c.op);u16(at+2,8+c.ints.length*4);v.setInt16(at+4,c.time,true);u16(at+6,c.mask);c.ints.forEach((word,j)=>u32(at+8+j*4,word));end=at+8+c.ints.length*4;}
      u16(end,65535);
    });
    entries.push(bytes);
  }
  for(let i=0;i+1<entries.length;i++)new DataView(entries[i].buffer).setUint32(36,entries[i].length,true);
  const w=new Writer();for(const entry of entries)w.bytes(entry);return w.finish();
}
function endingMessage(ending){
  const w=new Writer().word(1).word(12).word(0);for(const command of ending.commands){
    const data=hex(command.payloadHex);if(data.length>255)throw Error('结局指令过长');const header=new Uint8Array(4),view=new DataView(header.buffer);view.setUint16(0,command.time,true);header[2]=command.op;header[3]=data.length;w.bytes(header).bytes(data);
  }return w.finish();
}
function translationData(provider,original,translated){
  const records=new Map(),ambiguous=new Set();
  const add=(scope,source,target)=>{if(!source||target===undefined||target===null)return;const key=scope+'\0'+source,value=plain(target);if(records.has(key)&&records.get(key)!==value){ambiguous.add(key);return;}records.set(key,value);};
  const overlay=provider?.overlay;
  for(const [resource,table]of Object.entries(overlay?.dialogue??{}))for(const record of Object.values(table))add(resource,sjis.decode(hex(record.sourceHex)),record.text);
  for(const record of Object.values(overlay?.spells??{}))add('',sjis.decode(hex(record.sourceHex)),record.text);
  for(let i=0;i<(original.endings??[]).length;i++){
    const source=original.endings[i],target=translated.endings[i];
    for(const command of source.commands){if(command.op!==3)continue;const replacement=target.commands.find(c=>c.op===3&&c.offset===command.offset);if(replacement?.localized)add('',command.text,replacement.text);}
  }
  const values=[...records].filter(([key])=>!ambiguous.has(key)),w=new Writer().bytes('TH12LOC1').word(values.length);
  for(const [key,value]of values){const at=key.indexOf('\0');w.text(key.slice(0,at)).text(key.slice(at+1)).text(value);}return w.finish();
}
export async function mountSdlContent(core,assets,{progress=()=>{}}={}){
  const FS=core.FS;FS.mkdirTree('/game/overrides');FS.mkdirTree('/game/music');FS.mkdirTree('/savesth12/replay');
  const json=async name=>{const response=await assets.fetch('/assets/'+name);if(!response.ok)throw Error('缺少 SDL 资源：'+name);return response.json();};
  const [atlases,originalMenu,sounds,tracks,background]=await Promise.all(['atlases.json','menu-resources.json','sound-index.json','music-index.json','stage01-background.json'].map(json));
  const menu=assets.localization?.applyMenuResources(originalMenu)??originalMenu;
  const allAtlases={...atlases,...originalMenu.atlases};
  const known=assets.content?.files?new Set([...assets.content.files.keys()].map(path=>path.replace(/^assets\//,''))):null;
  const rawNames=new Set(['default.ecl','stage07boss.ecl',...Array.from({length:6},(_,i)=>'pl0'+(i>>1)+(i%2?'b':'a')+'.sht'),...Array.from({length:4},(_,i)=>'demo'+i+'.rpy')]);
  for(let stage=1;stage<=7;stage++){const n=String(stage).padStart(2,'0');rawNames.add('stage'+n+'.ecl');rawNames.add('stage'+n+'.std');for(let i=0;i<6;i++)rawNames.add('st'+n+'_0'+(i>>1)+(i%2?'b':'a')+'.msg');}
  for(const name of Object.keys(allAtlases))rawNames.add(name+'.anm');
  for(const sound of sounds)rawNames.add(sound.file);
  for(const ending of [...originalMenu.endings,originalMenu.credits])rawNames.add(ending.file);
  let count=0;
  for(const name of rawNames){
    if(known&&!known.has(name))continue;const response=await assets.fetchBase('/assets/'+name);if(!response.ok)continue;
    FS.writeFile('/game/'+name,new Uint8Array(await response.arrayBuffer()));progress(++count,rawNames.size);
  }
  for(const [name,meta]of Object.entries(allAtlases)){
    const missing=!FS.analyzePath('/game/'+name+'.anm').exists;
    if(missing)FS.writeFile('/game/'+name+'.anm',presentationAnm(meta));
    for(const entry of meta.entries){if(!entry.image||!missing&&!assets.localization?.overlay?.textures?.[entry.image])continue;
      const response=await assets.fetch('/assets/'+entry.image);if(!response.ok)throw Error('缺少贴图：'+entry.image);FS.writeFile('/game/overrides/'+entry.image,new Uint8Array(await response.arrayBuffer()));
    }
  }
  for(const ending of [...originalMenu.endings,originalMenu.credits])if(!FS.analyzePath('/game/'+ending.file).exists)FS.writeFile('/game/'+ending.file,endingMessage(ending));
  FS.writeFile('/game/native-tables.bin',platformTables(atlases,sounds,menu,background));
  FS.writeFile('/game/native-music.bin',musicComments(menu));FS.writeFile('/game/thbgm.fmt',musicFormat(tracks));
  FS.writeFile('/game/native-translations.bin',translationData(assets.localization,originalMenu,menu));
  for(const [name,blob]of assets.music)FS.writeFile('/game/music/'+name.split('/').at(-1),new Uint8Array(await blob.arrayBuffer()));
  for(const [name,blob]of assets.fonts)FS.writeFile('/game/'+name.split('/').at(-1),new Uint8Array(await blob.arrayBuffer()));
  return {tracks,files:count};
}
