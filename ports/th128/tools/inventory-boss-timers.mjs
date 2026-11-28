import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const read=file=>JSON.parse(fs.readFileSync(file,'utf8'));
const catalog=read(path.join(root,'local/catalog.json'));
const directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
const table=read(path.join(root,'local/research/stage-table.json'));
const routes=table.stages.map(stage=>{
  const main=read(path.join(directory,'ecl',stage.ecl.replace(/\.ecl$/,'.json')));
  const phases=[];
  for(const source of new Set([stage.ecl,...(main.includes?.ECLI??[])])){
    const program=read(path.join(directory,'ecl',source.replace(/\.ecl$/,'.json')));
    const sha256=createHash('sha256').update(fs.readFileSync(path.join(directory,'raw',source))).digest('hex');
    for(const subroutine of program.subroutines)for(const command of subroutine.commands){
      if(command.opcode!==414)continue;
      const bytes=Buffer.from(command.raw,'hex');if(bytes.length<16)throw new Error(`Truncated original414: ${source}`);
      const literal=(command.mask&4)===0,frames=literal?bytes.readInt32LE(8):null;
      phases.push({source,sourceSha256:sha256,subroutine:subroutine.name,offset:command.offset,difficultyMask:command.rankMask,
        timeLimitFrames:frames,timeLimitSeconds:frames===null?null:frames/60,literalTime:literal,rawLimitWord:bytes.readUInt32LE(8)});
    }
  }
  return {stage:stage.ecl,phases,gameplayComplete:false};
});
const output={schema:'th128-original-boss-timer-inventory/1',sourceExeSha256:table.exeSha256,routes,
  logicTicksPerSecond:60,displayRule:'truncate(remaining/60), truncate((remaining%60)*100/60), native99.99 cap',
  scope:'Static original414 time limits only. This inventory is not evidence of completed fights or route gameplay.'};
fs.writeFileSync(path.join(root,'docs/boss-timer-inventory.json'),JSON.stringify(output,null,2)+'\n');
console.log(`${routes.length} original route records, ${routes.reduce((total,route)=>total+route.phases.length,0)} authored phase limits; no gameplay completion claimed.`);
