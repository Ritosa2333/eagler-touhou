import {createHash} from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const catalogPath=path.join(root,'local/catalog.json');
const stageTablePath=path.join(root,'local/research/stage-table.json');
const scratch=path.join(root,'artifacts/dsh-menu-assets');
const texturePattern=/^textures\/[A-Za-z0-9_.-]+\.png$/;
const exeSha256='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4';

const mainRows=[
  {id:'start',label:'START',order:0,y:0,atlasY:0,baseSprite:1,selectedSprite:0,baseScript:10,selectedScript:3},
  {id:'replay',label:'REPLAY',order:1,y:28,atlasY:24,baseSprite:3,selectedSprite:2,baseScript:11,selectedScript:4},
  {id:'result',label:'RESULT',order:2,y:56,atlasY:48,baseSprite:5,selectedSprite:4,baseScript:12,selectedScript:5},
  {id:'music',label:'MUSIC',order:3,y:84,atlasY:72,baseSprite:7,selectedSprite:6,baseScript:13,selectedScript:6},
  {id:'options',label:'OPTION',order:4,y:112,atlasY:96,baseSprite:9,selectedSprite:8,baseScript:14,selectedScript:7},
  {id:'manual',label:'MANUAL',order:5,y:140,atlasY:144,baseSprite:17,selectedSprite:16,baseScript:15,selectedScript:8},
  {id:'quit',label:'QUIT',order:6,y:168,atlasY:120,baseSprite:11,selectedSprite:10,baseScript:16,selectedScript:9}
];
const difficultyRows=[
  {id:0,label:'Easy Mode',entry:8,name:'title/rank00.png',sprite:80,script:116,dimmedEntry:12,dimmedName:'title/rank00b.png',dimmedSprite:84,dimmedScript:120},
  {id:1,label:'Normal Mode',entry:9,name:'title/rank01.png',sprite:81,script:117,dimmedEntry:13,dimmedName:'title/rank01b.png',dimmedSprite:85,dimmedScript:121},
  {id:2,label:'Hard Mode',entry:10,name:'title/rank02.png',sprite:82,script:118,dimmedEntry:14,dimmedName:'title/rank02b.png',dimmedSprite:86,dimmedScript:122},
  {id:3,label:'Lunatic Mode',entry:11,name:'title/rank03.png',sprite:83,script:119,dimmedEntry:15,dimmedName:'title/rank03b.png',dimmedSprite:87,dimmedScript:123}
];
const routeLabels={
  ja:['魔法の森 Route A','霧の湖 Route B','春の小径 Route C'],
  zh:['魔法之森 Route A','雾之湖 Route B','春之小径 Route C']
};
const routeRows=[
  {stageId:1,id:'A',entry:16,name:'title/stage00.png',sprite:88,script:124,background:'stage_a1.std',ecl:'st_a1_1.ecl'},
  {stageId:6,id:'B',entry:17,name:'title/stage01.png',sprite:89,script:125,background:'stage_b1.std',ecl:'st_b1_1.ecl'},
  {stageId:11,id:'C',entry:18,name:'title/stage02.png',sprite:90,script:126,background:'stage_c1.std',ecl:'st_c1_1.ecl'}
];

function fail(message){
  throw new Error(message);
}

function sha256(bytes){
  return createHash('sha256').update(bytes).digest('hex');
}

function readJson(file){
  return JSON.parse(fs.readFileSync(file,'utf8'));
}

function pngSize(bytes){
  if(bytes.length<24)fail('short PNG');
  if(bytes.toString('hex',0,8)!=='89504e470d0a1a0a')fail('PNG signature');
  if(bytes.toString('ascii',12,16)!=='IHDR')fail('PNG IHDR');
  return {width:bytes.readUInt32BE(16),height:bytes.readUInt32BE(20)};
}

function intCommand(command){
  if(command.raw.length<8)fail('short integer command');
  return Buffer.from(command.raw,'hex').readInt32LE(0);
}

