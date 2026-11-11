import {compressTh03File} from '../../../launcher/lib/th03-brotli.mjs';
import {createHash} from 'node:crypto';
import {readFile,writeFile,mkdir,readdir,copyFile,mkdtemp,rename,rm} from 'node:fs/promises';
import {resolve,dirname,relative,join} from 'node:path';
import {fileURLToPath} from 'node:url';
import {DATA_SCHEMA,DATA_MAGIC,DATA_LAYOUT_SPEC} from '../web/managed-data.mjs';
const port=resolve(fileURLToPath(new URL('..',import.meta.url))),repo=resolve(port,'../..');
const args=Object.fromEntries(process.argv.slice(2).map(arg=>{const at=arg.indexOf('=');return [arg.slice(2,at<0?undefined:at),at<0?true:arg.slice(at+1)];}));
if(!args['native-content']||!args['native-ui']||!args['native-build'])throw Error('需要指定 --native-content、--native-ui 和 --native-build');
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
async function files(dir,prefix=''){const result=[];for(const e of await readdir(dir,{withFileTypes:true})){if(e.isSymbolicLink())throw Error('不支持符号链接');const path=prefix+e.name;if(e.isDirectory())result.push(...await files(join(dir,e.name),path+'/'));else if(e.isFile())result.push(path);}return result.sort();}
const cache=resolve(port,'.cache');await mkdir(cache,{recursive:true});const stage=await mkdtemp(resolve(cache,'native-build-')),output=resolve(stage,'build-eagler'),content=resolve(stage,'content');
try{
 await mkdir(output,{recursive:true});await mkdir(content,{recursive:true});
 const runtime=['managed-shell.mjs','managed-data.mjs','managed-input.mjs','managed-storage.mjs','managed-room-layout.mjs','managed-native-solo.mjs','managed-native-room.mjs','managed-transport.mjs','native-lockstep.mjs','native-match.mjs','native-match-rollback.mjs','native-rollback.mjs','native-versus-pause.mjs','native-confirmed-driver.mjs','native-hit-point.mjs','native-music-player.mjs','native-replay.mjs','native-replay-player.mjs','fire-control.js','touch-input.js','netplay/input-transport.js'];
 for(const name of runtime){await mkdir(dirname(resolve(output,name)),{recursive:true});await copyFile(resolve(port,'web',name),resolve(output,name));}
 for(const name of ['directory-keyboard.mjs','keyboard-input-bridge.mjs','keyboard-owners.mjs','direct-input-bridge.mjs','pc98-game-fps.mjs','pc98-spectator-publisher.mjs','pc98-spectator-channel.mjs','pc98-local-spectator.mjs','pc98-match-index.mjs','playback-clock.mjs']){await copyFile(resolve(repo,'common/browser',name),resolve(output,name));runtime.push(name);}
 for(const name of await files(resolve(port,'web/bgm/akyu'))){const target='bgm/akyu/'+name;await mkdir(dirname(resolve(output,target)),{recursive:true});await copyFile(resolve(port,'web/bgm/akyu',name),resolve(output,target));runtime.push(target);}
 for(const [from,to] of [['classic.js','native-classic.js'],['classic.wasm','native-classic.wasm']]){await copyFile(resolve(args['native-build'],from),resolve(output,to));runtime.push(to);}
 const html='<!doctype html><html><head><meta charset="utf-8"><meta name="eagler-data-provider" content="retail-memory"><meta name="viewport" content="width=device-width,initial-scale=1"><style>html,body,#screen{width:100%;height:100%;margin:0;background:#000;overflow:hidden}#canvas{outline:0;width:100%;height:100%;object-fit:contain;image-rendering:pixelated}</style><script type="module" src="managed-shell.mjs"></script></head><body><div id="screen"><canvas id="canvas" width="640" height="400" tabindex="0"></canvas></div></body></html>';
 await writeFile(resolve(output,'th03.html'),html);runtime.push('th03.html');
 const entries=[],payload=[];let offset=0;
 for(const [root,prefix] of [[resolve(args['native-content']),'native-th03/'],[resolve(args['native-ui']),'native-th03/PODDUI/']])for(const name of await files(root)){const bytes=await readFile(join(root,name));if(!bytes.length)continue;entries.push({path:prefix+name,bytes:bytes.length,sha256:hash(bytes),offset});payload.push(bytes);offset+=bytes.length;}
 const manifestBytes=Buffer.from(JSON.stringify({schema:DATA_SCHEMA,game:'th03',files:entries})),header=Buffer.alloc(12);header.write(DATA_MAGIC);header.writeUInt32LE(manifestBytes.length,8);await writeFile(resolve(content,'th03.data'),Buffer.concat([header,manifestBytes,...payload]));
 await compressTh03File(resolve(output,'native-classic.wasm'));await compressTh03File(resolve(content,'th03.data'));
 const identities={};for(const name of runtime.sort()){const bytes=await readFile(resolve(output,name));identities[name]={bytes:bytes.length,sha256:hash(bytes)};}
 await writeFile(resolve(output,'manifest.json'),JSON.stringify({game:'th03',protocol:'eagler-touhou/1',features:{thprac:false,languages:false,focusHitbox:false},files:identities},null,2)+'\n');const m=await readFile(resolve(output,'manifest.json'));identities['manifest.json']={bytes:m.length,sha256:hash(m)};await writeFile(resolve(output,'runtime-files.json'),JSON.stringify({schema:'eagler-touhou/runtime-directory/1',files:identities},null,2)+'\n');
 await writeFile(resolve(content,'source.json'),JSON.stringify({schema:'eagler-touhou/pc98-import/1',upstream:'https://github.com/christianazinn/th98port',dataLayout:'sha256-'+hash(JSON.stringify(DATA_LAYOUT_SPEC)),adaptation:'native-th03'},null,2)+'\n');
 const swaps=[];try{for(const name of ['build-eagler','content']){const target=resolve(port,name),backup=resolve(stage,'previous-'+name);let previous=false;try{await rename(target,backup);previous=true;}catch(error){if(error.code!=='ENOENT')throw error;}const swap={target,backup,previous,installed:false};swaps.push(swap);await rename(resolve(stage,name),target);swap.installed=true;}}catch(error){for(const swap of swaps.reverse()){if(swap.installed)await rename(swap.target,resolve(stage,'failed-'+relative(port,swap.target)));if(swap.previous)await rename(swap.backup,swap.target);}throw error;}
 console.log(JSON.stringify({game:'th03',runtimeFiles:runtime.length,contentFiles:entries.length,contentBytes:offset}));
}finally{if(dirname(stage)!==cache||!relative(cache,stage).startsWith('native-build-'))throw Error('Invalid staging directory');await rm(stage,{recursive:true,force:true});}
