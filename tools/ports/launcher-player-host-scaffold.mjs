import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdir,mkdtemp,readFile,rm,writeFile} from 'node:fs/promises';
import {dirname,resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {resolvePublicSource} from '../../launcher/lib/public-source.mjs';
import {staticContentType} from '../../launcher/server/static-content-policy.mjs';

const launcherRoot=resolve(import.meta.dirname,'../../launcher');

const rendererSource=`
import React from 'react';
import {renderToStaticMarkup} from 'react-dom/server';
import {PlayerSurface} from './app/components/player/PlayerSurface.tsx';
import {TouchControlPreview} from './app/components/settings/TouchControlPreview.tsx';
import {LocaleProvider} from './app/i18n.tsx';
import {DEFAULT_GAME_OPTIONS} from './src/launcher/game-preferences.mts';

export function renderPlayer(game){
  const options={...DEFAULT_GAME_OPTIONS,touchEnabled:true,touchMovementMode:'touch',
    touchFocusMode:'hold-button',thpracTouchControlsEnabled:true,restartButtonEnabled:true};
  const settings={gameId:game,options};
  const controls=React.createElement(React.Fragment,null,
    React.createElement('div',{className:'touch-layout-safe-zone',id:'touchLayoutSafeZone','aria-hidden':'true'}),
    React.createElement('div',{className:'touch-direct-surface',id:'touchDirectSurface','aria-hidden':'true',hidden:true}),
    React.createElement(TouchControlPreview,{settings}),
    React.createElement('nav',{className:'touch-utility-row'},
      React.createElement('button',{className:'game-zoom-toggle',id:'gameZoomToggle',type:'button',hidden:true,
        'aria-label':'Restore viewport',title:'Restore viewport'},
        React.createElement('strong',null,'Reset'),React.createElement('small',{id:'gameZoomScale'},'100%'))));
  const player=React.createElement(PlayerSurface,{open:true,editing:false,onElement:()=>{},onFrame:()=>{}},controls);
  return renderToStaticMarkup(React.createElement(LocaleProvider,{locale:'en'},player));
}
`;

/** Build a one-use renderer from the real Launcher surface and touch controls. */
export async function buildProductionPlayerHostRenderer({bundlePath,builder}){
  assert.equal(typeof bundlePath,'string','A report-owned renderer bundle path is required');
  assert.equal(typeof builder,'function','The shared esbuild instance is required');
  const built=await builder({stdin:{contents:rendererSource,resolveDir:launcherRoot,
    sourcefile:'desktop-player-host-scaffold.tsx',loader:'tsx'},bundle:true,write:false,
    format:'esm',platform:'node',target:'node22',packages:'external',jsx:'automatic',loader:{'.css':'empty'}});
  assert.equal(built.outputFiles?.length,1,'Production Player renderer must produce one deterministic bundle');
  const bytes=Buffer.from(built.outputFiles[0].contents);
  await mkdir(dirname(bundlePath),{recursive:true});
  await writeFile(bundlePath,bytes);
  const cacheRoot=resolve(launcherRoot,'.cache');await mkdir(cacheRoot,{recursive:true});
  const temporaryDirectory=await mkdtemp(resolve(cacheRoot,'desktop-player-host-'));
  const temporaryBundle=resolve(temporaryDirectory,'renderer.mjs');
  await writeFile(temporaryBundle,bytes);
  try{
    const module=await import(`${pathToFileURL(temporaryBundle).href}?sha256=${digest(bytes)}`);
    assert.equal(typeof module.renderPlayer,'function','Production Player renderer export is missing');
    return{render(game){return module.renderPlayer(game);},bytes,bundlePath};
  }finally{
    assert.equal(dirname(resolve(temporaryDirectory)),cacheRoot,'Renderer cleanup must stay in its Launcher cache directory');
    await rm(temporaryDirectory,{recursive:true,force:true});
  }
}

/** The same public stylesheet and workspace rules used by the real Launcher. */
export function productionPlayerStylesheetLinks(){
  return '<link rel="stylesheet" href="/launcher-public/styles.css">'+
    '<link rel="stylesheet" href="/launcher-app/workspace-viewport.css">';
}

/** Bound the fixture's root surface to its declared CSS-pixel viewport.
 * PlayerSurface and its iframe retain their production layout rules inside it.
 */
export function installProductionPlayerHostViewport({document:hostDocument=document,window:hostWindow=window}={}){
  const player=hostDocument.getElementById('player');
  if(!player)throw Error('Production Player viewport root is missing');
  const fit=()=>{
    const {innerWidth:width,innerHeight:height}=hostWindow;
    if(!Number.isFinite(width)||!Number.isFinite(height)||width<=0||height<=0)
      throw Error('Fixture viewport dimensions must be positive CSS pixels');
    Object.assign(player.style,{left:'0px',top:'0px',right:'auto',bottom:'auto',width:`${width}px`,height:`${height}px`});
  };
  fit();hostWindow.addEventListener('resize',fit);
  return()=>hostWindow.removeEventListener('resize',fit);
}

export function productionPlayerHostViewportScript(){
  return `(${installProductionPlayerHostViewport.toString()})();`;
}

/** Resolve referenced Launcher public files through the production path policy. */
export async function readLauncherPublicAsset(pathname){
  const prefix='/launcher-public/';
  if(typeof pathname!=='string'||!pathname.startsWith(prefix))return null;
  let publicPath;
  try{publicPath=decodeURIComponent(pathname.slice(prefix.length));}catch{return null;}
  let file;
  try{file=resolvePublicSource(publicPath);}catch{return null;}
  try{
    const bytes=await readFile(file);
    return{file,bytes,contentType:staticContentType(file)};
  }catch(error){
    if(error?.code==='ENOENT'||error?.code==='ENOTDIR')return null;
    throw error;
  }
}

function digest(bytes){
  return createHash('sha256').update(bytes).digest('hex');
}
