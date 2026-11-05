// TH12 uses the MSG_TH11 / END_TH10 formats in thcrap_tsa/th06_msg.cpp.
// Compile only translated presentation data. Japanese control programs remain
// in the user's prepared content, with verified offsets and source hashes.
import {createHash} from 'node:crypto';
import {execFile} from 'node:child_process';
import {promisify} from 'node:util';
import {readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

const PREFIX='/thcrap/th12/';
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const json=value=>Buffer.from(JSON.stringify(value)+'\n');
const widths=text=>[...text].map(c=>c.codePointAt(0)<128?1:2);
export function th12TextRecord(raw,{formatted=false}={}){
  let text=raw,offset=0,spacing=0,ruby;
  if(formatted&&raw.startsWith('|')){
    const numeric=/^\|(-?\d+),(-?\d+),([\s\S]*)$/.exec(raw);
    if(numeric){offset=Number(numeric[1]);spacing=Number(numeric[2]);text=numeric[3];}
    else{
      const fields=[];let quoted=false,start=1;
      for(let at=1;at<raw.length;at++){
        if(raw[at]==='\t')quoted=!quoted;
        if(raw[at]===','&&!quoted&&fields.length<2){fields.push(raw.slice(start,at));start=at+1;}
      }
      fields.push(raw.slice(start));
      if(fields.length!==3)throw Error('Invalid TH12 thcrap ruby');
      ruby={base:fields[0].replaceAll('\t',''),span:fields[1].replaceAll('\t','')};text=fields[2].replaceAll('\t','');spacing=1;
    }
  }
  // The lookup and core payload use identical visible UTF-8 bytes. Layout
  // directives stay in the text; the presentation renderer interprets them.
  return {text,sourceHex:Buffer.from(text).toString('hex'),byteLength:widths(text).reduce((a,b)=>a+b,0),
    byteWidths:widths(text),offset,spacing,...(ruby?{ruby}:{}),
    utf8Hex:Buffer.from(formatted&&raw.startsWith('|')?`|${offset},${spacing},${text}`:text).toString('hex')};
}
export function th12MessageEntries(input){
  const data=Buffer.from(input),count=data.readUInt32LE(0),entries=[];
  if(count<1||count>256||4+8*count>data.length)throw Error('Invalid TH12 MSG table');
  for(let entry=0;entry<count;entry++){
    let at=data.readUInt32LE(4+8*entry),end=entry+1<count?data.readUInt32LE(12+8*entry):data.length;
    if(at<4+8*count||at>=end||end>data.length)throw Error('Invalid TH12 MSG entry');
    const commands=[];
    while(at+4<=end){
      const time=data.readUInt16LE(at),op=data[at+2],size=data[at+3];
      if(at+4+size>end)throw Error('Truncated TH12 MSG instruction');
      const payload=data.subarray(at+4,at+4+size);let raw;
      if([3,15,16,17].includes(op)){
        const out=[];let key=0x77,step=7;
        for(const b of payload){const value=b^key;key=(key+step)&255;step=(step+16)&255;if(!value)break;out.push(value);}
        raw=Buffer.from(out);
      }
      commands.push({offset:at,time,op,payload,raw});at+=4+size;if(op===0)break;
    }
    if(commands.at(-1)?.op!==0)throw Error('Unterminated TH12 MSG entry');
    entries.push(commands);
  }
  return entries;
}
// Same box keys as thcrap: time_index, reset index on a time change, close
// automatic boxes on speaker/wait opcodes. Keep every timed source text group.
export function th12MessagePatch(original,diff,{ending=false}={}){
  const entries=th12MessageEntries(original),lookups={},patches=[];
  for(let entry=0;entry<entries.length;entry++){
    const commands=entries[entry],replacements=new Map();let time=-1,index=-1,box=null,rubyIndex=0;
    const finish=()=>{
      if(!box)return;
      const lines=diff?.[entry]?.[box.key]?.lines;
      if(lines!==undefined){
        if(!Array.isArray(lines)||!lines.every(s=>typeof s==='string'))throw Error('Invalid TH12 translated lines');
        const groups=[];
        for(const command of box.commands){const last=groups.at(-1);if(last?.time===command.time)last.commands.push(command);else groups.push({time:command.time,commands:[command]});}
        // Automatic dialogue can have more than one timestamp in a box. A
        // translation may change its line count, but must retain each timing node.
        let cursor=0;
        for(let i=0;i<groups.length;i++){
          const group=groups[i],remaining=groups.length-i-1;
          const count=i===groups.length-1?lines.length-cursor:Math.min(group.commands.length,Math.max(1,lines.length-cursor-remaining));
          const texts=lines.slice(cursor,cursor+Math.max(0,count));cursor+=Math.max(0,count);
          if(!texts.length)texts.push('');
          replacements.set(group.commands[0].offset,texts.map(raw=>{
            const record=th12TextRecord(raw,{formatted:!ending});
            // The native ruby slot carries an offset. Reserve negative offsets
            // as data identities so repeated annotations can retain distinct
            // measured base/span placement in the renderer.
            if(record.ruby){record.offset=-4096+rubyIndex++;record.utf8Hex=Buffer.from(`|${record.offset},${record.spacing},${record.text}`).toString('hex');}
            return {...record,time:group.time,op:ending?3:17};
          }));
          for(const command of group.commands.slice(1))replacements.set(command.offset,[]);
        }
      }
      box=null;
    };
    for(const command of commands){
      if(command.time!==time){time=command.time;index=-1;}
      if((ending?[5,6,9]:[7,8,9,11]).includes(command.op))finish();
      if(command.op!==(ending?3:17))continue;
      if(!box)box={key:`${time}_${++index}`,commands:[]};
      box.commands.push(command);
    }
    finish();
    const anchors=[];let pending=[];
    for(const command of commands){
      if((ending?[3]:[15,16,17]).includes(command.op)){
        let records=replacements.get(command.offset);
        if(records===undefined){
          // No patch: preserve the Japanese source payload without ever
          // interpreting CP932 as translated UTF-8.
          records=[{originalOffset:command.offset,time:command.time,op:command.op}];
        }
        for(const record of records){
          if(record.originalOffset===undefined){
            const {time,op,utf8Hex,...lookup}=record;lookups[entry+':'+record.sourceHex+(record.ruby?':'+record.offset+':'+record.spacing:'')]=lookup;
          }
          pending.push(record);
        }
      }else{anchors.push({offset:command.offset,texts:pending,...(!ending&&command.op===25?{skip:true}:{})});pending=[];}
    }
    patches.push({anchors});
  }
  return {lookup:lookups,patch:{sourceSha256:digest(original),entries:patches}};
}

function spellSources(data,extra){
  const result={};if(data.toString('ascii',0,4)!=='SCPT')throw Error('Expected TH12 ECL');
  const count=data.readUInt16LE(16),table=36+data.readUInt16LE(6),offsets=[];
  for(let i=0;i<count;i++)offsets.push(data.readUInt32LE(table+4*i));
  for(const start of offsets){
    const end=Math.min(data.length,...offsets.filter(n=>n>start));
    for(let at=start+16;at+16<=end;){
      const op=data.readUInt16LE(at+4),size=data.readUInt16LE(at+6);
      if(size<16||at+size>end)throw Error('Invalid TH12 ECL instruction');
      if([422,428,437,438,439].includes(op)&&size>=33){
        const base=data.readInt32LE(at+16),length=data.readInt32LE(at+28);
        if(length<1||length>size-32)throw Error('Invalid TH12 spell name');
        const raw=data.subarray(at+32,at+32+length),decoded=[];let key=0x77,step=7;
        for(const b of raw){const c=b^key;key=(key+step)&255;step=(step+16)&255;if(!c)break;decoded.push(c);}
        const hex=Buffer.from(decoded).toString('hex');
        for(const difficulty of extra?[4]:[0,1,2,3])if(data[at+10]&(1<<difficulty)){
          const id=base+(op===437?difficulty:op===438?difficulty-1:op===439?difficulty-2:0);
          if(id>=0&&id<=112)result[id]={sourceHex:hex,minimum:base};
        }
      }
      at+=size;
    }
  }
  return result;
}

export class Th12ThcrapCompiler{
  constructor({runner,archives,python='python',parseJson}){this.runner=runner;this.archives=archives;this.python=python;this.parseJson=parseJson;this.cache=new Map();}
  async base(name){
    if(this.cache.has(name))return this.cache.get(name);
    let error;for(const archive of this.archives){try{const data=Buffer.from(await this.runner.extractArchiveEntry(archive,name,12));this.cache.set(name,data);return data;}catch(e){error=e;}}
    throw error;
  }
  async processPack(resources,{language}={}){
    if(!/^lang_[a-z0-9-]+$/i.test(language??''))throw Error('TH12 language is required');
    const archiveHashes=await Promise.all(this.archives.map(async path=>digest(await readFile(path))));
    const sourceArchiveSha256='dca82df072ba51c4b74c3d41ea1cbdcb91f709ce7cfea23599ee52caa34f2d72';
    if(archiveHashes.some(hash=>hash!==sourceArchiveSha256))throw Error('TH12 thcrap requires the Japanese 1.00b th12.dat');
    const overlay={schema:'th12-localization-overlay/2',game:'th12',language,
      sourceExeSha256:'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417',
      sourceKind:'thcrap',sourceArchiveSha256,dialogue:{},dialoguePatches:{},endings:{},endingPatches:{},spells:{},textures:{},musicComments:{},themes:{}};
    const parsed=new Map(resources.filter(r=>['jdiff','table'].includes(r.kind)).map(r=>[r.path,this.parseJson(r)]));
    for(const resource of resources){
      const name=resource.path.slice(5);
      if(/^st0[1-7]_0[012][ab]\.msg\.jdiff$/.test(name)){
        const source=name.slice(0,-6),result=th12MessagePatch(await this.base(source),parsed.get(resource.path));
        overlay.dialogue[source]=result.lookup;overlay.dialoguePatches[source]=result.patch;
      }else if(/^e(?:0\d|1[01])\.msg\.jdiff$/.test(name)){
        const source=name.slice(0,-6),result=th12MessagePatch(await this.base(source),parsed.get(resource.path),{ending:true});
        overlay.endings[source]={};overlay.endingPatches[source]=result.patch;
      }
    }
    const spells=parsed.get('th12/spells.js')??{},sourceSpells={};
    for(const name of ['stage01','stage02','stage03','stage04','stage05','stage06','stage07','stage07boss'])Object.assign(sourceSpells,spellSources(await this.base(name+'.ecl'),name.startsWith('stage07')));
    for(const [id,{sourceHex,minimum}]of Object.entries(sourceSpells)){
      // thcrap's BP_spell_id/BP_spell_name searches down from the real
      // difficulty ID to the spell's base ID. Sparse tables share those names.
      let translated;for(let candidate=Number(id);candidate>=minimum;candidate--)if(typeof spells[candidate]==='string'){translated=spells[candidate];break;}
      if(translated!==undefined){const {utf8Hex,...record}=th12TextRecord(translated);overlay.spells[id]={...record,sourceHex};}
    }
    const themes=parsed.get('themes.js')??{},music=parsed.get('th12/musiccmt.js')??{};
    // thcrap numbers comments/themes by Music Room order, not the retail
    // OGG filename (the first stage is th12_00, the title is th12_01).
    const trackIds=[...(await this.base('musiccmt.txt')).toString('latin1').matchAll(/^@bgm\/(th12_\d\d)\.wav\r?$/gm)].map(m=>m[1]);
    if(trackIds.length!==17)throw Error('Expected 17 TH12 Music Room tracks');
    for(let n=1;n<=17;n++){
      const id=trackIds[n-1],title=themes['th12_'+String(n).padStart(2,'0')];
      const lines=music[n];if(!lines&&!title)continue;
      if(lines&&(!Array.isArray(lines)||lines.length>8||!lines.every(s=>s===null||typeof s==='string')))throw Error('Invalid TH12 Music Room lines');
      overlay.musicComments[id]={...(typeof title==='string'?{title,label:`No.${n} ${title}`}:{ }),lines:lines??[]};
    }
    const stringdefs={};
    // Shared stringdefs arrive as layered resources with identical source paths.
    for(const r of resources.filter(r=>r.sourceRole==='stringdefs').sort((a,b)=>a.sourceOrder-b.sourceOrder))Object.assign(stringdefs,this.parseJson(r));
    const warning=Array.from({length:8},(_,i)=>i<5?stringdefs['th10 Music Room spoiler '+(i+1)]:'');
    if(warning.every(s=>typeof s==='string'))overlay.musicWarning={lines:warning};
    const output=[];
    await this.runner.withTemporaryDirectory(async directory=>{
      const images=resources.filter(r=>r.kind==='image'&&r.path.startsWith('th12/'));
      const jobs=[];const banks=['ascii','front','title','title_v','staff',...Array.from({length:7},(_,i)=>'st0'+(i+1)+'logo'),...Array.from({length:7},(_,i)=>'stgenm0'+(i+1)),...Array.from({length:12},(_,i)=>'e'+String(i).padStart(2,'0'))];
      const requested=new Map(images.map(r=>[r.path.slice(5),r]));
      for(const bank of banks){
        const data=await this.base(bank+'.anm');let at=0,index=0;
        do{
          if(data.readUInt32LE(at)!==7)throw Error('Expected TH12 ANM v7');
          const nameAt=at+data.readUInt32LE(at+16),name=data.toString('utf8',nameAt,data.indexOf(0,nameAt)),patch=requested.get(name.replace(/^@/,''));
          if(patch&&data.readUInt16LE(at+32)&&data.readUInt32LE(at+28)){
            const input=resolve(directory,bank+'.anm'),png=resolve(directory,jobs.length+'.png'),out=resolve(directory,bank+'-'+index+'.png');
            await writeFile(input,data);await writeFile(png,patch.bytes);jobs.push({input,png,out,index,name:bank+'-'+index+'.png'});
          }
          const next=data.readUInt32LE(at+36);if(!next)break;at+=next;index++;
        }while(at<data.length);
      }
      const jobFile=resolve(directory,'jobs.json');await writeFile(jobFile,json(jobs));
      const result=await promisify(execFile)(this.python,[fileURLToPath(new URL('../scripts/th12-language-textures.py',import.meta.url)),jobFile],{windowsHide:true,maxBuffer:2*1024*1024});
      for(const record of JSON.parse(result.stdout)){
        const path=PREFIX+'textures/'+record.name;
        output.push({path:record.name,targetPath:path,format:'png',bytes:await readFile(record.out)});
        overlay.textures[record.name]={path,width:record.width,height:record.height,semantics:'replace'};
      }
    });
    overlay.provenance=resources.map(r=>({path:r.path,patch:r.patch,crc32:r.crc32}));
    output.push({path:'overlay.json',targetPath:PREFIX+'overlay.json',format:'th12-localization-overlay',bytes:json(overlay)});
    return output;
  }
}