function floatCommand(command,index){
  if(command.raw.length<index*8+8)fail('short float command');
  return Buffer.from(command.raw,'hex').readFloatLE(index*4);
}

function scriptById(metadata,id){
  const script=metadata.scripts[id];
  if(!script||script.index!==id)fail(`script ${id} missing`);
  return script;
}

function scriptSprite(metadata,id,expected){
  const script=scriptById(metadata,id);
  const command=script.commands.find(entry=>entry.opcode===3);
  if(!command)fail(`script ${id} has no sprite selection`);
  const sprite=intCommand(command);
  if(sprite!==expected)fail(`script ${id} selects sprite ${sprite}, expected ${expected}`);
  return script;
}

function scriptPosition(script,expectedY){
  const command=script.commands.find(entry=>entry.opcode===48);
  if(!command)fail(`script ${script.index} has no position`);
  const x=floatCommand(command,0),y=floatCommand(command,1);
  if(x!==0||y!==expectedY)fail(`script ${script.index} position ${x},${y}, expected 0,${expectedY}`);
}

function scriptInterrupt(script,code){
  if(!script.commands.some(entry=>entry.opcode===64&&entry.raw.length>=8&&intCommand(entry)===code))
    fail(`script ${script.index} lacks interrupt ${code}`);
}

function verifyMetadata(variantRoot,manifest,variant,name,counts){
  const animation=manifest.animations.find(entry=>entry.name===name);
  if(!animation)fail(`${variant}: manifest lacks ${name}`);
  if(animation.file!==`animations/${name.slice(0,-4)}.json`)fail(`${variant}: ${name} metadata path`);
  if(animation.entries!==counts.entries||animation.sprites!==counts.sprites
    ||animation.scripts!==counts.scripts||animation.textures!==counts.textures)
    fail(`${variant}: ${name} manifest counts`);
  const raw=manifest.files.find(entry=>entry.name===name);
  if(!raw||raw.sha256===undefined||raw.bytes===undefined)fail(`${variant}: ${name} raw identity`);
  const file=path.join(variantRoot,animation.file);
  if(!fs.existsSync(file))fail(`${variant}: ${animation.file} missing`);
  const bytes=fs.readFileSync(file);
  const metadata=JSON.parse(bytes.toString('utf8'));
  if(metadata.version!==7)fail(`${variant}: ${name} version`);
  if(metadata.provenance?.name!==name||metadata.provenance?.sha256!==raw.sha256
    ||metadata.provenance?.bytes!==raw.bytes)fail(`${variant}: ${name} provenance`);
  if(metadata.entries.length!==counts.entries||metadata.sprites.length!==counts.sprites
    ||metadata.scripts.length!==counts.scripts)fail(`${variant}: ${name} decoded counts`);
  return {metadata,rawSha256:raw.sha256,metadataSha256:sha256(bytes),textures:animation.textures};
}

function textureCache(variantRoot){
  const cache=new Map();
  return file=>{
    if(cache.has(file))return cache.get(file);
    const full=path.resolve(variantRoot,file);
    const texturesRoot=path.resolve(variantRoot,'textures')+path.sep;
    if(!full.startsWith(texturesRoot))fail(`texture escapes textures/: ${file}`);
    if(!fs.existsSync(full))fail(`missing ${file}`);
    const bytes=fs.readFileSync(full);
    const record={sha256:sha256(bytes),...pngSize(bytes)};
    cache.set(file,record);
    return record;
  };
}

