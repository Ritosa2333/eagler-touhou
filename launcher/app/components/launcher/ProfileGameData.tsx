import {useLayoutEffect, useRef, type CSSProperties} from 'react';
import {ArrowLeft} from 'lucide-react';
import {PRODUCT_GAMES, type GameId} from '../../../src/contracts/product-catalog.mts';
import {Button} from '../../ui/button';
import {useContentMotion} from '../../ui/use-surface-motion';
import {ClearMatrix as CareerMatrix} from './ClearMatrix';
import {WorkIdentity} from './WorkIdentity';
import {useSurfaceLayerState} from '../../navigation/surface-layer';
import {useLocale} from '../../i18n';
import {formatPlaytime, type ProfileStatistics} from '../../services/cloud-saves';
import {bookGameArtwork} from './book-artwork';
import './profile-game-data.css';

export function ProfileAllGames({data, showCareer, showRecords}: {
  data: ProfileStatistics; showCareer: boolean; showRecords: boolean;
}) {
  const {locale, t} = useLocale();
  const [selected, setSelected] = useSurfaceLayerState<GameId | null>('profile-work', null);
  const saved = new Map(data.games.map(({statistics}) => [statistics.game, statistics]));
  const works = (Object.keys(PRODUCT_GAMES) as GameId[]).filter(game => {
    const product = PRODUCT_GAMES[game];
    return !('testOnly' in product && product.testOnly) || saved.has(game);
  }).map(game => ({game, progress: saved.get(game)?.progress ?? []}))
    .sort((a, b) => Number(!!b.progress.length) - Number(!!a.progress.length) || Number(saved.has(b.game)) - Number(saved.has(a.game)));
  const work = showCareer ? works.find(item => item.game === selected) : undefined;
  const activeGame = work?.game;
  const careerMotion = useContentMotion<HTMLDivElement>(activeGame ?? 'overview');
  const previousGame = useRef(activeGame);
  const returnButton = useRef<HTMLButtonElement>(null);
  const workTrigger = useRef<HTMLButtonElement | null>(null);
  useLayoutEffect(() => {
    if (previousGame.current === activeGame) return;
    previousGame.current = activeGame;
    const target = activeGame ? returnButton.current : workTrigger.current;
    target?.focus({preventScroll:true});
  }, [activeGame]);
  const playtime = showRecords && work ? saved.get(work.game)?.playtimeSeconds : null;
  const played = showRecords ? data.games.map(({statistics: {game, playtimeSeconds}}) => ({game, seconds: playtimeSeconds ?? 0}))
    .filter(item => item.seconds > 0)
    .sort((a, b) => b.seconds - a.seconds) : [];
  const longest = played[0]?.seconds ?? 1;
  const total = showRecords ? data.playtimeSeconds : null;
  return <div id={showCareer ? 'profile-career' : undefined} className="profile-work-views">
    <div className="profile-work-overview" inert={!!work} ref={work ? undefined : careerMotion}>
      {showCareer && <section className="profile-career">
        <h2>{t('profile.tab.career')}</h2>
        <CareerMatrix works={works} onSelectWork={(game, trigger) => {
          workTrigger.current = trigger;
          setSelected(game);
        }}/>
      </section>}
      {(total !== null || played.length > 0) && <section id="profile-records" className="profile-playtime">
        <header><h2>{t('profile.flightTime')}</h2>
          {total !== null && <strong>{formatPlaytime(total, locale)}</strong>}
        </header>
        <ul>{played.map(({game, seconds}) => <li key={game}>
          <WorkIdentity game={game} title={PRODUCT_GAMES[game].title} number={PRODUCT_GAMES[game].number} cover={bookGameArtwork(game)}/>
          <span className="profile-playtime-bar" aria-hidden="true" style={{'--played': `${seconds / longest * 100}%`} as CSSProperties}/>
          <span>{formatPlaytime(seconds, locale)}</span>
        </li>)}</ul>
      </section>}
    </div>
    {work && <div ref={careerMotion} className="profile-career">
      <Button variant="ghost" size="sm" ref={returnButton} className="profile-game-return" onClick={() => setSelected(null)}>
        <ArrowLeft aria-hidden="true"/>{t('profile.clear.allGames')}
      </Button>
      <div className="profile-game-work">
        <WorkIdentity game={work.game} {...PRODUCT_GAMES[work.game]} cover={bookGameArtwork(work.game)}/>
        {typeof playtime === 'number' && <dl><dt>{t('profile.workPlaytime')}</dt><dd>{formatPlaytime(playtime, locale)}</dd></dl>}
      </div>
      <CareerMatrix key={work.game} work={work} readOnly/>
    </div>}
  </div>;
}
