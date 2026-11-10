import assert from 'node:assert/strict';
import {test} from 'node:test';
import {pc98RequiredBlockFiles} from '../package/pc98-block-selection.mjs';
import {desiredFilesForPublishedPackage} from '../package/package-launcher.mjs';

test('selects the disk for the chosen language and includes multiplayer preview', () => {
  const a = `pc98-block:${'a'.repeat(64)}`, b = `pc98-block:${'b'.repeat(64)}`;
  const file = name => ({source: `files/${name}`, target: `/files/${name}`, revision: '1'});
  const block = id => ({...file(id),target:`/pc98/disk-blocks/${id.slice('pc98-block:'.length)}.gz`});
  const descriptor = {schema: 'eagler-touhou/package/1', game: 'th02', revision: '1',
    runtimeRequirement: {protocol: 'eagler-touhou/1', target: 'th02', dataFile: 'data'}, base: {files: ['data']},
    files: {[a]: block(a), [b]: block(b), 'pc98-disk-manifest': {...file('manifest'),target:'/pc98/disks.json'}, data: file('data')}, components: {
    'pc98-manifest': {type: 'pc98-disk', files: ['pc98-disk-manifest']},
    'pc98-disk:th02-solo-jp.hdi.gz': {type: 'pc98-disk', files: [a]},
    'pc98-disk:th02-local-jp.hdi.gz': {type: 'pc98-disk', files: [a, b]},
  }};
  assert.deepEqual(pc98RequiredBlockFiles(descriptor, {game: 'th02', language: 'ja', multiplayer: false}), ['pc98-disk-manifest', a]);
  assert.deepEqual(pc98RequiredBlockFiles(descriptor, {game: 'th02', language: 'ja', multiplayer: true}), ['pc98-disk-manifest', a, b]);
  const current = {descriptor, files: {data: {objectId: 'data'}, [a]: {objectId: 'a'}}};
  assert.deepEqual(desiredFilesForPublishedPackage(descriptor, {current}), ['data', a], 'shared blocks must not pull in other disks');
  assert.throws(() => pc98RequiredBlockFiles(descriptor, {game: 'th02', language: 'lang_zh-hans', multiplayer: false}), /missing/);
});

test('TH03 Japanese multiplayer also installs the Chinese battle disk', () => {
  const a=`pc98-block:${'a'.repeat(64)}`,b=`pc98-block:${'b'.repeat(64)}`;
  const file=name=>({source:`files/${name}`,target:`/files/${name}`,revision:'1'});
  const block=id=>({...file(id),target:`/pc98/disk-blocks/${id.slice('pc98-block:'.length)}.gz`});
  const descriptor={schema:'eagler-touhou/package/1',game:'th03',revision:'1',
    runtimeRequirement:{protocol:'eagler-touhou/1',target:'th03',dataFile:'data'},base:{files:['data']},
    files:{[a]:block(a),[b]:block(b),data:file('data'),'pc98-disk-manifest':{...file('manifest'),target:'/pc98/disks.json'}},components:{
      'pc98-manifest':{type:'pc98-disk',files:['pc98-disk-manifest']},
      'pc98-disk:disks:3-jp.hdi.gz':{type:'pc98-disk',files:[a]},
      'pc98-disk:disks:3-cn.hdi.gz':{type:'pc98-disk',files:[b]},
    }};
  assert.deepEqual(pc98RequiredBlockFiles(descriptor,{game:'th03',language:'ja',multiplayer:true}),['pc98-disk-manifest',a,b]);
  assert.deepEqual(pc98RequiredBlockFiles(descriptor,{game:'th03',language:'lang_zh-hans',multiplayer:true}),['pc98-disk-manifest',b]);
  assert.deepEqual(pc98RequiredBlockFiles(descriptor,{game:'th03',language:'lang_zh-hans',multiplayer:false,replay:true}),['pc98-disk-manifest',a,b]);
});
