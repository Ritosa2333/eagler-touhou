import {useLayoutEffect, useMemo, useRef, useState, type CSSProperties} from 'react';
import {AnimatePresence, motion} from 'motion/react';
import {ArrowLeft} from 'lucide-react';
import {PRODUCT_GAMES, multiplayerConfigForProduct, multiplayerProductIdForGame, type GameId} from '../../../src/contracts/product-catalog.mts';
import {Button} from '../../ui/button';
import {useSiteReducedMotion} from '../../ui/use-site-reduced-motion';
import {useLocale, type Translate, type UiMessageKey} from '../../i18n';
import {formatPlaytime, type ProfileStatistics, type SaveProgress, type SaveStatistics} from '../../services/cloud-saves';
import {bookGameArtwork} from './book-artwork';
import {bookGameShortTitle} from './products';
import './profile-game-data.css';

const difficulties=['overdrive','phantasm','extra','lunatic','hard','normal','easy'] as const;
const difficultyName={easy:'Easy',normal:'Normal',hard:'Hard',lunatic:'Lunatic',extra:'Extra',phantasm:'Phantasm',overdrive:'Overdrive'};
const withoutExtra: ReadonlySet<GameId> = new Set(['th01', 'th03', 'th075', 'th19']);
function hasDifficulty(game: GameId, difficulty: SaveProgress['difficulty']) {
  if (difficulty === 'extra') return !withoutExtra.has(game);
  if (difficulty === 'phantasm') return game === 'th07';
  if (difficulty === 'overdrive') return game === 'th20';
  return true;
}
function getLoadouts(game: GameId) {
  const product = multiplayerProductIdForGame(game);
  return (product && multiplayerConfigForProduct(product)?.loadouts) || [];
}
const hasLetterShots: ReadonlySet<GameId> = new Set(['th04', 'th06', 'th07', 'th10', 'th11', 'th12', 'th14']);
function stateOf(items:SaveProgress[]) {return items.some(item=>item.oneCreditClear)?'one-credit':items.some(item=>item.cleared)?'cleared':items.length?items.some(item=>item.cleared!==null)?'uncleared':'unavailable':'unread';}
function loadoutName(game:GameId,character:number,shot:number|null,t:Translate,fixed=false) {
  const entry=getLoadouts(game).find(item=>item.character===character&&item.shot===(shot??0));
  if(entry) {
    if(fixed&&shot===null&&(game==='th075'||game==='th17')&&entry.selection?.[0])return t(entry.selection[0].labelKey as UiMessageKey);
    const team = (['profile.team.barrier','profile.team.magic','profile.team.scarlet','profile.team.ghost'] as const)[character];
    if(game==='th08'&&team)return t(team);
    return t(entry.labelKey as UiMessageKey);
  }
  const characterName = t('profile.loadoutNumber',{number:character+1});
  return shot===null ? characterName : `${characterName} ${String.fromCharCode(65+shot)}`;
}
function WorkArtwork({game}:{game:GameId}) {
  const source=bookGameArtwork(game);
  return source?<img src={source} alt="" loading="lazy" decoding="async"/>:<span className="profile-work-art-fallback" aria-hidden="true">{bookGameShortTitle(game)}</span>;
}
export function ProfileAllGames({data, showCareer, showRecords}: {
  data: ProfileStatistics; showCareer: boolean; showRecords: boolean;
}) {
  const {locale,t}=useLocale();
  const games = showRecords ? data.games.filter(game => game.statistics.playtimeSeconds !== null || game.statistics.playCount !== null)
    .sort((a,b) => (b.statistics.playtimeSeconds ?? -1) - (a.statistics.playtimeSeconds ?? -1)) : [];
  const total = showRecords ? data.playtimeSeconds : null;
  return <>
    {showCareer && <ClearMatrix data={data}/>}
    {(total !== null || games.length > 0) && <div className="profile-game-data-flight">
      {total !== null && <div className="profile-flight-total">{t('profile.flightTime')} <strong>{formatPlaytime(total,locale)}</strong></div>}
      <div className="profile-playtime-list">{games.map(({statistics}) => <div key={statistics.game} className="profile-playtime-game">
        <WorkArtwork game={statistics.game}/><strong>{PRODUCT_GAMES[statistics.game].title}</strong>
        <span>{statistics.playtimeSeconds !== null ? formatPlaytime(statistics.playtimeSeconds,locale) : t('profile.playCount',{count:statistics.playCount ?? 0})}</span>
      </div>)}</div>
    </div>}
    {!showCareer && total === null && games.length === 0 && <p className="profile-muted">{t('profile.gameDataEmpty')}</p>}
  </>;
}
type ClearMatrixProps = {data: ProfileStatistics; game?: never; statistics?: never} |
  {game: GameId; statistics?: SaveStatistics; data?: never};

