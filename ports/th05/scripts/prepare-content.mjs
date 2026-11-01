import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
import {readFile,readdir,mkdir,writeFile,stat} from 'node:fs/promises';
import {resolve,join} from 'node:path';
import {createHash} from 'node:crypto';
import {encodeContent} from '../web/data.mjs';
import {ORIGINAL_BGM,AKYU_BGM,ORIGINAL_SFX,TH05_MUSIC_URLS,musicFileName} from '../web/audio-layout.mjs';
const args=Object.fromEntries(process.argv.slice(2).map(arg=>{const at=arg.indexOf('=');if(!arg.startsWith('--')||at<3)throw Error('Use --original=DIR --output=DIR [--resources=DIR] [--pr-audio=DIR]');return [arg.slice(2,at),arg.slice(at+1)];}));
if(!args.original)throw Error('Provide --original=DIR containing kaiki1.dat, kaiki2.dat and zun.com');
const original=resolve(args.original),resources=resolve(args.resources??original),output=resolve(args.output??'artifacts/content');
const find=async(root,name,required=false)=>{const names=await readdir(root).catch(()=>[]),actual=names.find(p=>p.toLowerCase()===name.toLowerCase());if(!actual){if(required)throw Error('Missing '+join(root,name));return null;}return readFile(join(root,actual));};
const files=new Map();
const hashes={'kaiki1.dat':'cb6e2344aa93312c3cf40bf16f3a14f2','kaiki2.dat':'ca20e3f03946c173a02a62f37bacafd1','zun.com':'0005e6571252bed38d416799c0053c0b'};
const originals=new Map();
for(const [name,md5] of Object.entries(hashes)){
 let actual=(await readdir(original)).find(path=>path.toLowerCase()===name);
 // Original PC-98 distributions also use Japanese archive names. Identity,
 // rather than the host filesystem's code page, selects those archives.
 if(!actual&&name.endsWith('.dat'))for(const candidate of await readdir(original)){if(!/\.dat$/i.test(candidate))continue;const bytes=await readFile(join(original,candidate));if(createHash('md5').update(bytes).digest('hex')===md5){actual=candidate;break;}}
 if(!actual)throw Error('Missing original TH05 resource: '+name);const bytes=await readFile(join(original,actual));if(createHash('md5').update(bytes).digest('hex')!==md5)throw Error('Original game resource digest mismatch: '+name);files.set(name,bytes);originals.set(name,join(original,actual));
}
for(const name of ['anex86.bmp','practice.bmp','replay.bmp','chs.dat','music.dat','music-font.bmp']){const bytes=await find(resources,name)??await find(original,name);if(bytes)files.set(name,bytes);}
const audio=resolve(args['pr-audio']??fileURLToPath(new URL('../artifacts/pr143-audio',import.meta.url)));
try{await stat(join(audio,'source.json'));}catch{
 const imported=spawnSync(process.execPath,[fileURLToPath(new URL('./import-pr143-audio.mjs',import.meta.url)),'--output='+audio],{stdio:'inherit',windowsHide:true});if(imported.error)throw imported.error;if(imported.status)throw Error('PR #143 audio import failed');
}
async function verified(path,expected){const bytes=await readFile(path);if(bytes.length!==expected.bytes||createHash('sha256').update(bytes).digest('hex')!==(expected.audioSha256??expected.sha256))throw Error('PR #143 audio identity mismatch: '+path);return bytes;}
files.set('sfx/effects.pcm',await verified(join(audio,'sfx/effects.pcm'),ORIGINAL_SFX));
await mkdir(output,{recursive:true});await writeFile(join(output,'th05.data'),await encodeContent(files));
await mkdir(join(output,'music'),{recursive:true});
const music=[];
for(const url of TH05_MUSIC_URLS){const track=Object.values({...ORIGINAL_BGM.tracks,...AKYU_BGM.tracks}).find(track=>track.url===url)??Object.values(ORIGINAL_BGM.tracks).find(track=>track.url===url);if(!track)throw Error('Missing PR track metadata: '+url);const bytes=await verified(join(audio,'bgm',url),track),file=musicFileName(url);await writeFile(join(output,'music',file),bytes);music.push({source:url,file,bytes:bytes.length,sha256:track.audioSha256});}
await writeFile(join(output,'provenance.json'),JSON.stringify({schema:'eagler-touhou/uth05-content-provenance/1',audio:JSON.parse(await readFile(join(audio,'source.json'),'utf8')),music,files:Object.fromEntries([...files].map(([name,bytes])=>[name,{bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')}]))},null,2)+'\n');
console.log('Prepared '+join(output,'th05.data')+' ('+(await stat(join(output,'th05.data'))).size+' bytes)');

const artwork=spawnSync(process.env.EAGLER_CONTENT_PYTHON??process.env.TH_PYTHON??(process.platform==='win32'?'python':'python3'),[fileURLToPath(new URL('./prepare-artwork.py',import.meta.url)),'--archive',originals.get('kaiki1.dat'),'--output',join(output,'artwork/th05-card.webp')],{stdio:'inherit',windowsHide:true});if(artwork.error)throw artwork.error;if(artwork.status)throw Error('TH05 artwork preparation failed');
