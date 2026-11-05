import {mkdir,readFile,copyFile} from 'node:fs/promises';
import {existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const arg=name=>process.argv.find(value=>value.startsWith('--'+name+'='))?.slice(name.length+3);
if(!arg('original')||!arg('output'))throw Error('Use --original=PREPARED_CONTENT_ROOT --output=DIR');
const original=resolve(arg('original')),output=resolve(arg('output'));
const prepared=existsSync(resolve(original,'th03.data'))?original:resolve(original,'content');
const bytes=await readFile(resolve(prepared,'th03.data'));
if(bytes.length<14||bytes.length>1024*1024*1024||bytes.subarray(0,8).toString()!=='EPC98PK1')throw Error('Invalid TH03 DATA header');
const size=bytes.readUInt32LE(8),base=12+size;
if(size<2||size>4*1024*1024||base>bytes.length)throw Error('Invalid TH03 DATA manifest size');
const manifest=JSON.parse(bytes.subarray(12,base));
if(manifest.schema!=='eagler-touhou/pc98-content/1'||manifest.game!=='th03'||!Array.isArray(manifest.files)||!manifest.files.length||manifest.files.length>10000)throw Error('Invalid TH03 DATA identity');
const seen=new Set();let end=0;
for(const file of manifest.files){
  if(typeof file.path!=='string'||!file.path||/[\\:\0?#]/.test(file.path)||file.path.split('/').some(part=>!part||part==='.'||part==='..')||seen.has(file.path)||file.offset!==end||!Number.isSafeInteger(file.bytes)||file.bytes<=0||base+end+file.bytes>bytes.length)throw Error('Invalid TH03 DATA entry');
  seen.add(file.path);
  const hash=createHash('sha256').update(bytes.subarray(base+end,base+end+file.bytes)).digest('hex');
  if(hash!==file.sha256)throw Error('Corrupt TH03 DATA asset: '+file.path);
  end+=file.bytes;
}
if(base+end!==bytes.length)throw Error('Invalid TH03 DATA length');
await mkdir(output,{recursive:true});await copyFile(resolve(prepared,'th03.data'),resolve(output,'th03.data'));
if(existsSync(resolve(prepared,'source.json')))await copyFile(resolve(prepared,'source.json'),resolve(output,'source.json'));
console.log('Prepared verified TH03 content: '+output);
