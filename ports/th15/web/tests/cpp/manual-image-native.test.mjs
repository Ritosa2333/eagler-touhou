import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {root,target} from './helpers.mjs';
import {nativeImagePixels} from './native-image-upload.mjs';

const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
test('TH15 original DirectX image library produces all nine no-filter manual atlases',async()=>{
 const output=process.env.EAGLER_MAINTENANCE_OUTPUT;
 assert.ok(output,'EAGLER_MAINTENANCE_OUTPUT is required for native image capture');
 mkdirSync(output,{recursive:true});const pages=[];
 for(let i=0;i<9;i++){
  const name='help_0'+(i+1)+'.png',png=readFileSync(resolve(root,'reference/assets',name)),pixels=await nativeImagePixels(png);
  assert.equal(pixels.length,768*1024*2);assert.ok(pixels.some(value=>value!==0));
  writeFileSync(resolve(output,'manual-page-'+i+'.bin'),pixels);
  pages.push({name,sourceSha256:sha(png),nativeAtlasSha256:sha(pixels),bytes:pixels.length});
 }
 writeFileSync(resolve(output,'manual-image-native.json'),JSON.stringify({target,pages,
  scope:'Original D3DX43 PNG decode and no-filter crop into a 768x1024 A4R4G4B4 surface. This capture feeds the SDL browser-upload comparison.'},null,2)+'\n');
});
