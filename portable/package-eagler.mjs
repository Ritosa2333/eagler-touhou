// Freeze only code and measured font tables. Retail DATA/music stay in Package Store.
import {readFileSync,writeFileSync,mkdirSync,existsSync,readdirSync} from 'node:fs';
import {resolve,dirname,relative} from 'node:path';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'..'),game='th15';
const out=resolve(root,'build-eagler');
const buildRoot=resolve(root,'th15_web',process.env.TH15_OUTPUT||'artifacts/sdl-release');
const fonts=process.env.EAGLER_FONT_ROOT;
if(!fonts)throw Error('Set EAGLER_FONT_ROOT to the measured TH15 font directory');
const hash=b=>createHash('sha256').update(b).digest('hex');
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json'),'utf8'));
if(!Number.isFinite(Date.parse(build.builtAt)))throw Error('Rebuild Runtime with build timestamp');
if(build.kind!=='th15-sdl3-application-release'||!build.completeGame)throw Error('A complete production TH15 build is required');
for(const [path,expected] of Object.entries({...build.sources,...build.headers})){
 if(hash(readFileSync(resolve(root,path)))!==expected)throw Error('Rebuild changed source: '+path);
}
const wasm=readFileSync(resolve(buildRoot,'th15.wasm'));
if(hash(wasm)!==build.sha256)throw Error('Wasm build identity mismatch');
for(const {name} of WebAssembly.Module.exports(new WebAssembly.Module(wasm)))
 if(/^(?:application_|th15_probe|audit_|presentation_lab_)/.test(name))throw Error('Diagnostic export in production: '+name);
const fontNames=[...Array.from({length:8},(_,i)=>`font${i}.bin`),'cp932.bin','blend4444.bin'];
const names=['th15.html','manifest.json','startup-branding.mjs','shell.mjs','managed.css','keyboard.mjs','directory-keyboard.mjs','th15.mjs','th15.wasm','resources.json',...fontNames.map(n=>'fonts/'+n)];
const allowed=new Set([...names,'runtime-files.json']);
function walk(dir){return existsSync(dir)?readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(dir,e.name)):[resolve(dir,e.name)]):[];}
for(const file of walk(out))if(!allowed.has(relative(out,file).replaceAll('\\','/')))throw Error('Unexpected Runtime file: '+file);
function put(name,bytes){const file=resolve(out,name);mkdirSync(dirname(file),{recursive:true});writeFileSync(file,bytes);}
const shellRoot=resolve(root,'th15_web/sdl-runtime');
put('th15.html',readFileSync(resolve(shellRoot,'managed.html'),'utf8').replace('<head>','<head><meta name="eagler-data-provider" content="retail-memory">').replace('src="managed.mjs"','src="shell.mjs"'));
const version='1.0.3-eagler-'+build.sha256.slice(0,12);
const marker="/*TH15_BUILD_INFO*/{version:'development-incomplete',completeGame:false}";
const shell=readFileSync(resolve(shellRoot,'shell.mjs'),'utf8');
if(!shell.includes(marker))throw Error('Missing build-info marker');
put('shell.mjs',shell.replace(marker,JSON.stringify({version,completeGame:true})));
for(const name of ['startup-branding.mjs','managed.css','keyboard.mjs','directory-keyboard.mjs'])put(name,readFileSync(resolve(shellRoot,name)));
const loader=readFileSync(resolve(buildRoot,'th15.mjs'));
put('th15.mjs',loader);put('th15.wasm',wasm);
const resources=fontNames.map(name=>{const bytes=readFileSync(resolve(fonts,name));put('fonts/'+name,bytes);return {path:'/fonts/'+name,url:'./fonts/'+name,bytes:bytes.length};});
put('resources.json',JSON.stringify({schema:'eagler-sdl-resources/1',game,resources},null,2)+'\n');
const features={thprac:build.features?.thprac===true,languages:true,focusHitbox:false};
put('manifest.json',JSON.stringify({game,protocol:'eagler-touhou/1',adapter:'sdl3-eagler',profile:'production',builtAt:build.builtAt,version,features,music:['ogg-stream','ogg-full','none'],execution:{kind:build.kind,sha256:build.sha256,loaderSha256:hash(loader),architecture:build.architecture}},null,2)+'\n');
const files=Object.fromEntries(names.map(name=>{const bytes=readFileSync(resolve(out,name));return [name,{bytes:bytes.length,sha256:hash(bytes)}];}));
put('runtime-files.json',JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game,files},null,2)+'\n');
console.log(JSON.stringify({out,version,files:names.length,wasm:build.sha256}));
