import {createReadStream,existsSync} from 'node:fs';
import {readFile,readdir,stat} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve,dirname,relative,sep,isAbsolute} from 'node:path';
import {fileURLToPath} from 'node:url';

const root=fileURLToPath(new URL('../',import.meta.url));
const inside=path=>path.startsWith(resolve(root)+sep);
function local(name){const path=resolve(root,name);if(!inside(path))throw Error(`Path outside game: ${name}`);return path;}
async function hash(path){const h=createHash('sha256');for await(const chunk of createReadStream(path))h.update(chunk);return h.digest('hex');}
async function verify(name,expected){const path=local(name),s=await stat(path);if(s.size!==expected.bytes)throw Error(`Incomplete resource: ${name}. Run git lfs pull if this is an LFS clone.`);if(await hash(path)!==expected.sha256)throw Error(`SHA256 mismatch: ${name}`);}
for(const name of ['index.html','standalone.mjs','serve.mjs','package.json','.gitattributes','scripts/build-eagler.mjs','scripts/static-server.mjs','web/sdl-runtime/managed.html','web/sdl-runtime/shell.mjs','web/sdl-runtime/managed.css','web/sdl-runtime/keyboard.mjs','web/cpp/sdl/Application.cpp','web/cpp/sdl/GraphicsDevice.hpp','portable/sdl/Renderer.cpp','fonts/msgothic.ttf','content/msgothic.ttf'])if(!existsSync(local(name)))throw Error(`Missing game file: ${name}`);
const resources=JSON.parse(await readFile(local('game-resources.json'),'utf8'));
await verify(resources.gameData.source,resources.gameData);
for(let i=0;i<resources.music.ogg.files.length;i++)await verify(resources.music.ogg.base+resources.music.ogg.files[i],{bytes:resources.music.ogg.sizes[i],sha256:resources.music.ogg.sha256[i]});
const runtime=JSON.parse(await readFile(local('runtime/runtime-files.json'),'utf8'));
for(const [name,entry]of Object.entries(runtime.files))await verify('runtime/'+name,entry);
for(const [source,built]of [['managed.html','th19.html'],['shell.mjs','shell.mjs'],['managed.css','managed.css'],['keyboard.mjs','keyboard.mjs'],['netplay.mjs','netplay.mjs'],['peer-socket.mjs','peer-socket.mjs'],['rollback-session.mjs','rollback-session.mjs'],['native-adapter.mjs','native-adapter.mjs'],['th19-recording.mjs','th19-recording.mjs'],['replay-session.mjs','replay-session.mjs'],['directory-keyboard.mjs','directory-keyboard.mjs'],['keyboard-owners.mjs','keyboard-owners.mjs'],['keyboard-input-bridge.mjs','keyboard-input-bridge.mjs']])if(await hash(local('web/sdl-runtime/'+source))!==await hash(local('runtime/'+built)))throw Error(`Adapter changed; run npm run build: ${source}`);
const includes=['web/cpp/game','web/cpp/sdl','portable/sdl','portable/input','portable/compat'].map(local);let sourceFiles=0;
for(const dir of includes)for(const name of await readdir(dir)){
 if(!/\.(cpp|hpp|h)$/.test(name))continue;const file=resolve(dir,name);if(!(await stat(file)).isFile())continue;++sourceFiles;
 for(const match of (await readFile(file,'utf8')).matchAll(/^\s*#\s*include\s*"([^"]+)"/gm)){
  if(isAbsolute(match[1]))throw Error(`Absolute include in ${relative(root,file)}`);
  const found=[dirname(file),...includes].map(base=>resolve(base,match[1])).find(p=>inside(p)&&existsSync(p));
  if(!found)throw Error(`Missing local include ${match[1]} in ${relative(root,file)}`);
 }
}
console.log(JSON.stringify({passed:true,sourceFiles,musicFiles:resources.music.ogg.files.length,runtimeFiles:Object.keys(runtime.files).length,wasm:runtime.files['th19.wasm'].sha256}));

