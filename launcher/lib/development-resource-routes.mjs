import {createHash} from 'node:crypto';
import {basename,resolve,relative} from 'node:path';

/** Publish explicitly declared local inputs through a closed same-origin map. */
export function developmentResourceRoutes(input,{sourceRoot}) {
  const manifest=structuredClone(input),files=new Map();
  const encodePath=path=>path.split('/').map(encodeURIComponent).join('/');
  const prefix=path=>'/__development-content/'+createHash('sha256').update(path).digest('hex')+'/';
  const file=path=>{
    if(!path)return path;
    const absolute=resolve(sourceRoot,path),route=prefix(absolute)+basename(absolute);
    files.set(route,absolute);return encodePath(route);
  };
  for(const key of ['vanillaFont','unicodeFont'])manifest.shared[key]=file(manifest.shared[key]);
  for(const game of Object.values(manifest.games)){
    game.gameData.source=file(game.gameData.source);
    for(const music of Object.values(game.music??{})){
      if(!music.base||!Array.isArray(music.files))continue;
      const directory=resolve(sourceRoot,music.base),route=prefix(directory);
      for(const name of music.files){
        const absolute=resolve(directory,name);
        if(relative(directory,absolute).startsWith('..'))throw Error('Music file outside declared directory');
        files.set(route+name,absolute);
      }
      music.base=encodePath(route);
    }
  }
  return{manifest,files};
}
