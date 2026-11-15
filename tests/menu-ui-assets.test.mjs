import assert from 'node:assert/strict';
import test from 'node:test';
import {createHash} from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
const stageTable=JSON.parse(fs.readFileSync(path.join(root,'local/research/stage-table.json'),'utf8'));
const mainIds=['start','replay','result','music','options','quit','manual'];
const difficultyLabels=['Easy Mode','Normal Mode','Hard Mode','Lunatic Mode'];
const routeIds={1:'A',6:'B',11:'C'};
const routeLabels={ja:['魔法の森 Route A','霧の湖 Route B','春の小径 Route C'],
  zh:['魔法之森 Route A','雾之湖 Route B','春之小径 Route C']};

function sha256(bytes){
  return createHash('sha256').update(bytes).digest('hex');
}

function pngSize(bytes){
  assert.equal(bytes.toString('hex',0,8),'89504e470d0a1a0a');
  assert.equal(bytes.toString('ascii',12,16),'IHDR');
  return {width:bytes.readUInt32BE(16),height:bytes.readUInt32BE(20)};
}

function variantRoot(variant){
  return path.dirname(path.resolve(root,'local',catalog.variants[variant].manifest));
}

function metadataFor(variant){
  const bytes=fs.readFileSync(path.join(variantRoot(variant),'animations/title.json'));
  return {bytes,metadata:JSON.parse(bytes.toString('utf8'))};
}

function checkArt(variant,metadata,cache,art,expected){
  assert.match(art.file,/^textures\/[A-Za-z0-9_.-]+\.png$/,`${variant}: unsafe texture path`);
  const texturesRoot=path.resolve(variantRoot(variant),'textures')+path.sep;
  const full=path.resolve(variantRoot(variant),art.file);
  assert.ok(full.startsWith(texturesRoot),`${variant}: texture outside textures/`);
  assert.ok(fs.existsSync(full),`${variant}: missing ${art.file}`);
  if(!cache.has(art.file)){
    const bytes=fs.readFileSync(full);
    cache.set(art.file,{sha256:sha256(bytes),...pngSize(bytes)});
  }
  const actual=cache.get(art.file);
  assert.equal(actual.sha256,art.sha256,`${variant}: ${art.file} digest`);
  assert.equal(actual.width,art.textureWidth,`${variant}: ${art.file} width`);
  assert.equal(actual.height,art.textureHeight,`${variant}: ${art.file} height`);
  for(const value of [art.x,art.y,art.width,art.height,art.textureWidth,art.textureHeight])
    assert.ok(Number.isInteger(value),`${variant}: non-integer bound`);
  assert.ok(art.x>=0&&art.y>=0&&art.width>0&&art.height>0,`${variant}: empty bounds`);
  assert.ok(art.x+art.width<=art.textureWidth,`${variant}: horizontal overflow`);
  assert.ok(art.y+art.height<=art.textureHeight,`${variant}: vertical overflow`);
  const sprite=metadata.sprites[expected.sprite];
  assert.ok(sprite,`${variant}: sprite ${expected.sprite} missing`);
  assert.equal(sprite.entry,expected.entry,`${variant}: sprite ${expected.sprite} entry`);
  assert.equal(metadata.entries[expected.entry].name,expected.name,`${variant}: entry name`);
  assert.equal(metadata.entries[expected.entry].texture.sha256,art.sha256,`${variant}: metadata texture digest`);
  assert.deepEqual([art.x,art.y,art.width,art.height],
    [sprite.x,sprite.y,sprite.width,sprite.height],`${variant}: sprite ${expected.sprite} rect`);
  const command=metadata.scripts[expected.script]?.commands.find(entry=>entry.opcode===3);
  assert.ok(command,`${variant}: script ${expected.script} sprite selection`);
  assert.equal(Buffer.from(command.raw,'hex').readInt32LE(0),expected.sprite,
    `${variant}: script ${expected.script} sprite`);
}

