import {useEffect, useLayoutEffect, useMemo, useRef, useState, type CSSProperties} from 'react';
import {AnimatePresence, motion, useReducedMotion} from 'motion/react';
import {ArrowLeft, CloudUpload} from 'lucide-react';
import {Link} from 'react-router';
import {PRODUCT_GAMES, multiplayerConfigForProduct, multiplayerProductIdForGame, type GameId} from '../../../src/contracts/product-catalog.mts';
import {Button} from '../../ui/button';
import {useLocale, type Translate, type UiMessageKey} from '../../i18n';
import {formatPlaytime, type ProfileStatistics, type SaveProgress} from '../../services/cloud-saves';
import {bookGameArtwork} from './book-artwork';
import {bookGameShortTitle} from './products';
import './profile-game-data.css';

const difficulties=['overdrive','phantasm','extra','lunatic','hard','normal','easy'] as const;
const difficultyName={easy:'Easy',normal:'Normal',hard:'Hard',lunatic:'Lunatic',extra:'Extra',phantasm:'Phantasm',overdrive:'Overdrive'};
function stateOf(items:SaveProgress[]) {return items.some(item=>item.oneCreditClear)?'one-credit':items.some(item=>item.cleared)?'cleared':items.length?items.some(item=>item.cleared!==null)?'uncleared':'unavailable':'unread';}
function loadoutName(game:GameId,character:number,shot:number|null,t:Translate) {
  const product=multiplayerProductIdForGame(game),config=product?multiplayerConfigForProduct(product):null;
  const entry=config?.loadouts.find(item=>item.character===character&&item.shot===(shot??0));
  if(entry) {
    if(game==='th08'&&character<4)return t((['profile.team.barrier','profile.team.magic','profile.team.scarlet','profile.team.ghost'] as const)[character]!);
    return t(entry.labelKey as UiMessageKey);
  }
  return t('profile.loadoutNumber',{number:character+1})+(shot===null?'':' '+String.fromCharCode(65+shot));
}
function WorkArtwork({game}:{game:GameId}) {
  const source=bookGameArtwork(game);
  return source?<img src={source} alt="" loading="lazy" decoding="async"/>:<span className="profile-work-art-fallback" aria-hidden="true">{bookGameShortTitle(game)}</span>;
}
export default function ProfileGameData({data,order,owner,settingsHref}: {data:ProfileStatistics;order:string[];owner:boolean;settingsHref:string}) {
  const {locale,t}=useLocale();
  return <>{order.map(key=>key==='career'&&data.career&&data.games.some(game=>game.statistics.progress.length)?<section key={key} id="profile-career" className="profile-panel profile-public-section profile-game-data">
    <div className="profile-section-title"><h2>{t('profile.tab.career')}</h2>{owner&&<Button variant="ghost" size="sm" asChild><Link to={settingsHref}><CloudUpload/>{t('cloudSave.title')}</Link></Button>}</div>
    <ClearMatrix data={data}/>
  </section>:key==='records'&&data.records&&data.games.some(game=>game.statistics.playtimeSeconds!==null||game.statistics.playCount!==null)?<section key={key} id="profile-records" className="profile-panel profile-public-section profile-game-data">
    <div className="profile-section-title"><h2>{t('profile.tab.records')}</h2>{data.playtimeSeconds!==null&&<span className="profile-flight-total">{t('profile.flightTime')} <strong>{formatPlaytime(data.playtimeSeconds,locale)}</strong></span>}</div>
    <div className="profile-playtime-list">{data.games.filter(game=>game.statistics.playtimeSeconds!==null||game.statistics.playCount!==null).sort((a,b)=>(b.statistics.playtimeSeconds??-1)-(a.statistics.playtimeSeconds??-1)).map(game=><div key={game.statistics.game} className="profile-playtime-game">
      <WorkArtwork game={game.statistics.game}/><strong>{PRODUCT_GAMES[game.statistics.game].title}</strong>
      <span>{game.statistics.playtimeSeconds!==null?formatPlaytime(game.statistics.playtimeSeconds,locale):t('profile.playCount',{count:game.statistics.playCount??0})}</span>
    </div>)}</div>
  </section>:null)}</>;
}
function ClearMatrix({data}: {data:ProfileStatistics}) {
  const {t}=useLocale(),systemReduced=useReducedMotion();
  const [lessMotion,setLessMotion]=useState(()=>typeof document!=='undefined'&&document.body.classList.contains('less-motion'));
  useEffect(()=>{const observer=new MutationObserver(()=>setLessMotion(document.body.classList.contains('less-motion')));observer.observe(document.body,{attributes:true,attributeFilter:['class']});return()=>observer.disconnect();},[]);
  const [selected,setSelected]=useState<GameId|null>(null),[mode,setMode]=useState<'pointdevice'|'legacy'>('legacy');
  const plot=useRef<HTMLDivElement>(null),[tilted,setTilted]=useState(false);
  const games=useMemo(()=>Object.entries(PRODUCT_GAMES).filter(([game,definition])=>!('testOnly'in definition&&definition.testOnly)||data.games.some(item=>item.statistics.game===game)).map(([game])=>game as GameId),[data.games]);
  const statistics=data.games.find(item=>item.statistics.game===selected)?.statistics;
  const active=useMemo(()=>statistics?.progress.filter(item=>!item.mode||item.mode===mode)??[],[statistics,mode]);
  const rows=difficulties.filter(difficulty=>selected?active.some(item=>item.difficulty===difficulty):data.games.some(game=>game.statistics.progress.some(item=>item.difficulty===difficulty)));
  const transition={duration:systemReduced||lessMotion?0:0.18,ease:[0.2,0,0,1] as const};
  const statusLabel=(state:string)=>t(state==='one-credit'?'profile.clear.oneCredit':state==='cleared'?'profile.clear.cleared':state==='uncleared'?'profile.clear.uncleared':state==='unavailable'?'profile.clear.unavailable':'profile.clear.unread');
  const cells=(items:SaveProgress[],game:GameId)=>rows.map(difficulty=>{
    if(difficulty==='phantasm'&&game!=='th07'||difficulty==='extra'&&game==='th075'||difficulty==='overdrive'&&!items.some(item=>item.difficulty===difficulty))return <span key={difficulty} className="profile-clear-cell" data-status="not-applicable" aria-hidden="true"/>;
    const state=stateOf(items.filter(item=>item.difficulty===difficulty));return <span key={difficulty} className="profile-clear-cell" data-status={state} role="img" aria-label={PRODUCT_GAMES[game].title+' | '+difficultyName[difficulty]+' | '+statusLabel(state)} title={difficultyName[difficulty]+' | '+statusLabel(state)}/>;
  });
  const column=(game:GameId)=>{
    const stats=data.games.find(item=>item.statistics.game===game)?.statistics;
    return <motion.div key={game} layout="position" transition={transition} initial={false} exit={{opacity:0,x:game.localeCompare(selected??'')>0?16:-16}} className="profile-clear-game" data-unread={!stats}>
      <Button className="profile-clear-game-button" variant="ghost" aria-label={t('profile.clear.openGame',{game:PRODUCT_GAMES[game].title})} aria-expanded={selected===game}
        onClick={()=>{if(stats){setSelected(selected===game?null:game);setMode('legacy');}}} disabled={!stats}>
        <span className="profile-clear-cells">{cells(stats?.progress??[],game)}</span>
        <span className="profile-clear-cover"><WorkArtwork game={game}/><small>{bookGameShortTitle(game)}</small></span>
      </Button>
    </motion.div>;
  };
  const groups=useMemo(()=>{
    const map=new Map<number,Map<number|null,SaveProgress[]>>();
    for(const item of active){if(!map.has(item.character))map.set(item.character,new Map());const variants=map.get(item.character)!;if(!variants.has(item.shot))variants.set(item.shot,[]);variants.get(item.shot)!.push(item);}
    return [...map].sort(([a],[b])=>a-b).map(([character,variants])=>({character,variants:[...variants].sort(([a],[b])=>(a??0)-(b??0))}));
  },[active]);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Loadout and language changes can alter label overflow at the same plot width.
  useLayoutEffect(()=>{
    const node=plot.current;
    if(!node||!selected){setTilted(false);return;}
    let current=true;
    const measure=()=>{if(current)setTilted([...node.querySelectorAll<HTMLElement>('.profile-clear-character')].some(group=>{
      const label=group.querySelector<HTMLElement>('.profile-clear-character-name'),variants=group.querySelector<HTMLElement>('.profile-clear-variants');
      return !!label&&!!variants&&label.scrollWidth>variants.getBoundingClientRect().width+2;
    }));};
    measure();const observer=typeof ResizeObserver==='undefined'?null:new ResizeObserver(measure);observer?.observe(node);
    window.addEventListener('resize',measure);void document.fonts?.ready.then(measure);
    return()=>{current=false;observer?.disconnect();window.removeEventListener('resize',measure);};
  },[selected,groups,t]);
  return <div className="profile-clear-chart">
    <div className="profile-clear-toolbar"><div className="profile-clear-legend">{(['one-credit','cleared','uncleared'] as const).map(state=><span key={state}><i className="profile-clear-cell" data-status={state}/>{statusLabel(state)}</span>)}</div>
      {selected&&<Button variant="ghost" size="sm" onClick={()=>setSelected(null)}><ArrowLeft/>{t('profile.clear.allGames')}</Button>}
      {selected==='th15'&&<div className="profile-clear-modes">{(['legacy','pointdevice'] as const).map(value=><Button key={value} variant={mode===value?'secondary':'ghost'} size="sm" aria-pressed={mode===value} onClick={()=>setMode(value)}>{t(value==='legacy'?'profile.mode.legacy':'profile.mode.pointdevice')}</Button>)}</div>}
    </div>
    <div className="profile-clear-scroll"><div ref={plot} className="profile-clear-plot" data-detail={!!selected} data-tilted={tilted}>
      <div className="profile-clear-axis">{rows.map(difficulty=><span key={difficulty}>{difficultyName[difficulty]}</span>)}</div>
      <AnimatePresence initial={false} mode="popLayout">{(selected?[selected]:games).map(column)}
        {selected&&groups.map(({character,variants})=>{
          const grouped=variants.length>1&&['th06','th07','th10','th11','th12'].includes(selected);
          const name=loadoutName(selected,character,variants[0]?.[0]??0,t).replace(grouped?/\s*[ABC]$/u:/$^/u,'');
          return <motion.div key={'character-'+character} layout="position" initial={{opacity:0,x:12}} animate={{opacity:1,x:0}} exit={{opacity:0,x:12}} transition={transition} className="profile-clear-character" style={{'--clear-group-columns':variants.length} as CSSProperties}>
            <div className="profile-clear-variants">{variants.map(([shot,items])=><div key={shot??0} className="profile-clear-shot"><div className="profile-clear-cells">{cells(items,selected)}</div><span className="profile-clear-shot-name" title={loadoutName(selected,character,shot,t)}>{grouped?String.fromCharCode(65+(shot??0)):variants.length>1?loadoutName(selected,character,shot,t):''}</span></div>)}</div>
            {grouped&&<span className="profile-clear-bracket" aria-hidden="true"/>}<span className="profile-clear-character-name">{name}</span>
          </motion.div>;
        })}
      </AnimatePresence>
    </div></div>
  </div>;
}
