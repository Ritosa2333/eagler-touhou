import test, {before, after} from 'node:test';
import assert from 'node:assert/strict';
import {existsSync} from 'node:fs';
import {mkdtemp, rm} from 'node:fs/promises';
import {createRequire} from 'node:module';
import {dirname, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {installMountedDom} from './mounted-dom-environment.mjs';

// Mounted components test the native-selection boundary independently of the
// legacy frontend's pinned DOM. Gameplay and spectators use the browser test.
const project = fileURLToPath(new URL('../../', import.meta.url));
const require = createRequire(resolve(project, 'package.json'));
let env, React, createRoot, owners, work;
before(async () => {
  env = installMountedDom();
  React = await import('react');
  ({createRoot} = await import('react-dom/client'));
  work = await mkdtemp(resolve(project, '.cache/th09-selection-components-'));
  await require('esbuild').build({absWorkingDir:project,
    stdin:{resolveDir:project, loader:'ts', contents:`
      export * from './app/components/room/RoomPersonal.tsx';
      export * from './app/components/room/RoomGameSettings.tsx';
      export * from './app/components/directory/DirectoryRoomDialog.tsx';
      export * from './app/components/directory/DirectoryRoomRows.tsx';
      export * from './app/i18n.tsx';
      export * from './src/contracts/product-catalog.mts';
    `}, bundle:true, platform:'node', format:'esm', jsx:'automatic',
    outfile:resolve(work,'components.mjs'), logLevel:'silent',
    plugins:[{name:'installed-dependencies',setup(ctx){
      ctx.onResolve({filter:/^[^./]/},args=>({path:pathToFileURL(require.resolve(args.path)).href,external:true}));
      ctx.onResolve({filter:/^\.\.?\/.*\.mjs$/},args=>{
        const path=resolve(dirname(args.importer),args.path).replace(/\.mjs$/,'.mts');
        if(existsSync(path))return {path};
      });
    }}]});
  owners = await import(pathToFileURL(resolve(work,'components.mjs')));
});
after(async () => {env.close(); await rm(work,{recursive:true,force:true});});

for(const locale of ['zh-CN','en']) for(const product of ['th09mp','th06mp']) {
  test(`${product} room and directory selection controls (${locale})`,async()=>{
    const native=product==='th09mp', calls=[];
    const state={product,preferredLoadout:0,connected:true,seat:0,displayName:'Alice',rollbackEnabled:true,timingChoice:'auto',
      room:{synced:true,playerCount:2,difficulty:1,phase:'lobby',seats:[{clientId:'host'},null]}};
    const service={timingRecommendation:()=>({inputDelay:0}),setRoomSettings:value=>calls.push(value)};
    const form={mode:'create',product,capacity:2,difficulty:1,code:'',visibility:'public',disableCheatMovement:false,validationMessage:''};
    const snapshot={connection:'live',products:[product],form,visibleRooms:[{
      product,code:'1234',capacity:2,players:1,ready:0,difficulty:1,spectators:0,
      phase:'lobby',joinable:true,seats:[{initial:'A',name:'Alice',ready:false},null],
    }]};
    const directory={getSnapshot:()=>snapshot,subscribe:()=>()=>{},formDidClose(){}};
    const container=env.document.createElement('div'); env.document.body.append(container);
    const root=createRoot(container), h=React.createElement;
    try {
      await React.act(async()=>root.render(h(owners.LocaleProvider,{locale},h(React.Fragment,null,
        h(owners.RoomPersonal,{state,service,assetUrl:x=>x,hidden:false,th09NetworkOverlayOpen:false}),
        h(owners.RoomGameSettings,{state,service,hidden:false}),
        h(owners.DirectoryRoomDialog,{service:directory,open:true,onCloseRequest(){}}),
        h(owners.DirectoryRoomRows,{snapshot,service:directory,assetUrl:x=>x}),
      ))));
      const q=selector=>{const node=container.querySelector(selector);assert.ok(node,selector);return node;};
      assert.equal(container.querySelectorAll('#mpLoadoutPrevSeat, #mpLoadoutNextSeat, #mpLoadoutPrev, #mpLoadoutNext').length,native?0:4);
      assert.deepEqual([...container.querySelectorAll('[data-mp-difficulty]')].map(node=>node.textContent),
        native?[]:owners.multiplayerConfigForProduct(product).difficulties);
      assert.equal(q('#difficultySelect').disabled,native);
      assert.equal(q('#difficultySelect').closest('label').hidden,native);
      if(native){
        const label='';
        assert.equal(q('#mpLocalRoleLabel').textContent,label);
        assert.equal(q('.lobby-difficulty').textContent,label);
        assert.match(q('#mpRoomSettings').textContent,locale==='zh-CN'?/双方/:/Both players/);
        assert.match(q('#roomForm').textContent,locale==='zh-CN'?/双方/:/Both players/);
        assert.deepEqual(calls,[]);
      } else {
        await React.act(async()=>q('[data-mp-difficulty="3"]').click());
        assert.deepEqual(calls,[{difficulty:3}]);
        assert.equal(q('.lobby-difficulty').textContent,'Normal');
      }
    } finally {
      await React.act(async()=>root.unmount()); container.remove();
    }
    assert.deepEqual(env.errors.splice(0),[]);
  });
}