for(const variant of ['ja','zh']){
  test(`menu-ui ${variant}: published original-record catalog`,()=>{
    const file=path.join(variantRoot(variant),'animations/menu-ui.json');
    assert.ok(fs.existsSync(file),'menu-ui.json is published');
    const menu=JSON.parse(fs.readFileSync(file,'utf8'));
    const manifest=JSON.parse(fs.readFileSync(path.join(variantRoot(variant),'manifest.json'),'utf8'));
    const title=metadataFor(variant);
    assert.equal(menu.schema,'th128-original-menu-ui/1');
    assert.equal(menu.variant,variant);
    assert.equal(menu.sourceTitleSha256,manifest.files.find(entry=>entry.name==='title.anm').sha256);
    assert.equal(menu.sourceMetadataHash,sha256(title.bytes));
    assert.equal(menu.titleVersionSha256,manifest.files.find(entry=>entry.name==='title_v.anm').sha256);
    assert.equal(menu.titleVersionMetadataHash,
      sha256(fs.readFileSync(path.join(variantRoot(variant),'animations/title_v.json'))));
    assert.equal(title.metadata.entries.length,26);
    assert.equal(title.metadata.sprites.length,170);
    assert.equal(title.metadata.scripts.length,214);

    const cache=new Map();
    assert.equal(menu.main.length,7);
    assert.deepEqual([...menu.main].map(entry=>entry.id).sort(),[...mainIds].sort());
    assert.equal(new Set(menu.main.map(entry=>entry.id)).size,7);
    for(const row of menu.main){
      assert.equal(row.key,row.id);
      assert.ok(row.label.length>0);
      assert.equal(row.y,row.order*28,`${row.id}: row pitch`);
      assert.equal(row.sprite.file,'textures/title-000.png');
      assert.equal(row.selectedSprite.file,row.sprite.file);
      assert.equal(row.sprite.width,160);
      assert.equal(row.sprite.height,24);
      assert.equal(row.sprite.textureWidth,512);
      assert.equal(row.sprite.textureHeight,512);
      assert.equal(row.selectedSprite.width,160);
      assert.equal(row.selectedSprite.height,24);
      assert.equal(row.selectedSprite.y,row.sprite.y);
      assert.equal(row.selectedSprite.x,0);
      assert.equal(row.sprite.x,160);
      checkArt(variant,title.metadata,cache,row.sprite,
        {entry:0,name:'title/title01.png',sprite:row.source.sprite,script:row.source.script});
      checkArt(variant,title.metadata,cache,row.selectedSprite,
        {entry:0,name:'title/title01.png',sprite:row.source.selectedSprite,script:row.source.selectedScript});
      const labels=title.metadata.scripts[row.source.selectedScript].commands
        .filter(entry=>entry.opcode===64&&entry.raw.length>=8)
        .map(entry=>Buffer.from(entry.raw,'hex').readInt32LE(0));
      assert.ok(labels.includes(row.source.selectedInterrupt),`${row.id}: selected interrupt`);
      assert.equal(row.source.selectedInterrupt,17+row.order);
    }
    assert.deepEqual(menu.main.map(entry=>entry.label),
      ['START','REPLAY','RESULT','MUSIC','OPTION','MANUAL','QUIT']);

    assert.equal(menu.difficulty.length,4);
    assert.deepEqual(menu.difficulty.map(entry=>entry.id),[0,1,2,3]);
    assert.deepEqual(menu.difficulty.map(entry=>entry.label),difficultyLabels);
    for(const row of menu.difficulty){
      assert.equal(row.art.width,254);
      assert.equal(row.art.height,254);
      assert.equal(row.art.x,1);
      assert.equal(row.art.y,1);
      assert.equal(row.art.textureWidth,256);
      assert.equal(row.art.textureHeight,256);
      assert.equal(row.dimmedArt.width,254);
      assert.equal(row.dimmedArt.textureWidth,256);
      assert.notEqual(row.art.sha256,row.dimmedArt.sha256,`difficulty ${row.id}: distinct states`);
      checkArt(variant,title.metadata,cache,row.art,
        {entry:row.source.entry,name:row.source.name,sprite:row.source.sprite,script:row.source.script});
      checkArt(variant,title.metadata,cache,row.dimmedArt,
        {entry:row.source.dimmedEntry,name:row.source.dimmedName,sprite:row.source.dimmedSprite,
          script:row.source.dimmedScript});
    }

    assert.equal(menu.routes.length,3);
    assert.deepEqual(menu.routes.map(entry=>entry.stageId),[1,6,11]);
    assert.deepEqual(menu.routes.map(entry=>entry.id),['A','B','C']);
    assert.deepEqual(menu.routes.map(entry=>entry.label),routeLabels[variant]);
    for(const route of menu.routes){
      assert.equal(routeIds[route.stageId],route.id);
      const stage=stageTable.stages.find(entry=>entry.id===route.stageId);
      assert.equal(stage.background,route.source.background);
      assert.equal(stage.ecl,route.source.ecl);
      assert.equal(route.art.width,254);
      assert.equal(route.art.height,254);
      assert.equal(route.art.textureWidth,256);
      assert.equal(route.art.sha256,title.metadata.entries[route.source.entry].texture.sha256);
      checkArt(variant,title.metadata,cache,route.art,
        {entry:route.source.entry,name:route.source.name,sprite:route.source.sprite,script:route.source.script});
      assert.ok(!['textures/title-019.png','textures/title-020.png'].includes(route.art.file),
        'extra stage art is not a direct route choice');
      assert.ok(![91,92].includes(route.source.sprite),'extra stage sprites are not routes');
    }
    assert.equal(cache.size,12,`${variant}: referenced texture count`);
  });
}

test('menu-ui variants use their own localized difficulty and route art',()=>{
  const ja=JSON.parse(fs.readFileSync(path.join(variantRoot('ja'),'animations/menu-ui.json'),'utf8'));
  const zh=JSON.parse(fs.readFileSync(path.join(variantRoot('zh'),'animations/menu-ui.json'),'utf8'));
  assert.equal(ja.main[0].sprite.sha256,zh.main[0].sprite.sha256);
  for(const index of [0,1,2,3])assert.notEqual(ja.difficulty[index].art.sha256,zh.difficulty[index].art.sha256);
  for(const index of [0,1,2])assert.notEqual(ja.routes[index].art.sha256,zh.routes[index].art.sha256);
  assert.notEqual(ja.difficulty[0].art.sha256,ja.difficulty[1].art.sha256);
  assert.notEqual(ja.routes[0].art.sha256,ja.routes[1].art.sha256);
});
