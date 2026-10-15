import {readFile, writeFile, mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {inflateSync} from 'node:zlib';
import {execFile} from 'node:child_process';
import {promisify} from 'node:util';
import {crc32} from '../integrations/thcrap.mjs';

const run = promisify(execFile);
const jsonBytes = value => Buffer.from(JSON.stringify(value)+'\n');
const u32 = n => {const b=Buffer.alloc(4);b.writeUInt32LE(n);return b;};
export function encodeTh075Pixels(raw){
  if(raw.length%4)throw Error('Invalid BGRA pixels');
  const runs=Buffer.alloc(raw.length*2);let at=0,count=0,previous=0;
  const flush=()=>{if(count){runs.writeUInt32LE(count,at);runs.writeUInt32LE(previous,at+4);at+=8;}};
  for(let i=0;i<raw.length;i+=4){const color=raw.readUInt32LE(i);if(count&&color!==previous){flush();count=0;}previous=color;count++;}flush();
  return at<raw.length?{packed:at,bytes:runs.subarray(0,at)}:{packed:0,bytes:raw};
}
export function cryptTh075(source, key=99, step=98, delta=66) {
  const bytes=Buffer.from(source);
  for(let i=0;i<bytes.length;i++){bytes[i]^=key;key=(key+step)&255;step=(step+delta)&255;}
  return bytes;
}
export function th075QuotedRecords(source, music=false) {
  const plain=cryptTh075(source,...(music?[0x5c,0x5a,0x3d]:[99,98,66]));
  const records=[];let label=-1,record=-1,quote=0;
  for(const line of plain.toString('latin1').split(/\r?\n/)){
    if(line.trimStart().startsWith('#'))continue;
    if(music){for(const match of line.matchAll(/"([^"\r\n]*)"/g))records.push({quote:quote++,text:Buffer.from(match[1],'latin1')});continue;}
    const block=/^\s*B(\d+)/.exec(line);if(block){label=Number(block[1]);record=-1;continue;}
    const command=/^\s*([PENFMWVR])/.exec(line);if(!command)continue;record++;
    const text=/"([^"\r\n]*)"/.exec(line);if(text)records.push({quote:quote++,label,record,command:command[1],text:Buffer.from(text[1],'latin1')});
  }
  return records;
}
function bitmapRecords(bytes) {
  const paletteEnd=1+bytes[0]*512;if(paletteEnd>bytes.length)throw Error('Truncated TH075 palette');
  const records=[];let at=paletteEnd;
  while(at<bytes.length){
    if(at+17>bytes.length)throw Error('Truncated TH075 bitmap');
    const width=bytes.readUInt32LE(at),height=bytes.readUInt32LE(at+4),stride=bytes.readUInt32LE(at+8),depth=bytes[at+12],packed=bytes.readUInt32LE(at+13);
    if(!width||!height||width>8192||height>8192||stride<width||![8,16,24,32].includes(depth))throw Error('Invalid TH075 bitmap');
    const end=at+17+(packed||stride*height*(depth>=24?4:depth/8));if(end>bytes.length)throw Error('Truncated TH075 bitmap payload');
    records.push({width,height});at=end;
  }
  return records;
}
export class Th075BaseResources {
  constructor(paths){this.paths=paths;this.editions=new Map();this.ready=null;}
  async initialize(){
    if(this.ready)return this.ready;
    return this.ready=(async()=>{for(const path of this.paths){
      const bytes=await readFile(path);
      if(bytes.subarray(0,8).toString()==='E75DATA1'){
        const n=bytes.readUInt32LE(8),table=JSON.parse(bytes.subarray(12,12+n));
        for(const file of table.files){const match=/^\/resources\/(base|patched)\/(.+)\.z$/.exec(file.path);if(match)this.editions.set(match[1]+'/'+match[2],()=>inflateSync(bytes.subarray(12+n+file.offset+4,12+n+file.offset+file.bytes)));}
      }else{
        const count=bytes.readUInt16LE(0),end=2+count*108;if(end>bytes.length)throw Error('Invalid TH075 archive index');
        const table=cryptTh075(bytes.subarray(2,end),0x64,0x64,0x4d),edition=/th075b\.dat$/i.test(path)?'patched':'base';
        for(let i=0;i<count;i++){const at=i*108,nameBytes=table.subarray(at,at+100),zero=nameBytes.indexOf(0);if(zero<0)throw Error('Invalid TH075 archive name');const name=new TextDecoder('shift_jis',{fatal:true}).decode(nameBytes.subarray(0,zero)).replaceAll('\\','/'),size=table.readUInt32LE(at+100),offset=table.readUInt32LE(at+104);if(offset<end||offset+size>bytes.length)throw Error('Invalid TH075 archive range');this.editions.set(edition+'/'+name,()=>Buffer.from(bytes.subarray(offset,offset+size)));}
      }
    }})();
  }
  async read(path){
    await this.initialize();
    // Follow CharacterFactory and Application's actual resource editions.
    // A base-only bank must not accidentally bind to a newer patch-bank CRC.
    const sprite=/^data\/character\/(\d{2})(2)?\.dat$/.exec(path);
    const patched=path.endsWith('.sce')||path==='musicroom.dat'||/^data\/system\/(select|selectchar|selectbg|replay)\.dat$/.test(path)||
      (sprite&&(sprite[2]?[4,5,6,7,8,10].includes(Number(sprite[1])):[3,10].includes(Number(sprite[1]))));
    const get=(patched&&this.editions.get('patched/'+path))||this.editions.get('base/'+path);
    if(!get)throw Error('Missing TH075 original resource: '+path);return get();
  }
}

