import {createHash} from 'node:crypto';
import {mkdtemp,readFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join,resolve} from 'node:path';
import {publishPC98Blocks} from '../../launcher/lib/pc98-static-publication.mjs';
import {pc98RequiredBlockFiles} from '../../launcher/package/pc98-block-selection.mjs';

export async function pc98SplitFixture(portRoot,game) {
  const stage=await mkdtemp(join(tmpdir(),'pc98-split-fixture-'));
  try {
    const source=resolve(portRoot,'content'),files={},components={};
    const descriptorFile=async(source,target)=>{const b=await readFile(join(stage,source)),sha256=createHash('sha256').update(b).digest('hex');return {source,target,sha256,bytes:b.length,revision:sha256.slice(0,16)};};
    await publishPC98Blocks({game,source,data:resolve(source,`${game}.data`),staging:stage,descriptorFile,packageFiles:files,components});
    files.data={source:`games/${game}/${game}.data`,target:`/${game}.data`,revision:'1'};
    const descriptor={schema:'eagler-touhou/package/1',game,revision:'1',files,components,runtimeRequirement:{protocol:'eagler-touhou/1',target:game,dataFile:'data'},base:{files:['data']}};
    const ids=pc98RequiredBlockFiles(descriptor,{game,language:'ja',multiplayer:true});
    return {data:await readFile(join(stage,`games/${game}/${game}.data`)),resources:ids.map(id=>files[id]),buffers:await Promise.all(ids.map(id=>readFile(join(stage,files[id].source))))};
  } finally {await rm(stage,{recursive:true,force:true});}
}
