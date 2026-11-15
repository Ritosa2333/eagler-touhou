import {readFile,readdir,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createStaticThcrapPack} from '../server/thcrap-static-pack.mjs';
const args={};for(let i=2;i<process.argv.length;i++){const arg=process.argv[i],at=arg.indexOf('=');if(at>=0)args[arg.slice(2,at)]=arg.slice(at+1);else args[arg.slice(2)]=process.argv[++i];}
const game=args.game??'th05',language=args.language??'lang_zh-hans';
if(game!=='th05'||language!=='lang_zh-hans'||!args.original||!args.output)throw Error('Use --game th05 --language lang_zh-hans --original DIR --output DIR');
const original=resolve(args.original),output=resolve(args.output),names=await readdir(original),name=names.find(n=>n.toLowerCase()==='chs.dat');
if(!name){console.log(JSON.stringify({game,skipped:true,languages:[]}));}
else{
 const bytes=new Uint8Array(await readFile(resolve(original,name)));if(!bytes.length||bytes.length>16*1024*1024)throw Error('Invalid uth05win Chinese resource');
 const result=createStaticThcrapPack({pack:{game,language,title:'简体中文',assets:[],dependencies:[]},resources:[{targetPath:'/thcrap/th05/chs.dat',bytes,sourcePath:'chs.dat',format:'uth05-mod'}]});
 await mkdir(resolve(output,'language'),{recursive:true});await writeFile(resolve(output,'language',result.fileName),result.archive);await writeFile(resolve(output,'language',language+'.manifest.json'),JSON.stringify(result.manifest,null,2)+'\n');
 await writeFile(resolve(output,'catalog.json'),JSON.stringify({schema:'eagler-touhou/thcrap-static-catalog/1',game,runtimeVersion:'independent',languages:[result.catalog]},null,2)+'\n');
 console.log(JSON.stringify({game,skipped:false,languages:[language]}));
}