// Compiled packs contain replacements only, never copies of unmodified retail
// banks/scripts. CRC/size bind each replacement to its Japanese resource.
export class Th075ThcrapCompiler {
  constructor({archives=[],fontFile,fontLicense,python='python',parseJson=resource=>JSON.parse(Buffer.from(resource.bytes).toString('utf8'))}={}){
    this.base=new Th075BaseResources(archives);this.fontFile=fontFile;this.fontLicense=fontLicense;this.python=python;this.parseJson=parseJson;
  }
  async processPack(resources){
    const images=new Map(),text=new Map(),unsupported=[],glyphs=new Map();
    for(let c=32;c<127;c++)glyphs.set(String.fromCharCode(c),c);
    // Both bytes stay above ASCII: compiled quotes/commas cannot accidentally
    // terminate the original script/CSV parser. Markers select this glyph bank.
    const encode=value=>{
      const bytes=[1];for(const ch of value.normalize('NFC').replaceAll('\u00a0',' ')){
        if(ch==='\n'){bytes.push(92,110);continue;}
        if(ch.codePointAt(0)<32)throw Error('Unsupported localization control character');
        let code=glyphs.get(ch);if(code===undefined||['"',',','\\'].includes(ch)){
          if(code===undefined||code<256){const n=[...glyphs.values()].filter(n=>n>=256).length;if(n>=126*127)throw Error('Too many TH075 glyphs');code=((0x81+Math.floor(n/127))<<8)|(0x80+n%127);glyphs.set(ch,code);}
        }
        if(code<256)bytes.push(code);else bytes.push(code>>8,code&255);
      }bytes.push(2);return Buffer.from(bytes);
    };
    const replacement=async(path,quote,value,prefix=Buffer.alloc(0))=>{
      let item=text.get(path);if(!item){const base=await this.base.read(path);item={base,entries:new Map()};text.set(path,item);}
      item.entries.set(quote,Buffer.concat([prefix,encode(value)]));
    };
    const themes=resources.find(r=>r.path==='themes.js');
    if(themes){const table=this.parseJson(themes),keys=Object.keys(table).filter(key=>/^th075_\d+$/.test(key));if(keys.length){const rows=th075QuotedRecords(await this.base.read('musicroom.dat'),true);for(const key of keys){const at=(Number(key.slice(6))-1)*3;if(rows[at]&&typeof table[key]==='string')await replacement('musicroom.dat',rows[at].quote,table[key]);}}}
    for(const resource of resources){
      const picture=/^th075\/(system\/[a-z0-9_]+|character\/[a-z0-9_/]+)\/(\d+)\.png$/i.exec(resource.path);
      if(picture){const path='data/'+picture[1]+'.dat',index=Number(picture[2]);let bank=images.get(path);if(!bank){const base=await this.base.read(path);bank={base,records:bitmapRecords(base),patches:new Map()};images.set(path,bank);}if(!bank.records[index])throw Error(resource.path+': bitmap index unavailable');bank.patches.set(index,resource);continue;}
      const scenario=/^th075\/character\/([a-z]+)\/scenario\.txt\.jdiff$/i.exec(resource.path);
      if(scenario){
        const path=`data/character/${scenario[1]}/${scenario[1]}.sce`,base=await this.base.read(path),records=th075QuotedRecords(base);
        // thpatch scenario.txt is the ordered ending-page transcript. Color
        // prefixes identify those pages, rather than guessing story labels.
        const pages=records.filter(r=>r.label<100&&/^\\c[0-9a-f]{6}/i.test(r.text.toString('latin1'))),patch=this.parseJson(resource);
        for(const [key,value]of Object.entries(patch)){
          if(!/^\d+$/.test(key)||!Array.isArray(value?.lines)||!value.lines.every(s=>typeof s==='string'))throw Error(resource.path+': unsupported scenario entry');
          const page=pages[Number(key)-1];if(!page){unsupported.push({path:resource.path,key,reason:'No corresponding Japanese ending page'});continue;}
          await replacement(path,page.quote,value.lines.join('\n'),page.text.subarray(0,8));
        }continue;
      }
      if(resource.path==='th075/musiccmt.js'){
        const path='musicroom.dat',base=await this.base.read(path),records=th075QuotedRecords(base,true),patch=this.parseJson(resource);
        for(const [key,lines]of Object.entries(patch)){
          if(!/^\d+$/.test(key)||!Array.isArray(lines)||!lines.every(s=>typeof s==='string'))throw Error('Unsupported TH075 music comment');
          const at=(Number(key)-1)*3;if(!records[at+2])throw Error('TH075 music comment index unavailable');
          await replacement(path,records[at+2].quote,lines.slice(1).join('\n'));
          if(lines[0]&&lines[0]!=='@')await replacement(path,records[at].quote,lines[0]);
        }continue;
      }
      if(!['themes.js','stringdefs.js','global.js','th075.js'].includes(resource.path)&&!resource.path.startsWith('th075/fonts/'))unsupported.push({path:resource.path,reason:'No TH075 data adapter'});
    }
    if(!images.size&&!text.size)throw Error('No supported TH075 localization resources');
    if(!this.fontFile)throw Error('TH075 language compilation requires an explicit licensed font file');
    const temporary=await mkdtemp(resolve(tmpdir(),'eagler-th075-language-'));
    try{
      const jobs=[];let serial=0;
      for(const [path,bank]of images)for(const [index,resource]of bank.patches){const input=`image-${serial}.png`,output=`image-${serial++}.bin`;await writeFile(resolve(temporary,input),resource.bytes);jobs.push({input,output,path,index});}
      await writeFile(resolve(temporary,'job.json'),jsonBytes({images:jobs,font:this.fontFile,glyphs:[...glyphs].map(([character,code])=>({character,code}))}));
      await run(this.python,[resolve(import.meta.dirname,'../scripts/th075-language-assets.py'),resolve(temporary,'job.json')],{maxBuffer:4*1024*1024,windowsHide:true});
      const result=[],emit=(path,bytes,format)=>result.push({game:'th075',path,targetPath:'/thcrap/th075/'+path,bytes,format});
      for(const [path,bank]of images){const records=[];for(const job of jobs.filter(j=>j.path===path)){const data=await readFile(resolve(temporary,job.output)),width=data.readUInt32LE(0),height=data.readUInt32LE(4),expected=bank.records[job.index];if(width!==expected.width||height!==expected.height)throw Error(`${path}#${job.index}: localized image dimensions differ (${width}x${height}, expected ${expected.width}x${expected.height})`);const encoded=encodeTh075Pixels(data.subarray(8));records.push(Buffer.concat([u32(job.index),u32(width),u32(height),u32(encoded.packed),u32(encoded.bytes.length),encoded.bytes]));}emit('images/'+path+'.bin',Buffer.concat([Buffer.from('E75I'),u32(2),u32(crc32(bank.base)),u32(bank.base.length),u32(records.length),...records]),'th075-image-replacements/2');}
      for(const [path,item]of text){const entries=[...item.entries].sort(([a],[b])=>a-b).map(([index,bytes])=>Buffer.concat([u32(index),u32(bytes.length),bytes]));emit('text/'+path+'.bin',Buffer.concat([Buffer.from('E75T'),u32(1),u32(crc32(item.base)),u32(item.base.length),u32(entries.length),...entries]),'th075-text-replacements/1');}
      emit('fonts/localized-glyphs.bin',await readFile(resolve(temporary,'glyphs.bin')),'th075-glyphs/1');
      let license;if(this.fontLicense)license=await readFile(this.fontLicense);else{try{license=await readFile(new URL('../public/assets/fonts/OFL-Unifont.txt',import.meta.url));}catch{license=await readFile(new URL('../assets/fonts/OFL-Unifont.txt',import.meta.url));}}
      emit('fonts/LICENSE.txt',license,'text');
      emit('localization/options.json',jsonBytes({schema:'th075-localization/1',imageBanks:images.size,textResources:text.size,glyphs:glyphs.size,unsupported,source:'thcrap',storyDialogue:'Japanese fallback where the selected patch has no dialogue resource'}),'json');
      return result;
    }finally{await rm(temporary,{recursive:true,force:true});}
  }
}
