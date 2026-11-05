import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {existsSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve,relative,isAbsolute,dirname} from 'node:path';
const option=name=>{const i=process.argv.indexOf(name);return i<0?null:process.argv[i+1];};
const file=option('--corpus'),repository=resolve(option('--repository-root')||process.cwd());
if(!file)throw Error('Use --corpus corpus.json [--repository-root directory]');
const corpus=JSON.parse(await readFile(file,'utf8')),sha=b=>createHash('sha256').update(b).digest('hex');
for(const fixture of corpus.cases.filter(f=>f.kind!=='demo')){
  const out=resolve(repository,fixture.source),outside=relative(repository,out);
  if(outside.startsWith('..')||isAbsolute(outside))throw Error('Fixture escapes repository');
  if(existsSync(out)){if(sha(await readFile(out))!==fixture.replaySha256)throw Error('Preserving different existing fixture: '+fixture.id);console.log('Verified:',fixture.id);continue;}
  if(!fixture.sourceUrl||!/^https:\/\//.test(fixture.sourceUrl))throw Error('Missing HTTPS source: '+fixture.id);
  const response=await fetch(fixture.sourceUrl);if(!response.ok)throw Error('Download failed: '+response.status);
  const bytes=Buffer.from(await response.arrayBuffer());if(bytes.length>64*1024*1024||sha(bytes)!==fixture.replaySha256)throw Error('Fixture hash/size mismatch: '+fixture.id);
  await mkdir(dirname(out),{recursive:true});await writeFile(out,bytes,{flag:'wx'});console.log('Fetched:',fixture.id);
}
