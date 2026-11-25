import {mkdir,readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {contentPath} from '../web/managed-data.mjs';

// Runtime code is pinned to the Eagler generation. Original art, the glyph
// atlas and optional audio live in the installed TH02 DATA instead.
export async function stageNativeSolo({build,output,port,runtime,addData}) {
  const native=resolve(output,'native');
  await mkdir(native,{recursive:true});
  for(const name of ['soew-full.mjs','soew-full.wasm','full.mjs','glyph-cells.mjs','LICENSE.txt','NOTICE.txt']) {
    const source=name.endsWith('.txt')?resolve(port,'native-solo',name.slice(0,-4)):
      name.startsWith('soew-')?resolve(build,name):resolve(port,'native-solo/web',name);
    await writeFile(resolve(native,name),await readFile(source));
    runtime.push(`native/${name}`);
  }
  await addData('native/soew-glyphs.bin',await readFile(resolve(build,'soew-glyphs.bin')));
  const content=resolve(build,'content'),manifest=await readFile(resolve(content,'full-files.json'));
  const files=new Set(JSON.parse(manifest).map(contentPath));
  if(files.has('akyu.pack')) {
    const tracks=JSON.parse(await readFile(resolve(content,'akyu.pack')));
    for(const track of tracks) files.add(contentPath(track.url));
  }
  await addData('native/content/full-files.json',manifest);
  for(const name of files) await addData(`native/content/${name}`,await readFile(resolve(content,name)));
}
