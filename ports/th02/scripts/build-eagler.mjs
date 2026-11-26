import {createHash} from 'node:crypto';
import {copyFile,mkdir,mkdtemp,readFile,rename,rm,writeFile} from 'node:fs/promises';
import {dirname,relative,resolve} from 'node:path';
import {assertPortableRuntimeSources} from '../../../launcher/lib/runtime-generations.mjs';
import {printRuntimeInputs} from '../../../tools/build/runtime-inputs.mjs';
import {DATA_SCHEMA,DATA_MAGIC,DATA_LAYOUT_SPEC} from '../web/managed-data.mjs';
import {stageNativeSolo} from './native-solo-package.mjs';

const port=resolve(import.meta.dirname,'..'),repo=resolve(port,'../..');
const args=Object.fromEntries(process.argv.slice(2).map(arg=>{
  const at=arg.indexOf('=');return [arg.slice(2,at<0?undefined:at),at<0?true:arg.slice(at+1)];
}));
const web=['th02.html','managed-shell.mjs','managed-data.mjs','managed-input.mjs','managed-native-solo.mjs',
  'managed-native-room.mjs','native-coop-peers.mjs','touch-input.js','hosted-touch.mjs','native-settings.mjs'];
const shared=['directory-keyboard.mjs','keyboard-input-bridge.mjs','keyboard-owners.mjs','direct-input-bridge.mjs',
  'pc98-game-fps.mjs','pc98-spectator-publisher.mjs','pc98-spectator-channel.mjs','pc98-local-spectator.mjs','pc98-match-index.mjs'];
const network=['managed-transport.mjs','input-transport.mjs'];
const inputs=[...web.map(name=>[resolve(port,'web',name),name]),
  ...shared.map(name=>[resolve(repo,'common/browser',name),name]),
  ...network.map(name=>[resolve(repo,'common/browser',name),'native/network/'+name])];
if(args['print-inputs'])await printRuntimeInputs({workspace:port,root:port,sources:[],headers:[
  ...inputs.map(([source])=>source),...['build-eagler.mjs','native-solo-package.mjs'].map(name=>resolve(port,'scripts',name)),
  ...['full.mjs','audio.mjs','glyph-cells.mjs','clock.mjs'].map(name=>resolve(port,'native-solo/web',name)),resolve(port,'source-import.json'),
],flags:['game=th02','assembly=managed-import','native-core=external-precompiled']});

const build=resolve(args['native-build']||process.env.TH02_NATIVE_BUILD||resolve(port,'native-solo/build-web'));
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const cache=resolve(port,'.cache');await mkdir(cache,{recursive:true});
const staging=await mkdtemp(resolve(cache,'native-build-')),output=resolve(staging,'build-eagler'),content=resolve(staging,'content');
try{
  await mkdir(output);await mkdir(content);
  const runtime=[],payload=[],entries=[],music=[];let offset=0;
  for(const [source,target] of inputs){
    const file=resolve(output,target);await mkdir(dirname(file),{recursive:true});await copyFile(source,file);runtime.push(target);
  }
  await stageNativeSolo({build,output,port,runtime,addData(path,bytes){
    entries.push({path,offset,bytes:bytes.length,sha256:sha(bytes)});payload.push(bytes);offset+=bytes.length;
  },async addMusic(name,bytes){
    const path='music/'+name;
    music.push({path,bytes:bytes.length,sha256:sha(bytes)});
    await mkdir(resolve(content,'music'),{recursive:true});await writeFile(resolve(content,path),bytes);
  }});
  if(args['print-package-inputs']){
    console.log(JSON.stringify({game:'th02',kind:'managed-import',inputs:[...entries.map(({offset,...entry})=>entry),...music]}));
  }else{
    const index=Buffer.from(JSON.stringify({schema:DATA_SCHEMA,game:'th02',files:entries}));
    const header=Buffer.alloc(12);header.write(DATA_MAGIC);header.writeUInt32LE(index.length,8);
    await writeFile(resolve(content,'th02.data'),Buffer.concat([header,index,...payload]));
    await writeFile(resolve(content,'source.json'),JSON.stringify({schema:'eagler-touhou/pc98-import/1',
      game:'th02',upstream:'https://github.com/Hydrangea-Z/SoEW-native',adaptation:'ports/th02/native-solo',
      dataLayout:'sha256-'+sha(JSON.stringify(DATA_LAYOUT_SPEC))},null,2)+'\n');
    const sources=new Map();for(const path of runtime.sort())sources.set(path,await readFile(resolve(output,path)));
    assertPortableRuntimeSources(sources);
    const files=Object.fromEntries([...sources].map(([path,bytes])=>[path,{bytes:bytes.length,sha256:sha(bytes)}]));
    const manifest=Buffer.from(JSON.stringify({game:'th02',protocol:'eagler-touhou/1',
      features:{thprac:false,languages:false,focusHitbox:false},files},null,2)+'\n');
    await writeFile(resolve(output,'manifest.json'),manifest);
    files['manifest.json']={bytes:manifest.length,sha256:sha(manifest)};
    await writeFile(resolve(output,'runtime-files.json'),JSON.stringify({schema:'eagler-touhou/runtime-directory/1',files},null,2)+'\n');
    const swaps=[];
    try{
      for(const name of ['build-eagler','content']){
        const target=resolve(port,name),backup=resolve(staging,'previous-'+name);
        let previous=false;
        try{await rename(target,backup);previous=true;}catch(error){if(error.code!=='ENOENT')throw error;}
        const swap={target,backup,previous,installed:false};swaps.push(swap);
        await rename(resolve(staging,name),target);swap.installed=true;
      }
    }catch(error){
      for(const swap of swaps.reverse()){
        if(swap.installed)await rename(swap.target,resolve(staging,'failed-'+relative(port,swap.target)));
        if(swap.previous)await rename(swap.backup,swap.target);
      }
      throw error;
    }
    console.log(JSON.stringify({game:'th02',runtimeFiles:Object.keys(files).length,contentFiles:entries.length,contentBytes:offset}));
  }
}finally{
  if(dirname(staging)!==cache||!relative(cache,staging).startsWith('native-build-'))throw Error('Invalid staging directory');
  await rm(staging,{recursive:true,force:true});
}