function art(metadata,textures,spriteIndex,expected){
  const sprite=metadata.sprites[spriteIndex];
  if(!sprite||sprite.index!==spriteIndex)fail(`sprite ${spriteIndex} missing`);
  if(sprite.entry!==expected.entry)fail(`sprite ${spriteIndex} entry ${sprite.entry}`);
  const entry=metadata.entries[expected.entry];
  if(!entry||entry.name!==expected.name)fail(`entry ${expected.entry} is not ${expected.name}`);
  if(!entry.texture)fail(`entry ${expected.entry} has no texture`);
  const file=entry.texture.file;
  if(typeof file!=='string'||!texturePattern.test(file))fail(`unsafe texture path ${file}`);
  const texture=textures(file);
  if(texture.sha256!==entry.texture.sha256)fail(`${file} sha256 differs from metadata`);
  if(texture.width!==entry.texture.width||texture.height!==entry.texture.height)
    fail(`${file} pixels differ from metadata dimensions`);
  for(const value of [sprite.x,sprite.y,sprite.width,sprite.height])
    if(!Number.isInteger(value)||!Number.isFinite(value))fail(`sprite ${spriteIndex} fractional bounds`);
  if(sprite.x<0||sprite.y<0||sprite.width<=0||sprite.height<=0)fail(`sprite ${spriteIndex} empty bounds`);
  if(sprite.x+sprite.width>texture.width||sprite.y+sprite.height>texture.height)
    fail(`sprite ${spriteIndex} leaves texture`);
  if(expected.x!==undefined&&(sprite.x!==expected.x||sprite.y!==expected.y
    ||sprite.width!==expected.width||sprite.height!==expected.height))
    fail(`sprite ${spriteIndex} rect ${sprite.x},${sprite.y},${sprite.width},${sprite.height}`);
  return {file,x:sprite.x,y:sprite.y,width:sprite.width,height:sprite.height,
    textureWidth:texture.width,textureHeight:texture.height,sha256:entry.texture.sha256};
}

const stageTable=readJson(stageTablePath);
if(stageTable.schema!=='th128-original-stage-table/1'||stageTable.exeSha256!==exeSha256)
  fail('invalid original stage table');
for(const route of routeRows){
  const stage=stageTable.stages.find(entry=>entry.id===route.stageId);
  if(!stage)fail(`stage table lacks id ${route.stageId}`);
  if(stage.background!==route.background||stage.ecl!==route.ecl)
    fail(`stage ${route.stageId} table record changed`);
}
const catalog=readJson(catalogPath);
if(catalog.schema!=='th128-asset-catalog/1'||catalog.playable!==false)fail('invalid asset catalog');
if(!catalog.variants?.ja?.manifest||!catalog.variants?.zh?.manifest)fail('catalog lacks variants');

