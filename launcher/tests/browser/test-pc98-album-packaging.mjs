import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {PRODUCT_CONTENT} from '../../lib/content-definition.mjs';
for(const game of ['th02','th04'])test(`${game} optional album is separately packaged and verified`,async()=>{
 const root=new URL(`../../../ports/${game}/`,import.meta.url),bytes=await readFile(new URL(`content/${game}.data`,root));
 const len=bytes.readUInt32LE(8),manifest=JSON.parse(bytes.subarray(12,12+len));
 assert(!manifest.files.some(f=>/^bgm\/akyu\/\d+\.(opus|ogg)$/.test(f.path)));
 assert.deepEqual(PRODUCT_GAMES[game].musicRuntime.sources,['original','akyu']);
 const album=JSON.parse(await readFile(new URL('build-eagler/bgm/akyu/manifest.json',root)));
 const files=PRODUCT_CONTENT[game].music.ogg.files;
 for(const file of files)await readFile(new URL(`content/music/${file}`,root));
 for(const track of Object.values(album.tracks)){
  const name=track.url.split('/').at(-1);assert(files.includes(name));
  const audio=await readFile(new URL(`content/music/${name}`,root));
  assert.equal(audio.length,track.bytes);assert.equal(createHash('sha256').update(audio).digest('hex'),track.audioSha256||track.sha256);
 }
 const {ManagedContent}=await import(new URL('web/managed-data.mjs',root));
 const content=await ManagedContent.open(bytes,game),name=files[0],audio=await readFile(new URL(`content/music/${name}`,root));
 const blob=new Blob([audio]),url=URL.createObjectURL(blob),sha=createHash('sha256').update(audio).digest('hex');
 try{
  await assert.rejects(content.addResources([{path:'/bgm/akyu/'+name,url,bytes:audio.length,sha256:'0'.repeat(64)}]),/校验/);
  await content.addResources([{path:'/bgm/akyu/'+name,url,bytes:audio.length,sha256:sha}]);
  const env={fetch:globalThis.fetch,location:{href:'https://example.test/runtime/'+game+'/'}};content.install(env,env.location.href);
  const result=await env.fetch(new URL('bgm/akyu/'+name,env.location.href));assert.equal(result.status,200);assert.equal((await result.arrayBuffer()).byteLength,audio.length);
 }finally{URL.revokeObjectURL(url);}
});
