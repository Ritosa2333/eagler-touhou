// Stage a packaged Runtime in an explicitly selected private validation site.
import {cp,mkdir,readFile,stat,writeFile} from 'node:fs/promises';
import {dirname,resolve} from 'node:path';
import {createHash} from 'node:crypto';
const root=resolve(process.env.TH08_RUNTIME_ROOT??resolve(import.meta.dirname,'..'));
const runtime=resolve(process.env.TH08_VALIDATION_RUNTIME??resolve(root,'build-eagler'));
const template=process.env.TH08_VALIDATION_BASE_SITE;
if(!template)throw Error('Set TH08_VALIDATION_BASE_SITE to a prepared Host site with a TH08 runtime entry');
const site=resolve(process.env.TH08_VALIDATION_SITE??resolve(root,'web/artifacts',`local-import-site-${Date.now()}`));
try { await stat(site); throw Error('Use a new TH08_VALIDATION_SITE directory'); }
catch(error) { if(error.code!=='ENOENT')throw error; }
const manifest=JSON.parse(await readFile(resolve(template,'host-manifest.json'),'utf8'));
const url=new URL(manifest.games?.th08?.runtime??'', 'https://validation.local/');
if(url.origin!=='https://validation.local'||!url.pathname.startsWith('/runtime/th08/')||!url.pathname.endsWith('.html'))
    throw Error('Validation Host must declare a relative runtime/th08/*.html entry');
const inventory=JSON.parse(await readFile(resolve(runtime,'runtime-files.json'),'utf8'));
if(!inventory.files||!Object.keys(inventory.files).length)throw Error('Package the candidate Runtime before staging validation');
if(!Object.hasOwn(inventory.files,url.pathname.slice(url.pathname.lastIndexOf('/')+1)))throw Error('Host entry does not match the packaged Runtime');
const inputs=[];
for(const [name,identity] of Object.entries(inventory.files)){
    if(!name||name.includes('..')||name.includes('\\')||name.startsWith('/')||name.includes(':'))throw Error('Invalid Runtime inventory path');
    const bytes=await readFile(resolve(runtime,name));
    if(bytes.length!==identity.bytes||createHash('sha256').update(bytes).digest('hex')!==identity.sha256)throw Error('Stale packaged Runtime: '+name);
    inputs.push(name);
}
await cp(resolve(template),site,{recursive:true});
const destination=resolve(site,'.'+url.pathname.slice(0,url.pathname.lastIndexOf('/')));
for(const name of [...inputs,'runtime-files.json']){
    await mkdir(dirname(resolve(destination,name)),{recursive:true});
    await cp(resolve(runtime,name),resolve(destination,name));
}
await writeFile(resolve(site,'validation-build.json'),await readFile(resolve(runtime,'manifest.json')));
console.log(JSON.stringify({site,runtime,files:inputs.length}));