export function ClearMatrix({data, game: fixedGame, statistics: fixedStatistics}: ClearMatrixProps) {
  const {locale,t}=useLocale(),lessMotion=useSiteReducedMotion();
  const [selectedGame,setSelected]=useState<GameId|null>(null),[mode,setMode]=useState<'pointdevice'|'legacy'>('legacy');
  const selected = fixedGame ?? selectedGame;
  const plot=useRef<HTMLDivElement>(null),[tilted,setTilted]=useState(false);
  const chart=useRef<HTMLDivElement>(null),[chartHeight,setChartHeight]=useState<number>();
  const games=useMemo(()=>data ? Object.entries(PRODUCT_GAMES).filter(([game,definition])=>!('testOnly'in definition&&definition.testOnly)||data.games.some(item=>item.statistics.game===game)).map(([game])=>game as GameId) : [],[data]);
  const statistics=fixedStatistics ?? data?.games.find(item=>item.statistics.game===selected)?.statistics;
  const active=useMemo(()=>statistics?.progress.filter(item=>!item.mode||item.mode===mode)??[],[statistics,mode]);
  const rows=difficulties.filter(difficulty=>selected ? active.some(item=>item.difficulty===difficulty) ||
    !!fixedGame && hasDifficulty(fixedGame, difficulty) : data?.games.some(game=>game.statistics.progress.some(item=>item.difficulty===difficulty)));
  const transition={duration:lessMotion?0:0.18,ease:[0.2,0,0,1] as const};
  const statusLabel=(state:string)=>t(state==='one-credit'?'profile.clear.oneCredit':state==='cleared'?'profile.clear.cleared':state==='uncleared'?'profile.clear.uncleared':state==='unavailable'?'profile.clear.unavailable':'profile.clear.unread');
  const numberFormat = new Intl.NumberFormat(locale);
  const cells=(items:SaveProgress[],game:GameId,loadout?:string)=>rows.map(difficulty=>{
    const records = items.filter(item=>item.difficulty===difficulty);
    if(fixedGame ? !hasDifficulty(game,difficulty) && !active.some(item=>item.difficulty===difficulty) :
      difficulty==='phantasm'&&game!=='th07'||difficulty==='extra'&&game==='th075'||difficulty==='overdrive'&&!records.length)
      return <span key={difficulty} className="profile-clear-cell" data-status="not-applicable" aria-hidden="true"/>;
    const state=stateOf(records), scores=fixedGame ? records.flatMap(record=>record.bestScore===null?[]:[record.bestScore]) : [];
    const description = [loadout, difficultyName[difficulty], statusLabel(state),
      scores.length ? `${t('cloudSave.bestScore')}: ${numberFormat.format(Math.max(...scores))}` : undefined].filter(Boolean).join(' | ');
    return <span key={difficulty} className="profile-clear-cell" data-difficulty={difficulty} data-status={state} role="img"
      aria-label={`${PRODUCT_GAMES[game].title} | ${description}`} title={description}/>;
  });
  const column=(game:GameId)=>{
    const stats=data?.games.find(item=>item.statistics.game===game)?.statistics;
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
    const records = (character: number, shot: number|null) => {
      let variants = map.get(character);
      if (!variants) {
        variants = new Map();
        map.set(character, variants);
      }
      let items = variants.get(shot);
      if (!items) {
        items = [];
        variants.set(shot, items);
      }
      return items;
    };
    const collapsedRoster = new Set<number>();
    if (fixedGame) {
      // TH07.5's room entries distinguish spell decks, not story-save rows.
      const roster = fixedGame==='th03' ? [] : getLoadouts(fixedGame);
      const counts = new Map<number,number>();
      for (const {character} of roster) counts.set(character,(counts.get(character)??0)+1);
      for (const {character,shot} of roster) {
        if (fixedGame==='th075' || counts.get(character)===1 && shot===0) {
          records(character,null);
          collapsedRoster.add(character);
        } else records(character,shot);
      }
    }
    for (const item of active) records(item.character,collapsedRoster.has(item.character)&&item.shot===0?null:item.shot).push(item);
    return [...map].sort(([a],[b])=>a-b).map(([character,variants])=>({character,variants:[...variants].sort(([a],[b])=>(a??0)-(b??0))}));
  },[active,fixedGame]);
  useLayoutEffect(()=>{
    const node=chart.current;
    if(!node||typeof ResizeObserver==='undefined')return;
    const measure=()=>setChartHeight(node.getBoundingClientRect().height);
    const observer=new ResizeObserver(measure);
    measure();observer.observe(node);
    return()=>observer.disconnect();
  },[]);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Loadout and language changes can alter label overflow at the same plot width.
  useLayoutEffect(()=>{
    const node=plot.current;
    if(!node||!selected||fixedGame){setTilted(false);return;}
    let current=true;
    const measure=()=>{if(current)setTilted([...node.querySelectorAll<HTMLElement>('.profile-clear-character')].some(group=>{
      const label=group.querySelector<HTMLElement>('.profile-clear-character-name'),variants=group.querySelector<HTMLElement>('.profile-clear-variants');
      return !!label&&!!variants&&label.scrollWidth>variants.getBoundingClientRect().width+2;
    }));};
    measure();const observer=typeof ResizeObserver==='undefined'?null:new ResizeObserver(measure);observer?.observe(node);
    window.addEventListener('resize',measure);void document.fonts?.ready.then(measure);
    return()=>{current=false;observer?.disconnect();window.removeEventListener('resize',measure);};
  },[selected,groups,t,fixedGame]);
  return <div className="profile-clear-frame" style={{height:chartHeight}}>
    <div ref={chart} className="profile-clear-chart" data-fixed-game={fixedGame}>
    <div className="profile-clear-toolbar"><div className="profile-clear-legend">{(['one-credit','cleared','uncleared'] as const).map(state=><span key={state}><i className="profile-clear-cell" data-status={state}/>{statusLabel(state)}</span>)}</div>
      {selected&&!fixedGame&&<Button variant="ghost" size="sm" aria-label={t('profile.clear.allGames')} onClick={()=>setSelected(null)}>
        <ArrowLeft/><span className="profile-clear-back-label">{t('profile.clear.allGames')}</span>
      </Button>}
      {selected==='th15'&&<div className="profile-clear-modes">{(['legacy','pointdevice'] as const).map(value=><Button key={value} variant={mode===value?'secondary':'ghost'} size="sm" aria-pressed={mode===value} onClick={()=>setMode(value)}>{t(value==='legacy'?'profile.mode.legacy':'profile.mode.pointdevice')}</Button>)}</div>}
    </div>
    <section className="profile-clear-scroll" aria-label={t('profile.clearChart')}
      tabIndex={fixedGame ? 0 : undefined}><div ref={plot} className="profile-clear-plot" data-detail={!!selected} data-tilted={tilted}>
      <div className="profile-clear-axis">{rows.map(difficulty=><span key={difficulty}>{difficultyName[difficulty]}</span>)}</div>
      <AnimatePresence initial={false} mode="popLayout">{!fixedGame&&(selected?[selected]:games).map(column)}
        {fixedGame&&!groups.length&&<div key={fixedGame} className="profile-clear-cells">{cells(active,fixedGame)}</div>}
        {selected&&groups.map(({character,variants})=>{
          const grouped=variants.length>1&&(fixedGame ? hasLetterShots.has(selected) : ['th06','th07','th10','th11','th12'].includes(selected));
          const name=loadoutName(selected,character,variants[0]?.[0]??(fixedGame?null:0),t,!!fixedGame).replace(grouped?/\s*[ABC]$/u:/$^/u,'');
          return <motion.div key={`${selected}-character-${character}`} layout="position" initial={{opacity:0,x:12}} animate={{opacity:1,x:0}} exit={{opacity:0,x:12}} transition={transition} className="profile-clear-character" data-character={character} style={{'--clear-group-columns':variants.length} as CSSProperties}>
            <div className="profile-clear-variants">{variants.map(([shot,items])=>{
              const unknownShot = !!fixedGame && shot===null && variants.some(([value])=>value!==null);
              const label = unknownShot ? `${name} | ${t('cloudSave.unknownValue')}` : loadoutName(selected,character,shot,t,!!fixedGame);
              return <div key={shot??'character'} data-shot={shot??'none'} className="profile-clear-shot">
                <div className="profile-clear-cells">{cells(items,selected,label)}</div>
                <span className="profile-clear-shot-name" title={label}>{unknownShot?'—':grouped?String.fromCharCode(65+(shot??0)):variants.length>1?label:''}</span>
              </div>;
            })}</div>
            {grouped&&<span className="profile-clear-bracket" aria-hidden="true"/>}<span className="profile-clear-character-name">{name}</span>
          </motion.div>;
        })}
      </AnimatePresence>
    </div></section>
    </div>
  </div>;
}
