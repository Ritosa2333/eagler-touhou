// Materialize the author's Launcher for ONE reviewed, local co-op Runtime generation.
// Old preview and public Launcher sources never advertise this policy. No original assets or saves are copied.
import {copyAuthorUiInputs} from './copy-author-ui-inputs.mjs';
import {createHash} from 'node:crypto';import {readdir,readFile,writeFile,mkdir,lstat,realpath} from 'node:fs/promises';import {join,dirname,resolve,relative,isAbsolute,sep} from 'node:path';
import {canonicalRuntimePayload,validateRuntimeGeneration} from '../../../launcher/src/contracts/runtime-generations.mts';
const [programArg,outArg]=process.argv.slice(2);if(!programArg||!outArg)throw Error('usage: stage-coop-launcher.mjs <projected multiplayer directory> <fresh private source tree>');
const repo=resolve(import.meta.dirname,'../../..'),program=await realpath(programArg),out=join(await realpath(dirname(resolve(outArg))),resolve(outArg).slice(dirname(resolve(outArg)).length+1));
const within=(a,b)=>{const rel=relative(a,b);return !isAbsolute(rel)&&rel!=='..'&&!rel.startsWith('..'+sep);};if(within(repo,out)||within(program,out))throw Error('Refusing to stage into an input tree');
const sha=b=>createHash('sha256').update(b).digest('hex'),manifest=JSON.parse(await readFile(join(program,'runtime-generation.json'))),idx=validateRuntimeGeneration(manifest),gen=idx.generation;
if(sha(canonicalRuntimePayload(idx.entry,idx.files))!==gen)throw Error('Invalid cooperative runtime generation');
for(const f of idx.files){const path=join(program,f.path);if(!within(program,await realpath(path)))throw Error('Program escapes generation');const b=await readFile(path);if(b.length!==f.bytes||sha(b)!==f.sha256)throw Error('Runtime bytes changed: '+f.path);}
const spec=JSON.parse(await readFile(join(program,'runtime/program.json')));if(spec.schema!=='th128mp-program/2'||![spec.coopGeneration,spec.peerGeneration].every(v=>typeof v==='string'&&/^[0-9a-f]{64}$/.test(v))||spec.coopGeneration!==spec.files.coopWasm?.sha256)throw Error('Needs real cooperative battle and transport');
for(const [k,f] of Object.entries(spec.files))if(!idx.files.some(row=>row.path===f.path&&row.bytes===f.bytes&&row.sha256===f.sha256))throw Error('Missing cooperative program receipt '+k);
if(spec.files.peer.path!==`runtime/transport/${spec.peerGeneration}/author-peer.mjs`||spec.files.peerWasm.path!==`runtime/transport/${spec.peerGeneration}/author-peer.wasm`)throw Error('Wrong sender-aware peer route');
const patches=new Map();const patch=(file,edit)=>patches.set(file,edit);
function exactly(text,old,next){const i=text.indexOf(old);if(i<0||text.indexOf(old,i+old.length)!==-1)throw Error('Author launcher context changed: '+old);return text.slice(0,i)+next+text.slice(i+old.length);}
patch('common/contracts/product-catalog.mjs',s=>{const at=s.indexOf('const TH128_PREVIEW_ROOM'),end=s.indexOf('export const PRODUCT_IDS',at);if(at<0||end<0)throw Error('Missing local room overlay');let room=s.slice(at,end);room=exactly(room,"gameplay: 'parallel'","gameplay: 'cooperative'");room=exactly(room,"playerCounts: Object.freeze([2]), difficulties: Object.freeze(['Normal'])","playerCounts: Object.freeze([2,3]), difficulties: STANDARD_MULTIPLAYER_DIFFICULTIES");room=exactly(room,'challengeMode: false','challengeMode: true');return s.slice(0,at)+room+s.slice(end);});
patch('launcher/src/contracts/site-manifest.mts',s=>exactly(s,'TH128_MULTIPLAYER_PREVIEW_PROFILE = "th128-multiplayer-local-preview"','TH128_MULTIPLAYER_PREVIEW_PROFILE = "th128-multiplayer-local-coop"'));
patch('launcher/app/services/runtime-entry-authority.ts',s=>{s=exactly(s,'!!th128MultiplayerPreviewGeneration(host.games.th128?.multiplayerRuntime)',`th128MultiplayerPreviewGeneration(host.games.th128?.multiplayerRuntime) === '${gen}'`);return exactly(s,'if (host.profile !== TH128_MULTIPLAYER_PREVIEW_PROFILE || host.shared.testBuild !== true ||', 'if (!th128LocalRoomAvailable(host, base.href) || host.shared.testBuild !== true ||');});
patch('launcher/src/launcher/multiplayer-runtime-options.mts',s=>{const start=s.indexOf("    // The local room's one selectable Normal option"),end=s.indexOf("    netplayDifficulty:",start);if(start<0||end<start)throw Error('Missing native rank mapping');return exactly(s.slice(0,start)+s.slice(end),"netplayDifficulty: roomProduct === 'th128mp' ? 1 : difficulty,",'netplayDifficulty: difficulty,');});
// Both fresh network navigation AND verified service-worker cache hits must preserve the
// isolation of the author App Shell. Never disable COEP in the launcher or the browser.
patch('launcher/scripts/serve.mjs',s=>{
  s=exactly(s,'    const commonHeaders = {',`    const localCoopRoot = '/runtime/th128/multiplayer/${gen}/';
    const localPath = new URL(request.url || '/', 'http://localhost').pathname;
    const localCoopDocument = localPath === localCoopRoot+'th128.html';
    const commonHeaders = {
      ...(localCoopDocument ? {'Cross-Origin-Embedder-Policy':'require-corp','Cross-Origin-Opener-Policy':'same-origin','Cross-Origin-Resource-Policy':'same-origin'} : {}),`);
  // These verified program files are served on loopback, not through a CDN. Stream fixed-length
  // bytes instead of recompressing Wasm while every player is simulating and the viewer downloads it.
  return exactly(s,'const shouldCompress = staticContentCompressible(file, info.size);',
    'const shouldCompress = !localPath.startsWith(localCoopRoot) && staticContentCompressible(file, info.size);');
});
patch('launcher/src/launcher/runtime-launch.mts',s=>exactly(s,
  'timer = setTimeout(() => controller.abort(new Error(`Runtime download stalled for ${timeoutMs} ms: ${url}`)), timeoutMs);',
  'const roomEntry = new URL(baseUrl).pathname === "/" && path === "runtime/th128/multiplayer/'+gen+'/th128.html"; const targetPath = new URL(url).pathname; const idleLimit = roomEntry && (targetPath === "/runtime-manifest.json" || targetPath.startsWith("/runtime/th128/multiplayer/'+gen+'/")) ? Math.max(timeoutMs, 45000) : timeoutMs;\n        timer = setTimeout(() => controller.abort(new Error(`Runtime download stalled for ${idleLimit} ms: ${url}`)), idleLimit);'));
