import {copyFile,mkdir,readFile,writeFile} from 'node:fs/promises';
import {PRODUCT_GAMES,PRODUCT_IDS,gameIdForProduct,isMultiplayerProductId,multiplayerInputTimingPolicy,productEnabledForBuild} from '../../common/contracts/product-catalog.mjs';
import {bookGameShortTitle} from '../../launcher/app/components/launcher/products.ts';
import {createKeyboardPreferences} from '../../launcher/src/launcher/keyboard-preferences.mts';
import {defaultGamepadProfile} from '../../launcher/src/launcher/gamepad-mapping.mts';
import {localKeyboardPreset} from '../../launcher/src/launcher/th09-local-input.mts';
import {touchLayoutControlMeta,touchLayoutScaleMin,touchLayoutScaleMax} from '../../launcher/src/launcher/touch-layout-model.mts';
import {functionKeyGames} from '../../launcher/src/launcher/touch-function-key.mts';
import {replayStorageLocation} from '../../launcher/src/launcher/replay-storage-policy.mts';

const [source] = process.argv.slice(2);
if (!source) throw new Error('Usage: bun native/tools/generate-catalog.mjs <site-manifest.json>');
const manifest=JSON.parse(await readFile(source,'utf8'));
const products=PRODUCT_IDS.filter(id=>productEnabledForBuild(id,manifest.shared.testBuild===true));
const available=products.filter(id=>{
  const game=manifest.games[gameIdForProduct(id)];
  return game&&(!isMultiplayerProductId(id)||typeof game.multiplayerRuntime==='string'&&game.multiplayerRuntime);
});
const games=available.filter(id=>!isMultiplayerProductId(id)).map(id=>{
  const game=PRODUCT_GAMES[id];
  const number=Number(game.number);
  const published=manifest.games[id];
  const languages=(published.languageOptions??[]).map(({id,title,pack})=>({id,title,bytes:pack?.bytes??0}));
  const features=Object.keys(game.features).filter(feature=>published.features?.[feature]??game.features[feature]);
  const music=[];
  const {path,bytes,sha256}=published.gameData;
  const data={name:path.split('/').at(-1),bytes,sha256};
  if (published.music?.ogg?.files?.length) {
    music.push('ogg-stream','ogg-full');
    if (game.musicRuntime?.sources?.includes('akyu')) music.push('ogg-akyu');
  }
  if (published.music?.midi?.supported || published.music?.midi?.files?.length) music.push('midi');
  const category=id==='hsob'||/^th[a-z]/i.test(id)?'doujin':
    Number.isInteger(number)&&number<=5?'pc98':
    game.inputProfile==='fighting'?'fighting':game.multiplayer?.gameplay==='versus'?'versus':
    Number.isInteger(number)?'windows':'spin-off';
  const filePolicy=product=>{
    const policy=replayStorageLocation(product);
    return {namespace:policy.database.slice(1),prefix:policy.prefix,
      sharedExtensionSlot:policy.sharedExtensionSlot,
      slotPrefix:policy.prefix===game.replay.prefix?game.replay.slotPrefix:null,
      slots:game.replay.slots??0,
      filename:game.replay.naming==='filename',
      extensions:game.replay.extensions??(product==='th12mp'?['rpy']:['rpy','rpyx']),
      legacyPrefix:product==='th15mp'?game.replay.prefix:null};
  };
  const mp=available.find(product=>isMultiplayerProductId(product)&&gameIdForProduct(product)===id);
  const {rollback, measuredStartup, manualDelayLimit}=multiplayerInputTimingPolicy(game.multiplayer);
  const timing=game.multiplayer?.inputTiming
    ? {rollback, measuredStartup, runtimeTiming:game.multiplayer.inputTiming.runtimeTiming===true, manualDelayLimit}
    : null;
  const files={score:game.storage.scoreFile,hints:game.storage.hintFiles??[],
    saveArchive:id==='th15',languageScoped:id==='th10'||id==='th20',
    normal:filePolicy(id),multiplayer:mp?filePolicy(mp):null};
  return {id,number:game.number,title:game.title,subtitle:game.subtitle,category,artwork:id+'.webp',
    multiplayer:!!mp,shortTitle:bookGameShortTitle(id),
    ...(mp && { multiplayerRules: { product:mp, playerCounts:game.multiplayer.playerCounts,
      difficulties:game.multiplayer.difficulties, selectInGame:game.multiplayer.inGameSelection===true,
      challengeMode:game.multiplayer.gameplay==='cooperative'&&game.multiplayer.challengeMode!==false,
      ...(timing && {timing}) } }),
    data,
    ...(game.package.maxImportBytes && {packageMaxBytes:game.package.maxImportBytes}),
    files,settings:{highRefresh:game.support.highRefreshRate===true,features,languages,
      renderScales:game.renderScales??[],music:[...music,'none']}};
});
const nameOrder=new Map([...games].sort((a,b)=>a.title.localeCompare(b.title,'zh-CN')).map((game,index)=>[game.id,index]));
for(const game of games) game.nameOrder=nameOrder.get(game.id);
const directory=new URL('../assets/',import.meta.url);
const covers=new URL('../assets/book-covers/',import.meta.url);
const webCovers=new URL('../../launcher/app/components/launcher/book-covers/',import.meta.url);
await mkdir(covers,{recursive:true});
await Promise.all(games.map(game=>copyFile(new URL(game.artwork,webCovers),new URL(game.artwork,covers))));
await writeFile(new URL('game_catalog.json',directory),JSON.stringify(games,null,2)+'\n');
const keyboardLayouts=Object.fromEntries([undefined,'th075','th19'].map(scope=>{
  const model=createKeyboardPreferences(null,scope);
  model.selectLayout('wasd');
  return [scope??'general',{actions:model.actionCodes,wasd:model.getSnapshot()}];
}));
await writeFile(new URL('keyboard_layouts.json',directory),JSON.stringify(keyboardLayouts,null,2)+'\n');
await writeFile(new URL('gamepad_defaults.json',directory),JSON.stringify(defaultGamepadProfile(true),null,2)+'\n');
const localInputs=Object.fromEntries(['original','wasd','arrows'].map(preset=>[
  preset,[0,1].map(seat=>localKeyboardPreset(seat,preset)),
]));
await writeFile(new URL('th09_local_inputs.json',directory),JSON.stringify(localInputs,null,2)+'\n');
const touchProfiles=Object.fromEntries(await Promise.all([
  ['shared','touch-layout'],['th075','touch-layout-fighting'],
].map(async ([scope,screen])=>{
  const profiles=await Promise.all([
    ['portrait','mobile'],['landscape','desktop'],
  ].map(async ([orientation,device])=>{
    const evidence = new URL('../docs/evidence/web/'+device+'-'+screen+'.json',import.meta.url);
    const {touchGeometry}=JSON.parse(await readFile(evidence,'utf8'));
    const controls=Object.fromEntries(Object.entries(touchGeometry.controls).map(([name,rect])=>{
      const scale=name==='bomb'&&scope==='shared'?1.5:1;
      return [name,{
        ...rect,width:rect.width/scale,height:rect.height/scale,scale,
        priority:touchLayoutControlMeta[name].priority,
      }];
    }));
    return [orientation,{...touchGeometry,controls}];
  }));
  return [scope,Object.fromEntries(profiles)];
})));
await writeFile(new URL('touch_layout_catalog.json',directory),JSON.stringify({
  profiles:touchProfiles,
  functionGames:[...functionKeyGames],
  minScale:touchLayoutScaleMin,maxScale:touchLayoutScaleMax,
},null,2)+'\n');
console.log(JSON.stringify({single:games.length,multiplayer:games.filter(game=>game.multiplayer).length}));