const outputs=[];
for(const variant of ['ja','zh']){
  const variantRoot=path.dirname(path.resolve(root,'local',catalog.variants[variant].manifest));
  const manifest=readJson(path.join(variantRoot,'manifest.json'));
  if(manifest.schema!=='th128-prepared-assets/1'||manifest.variant!==variant||manifest.game!=='th128')
    fail(`${variant}: invalid prepared manifest`);
  const title=verifyMetadata(variantRoot,manifest,variant,'title.anm',
    {entries:26,sprites:170,scripts:214,textures:25});
  const version=verifyMetadata(variantRoot,manifest,variant,'title_v.anm',
    {entries:1,sprites:1,scripts:1,textures:1});
  if(title.metadata.entries[0]?.name!=='title/title01.png')fail(`${variant}: main atlas entry name`);
  const textures=textureCache(variantRoot);

  const main=[];
  for(const row of mainRows){
    const base=scriptSprite(title.metadata,row.baseScript,row.baseSprite);
    const selected=scriptSprite(title.metadata,row.selectedScript,row.selectedSprite);
    scriptPosition(base,row.y);
    scriptPosition(selected,row.y);
    scriptInterrupt(selected,17+row.order);
    const baseArt=art(title.metadata,textures,row.baseSprite,
      {entry:0,name:'title/title01.png',x:160,y:row.atlasY,width:160,height:24});
    const selectedArt=art(title.metadata,textures,row.selectedSprite,
      {entry:0,name:'title/title01.png',x:0,y:row.atlasY,width:160,height:24});
    if(baseArt.file!==selectedArt.file)fail(`${variant}: main ${row.id} columns differ by atlas`);
    if(baseArt.sha256!==selectedArt.sha256)fail(`${variant}: main ${row.id} atlas hash`);
    main.push({
      id:row.id,key:row.id,label:row.label,order:row.order,y:row.y,sprite:baseArt,selectedSprite:selectedArt,
      source:{entry:0,entryName:'title/title01.png',sprite:row.baseSprite,script:row.baseScript,
        selectedSprite:row.selectedSprite,selectedScript:row.selectedScript,selectedInterrupt:17+row.order}
    });
  }

  const difficulty=[];
  for(const row of difficultyRows){
    const active=scriptSprite(title.metadata,row.script,row.sprite);
    const dimmed=scriptSprite(title.metadata,row.dimmedScript,row.dimmedSprite);
    const activeArt=art(title.metadata,textures,row.sprite,
      {entry:row.entry,name:row.name,x:1,y:1,width:254,height:254});
    const dimmedArt=art(title.metadata,textures,row.dimmedSprite,
      {entry:row.dimmedEntry,name:row.dimmedName,x:1,y:1,width:254,height:254});
    if(activeArt.sha256===dimmedArt.sha256)fail(`${variant}: difficulty ${row.id} art duplicated`);
    difficulty.push({
      id:row.id,label:row.label,art:activeArt,dimmedArt,
      source:{entry:row.entry,name:row.name,sprite:row.sprite,script:row.script,
        dimmedEntry:row.dimmedEntry,dimmedName:row.dimmedName,dimmedSprite:row.dimmedSprite,dimmedScript:row.dimmedScript}
    });
  }

  const routes=[];
  for(let index=0;index<routeRows.length;index++){
    const row=routeRows[index];
    scriptSprite(title.metadata,row.script,row.sprite);
    const routeArt=art(title.metadata,textures,row.sprite,
      {entry:row.entry,name:row.name,x:1,y:1,width:254,height:254});
    routes.push({
      stageId:row.stageId,id:row.id,label:routeLabels[variant][index],art:routeArt,
      source:{entry:row.entry,name:row.name,sprite:row.sprite,script:row.script,
        background:row.background,ecl:row.ecl}
    });
  }

  for(const spriteIndex of [91,92]){
    const sprite=title.metadata.sprites[spriteIndex];
    if(!sprite)fail(`${variant}: extra stage sprite ${spriteIndex} missing`);
    if(sprite.entry!==19&&sprite.entry!==20)fail(`${variant}: extra stage sprite ${spriteIndex} entry`);
  }

  outputs.push({
    variant,target:path.join(variantRoot,'animations/menu-ui.json'),
    data:{
      schema:'th128-original-menu-ui/1',
      variant,
      sourceTitleSha256:title.rawSha256,
      sourceMetadataHash:title.metadataSha256,
      titleVersionSha256:version.rawSha256,
      titleVersionMetadataHash:version.metadataSha256,
      main,difficulty,routes
    }
  });
}

let pending=0;
for(const output of outputs){
  const text=JSON.stringify(output.data,null,2)+'\n';
  try{
    fs.mkdirSync(path.dirname(output.target),{recursive:true});
    const temporary=`${output.target}.tmp-${process.pid}`;
    fs.writeFileSync(temporary,text,{encoding:'utf8'});
    fs.renameSync(temporary,output.target);
    console.log(`published ${path.relative(root,output.target)}`);
  }catch(problem){
    if(!['EACCES','EPERM','EROFS'].includes(problem.code))throw problem;
    fs.mkdirSync(scratch,{recursive:true});
    const fallback=path.join(scratch,`menu-ui-${output.variant}.pending.json`);
    fs.writeFileSync(fallback,text,{encoding:'utf8'});
    pending++;
    console.log(`pending ${path.relative(root,fallback)} (generation write denied)`);
  }
}
if(!pending)console.log('menu-ui: 7 main rows, 4 difficulties, 3 routes per variant; hashes and bounds verified');
