import fs from 'node:fs/promises';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const read=async name=>JSON.parse(await fs.readFile(name,'utf8'));
const catalog=await read(path.join(root,'local/catalog.json'));
const directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
const table=await read(path.join(root,'local/research/stage-table.json'));
const runtime=await read(path.join(root,'site/runtime/build.json'));
const routes=[];
for(const stage of table.stages){
  const file=path.join(directory,'ecl',stage.ecl.replace(/\.ecl$/,'.json'));
  const program=await read(file);
  const stageSource=await fs.readFile(path.join(directory,'raw',stage.ecl));
  const dependencies=[];
  for(const include of program.includes?.ECLI??[]){
    const source=await fs.readFile(path.join(directory,'raw',include));
    dependencies.push({name:include,sha256:createHash('sha256').update(source).digest('hex')});
  }
  routes.push({...stage,raw:undefined,pointers:undefined,address:undefined,uninterpretedFields:undefined,
    sourceSha256:createHash('sha256').update(stageSource).digest('hex'),dependencies,
    subroutines:program.subroutines.length,instructions:program.subroutines.reduce((total,sub)=>total+sub.commands.length,0),
    bossEntries:program.subroutines.filter(sub=>/^(?:M?Boss)/.test(sub.name)).map(sub=>({name:sub.name,instructions:sub.commands.length})),
    resourceParsed:true,completeGameplayVerified:false,
    runStatus:stage.ecl==='st_a1_1.ecl'?'first-stage-midboss-development-only':'not-yet-executed-in-production-world'});
}
const output={schema:'th128-honest-stage-coverage/1',runtimeGeneration:runtime.generation,sourceExeSha256:table.exeSha256,
  preparedGeneration:path.relative(path.join(root,'local'),directory).replaceAll('\\','/'),routes,
  originalTableReadOnly:true,allStagesComplete:false,scope:'Static original route/resource inventory only. Parsing or listing a Boss is not gameplay completion. Runtime checkpoints are recorded separately.'};
await fs.writeFile(path.join(root,'docs/stage-coverage.json'),JSON.stringify(output,null,2)+'\n');
console.log(`${routes.length} original route records inventoried; complete gameplay verified: 0. Current world remains first-stage development.`);
