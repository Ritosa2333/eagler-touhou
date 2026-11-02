// Standard directory Runtime output consumed by the main station.
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
const root=path.resolve(import.meta.dirname,'..');
const argument=process.argv.find(value=>value.startsWith('--output='));
const out=path.resolve(argument?.slice(9)??process.env.EAGLER_RUNTIME_OUTPUT??path.join(root,'build-eagler'));
const manifest=path.join(out,'manifest.json');
if(fs.existsSync(manifest)&&JSON.parse(fs.readFileSync(manifest,'utf8')).adapter!=='cpp-sdl3-gles'){
  if(out!==path.join(root,'build-eagler'))throw Error('Use an empty output directory for the SDL Runtime');
  const archive=path.join(root,'artifacts','legacy-runtime-'+Date.now());fs.mkdirSync(path.dirname(archive),{recursive:true});fs.renameSync(out,archive);
}
const result=spawnSync(process.execPath,[path.join(import.meta.dirname,'package-sdl.mjs'),'--output='+out],{cwd:root,stdio:'inherit',windowsHide:true,env:process.env});
if(result.error)throw result.error;if(result.status!==0)process.exit(result.status??1);
