import {existsSync, statSync} from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
import {PRODUCT_CONTENT} from '../lib/content-definition.mjs';
import {workspacePath} from '../lib/workspace-layout.mjs';

const project=fileURLToPath(new URL('..',import.meta.url));
const preparation=PRODUCT_CONTENT.th12.hostPreparation.preparedContent;
function buildSource(role){
  const source=preparation.buildSources.find(entry=>entry.role===role);
  if(!source)throw Error('TH12: missing declared '+role+' build source');
  const bundled=path.resolve(project,source.target);
  if(statSync(bundled,{throwIfNoEntry:false})?.isFile())return bundled;
  const canonical=workspacePath(source.repository,source.source);
  if(statSync(canonical,{throwIfNoEntry:false})?.isFile())return canonical;
  throw Error('TH12: preparation source is unavailable: '+source.source+' (bundle: '+bundled+'; workspace: '+canonical+')');
}

const argument=(name,fallback)=>{
  const inline=process.argv.find(value=>value.startsWith(name+'='));if(inline)return inline.slice(name.length+1);
  const index=process.argv.indexOf(name);return index<0?fallback:process.argv[index+1];
};
const original=argument('--original',''),output=argument('--output','');
if(!original||!output)throw Error('Usage: node scripts/prepare-th12-content.mjs --original=DIR --output=DIR [--assets=PREPARED_ASSETS]');
const assets=argument('--assets','');
if(assets&&!existsSync(path.join(assets,'menu-resources.json')))throw Error('Prepared TH12 asset directory is incomplete.');
const script=buildSource(assets?'managed-content-preparer':'retail-preparer');
const options=assets?['--assets='+path.resolve(assets)]:['--original='+path.resolve(original),
  '--python='+(process.env.EAGLER_CONTENT_PYTHON??process.env.TH12_PYTHON??process.env.PYTHON??'python'),
  '--thdat='+(process.env.EAGLER_CONTENT_THDAT??process.env.TH12_THDAT??process.env.THDAT??'thdat')];
const result=spawnSync(process.execPath,[script,...options,'--output='+path.resolve(output)],{stdio:'inherit',windowsHide:true});
if(result.error)throw result.error;process.exitCode=result.status??1;
