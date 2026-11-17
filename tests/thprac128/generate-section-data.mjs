import fs from'node:fs';import path from'node:path';import assert from'node:assert/strict';import{createHash}from'node:crypto';
const W='D:/Codex-th128-owned-20261108/menu-lifecycle-fixes-20261113',R='D:/Codex-th128-owned-20261108/github-upload-20261115/th128',H=W+'/case890-thprac-complete-section-caller-red/run',out=process.argv[2];
const sha=b=>createHash('sha256').update(b).digest('hex'),traces=JSON.parse(fs.readFileSync(H+'/upstream-section-writes.json')),catalog=JSON.parse(fs.readFileSync(R+'/docs/thprac128-fixed-section-catalog.json')).sections;
const identityText=fs.readFileSync(R+'/src/practice/Thprac128Source.hpp','utf8'),identities=[...identityText.matchAll(/\{"([^"]+)",(\d+),"([a-f0-9]{64})",(\d+)\}/g)].map(m=>({name:m[1],bytes:+m[2],sha:m[3]}));assert.equal(identities.length,55);
const rawCache=new Map();function asset(variant,name){const key=variant+'/'+name;if(!rawCache.has(key)){const b=fs.readFileSync(H+'/original/'+key);rawCache.set(key,{bytes:b,...parse(b)});}return rawCache.get(key);}
function parse(b){const count=b.readUInt32LE(16),table=36+b.readUInt16LE(6);let pos=table+count*4;const subs=[];
 for(let i=0;i<count;i++){const z=b.indexOf(0,pos),name=b.toString('ascii',pos,z);pos=z+1;subs.push({name,start:b.readUInt32LE(table+i*4)+16,items:[]});}
 const starts=subs.map(x=>x.start-16).sort((a,b)=>a-b);
 for(const s of subs){s.end=starts.find(x=>x>s.start)??b.length;for(let p=s.start;p<s.end;){const n=b.readUInt16LE(p+6);assert(n>=16&&n%4===0&&p+n<=s.end,'base layout');s.items.push(decode(b,p));p+=n;}}
 return{subs};
}
function decode(b,p){let size=b.readUInt16LE(p+6);if(b.readUInt16LE(p+4)===11&&b[p+11]===1){const stringBytes=b.readUInt32LE(p+16);if(stringBytes<=256&&stringBytes%4===0)size=Math.max(size,20+stringBytes);}assert(size>=16&&size%4===0&&p+size<=b.length,'decode');return{abs:p,time:b.readInt32LE(p),op:b.readUInt16LE(p+4),size,mask:b.readUInt16LE(p+8),rank:b[p+10],count:b[p+11],stack:b.readUInt32LE(p+12),words:Array.from({length:(size-16)/4},(_,i)=>b.readUInt32LE(p+16+i*4))};}
function locate(a,off){for(let s=0;s<a.subs.length;s++){const sub=a.subs[s];if(off<sub.start||off>=sub.end)continue;const i=sub.items.findIndex(x=>off>=x.abs&&off<x.abs+x.size);if(i>=0)return{s,i,delta:off-sub.items[i].abs};}throw Error('coordinate '+off.toString(16));}
function map(a,b,off){if(a.bytes.equals(b.bytes))return off;for(let n=0;n<a.subs.length;n++)if(off===a.subs[n].end)return b.subs.find(x=>x.name===a.subs[n].name).end;
 const at=locate(a,off),old=a.subs[at.s],sub=b.subs.find(x=>x.name===old.name);assert(sub&&sub.items.length===old.items.length,'canonical topology '+old.name);const i=sub.items[at.i];assert(at.delta<i.size,'translated field displacement');return i.abs+at.delta;}
function build(name,writes,variant,label){const a=asset('ja',name),b=asset(variant,name),shadow=Buffer.from(a.bytes),streams=[];
 for(const w of writes){assert(w.offset+w.bytes.length<=shadow.length);Buffer.from(w.bytes).copy(shadow,w.offset);if(w.stream)streams.push({start:w.offset,end:w.offset+w.bytes.length});}
 streams.sort((x,y)=>x.start-y.start);const merged=[];for(const s of streams){const last=merged.at(-1);if(last&&s.start<=last.end)last.end=Math.max(last.end,s.end);else merged.push({...s});}
 const regions=[];
 for(const q of merged){const l=locate(a,q.start);assert(l.delta===0,'stream original instruction boundary');const sub=a.subs[l.s],items=[];let p=q.start;
  while(p<q.end){const i=(()=>{try{return decode(shadow,p);}catch(e){throw Error('decode stream '+label+' '+name+' start='+q.start.toString(16)+' p='+p.toString(16)+' end='+q.end.toString(16)+' length='+shadow.readUInt16LE(p+6));}})();assert(p+i.size<=shadow.length,'virtual stream leaves resource');items.push(i);p+=i.size;}
  const end=sub.items.find(x=>x.abs>=p)?.abs??sub.end;
  regions.push({start:q.start,end,sub:sub.name,items});}
 for(const w of writes.filter(x=>!x.stream)){for(let p=w.offset;p<w.offset+w.bytes.length;){const l=locate(a,p),sub=a.subs[l.s],ins=sub.items[l.i];
   const region=regions.find(x=>ins.abs>=x.start&&ins.abs<x.end);
   if(!region){let current=regions.find(x=>x.start===ins.abs);if(!current)regions.push({start:ins.abs,end:ins.abs+ins.size,sub:sub.name,items:[decode(shadow,ins.abs)]});}
   p=Math.min(w.offset+w.bytes.length,ins.abs+ins.size);
  }}
 regions.sort((x,y)=>x.start-y.start);for(let n=1;n<regions.length;n++)assert(regions[n-1].end<=regions[n].start,'region overlap');
 const result=[];for(const q of regions){const start=map(a,b,q.start),end=map(a,b,q.end),sub=b.subs.find(x=>x.name===q.sub);const items=q.items.map(i=>{
    let words=i.words.slice();if(i.op===12||i.op===13||i.op===14){const dest=i.abs+(words[0]|0);let target;
     const generated=regions.find(r=>dest>=r.start&&dest<r.end&&r.items.some(x=>x.abs===dest));
     if(generated)target=map(a,b,generated.start)+(dest-generated.start);else target=map(a,b,dest);
     words[0]=(target-(start+i.abs-q.start))>>>0;
    }
    const op=i.op,empty=op===0;const off=start+i.abs-q.start;assert(off>=sub.start&&off<sub.start+1024*1024,'virtual stream span');
    return{offset:off-sub.start,time:i.time,op,size:empty?16:i.size,mask:empty?0:i.mask,rank:i.rank,count:empty?0:i.count,stack:empty?0:i.stack,words:empty?[]:words};
   });result.push({start,end,items});}
 return result;
}
const encoded=[],rows=[],seen=new Map();let regionsCount=0,instructionsCount=0;
for(const trace of traces){const sec=catalog.find(x=>x.id===trace.section),stageName=['st_a1_1.ecl','st_a1_2.ecl','st_a1_3.ecl','st_a2_2.ecl','st_a2_3.ecl','st_b1_1.ecl','st_b1_2.ecl','st_b1_3.ecl','st_b2_2.ecl','st_b2_3.ecl','st_c1_1.ecl','st_c1_2.ecl','st_c1_3.ecl','st_c2_2.ecl','st_c2_3.ecl','st_ex.ecl'][sec.stage-1];
 const bossName=stageName==='st_ex.ecl'?'st_exb.ecl':stageName.replace('.ecl','b.ecl');for(const file of[...new Set(trace.writes.map(x=>x.file))]){const name=file===0?stageName:bossName,ws=trace.writes.filter(x=>x.file===file);
  for(const variant of['ja','zh']){const b=asset(variant,name),identity=identities.findIndex(x=>x.name===name&&x.sha===sha(b.bytes)&&x.bytes===b.bytes.length);assert(identity>=0,'fixed identity');
   const key=[trace.section,trace.dlg,trace.phase,identity].join(':');if(rows.some(x=>x.key===key))continue;
   const regions=build(name,ws,variant,trace.section+":"+trace.phase+":"+trace.dlg),words=[regions.length];for(const r of regions){words.push(r.start,r.end,r.items.length);regionsCount++;for(const i of r.items){instructionsCount++;words.push(i.offset,i.time>>>0,i.op,i.size,i.mask,i.rank,i.count,i.stack,i.words.length,...i.words);}}
   const payload=words.join(','),previous=seen.get(payload);let at;if(previous!==undefined)at=previous;else{at=encoded.length;encoded.push(...words);seen.set(payload,at);}
   rows.push({key,section:trace.section,dlg:trace.dlg,phase:trace.phase,identity,at,length:words.length});
  }
 }}
assert.equal(traces.length,318);const header='#pragma once\n// Generated from fixed original TH128 source and known original/Chinese assets. MIT thprac attribution retained.\n#include <cstdint>\nnamespace th128::practice::section_data {\nstruct Row {unsigned section,dialogue,phase,identity,begin,count;};\ninline constexpr unsigned stage[157]={0,'+catalog.map(x=>x.stage).join(',')+'};\ninline constexpr Row rows[]={\n'+rows.map(x=>'{'+[x.section,x.dlg,x.phase,x.identity,x.at,x.length].join(',')+'},').join('\n')+'\n};\ninline constexpr std::uint32_t words[]={\n'+Array.from({length:Math.ceil(encoded.length/16)},(_,i)=>encoded.slice(i*16,i*16+16).map(x=>String(x>>>0)+'u').join(',')+',').join('\n')+'\n};\n}\n';
fs.mkdirSync(out,{recursive:false});fs.writeFileSync(path.join(out,'Thprac128SectionData.hpp'),header,{flag:'wx'});fs.writeFileSync(path.join(out,'manifest.json'),JSON.stringify({traceBytes:fs.statSync(H+'/upstream-section-writes.json').size,traceSHA256:sha(fs.readFileSync(H+'/upstream-section-writes.json')),plans:traces.length,rows:rows.length,uniquePayloads:seen.size,words:encoded.length,regionsCount,instructionsCount,headerBytes:Buffer.byteLength(header),headerSHA256:sha(Buffer.from(header))},null,2)+'\n',{flag:'wx'});console.log(JSON.stringify({plans:traces.length,rows:rows.length,words:encoded.length,headerBytes:Buffer.byteLength(header)}));
