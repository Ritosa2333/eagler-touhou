import {readFile,writeFile,mkdir,mkdtemp,rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {createHash} from 'node:crypto';
import {execFile} from 'node:child_process';
import {promisify} from 'node:util';
import {crc32} from '../integrations/thcrap.mjs';
import {unzipSync} from 'fflate';
import {Th075ThcrapCompiler} from '../server/th075-thcrap-compiler.mjs';
import {englishStaticResources} from '../server/th075-english-static.mjs';
import {createStaticThcrapPack} from '../server/thcrap-static-pack.mjs';
import {parseThcrapJson} from '../server/thcrap-compiler.mjs';

const [archive,data,font,output]=process.argv.slice(2);
if(!output)throw Error('Usage: prepare-th075-english-static.mjs <th075e.dat v1.1> <Japanese DATA> <licensed Unifont> <output>');
const source=await readFile(archive),digest=createHash('sha256').update(source).digest('hex');
if(digest!=='d82485758e9ec536656b1f8ba1d6ccc7605c949f39fe407c1bd076f76deb67f9')throw Error('Unrecognized IaMP English v1.1 data archive');
const files=unzipSync(source),temporary=await mkdtemp(resolve(tmpdir(),'th075-english-'));
try{
 // Seven legacy spell-name textures are 512px while the original native
 // slots are 256px. Fit artwork into those slots without changing PAT data,
 // animation coordinates, hitboxes or any game logic.
 const fitted=['data_character_05-730.png','data_character_05-734.png','data_character_07-253.png','data_character_07-254.png','data_character_07-257.png','data_character_07-258.png','data_character_08-297.png'];
 for(const name of fitted)await writeFile(resolve(temporary,name),files[name]);
 await promisify(execFile)('python',['-c',"from PIL import Image; from pathlib import Path; import sys; p=Path(sys.argv[1]); [(lambda im,n: im.resize((256,32),Image.Resampling.LANCZOS).save(n))(Image.open(n),n) for n in p.glob('*.png')]",temporary],{windowsHide:true});
 for(const name of fitted)files[name]=await readFile(resolve(temporary,name));
 const compiler=new Th075ThcrapCompiler({archives:[data],fontFile:font,parseJson:r=>parseThcrapJson(r)});
 const adapted=await englishStaticResources(files,compiler.base);
 for(const path of ['themes.js','th075/musiccmt.js']){
  const response=await fetch('https://srv.thpatch.net/lang_en/'+path);if(!response.ok)throw Error('Unable to read thpatch English '+path);
  adapted.resources.push({game:'th075',path,bytes:Buffer.from(await response.arrayBuffer())});
 }
 const resources=await compiler.processPack(adapted.resources,{language:'lang_en'});
 const options=resources.find(r=>r.path==='localization/options.json');
 options.bytes=Buffer.from(JSON.stringify({...JSON.parse(options.bytes),source:'thcrap + IaMP English v1.1',english:{source:'IaMP English Translation v1.1 (2009-02-28)',archiveSha256:digest,homepage:'http://iamp.mizuumi.net/index.php/English_translation',mirror:'https://www.touhou.pl/?cat=7&site=download/files',credits:{translation:'Touhou Wiki, Akira, sibladeko',script:'Mefidex, TMN',graphics:'Halbarad',programming:'mauve'},reflowedSpeakerGroups:adapted.reflowed,fittedImages:fitted,scope:'Menu, selection, spell and stage artwork; Story, endings, win quotes; Music Room. Japanese controls preserved; no EXE, DLL or PAT patches.'}}));
 const pack={game:'th075',language:'lang_en',title:'English',dependencies:['IaMP English Translation v1.1','thpatch/lang_en'],assets:[{path:'IaMP-English-v1.1/th075e.dat',crc32:0},{path:digest,crc32:0},...adapted.resources.filter(r=>r.path==='themes.js'||r.path==='th075/musiccmt.js').map(r=>({path:r.path,crc32:crc32(r.bytes)}))]};
 const result=createStaticThcrapPack({pack,resources}),root=resolve(output);await mkdir(resolve(root,'language'),{recursive:true});
 await writeFile(resolve(root,'language',result.fileName),result.archive);
 await writeFile(resolve(root,'language/lang_en.manifest.json'),JSON.stringify(result.manifest,null,2)+'\n');
 const catalog=JSON.parse(await readFile(resolve(root,'catalog.json'),'utf8'));catalog.languages=catalog.languages.filter(l=>l.id!=='lang_en');catalog.languages.push(result.catalog);catalog.languages.sort((a,b)=>a.id.localeCompare(b.id));
 await writeFile(resolve(root,'catalog.json'),JSON.stringify(catalog,null,2)+'\n');
 console.log(JSON.stringify({sha256:result.sha256,bytes:result.archive.length,files:result.manifest.files.length,reflowed:adapted.reflowed}));
}finally{await rm(temporary,{recursive:true,force:true});}

