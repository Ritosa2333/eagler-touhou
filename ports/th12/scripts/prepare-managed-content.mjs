// Build local original-content DATA separately from distributable Runtime code.
import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
import {DATA_SCHEMA,DATA_MAGIC,DATA_LAYOUT_SPEC} from '../src/managed-data.mjs';
const root=path.resolve(import.meta.dirname,'..');
const argument=(name,fallback)=>process.argv.find(value=>value.startsWith(name+'='))?.slice(name.length+1)??fallback;
const assets=path.resolve(argument('--assets',path.join(root,'site/assets'))),out=path.resolve(argument('--output',path.join(root,'artifacts/eagler-content')));
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const layout='sha256-'+hash(JSON.stringify(DATA_LAYOUT_SPEC));
const files=[],payload=[];let offset=0;
for(const name of fs.readdirSync(assets).sort()){
  if(!/^[a-zA-Z0-9_-]+\.(?:json|png|anm|ecl|std|sht|msg|wav|rpy)$/.test(name))continue;
  if(name.endsWith('.rpy')&&!/^demo[0-3]\.rpy$/.test(name))continue;
  const file=path.join(assets,name);if(!fs.statSync(file).isFile())continue;
  const bytes=fs.readFileSync(file);if(!bytes.length||bytes.length>128*1024*1024)throw Error('资源大小无效：'+name);
  files.push({path:'assets/'+name,offset,bytes:bytes.length,sha256:hash(bytes)});payload.push(bytes);offset+=bytes.length;
}
for(const name of ['loading-resources.json','menu-resources.json','atlases.json','sound-index.json','music-index.json','demo0.rpy','demo1.rpy','demo2.rpy'])if(!files.some(file=>file.path==='assets/'+name))throw Error('请先准备原版资源：'+name);
const manifest=Buffer.from(JSON.stringify({schema:DATA_SCHEMA,game:'th12',layout,files})),header=Buffer.alloc(12);
header.write(DATA_MAGIC,0,'ascii');header.writeUInt32LE(manifest.length,8);
fs.mkdirSync(out,{recursive:true});const data=Buffer.concat([header,manifest,...payload]);fs.writeFileSync(path.join(out,'th12.data'),data);
const music=JSON.parse(fs.readFileSync(path.join(assets,'music-index.json'),'utf8')),musicFiles={};
for(const track of music){
  if(!/^(?:th12_\d\d|th10_17)\.ogg$/.test(track.file))throw Error('音乐名称无效');
  const bytes=fs.readFileSync(path.join(assets,'music',track.file));fs.mkdirSync(path.join(out,'music'),{recursive:true});fs.writeFileSync(path.join(out,'music',track.file),bytes);musicFiles['music/'+track.file]={bytes:bytes.length,sha256:hash(bytes)};
}
fs.writeFileSync(path.join(out,'provenance.json'),JSON.stringify({schema:'th12-prepared-content-provenance/1',game:'th12',dataLayout:layout,gameData:{path:'th12.data',bytes:data.length,sha256:hash(data)},music:musicFiles,files},null,2)+'\n');
console.log(JSON.stringify({content:out,dataBytes:data.length,files:files.length,music:music.length,dataLayout:layout}));
