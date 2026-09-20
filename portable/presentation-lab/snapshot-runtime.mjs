// Preserve a byte-attested local A/B runtime, never game DATA or user saves.
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'../..'),name=process.argv[2];
if(!/^[a-z0-9-]{1,40}$/.test(name||''))throw Error('Expected snapshot name');
const input=resolve(root,'build-eagler'),out=resolve(root,'artifacts/presentation-lab/builds',name);
if(existsSync(out))throw Error('Snapshot exists; choose another name instead of overwriting evidence');
const index=readFileSync(resolve(input,'runtime-files.json'));
const sha=b=>createHash('sha256').update(b).digest('hex');
for(const [file,info] of Object.entries(JSON.parse(index).files)){
 if(file.includes('..')||file.startsWith('/')||file.includes('\\'))throw Error('Unsafe file');
 const bytes=readFileSync(resolve(input,file));if(sha(bytes)!==info.sha256)throw Error('Stale runtime '+file);
 const to=resolve(out,file);mkdirSync(dirname(to),{recursive:true});writeFileSync(to,bytes);
}
writeFileSync(resolve(out,'runtime-files.json'),index);
const build=readFileSync(resolve(root,'th08_web/artifacts/sdl3/build.json'));writeFileSync(resolve(out,'build.json'),build);
console.log(JSON.stringify({snapshot:name,out,wasm:JSON.parse(build).sha256}));
