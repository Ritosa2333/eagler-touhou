// Format adapter: TH075 uses prepared zlib resources and measured glyph tables.
// Shared Host orchestration calls the same --original/--output interface as other preparers.
import {existsSync,readFileSync,copyFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {workspacePath} from '../lib/workspace-layout.mjs';
import {PRODUCT_CONTENT} from '../lib/content-definition.mjs';
const arg=name=>process.argv.find(v=>v.startsWith('--'+name+'='))?.slice(name.length+3);
if(!arg('original')||!arg('output'))throw Error('Use --original=DEVELOPMENT_ROOT --output=DIR');
const original=resolve(arg('original')),output=resolve(arg('output'));
const prepared=existsSync(resolve(original,'th075.data'))?original:resolve(original,'assets-ogg');
if(!existsSync(resolve(prepared,'th075.data'))){
  const assets=existsSync(resolve(original,'th075_web/assets'))?resolve(original,'th075_web/assets'):original;
  const result=spawnSync(process.execPath,[workspacePath('th075','portable/prepare-content.mjs'),'--assets='+assets,'--output='+output],{stdio:'inherit',windowsHide:true});
  if(result.error)throw result.error;if(result.status!==0)throw Error('TH075 content preparation failed');
}else{
  const provenance=JSON.parse(readFileSync(resolve(prepared,'provenance.json'),'utf8'));
  if(provenance.schema!=='eagler-touhou/th075-content/1'||provenance.data.layout!==PRODUCT_CONTENT.th075.dataLayout)throw Error('TH075 prepared content layout mismatch');
  const rows=[{path:'th075.data',...provenance.data},...provenance.music];
  const declared=PRODUCT_CONTENT.th075.music.ogg.files;
  if(JSON.stringify(provenance.music.map(f=>f.path.slice(6)))!==JSON.stringify(declared))throw Error('TH075 prepared music set mismatch');
  for(const file of rows){
    if(!/^(?:th075\.data|music\/[a-z0-9_]+\.ogg)$/.test(file.path))throw Error('Unsafe prepared content path');
    const source=resolve(prepared,file.path),bytes=readFileSync(source);
    if(bytes.length!==file.bytes||createHash('sha256').update(bytes).digest('hex')!==file.sha256)throw Error('Corrupt TH075 content: '+file.path);
  }
  mkdirSync(resolve(output,'music'),{recursive:true});
  for(const file of [...rows,{path:'provenance.json'}])copyFileSync(resolve(prepared,file.path),resolve(output,file.path));
}
console.log(JSON.stringify({game:'th075',output}));