for(const [locale,value] of [['zh-CN','妖精大战争 | 多人合作'],['en','Fairy Wars | Co-op'],['ja','妖精大戦争 | 協力プレイ']])patch(`launcher/messages/${locale}.json`,s=>{const src=JSON.parse(s),key='game_title_th128mp';if(typeof src[key]!=='string')throw Error('Local game title missing');return exactly(s,`${JSON.stringify(key)}: ${JSON.stringify(src[key])}`,`${JSON.stringify(key)}: ${JSON.stringify(value)}`);});
const skip=new Set(['node_modules','build','dist','.cache','.react-router','.git','.github','.env','coverage','tests']);const seen=new Map();
async function copy(file){const src=join(repo,file),dst=join(out,file),info=await lstat(src);if(info.isSymbolicLink())throw Error('Unexpected symlink in author input: '+file);if(info.isDirectory()){for(const e of await readdir(src,{withFileTypes:true}))if(!skip.has(e.name)&&!e.name.startsWith('.env')&&e.name!=='.DS_Store')await copy(file+'/'+e.name);return;}
 if(!info.isFile()||/\.(?:dat|ogg|rpy|th128mp|zip|sqlite|key|pem)$/i.test(file))throw Error('Unexpected non-program input: '+file);const b=await readFile(src);let output=b;if(patches.has(file)){output=Buffer.from(patches.get(file)(b.toString('utf8').replaceAll('\r\n','\n')));if(output.equals(b))throw Error('Local source overlay had no effect');seen.set(file,{source:sha(b),local:sha(output)});}await mkdir(dirname(dst),{recursive:true});await writeFile(dst,output,{flag:'wx'});}
await mkdir(out);for(const src of ['common/contracts','launcher','ports/th128-mp/tools/assemble-local-host.mjs'])await copy(src);await copyAuthorUiInputs(repo,out);if(seen.size!==patches.size)throw Error('Incomplete launcher overlay');
const result={schema:'th128-local-coop-launcher-source/1',profile:'th128-multiplayer-local-coop',generation:gen,policyEdits:Object.fromEntries(seen)};await writeFile(join(out,'coop-source.json'),JSON.stringify(result,null,2)+'\n',{flag:'wx'});console.log(JSON.stringify({out,generation:gen,edited:seen.size}));
