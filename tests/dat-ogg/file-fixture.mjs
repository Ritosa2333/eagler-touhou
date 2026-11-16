// Small authored THA1/ZIP fixtures. This is format coverage, not retail game data.
import {createHash} from 'node:crypto';
import {buildPackageZip} from '../../tools/package-import-dat-ogg.mjs';
export const origin='https://th128.invalid';
export const rawBytes=new Uint8Array([11,22,33,44,55,66,77,88]);
export const denseBytes=new Uint8Array(4096).fill(73);
export const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const text=value=>new TextEncoder().encode(typeof value==='string'?value:JSON.stringify(value));
const layout='sha256-58cd45fe7cac9af281d9de993a527ccfbbe6f27b89bcd0ca98a7031333fce200';
function authoredDat(){
 const table=new Uint8Array(24),view=new DataView(table.buffer);
 table.set(text('front.anm'));view.setUint32(12,16,true);view.setUint32(16,8,true);
 const bits=[];
 for(const byte of table){bits.push(1);for(let bit=7;bit>=0;bit--)bits.push(byte>>>bit&1);}
 const compressed=new Uint8Array(Math.ceil(bits.length/8));for(let i=0;i<bits.length;i++)compressed[i>>>3]|=bits[i]<<(7-(i&7));
 const header=new Uint8Array(16),hv=new DataView(header.buffer);header.set(text('THA1'));
 hv.setUint32(4,123456789+24,true);hv.setUint32(8,987654321+compressed.length,true);hv.setUint32(12,135792468+1,true);
 const encrypted=new Uint8Array(16);
 for(let i=0;i<8;i++){encrypted[i]=header[15-i*2]^((0x1b+0x37*i)&255);encrypted[i+8]=header[14-i*2]^((0x1b+0x37*(i+8))&255);}
 const result=new Uint8Array(16+rawBytes.length+compressed.length);result.set(encrypted);result.set(rawBytes,16);result.set(compressed,24);return result;
}
export function fixture(changeIndex=null){
 const catalog=text({variants:{zh:{title:'Synthetic fixture'}}});
 const index={schema:'th128-dat-resource-map/1',variant:'zh',dataFile:'game-data',dataLayout:layout,
  raw:{'front.anm':{bytes:8,sha256:sha(rawBytes)}},
  resources:{
   '/assets/catalog.json':{kind:'entry',entry:'assets/catalog.json',bytes:catalog.length,sha256:sha(catalog)},
   '/assets/runtime/front.anm':{kind:'raw',name:'front.anm',bytes:8,sha256:sha(rawBytes)},
   '/assets/dense.zip':{kind:'entry',entry:'assets/dense.zip',bytes:denseBytes.length,sha256:sha(denseBytes)},
   '/assets/music/first.ogg':{kind:'music',fileId:'music-00',bytes:8,sha256:sha(new Uint8Array([79,103,103,83,0,0,0,0]))}
  }};
 if(changeIndex)changeIndex(index);
 const language=buildPackageZip([{name:'runtime-assets.json',bytes:text(index)},{name:'assets/catalog.json',bytes:catalog},{name:'assets/dense.zip',bytes:denseBytes}]).zip;
 const files=new Map([['game-data',authoredDat()],['language-zh',new Uint8Array(language)]]);
 const descriptor={schema:'eagler-touhou/package/1',game:'th128',revision:'authored-files-fixture-1',
  runtimeRequirement:{protocol:'eagler-touhou/1',target:'th128',dataFile:'game-data',dataLayout:layout},files:{},
  base:{files:['game-data']},components:{ogg:{type:'ogg',files:[]},language:{type:'language',entries:[{id:'lang_zh-hans',file:'language-zh'}]}}};
 function declare(id,source,target,body){descriptor.files[id]={source,target,bytes:body.length,sha256:sha(body)};}
 declare('game-data','games/th128/th128.dat','/th128.dat',files.get('game-data'));
 declare('language-zh','games/th128/language/lang_zh-hans.zip','/__eagler/language/lang_zh-hans.zip',files.get('language-zh'));
 for(let i=0;i<10;i++){
  const id='music-'+String(i).padStart(2,'0'),name='th128_'+String(i).padStart(2,'0')+'.ogg',body=new Uint8Array([79,103,103,83,0,i,0,0]);
  files.set(id,body);declare(id,'games/th128/music/ogg/'+name,'/music/'+name,body);descriptor.components.ogg.files.push(id);
 }
 return {descriptor,files,descriptorBytes:text(descriptor),index,catalog};
}
export function refreshDescriptor(f){f.descriptorBytes=text(f.descriptor);return f;}
export function packageZip(f){
 return new Uint8Array(buildPackageZip([{name:'package.json',bytes:f.descriptorBytes},...Object.entries(f.descriptor.files).map(([id,row])=>({name:row.source,bytes:f.files.get(id)}))]).zip);
}
