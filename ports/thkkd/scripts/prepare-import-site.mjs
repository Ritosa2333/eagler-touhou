import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {fileURLToPath} from 'node:url';
import {createPublicationHostSeed} from '../../../launcher/lib/publication-host-seed.mjs';
const root=new URL('../',import.meta.url), output=new URL('build/site-inputs/',root);
const data=JSON.parse(await readFile(new URL('data-reference.json',root),'utf8'));
const runtime=JSON.parse(await readFile(new URL('runtime/manifest.json',root),'utf8'));
const manifest=createPublicationHostSeed('web-validation-thkkd-original',['thkkd']);
manifest.schema='eagler-touhou/site-manifest/1';
manifest.shared={resourceMode:'import'};
Object.assign(manifest.games.thkkd,{runtime:`runtime/thkkd/thkkd.html?hosted=1&v=${runtime.buildId}`,
  gameData:data,features:{thprac:false,focusHitbox:false},music:{midi:{files:[],supported:false}},
  offlineCompatibility:{schema:'eagler-touhou/offline-game-pack/1',runtimeCompatibility:{protocol:manifest.protocol,dataLayout:data.layout,versionSource:'offline-pack'},requiredShared:[],languages:{source:'offline-pack',baseline:['ja']}}});
await mkdir(output,{recursive:true});
await writeFile(new URL('import-site.json',output),JSON.stringify(manifest,null,2)+'\n');
console.log(JSON.stringify({manifest:fileURLToPath(new URL('import-site.json',output)),resources:'user-import'}));
