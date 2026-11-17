import {test} from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createRequire} from 'node:module';
import {mkdir,mkdtemp,rm} from 'node:fs/promises';
import {resolve,join} from 'node:path';
import {pathToFileURL} from 'node:url';
import {authoredSourcesPlugin} from './react-main/authored-sources.mjs';
import {PRODUCT_CONTENT} from '../lib/content-definition.mjs';
import {DATA_LAYOUT_SPEC} from '../../ports/th01/web/managed-data.mjs';

const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const legacy = 'sha256-967f66a7de83b3e5580afa2bc0db41aac8b8c41ec90eb4b6767286570d76a579';

test('PC-98 legacy installations upgrade before pinning a runtime when required', async () => {
  const project=resolve(import.meta.dirname,'..'),require=createRequire(resolve(project,'package.json'));
  const {IDBFactory,IDBKeyRange}=require('fake-indexeddb');
  globalThis.indexedDB=new IDBFactory(); globalThis.IDBKeyRange=IDBKeyRange;
  assert.equal('sha256-'+hash(JSON.stringify(DATA_LAYOUT_SPEC)), PRODUCT_CONTENT.th01.dataLayout);
  assert.notEqual(PRODUCT_CONTENT.th01.dataLayout,legacy);
  await mkdir(resolve(project,'.cache'),{recursive:true});
  const root=await mkdtemp(resolve(project,'.cache/pc98-upgrade-'));
  try {
    const {build}=require('esbuild'),outfile=join(root,'owners.mjs');
    await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`export {createPackageAcquisition} from './app/services/package-acquisition.ts'; export {ensurePC98Disks} from './app/services/game-resources.ts'; export {installPackageFromRemote} from './package/package-installer.mjs'; export {readCurrentPackageGeneration} from './package/package-store.mjs';`},outfile,bundle:true,platform:'node',format:'esm',packages:'external',plugins:[{name:'fixture-assets',setup(context){context.onResolve({filter:/\?url$/},args=>({path:args.path,namespace:'fixture-url'}));context.onLoad({filter:/.*/,namespace:'fixture-url'},()=>({contents:'export default "https://fixture.invalid/asset"',loader:'js'}));}},authoredSourcesPlugin(project)],logLevel:'silent'});
    const {createPackageAcquisition,ensurePC98Disks,installPackageFromRemote,readCurrentPackageGeneration}=await import(pathToFileURL(outfile).href);
    for (const game of ['th01','th02','th04']) {
      const bodies=new Map();
      const file=(name,value,target)=>{const bytes=Buffer.from(value), digest=hash(bytes);bodies.set(name,bytes);return {source:name,target,bytes:bytes.length,sha256:digest,revision:digest.slice(0,16)};};
      const oldData=file(`${game}-old.data`,'old '+game,`/${game}.data`);
      const newData=file(`${game}-new.data`,'new '+game,`/${game}.data`);
      const layout=PRODUCT_CONTENT[game].dataLayout;
      const descriptor=(revision,data,components={},files={})=>({schema:'eagler-touhou/package/1',game,revision,
        runtimeRequirement:{protocol:'eagler-touhou/1',target:game,dataFile:'game-data',dataLayout:revision==='old'?legacy:layout},
        files:{'game-data':data,...files},base:{files:['game-data']},components});
      let old=descriptor('old',oldData), next=descriptor('new',newData);
      if(game!=='th01') {
        const path=game==='th02'?'th02-solo-jp.hdi.gz':'th04-solo-jp.hdi.gz';
        const id='pc98-block:'+hash('block-'+game);
        const manifest=file(`${game}-manifest.json`,'manifest '+game,'/pc98/disks.json');
        const block=file(`${game}-block.gz`,'block '+game,`/pc98/disk-blocks/${id.slice(11)}.gz`);
        const components={'pc98-manifest':{type:'pc98-disk',files:['pc98-disk-manifest']},[`pc98-disk:${path}`]:{type:'pc98-disk',files:[id]}};
        old=descriptor('old',oldData,components,{'pc98-disk-manifest':manifest,[id]:block});
        // The current publication contains static DATA, with no disk components.
        next=descriptor('new',newData);
      }
      const requested=[]; let failNew=false;
      const fetchImpl=async url=>{const path=new URL(url).pathname.slice(1);requested.push(path);
        if(path===`${game}.package.json`) return Response.json(next);
        if(failNew && path===`${game}-new.data`) return new Response('',{status:503});
        return bodies.has(path)?new Response(bodies.get(path)):new Response('',{status:404});};
      const descriptorUrl=`https://example.test/${game}.package.json`;
      const initial=await installPackageFromRemote(old,{descriptorUrl,fetchImpl,desiredFileIds:Object.keys(old.files)});
      const catalog={schema:'eagler-touhou/release-catalog/1',games:{[game]:{revision:'new',descriptor:`${game}.package.json`}}};
      const snapshot={siteManifest:{games:{[game]:{gameData:{layout}}}},releaseCatalog:catalog};
      const acquisition=createPackageAcquisition({baseUrl:'https://example.test/',translate:key=>key,fetchImpl,
        metadata:{getSnapshot:()=>snapshot,initialize:async()=>{},refreshInstalled:async()=>{}},
        dependencies:{repairInstalled:async current=>current.generation},testBuild:true});
      requested.length=0; failNew=true;
      await assert.rejects(acquisition.acquire({productId:game,music:'none',language:'ja'}));
      assert.equal((await readCurrentPackageGeneration(game)).generation.id,initial.generation.id, 'failed update preserves previous installation');
      requested.length=0; failNew=false;
      const result=await acquisition.acquire({productId:game,music:'none',language:'ja'});
      assert.equal(result.generation.descriptor.revision,'new',`${game} must update before runtime selection`);
      assert.notEqual(result.generation.id,initial.generation.id);
      assert.equal((await readCurrentPackageGeneration(game)).generation.id,result.generation.id);
      assert(requested.includes(`${game}-new.data`));
      assert.equal(result.generation.descriptor.components['pc98-manifest'],undefined);
      assert(!requested.includes(`${game}-block.gz`),'upgrade must not redownload old disk blocks');
    }
  } finally {await rm(root,{recursive:true,force:true});}
});
