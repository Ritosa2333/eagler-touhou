import test from 'node:test';
import assert from 'node:assert/strict';
import {PRODUCT_GAMES,HOST_PROTOCOL} from '../../common/contracts/product-catalog.mjs';
import {assertPackageImportParity} from './package-import-parity.mjs';

function fixture(){
  const game='th08',p=PRODUCT_GAMES[game].package,expectedData={bytes:4096,sha256:'a'.repeat(64)},generation='runtime123';
  const options={game,expectedData,expectedLayout:'layout',publishedRevision:'release123',runtimeGeneration:generation,
    archiveIdentity:{path:'input.zip',bytes:8192,sha256:'b'.repeat(64)},originalIdentity:{path:p.rawDataImport.fileNames[0],...expectedData}};
  const routes=['zip','online','original-file'].map((source,index)=>{
    const raw=source==='original-file',id='installed'+index,epoch=1;
    const descriptor={game,revision:raw?'raw-'+expectedData.sha256.slice(0,16):options.publishedRevision,base:{files:[p.dataFileId]},files:{[p.dataFileId]:{},optional:{}},
      runtimeRequirement:{protocol:HOST_PROTOCOL,target:game,dataLayout:options.expectedLayout,dataFile:p.dataFileId}};
    const installed={installation:{game,currentGeneration:id,pendingGeneration:null,source:source==='online'?'remote':'local'},
      generation:{id,game,descriptor,files:{[p.dataFileId]:{objectId:'object'+index}}},data:{fileId:p.dataFileId,objectId:'object'+index,target:p.dataTarget,declaredBytes:expectedData.bytes,
        declaredSha256:raw?null:expectedData.sha256,...expectedData}};
    const event=name=>({event:name,protocol:HOST_PROTOCOL,game,epoch,...(name==='first-frame'?{presentation:{nativePresentCount:1}}:{})});
    const runtime={frameUrl:`https://fixture/runtime/${game}/${generation}/index.html?runtimeVariant=normal&managedData=1&gameGeneration=${id}&runtimeEpoch=${epoch}`,
      ready:event('ready'),first:event('first-frame'),workerControlled:true,dataRead:{game,epoch,generation:id,fileId:p.dataFileId,...expectedData}};
    return{source,contextId:'context'+index,status:'passed',before:{currentGeneration:null},ui:{gameVersion:'original',imported:source!=='online',importClosed:true},
      ...(source==='online'?{responses:[{expected:expectedData,actual:expectedData,contentIdentity:'passed'}]}:{input:raw?options.originalIdentity:options.archiveIdentity}),
      installed,pinned:structuredClone(installed),runtime,reopened:{installed:structuredClone(installed),pinned:structuredClone(installed),runtime:structuredClone(runtime),ui:{gameVersion:'original'}}};
  });return{e:{schema:'eagler-touhou/package-import-parity/1',routes,cleanupErrors:[]},options};
}
test('source parity accepts each committed version, DATA and cold reopen',()=>{
  const {e,options}=fixture();assert.equal(assertPackageImportParity(e,options).complete,true);
});
test('optional resources may extend the current generation while the running DATA stays pinned',()=>{
  const {e,options}=fixture(),row=e.routes[1];
  for(const [index,state]of [row.installed,row.reopened.installed].entries()){
    state.generation.id='extended'+index;state.installation.currentGeneration=state.generation.id;
    state.generation.files.optional={objectId:'optional-object'};
  }
  assert.equal(assertPackageImportParity(e,options).complete,true);
  row.reopened.installed.generation.files[PRODUCT_GAMES.th08.package.dataFileId].objectId='replacement';
  assert.throws(()=>assertPackageImportParity(e,options),/base object/);
});
for(const [name,mutate]of Object.entries({
  'missing raw route':e=>e.routes.pop(),
  'shared browser store':e=>e.routes[1].contextId=e.routes[0].contextId,
  'preinstalled route':e=>e.routes[0].before.currentGeneration='old',
  'online import':e=>e.routes[1].ui.imported=true,
  'no HTTP DATA':e=>e.routes[1].responses=[],
  'modified HTTP DATA':e=>e.routes[1].responses[0].actual={bytes:4096,sha256:'c'.repeat(64)},
  'undeclared raw filename':e=>e.routes[2].input={...e.routes[2].input,path:'invented.dat'},
  'wrong version':e=>e.routes[0].installed.generation.descriptor.gameVersion='beauty',
  'pending generation':e=>e.routes[0].installed.installation.pendingGeneration='pending',
  'wrong source':e=>e.routes[1].installed.installation.source='local',
  'wrong layout':e=>e.routes[0].installed.generation.descriptor.runtimeRequirement.dataLayout='other',
  'stale DATA':e=>e.routes[0].installed.data.sha256='c'.repeat(64),
  'wrong raw revision':e=>e.routes[2].installed.generation.descriptor.revision='guessed',
  'detached Runtime':e=>e.routes[0].runtime.frameUrl=e.routes[0].runtime.frameUrl.replace('installed0','stale'),
  'stale first frame':e=>e.routes[0].runtime.first.epoch=99,
  'no native present':e=>e.routes[0].runtime.first.presentation.nativePresentCount=0,
  'wrong provider':e=>e.routes[0].runtime.dataRead.generation='stale',
  'changed generation on reopen':e=>e.routes[0].reopened.installed.generation.id='other',
  'wrong version on reopen':e=>e.routes[2].reopened.ui.gameVersion='beauty',
  'cleanup error':e=>e.cleanupErrors.push('failure'),
}))test(`source parity rejects ${name}`,()=>{const {e,options}=fixture();mutate(e);assert.throws(()=>assertPackageImportParity(e,options));});
