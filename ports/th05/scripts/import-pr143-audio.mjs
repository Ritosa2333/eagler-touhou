import {mkdir,readFile,writeFile} from 'node:fs/promises';
import {resolve,dirname} from 'node:path';
import {createHash} from 'node:crypto';
const repository='IxinorTyan/th05-coop-lab',commit='6eb46ed1d4f581f109ffa494ea0d21d54c8b38df';
const args=Object.fromEntries(process.argv.slice(2).map(value=>{const at=value.indexOf('=');if(!value.startsWith('--')||at<3)throw Error('Use --output=DIR [--source=LOCAL_CHECKOUT]');return [value.slice(2,at),value.slice(at+1)];}));
const output=resolve(args.output??'artifacts/pr143-audio');
const sha=(kind,bytes)=>createHash(kind).update(bytes).digest('hex');
async function request(url){for(let attempt=0;attempt<3;attempt++){try{const response=await fetch(url,{headers:{'User-Agent':'eagler-uth05-audio-import'},signal:AbortSignal.timeout(45000)});if(!response.ok)throw Error('HTTP '+response.status+' '+url);return new Uint8Array(await response.arrayBuffer());}catch(error){if(attempt===2)throw error;}}}
const tree=JSON.parse(new TextDecoder().decode(await request(`https://api.github.com/repos/${repository}/git/trees/${commit}?recursive=1`)));
if(!Array.isArray(tree.tree)||tree.truncated)throw Error('Incomplete pinned PR #143 source tree');
const entries=tree.tree.filter(entry=>entry.type==='blob'&&(/^web\/bgm\/(?:akyu\/)?[a-z0-9_.-]+\.(?:opus|json)$/.test(entry.path)||/^web\/sfx\/(?:effects\.pcm|manifest\.json)$/.test(entry.path)));
if(!entries.some(e=>e.path==='web/sfx/effects.pcm')||!entries.some(e=>e.path==='web/bgm/akyu/manifest.json'))throw Error('Pinned PR audio assets missing');
const verified=[];let cursor=0,done=0;await mkdir(output,{recursive:true});
await Promise.all(Array.from({length:4},async()=>{while(cursor<entries.length){const entry=entries[cursor++],name=entry.path.slice(4),path=resolve(output,name);let bytes;
 try{bytes=new Uint8Array(await readFile(path));}catch{}
 const gitHash=data=>sha('sha1',Buffer.concat([Buffer.from('blob '+data.length+String.fromCharCode(0)),data]));
 if(!bytes||bytes.length!==entry.size||gitHash(bytes)!==entry.sha){bytes=args.source?new Uint8Array(await readFile(resolve(args.source,entry.path))):await request(`https://raw.githubusercontent.com/${repository}/${commit}/${entry.path}`);if(bytes.length!==entry.size||gitHash(bytes)!==entry.sha)throw Error('PR audio Git identity mismatch: '+name);await mkdir(dirname(path),{recursive:true});await writeFile(path,bytes);}
 verified.push({path:name,bytes:bytes.length,gitBlob:entry.sha,sha256:sha('sha256',bytes)});if(++done%10===0)console.log(`PR #143 audio: ${done}/${entries.length}`);
}}));
await writeFile(resolve(output,'source.json'),JSON.stringify({schema:'eagler-touhou/uth05-audio-source/1',reference:'https://github.com/Ritosa2333/eagler-touhou/pull/143',repository:'https://github.com/'+repository,commit,files:verified.sort((a,b)=>a.path.localeCompare(b.path))},null,2)+'\n');
console.log('Verified PR #143 BGM, Akyu album and independent FM SFX: '+output);
