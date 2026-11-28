import {createHash} from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const tablePath=path.join(root,'local/research/stage-table.json');
const catalogPath=path.join(root,'local/catalog.json');
const pendingPath=path.join(root,'artifacts/dsh-stage-loading/stage-catalog.json');
const exeSha256='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4';
const namePattern=/^[A-Za-z0-9_][A-Za-z0-9_.-]{0,63}$/;
const extensions={background:'.std',ecl:'.ecl',message:'.msg',logo:'.anm'};

function resourceName(value,extension,label){
  if(typeof value!=='string'||!namePattern.test(value)||value.includes('..')||!value.endsWith(extension))
    throw new Error(`${label}: unsafe ${extension} basename`);
  return value;
}

function sha256(bytes){
  return createHash('sha256').update(bytes).digest('hex');
}

function readJson(file){
  return JSON.parse(fs.readFileSync(file,'utf8'));
}

const tableBytes=fs.readFileSync(tablePath);
const tableSha256=sha256(tableBytes);
const table=JSON.parse(tableBytes.toString('utf8'));
if(table.schema!=='th128-original-stage-table/1'||table.exeSha256!==exeSha256||table.stride!==40
  ||!Array.isArray(table.stages)||table.stages.length!==16)throw new Error('Invalid original stage table');
const stages=[];
const seen=new Set();
for(const stage of table.stages){
  if(!Number.isInteger(stage.id)||stage.id<1||stage.id>16||seen.has(stage.id))
    throw new Error(`Invalid stage id: ${stage.id}`);
  seen.add(stage.id);
  const record={id:stage.id};
  for(const [field,extension] of Object.entries(extensions))
    record[field]=resourceName(stage[field],extension,`stage ${stage.id} ${field}`);
  if(!Array.isArray(stage.music)||stage.music.length!==2)
    throw new Error(`stage ${stage.id} music count`);
  record.music=stage.music.map(value=>resourceName(value,'.wav',`stage ${stage.id} music`));
  stages.push(record);
}
if(seen.size!==16)throw new Error('Stage id coverage is not 1..16');
stages.sort((left,right)=>left.id-right.id);

const catalog=readJson(catalogPath);
if(catalog.schema!=='th128-asset-catalog/1'||!catalog.variants||catalog.variants.ja?.manifest===undefined
  ||catalog.variants.zh?.manifest===undefined)throw new Error('Invalid asset catalog');
const published={schema:'th128-stage-catalog/1',sourceExeSha256:exeSha256,sourceTableSha256:tableSha256,stages};
const text=JSON.stringify(published,null,2)+'\n';
const targets=[];

for(const variant of ['ja','zh']){
  const manifestPath=path.resolve(root,'local',catalog.variants[variant].manifest);
  const variantRoot=path.dirname(manifestPath);
  const manifest=readJson(manifestPath);
  if(manifest.schema!=='th128-prepared-assets/1'||manifest.variant!==variant)
    throw new Error(`Invalid prepared manifest: ${variant}`);
  const animations=new Set((manifest.animations??[]).map(entry=>entry.name));
  const ecls=new Set((manifest.ecl??[]).map(entry=>entry.name));
  for(const stage of stages){
    const backgroundAnm=`${stage.background.slice(0,-4)}.anm`;
    if(!animations.has(backgroundAnm))throw new Error(`stage ${stage.id} background ANM missing: ${backgroundAnm}`);
    if(!ecls.has(stage.ecl))throw new Error(`stage ${stage.id} ECL missing: ${stage.ecl}`);
    const eclFile=path.join(variantRoot,'ecl',`${stage.ecl.slice(0,-4)}.json`);
    if(!fs.existsSync(eclFile))throw new Error(`stage ${stage.id} ECL parse missing`);
    const parsed=readJson(eclFile);
    const includes=parsed.includes??{};
    if(parsed.format!=='SCPT/1'||!Array.isArray(includes.ECLI)||!Array.isArray(includes.ANIM))
      throw new Error(`stage ${stage.id} include metadata invalid`);
    for(const include of includes.ECLI){
      resourceName(include,'.ecl',`stage ${stage.id} ECLI`);
      if(!ecls.has(include))throw new Error(`stage ${stage.id} ECLI missing: ${include}`);
    }
    for(const include of includes.ANIM){
      resourceName(include,'.anm',`stage ${stage.id} ANIM`);
      if(!animations.has(include))throw new Error(`stage ${stage.id} ANIM missing: ${include}`);
    }
  }
  targets.push(path.join(variantRoot,'animations/stage-catalog.json'));
}

try{
  fs.mkdirSync(path.dirname(targets[0]),{recursive:true});
  for(const target of targets)fs.writeFileSync(target,text,{encoding:'utf8',flag:'w'});
  console.log(`published ${targets.map(target=>path.relative(root,target)).join(', ')}`);
}catch(problem){
  if(!['EACCES','EPERM','EROFS'].includes(problem.code))throw problem;
  fs.mkdirSync(path.dirname(pendingPath),{recursive:true});
  fs.writeFileSync(pendingPath,text,{encoding:'utf8'});
  console.log(`pending ${path.relative(root,pendingPath)} (asset generation ACL denied)`);
}
