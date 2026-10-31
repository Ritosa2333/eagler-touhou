import {useLayoutEffect, useRef, useState} from 'react';
import {useLocale} from '../../i18n';
import {bindLibraryRail} from './library-rail';

export const CLASSICS_ENTRIES = [
  {id:'th01',number:'01',title:'東方靈異伝',subtitle:'Highly Responsive to Prayers'},
  {id:'th02',number:'02',title:'東方封魔録',subtitle:'The Story of Eastern Wonderland'},
  {id:'th03',number:'03',title:'東方夢時空',subtitle:'Phantasmagoria of Dim.Dream'},
  {id:'th04',number:'04',title:'東方幻想郷',subtitle:'Lotus Land Story'},
  {id:'th05',number:'05',title:'東方怪綺談',subtitle:'Mystic Square'},
] as const;

/** These existing standalone pages own their game state outside the managed Launcher. */
export function ClassicsShelf({assetUrl}: {assetUrl:(path:string)=>string}) {
  const {locale,t}=useLocale(),shelf=useRef<HTMLElement>(null),[preview,setPreview]=useState('th01');
  useLayoutEffect(()=>{
    if(!shelf.current)return;
    const rail=bindLibraryRail(shelf.current,{initialProduct:'th01',openOnFirstClick:()=>true,onSelectionChange:setPreview});
    return ()=>rail.dispose();
  },[]);
  return <section ref={shelf} className="game-shelf classics-shelf" data-shelf="classics" aria-labelledby="classicsHeading">
    <header className="shelf-heading"><h2 id="classicsHeading">{locale==='en'?'PC-98 Classics':'旧作'}</h2><span className="shelf-caption">TH01–TH05 · PC-98</span></header>
    <div className="game-rail" id="classicsRail" role="group" aria-labelledby="classicsHeading">
      {CLASSICS_ENTRIES.map(game=><a key={game.id} className={`game game-${game.id} card-art-missing${preview===game.id?' nav-preview':''}`} data-game={game.id} href={assetUrl(game.id)} draggable={false}>
        <div className="no"><span className="no-label">{game.number}</span></div>
        <div className="game-copy"><h2><span className="game-title">{game.title}</span></h2><small>{game.subtitle}</small></div>
      </a>)}
    </div>
    <div className="shelf-minimap"><div className="minimap-dock">
      {CLASSICS_ENTRIES.map(game=><button key={game.id} type="button" className={`minimap-toggle${preview===game.id?' is-current':''}`} data-minimap-preview={game.id} aria-label={game.title} aria-current={preview===game.id} aria-controls="classicsRail" aria-describedby="classicsMinimapLabel"><span className="minimap-index" aria-hidden="true">{game.number}</span></button>)}
    </div><span className="sr-only" id="classicsMinimapLabel">{t('library.holdNav')}</span></div>
  </section>;
}
