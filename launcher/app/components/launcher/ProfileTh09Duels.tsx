import {ArrowLeft, ChevronDown, UserRound} from 'lucide-react';
import {AnimatePresence, motion} from 'motion/react';
import {useEffect, useLayoutEffect, useMemo, useRef, useState, type KeyboardEvent, type PointerEvent} from 'react';
import {useLocation, useNavigate} from 'react-router';
import {z} from 'zod';
import {formatTh09WinRate, parseTh09AccountSummary, th09CharacterLoadout, type Th09AccountSummary} from '../../../src/launcher/th09-room-stats.mjs';
import {useLocale} from '../../i18n';
import {useSurfaceLayerState} from '../../navigation/surface-layer';
import {useCompactNavigation} from '../../navigation/navigation-motion';
import {profileTh09Character, th09CharacterName, th09CharacterRating, th09RatingPoint} from '../../models/th09-character-ratings';
import {th09Duel, type WatchTh09Replay} from '../../models/th09-replay-archive';
import {accountRequest} from '../../services/account-client';
import {Button} from '../../ui/button';
import {ChoiceGroup, ChoiceItem} from '../../ui/choice-group';
import {Disclosure, DisclosurePanel} from '../../ui/disclosure';
import {useContentMotion} from '../../ui/use-surface-motion';
import {useSiteReducedMotion} from '../../ui/use-site-reduced-motion';
import {DuelReplayGroup} from '../player/DuelReplayGroup';
import {formatPlaytime} from '../../services/cloud-saves';
import {AccountAvatar} from './AccountAvatar';
import {IdentityMetadata} from './AccountIdentity';
import {AppLink} from './AppLink';
import {MainSelect} from './MainSelect';
import {ProfilePanelLoading} from './ProfileLoading';
import {PlayerAvatarCard} from './PlayerProfileCard';
import {Th09CharacterPortrait} from './Th09CharacterPortrait';
import './profile-th09-duels.css';

const initialCharacterCount = 3;
const initialDuelRows = 2;
const ratingEase = [0.2, 0, 0, 1] as const;

const count = z.number().int().nonnegative().max(Number.MAX_SAFE_INTEGER);
const duration = count.positive().nullish();
const characterRecord = z.object({character: count.refine(id => !!th09CharacterLoadout(id)), games: count.positive(), wins: count})
  .refine(value => value.wins <= value.games);
const rankPoint = z.object({at: count.max(8.64e15), rank: count.positive()});
type ChartSeries = 'whr' | 'rank';
type RatingPoint = z.infer<typeof th09RatingPoint>;
type CharacterRating = z.infer<typeof th09CharacterRating>;
const page = z.object({summary: z.custom<Th09AccountSummary>(value => !!parseTh09AccountSummary(value)),
  durationFrames: duration, characters: z.array(characterRecord).max(16).optional(),
  characterRatings:z.array(th09CharacterRating).max(16).optional(),
  characterRatingPreview:z.object({rating:z.number().int()}).optional(),
  ratingHistory: z.array(th09RatingPoint).optional(), rankHistory: z.array(rankPoint).optional(), duels: z.array(th09Duel).max(30), nextCursor: z.string().max(256).nullable().optional()});

export type {WatchTh09Replay} from '../../models/th09-replay-archive';

export function ProfileTh09Duels({username, owner, watchTh09Replay}: {
  username: string; owner: boolean; watchTh09Replay?: WatchTh09Replay;
}) {
  const {t, locale} = useLocale();
  const location = useLocation(), navigate = useNavigate();
  const compact = useCompactNavigation();
  const initialSelection = profileTh09Character(location.search);
  const [data, setData] = useState<z.infer<typeof page>>();
  const [request, setRequest] = useState<{cursor: string | null}>({cursor: null});
  const [pending, setPending] = useState(true);
  const [failed, setFailed] = useState(false);
  const [charactersExpanded, setCharactersExpanded] = useState(false);
  const [series, setSeries] = useState<ChartSeries>('whr');
  const [selectedCharacter, setSelectedCharacter] = useSurfaceLayerState<string | null>('profile-character-comparison', null, compact);
  function closeCharacter() {
    if (selectedCharacter !== null) {setSelectedCharacter(null); return;}
    if (initialSelection === null) return;
    const params = new URLSearchParams(location.search);
    params.delete('character');
    params.delete('loadout');
    void navigate({pathname:location.pathname,search:params.toString(),hash:location.hash},
      {state:location.state,replace:true,preventScrollReset:true});
  }
  const [duelsExpanded, setDuelsExpanded] = useState(false);
  const section = useRef<HTMLDivElement>(null);
  const [duelColumns, setDuelColumns] = useState(1);
  const preview = import.meta.env?.DEV ? data?.characterRatingPreview : undefined;
  const available = data ? owner || data.summary.games > 0 || !!data.characterRatings?.length : failed;
  useEffect(() => {
    if (!available || !section.current || typeof ResizeObserver === 'undefined') return;
    const observer = new ResizeObserver(([entry]) => setDuelColumns(entry.contentRect.width >= 600 ? 2 : 1));
    observer.observe(section.current);
    return () => observer.disconnect();
  }, [available]);
  useEffect(() => {
    const controller = new AbortController();
    setPending(true);
    setFailed(false);
    const path = 'v1/profiles/' + encodeURIComponent(username) + '/th09-duels';
    const query = request.cursor ? new URLSearchParams({cursor: request.cursor}) : undefined;
    void accountRequest(path, undefined, {signal: controller.signal, query, anonymous:!owner}).then(value => {
      if (controller.signal.aborted) return;
      const next = page.parse(value);
      setData(current => request.cursor && current ? {
        ...next,
        ratingHistory: next.ratingHistory ?? current.ratingHistory,
        rankHistory: next.rankHistory ?? current.rankHistory,
        characterRatings: next.characterRatings ?? current.characterRatings,
        characterRatingPreview: next.characterRatingPreview ?? current.characterRatingPreview,
        duels: [...new Map([...current.duels, ...next.duels].map(item => [item.duelId, item])).values()],
      } : next);
    }).catch(() => {
      if (!controller.signal.aborted) setFailed(true);
    }).finally(() => {
      if (!controller.signal.aborted) setPending(false);
    });
    return () => controller.abort();
  }, [username, request, owner]);
  const character = selectedCharacter === null ? initialSelection : Number(selectedCharacter);
  const activeRating = data?.characterRatings?.find(rating => rating.character === character);
  const selected = activeRating?.history?.length ? activeRating : undefined;
  const comparisonPage = compact && !!selected;
  const comparisonTitle = useRef<HTMLHeadingElement>(null);
  const comparisonTrigger = useRef<HTMLButtonElement | 'picker' | null>(null);
  const comparisonMotion = useContentMotion<HTMLDivElement>(comparisonPage ? 'comparison' : 'overview');
  function selectCharacter(value: string, trigger: HTMLButtonElement | 'picker') {
    if (compact && !comparisonPage) comparisonTrigger.current = trigger;
    setSelectedCharacter(value);
  }
  useLayoutEffect(() => {
    const target = comparisonPage ? comparisonTitle.current
      : comparisonTrigger.current === 'picker' ? section.current?.querySelector<HTMLButtonElement>('#profileTh09Character-trigger')
      : comparisonTrigger.current;
    target?.focus({preventScroll:true});
  }, [comparisonPage]);
  if (!available) return <>{pending ? <ProfilePanelLoading label={t('th09.accountRecord')}/> :
    <p className="profile-muted">{t('th09.noDuels')}</p>}</>;
  const numbers = new Intl.NumberFormat(locale);
  const ratings = new Map(data?.characterRatings?.map(rating => [rating.character, rating]));
  const characters = [...(data?.characters ?? (preview ? data?.characterRatings : undefined) ?? [])]
    .sort((left, right) => right.games - left.games || left.character - right.character);
  const visibleCharacters = charactersExpanded ? characters : characters.slice(0, initialCharacterCount);
  const showChart = !!data?.ratingHistory?.length || !!data?.rankHistory?.length || !!preview
    || !!data?.characterRatings?.some(rating => rating.history?.length);
  const duels = data?.duels ?? [];
  const duelGroups = groupDuels(duels);
  const previewGroups = previewDuelGroups(duelGroups, duelColumns);
  const canExpandDuels = duels.length > previewGroups.reduce((count, [, games]) => count + games.length, 0);

  const durationLabel = data?.durationFrames == null ? undefined : formatPlaytime(data.durationFrames / 60, locale);

  return <div className="profile-th09" ref={section} aria-busy={pending} data-duel-columns={duelColumns}
    data-character-page={comparisonPage || undefined}>
    {comparisonPage ? <header className="profile-th09-comparison-page-heading">
      <Button className="profile-th09-comparison-back" variant="ghost" size="icon-sm" aria-label={t('action.back')}
        onClick={closeCharacter}><ArrowLeft aria-hidden="true"/></Button>
      <h2 ref={comparisonTitle} tabIndex={-1}>{t('th09.characterRatingCompare')}</h2>
    </header> : <h2 className="sr-only">{t('th09.accountRecord')}</h2>}
    <div ref={comparisonMotion}>
    {data && <div className="profile-th09-overview-data">
      {showChart &&
        <RatingHistory ratingHistory={data.ratingHistory} rankHistory={data.rankHistory}
          previewRating={preview?.rating} characterRating={selected}
          showRank={series === 'rank'} onCloseCharacter={closeCharacter} backInHeader={comparisonPage}
          onSeriesChange={setSeries} />}
      {!preview && <div className="profile-th09-summary-row">
        <dl className="profile-th09-summary">
          <div><dt>{t('th09.winSummary')}</dt>
            <dd className="profile-th09-win-summary">
              <span>{numbers.format(data.summary.wins)} /</span> <strong>{numbers.format(data.summary.games)}</strong>{' '}
              <span className="profile-th09-win-secondary">({formatTh09WinRate(data.summary, locale) ?? '—'})</span>
            </dd>
          </div>
          {durationLabel && <div><dt>{t('th09.recordedDuration')}</dt><dd>{durationLabel}</dd></div>}
        </dl>
      </div>}
    </div>}
    {data && <div className="profile-th09-records">
      {characters.length > 0 && <div className="profile-th09-character-record">
        <div className="profile-th09-record-toolbar">
          <RecordHeading title={t('th09.characterRecord')} controls="profile-th09-character-list"
            expanded={characters.length > initialCharacterCount ? charactersExpanded : undefined}
            onToggle={() => setCharactersExpanded(expanded => !expanded)} />
          {!comparisonPage && characters.some(item => ratings.get(item.character)?.history?.length) &&
            <MainSelect id="profileTh09Character" aria-label={t('th09.characterRecord')} value={selected ? String(selected.character) : 'all'} onChange={event => {
              if (event.target.value === 'all') closeCharacter();
              else selectCharacter(event.target.value, 'picker');
            }}>
              <option value="all">{t('th09.allLoadouts')}</option>
              {characters.filter(item => ratings.get(item.character)?.history?.length).map(item =>
                <option key={item.character} value={item.character}>{th09CharacterName(item.character,t)}</option>)}
            </MainSelect>}
        </div>
        <ul id="profile-th09-character-list" className="profile-th09-characters">
          {visibleCharacters.map(item => {
            const label = th09CharacterName(item.character,t);
            const rating = ratings.get(item.character);
            const isSelected = selected?.character === item.character;
            const score = rating && numbers.format(rating.rating);
            return <li className="profile-th09-character" key={item.character} data-selected={isSelected || undefined}>
              <div className="profile-th09-character-copy">
                <h4>{label}</h4>
                <div className="profile-th09-character-meta">
                  <span>{t('th09.characterGames',{count:numbers.format(item.games)})}</span>
                  {'wins' in item && <span>| {t('th09.winRate')} {formatTh09WinRate(item, locale)}</span>}
                  {rating && (rating.history?.length ? <Button variant="link" size="sm" className="profile-th09-rating-toggle"
                    aria-label={`${label} | ${t('th09.characterRatingHistory')} | ${score}`}
                    aria-pressed={isSelected} aria-controls={showChart ? 'profile-th09-rating-history' : undefined}
                    onClick={event => isSelected ? closeCharacter() : selectCharacter(String(item.character), event.currentTarget)}>
                    {t('th09.characterRating')} <strong>{score}</strong></Button> :
                    <span>{t('th09.characterRating')} <strong>{score}</strong></span>)}
                </div>
              </div>
              <Th09CharacterPortrait character={item.character} />
            </li>;
          })}
        </ul>
      </div>}
      {!preview && !comparisonPage && <div className="profile-th09-duels">
        <RecordHeading title={t('th09.duelHistory')} controls="profile-th09-duel-list"
          action={t(duelsExpanded ? 'announcements.less' : 'th09.moreDuels')}
          expanded={canExpandDuels ? duelsExpanded : undefined}
          onToggle={() => setDuelsExpanded(expanded => !expanded)} />
        {duels.length ? <DuelHistory username={username} groups={duelsExpanded ? duelGroups : previewGroups} watchTh09Replay={watchTh09Replay}/> : <p className="profile-muted">{t('th09.noDuels')}</p>}
        {!failed && data.nextCursor && (!canExpandDuels || duelsExpanded) && <div className="profile-th09-actions">
          <Button variant="outline" size="sm" disabled={pending} onClick={() => {
            setDuelsExpanded(true);
            setRequest({cursor: data.nextCursor ?? null});
          }}>{t('th09.moreDuels')}</Button>
        </div>}
      </div>}
    </div>}
    {failed && <div className="profile-th09-actions" role="status">
      <span>{t('account.requestFailed')}</span>
      <Button variant="outline" size="sm" onClick={() => setRequest({...request})}>{t('action.retry')}</Button>
    </div>}
    </div>
  </div>;
}

type DuelGroup = [day: string, duels: z.infer<typeof th09Duel>[]];

function groupDuels(duels: z.infer<typeof th09Duel>[]): DuelGroup[] {
  const days = new Map<string, typeof duels>();
  for (const item of duels) {
    const date = new Date(item.playedAt).toDateString();
    let items = days.get(date);
    if (!items) {
      items = [];
      days.set(date, items);
    }
    items.push(item);
  }
  return [...days];
}

function previewDuelGroups(groups: DuelGroup[], columns: number): DuelGroup[] {
  const preview: DuelGroup[] = [];
  let rows = initialDuelRows;
  for (const [day, duels] of groups) {
    const visible = duels.slice(0, rows * columns);
    preview.push([day, visible]);
    rows -= Math.ceil(visible.length / columns);
    if (!rows) break;
  }
  return preview;
}

function DuelHistory({username, groups, watchTh09Replay}: {username: string; groups: DuelGroup[]; watchTh09Replay?: WatchTh09Replay}) {
  const {t, locale} = useLocale();
  const numbers = new Intl.NumberFormat(locale);
  const dates = new Intl.DateTimeFormat(locale, {dateStyle:'medium'});
  const recentDates = new Intl.DateTimeFormat(locale, {month:'long', day:'numeric'});
  const weekday = new Intl.DateTimeFormat(locale, {weekday:'short'});
  const currentYear = new Date().getFullYear();
  return <div id="profile-th09-duel-list" className="profile-th09-days">
    {groups.map(([date, items]) => <div key={date}>
      <h4 className="profile-th09-day">
        <time dateTime={new Date(items[0].playedAt).toISOString()}>
          {new Date(items[0].playedAt).getFullYear() === currentYear ? recentDates.format(items[0].playedAt) : dates.format(items[0].playedAt)}{' '}
          {weekday.format(items[0].playedAt)}
        </time>
      </h4>
      <ul className="profile-th09-history">
        {items.map(item => {
          const wins = numbers.format(item.wins);
          const losses = numbers.format(item.losses);
          const outcome = item.wins === item.losses ? 'draw' : item.wins > item.losses ? 'win' : 'loss';
          return <li key={item.duelId} className="profile-match" data-result={outcome}>
            <Disclosure>{({open, trigger, panel}) => <>
            <div className="profile-match-row">
            <span className="profile-match-result">{t(outcome === 'win' ? 'profile.matchWin' : outcome === 'loss' ? 'profile.matchLoss' : 'profile.matchDraw')}</span>
            <strong className="profile-match-score" role="img" aria-label={`${t('th09.score')} ${wins} : ${losses}`}>{wins}<span>:</span>{losses}</strong>
            {item.opponent ? <span className="profile-th09-opponent">
              <PlayerAvatarCard username={item.opponent.username} name={item.opponent.displayName}
                avatar={item.opponent.avatar ? `/api/v1/profiles/${encodeURIComponent(item.opponent.username)}/avatar/${item.opponent.avatar}` : undefined}>
                <button type="button" className="player-card-avatar-button" aria-label={item.opponent.displayName}><AccountAvatar src={item.opponent.avatar
                  ? `/api/v1/profiles/${encodeURIComponent(item.opponent.username)}/avatar/${item.opponent.avatar}`
                  : undefined} loading="lazy" fallback={<UserRound aria-hidden="true"/>}/></button>
              </PlayerAvatarCard>
              <AppLink className="profile-th09-opponent-name" href={'/u/' + encodeURIComponent(item.opponent.username)} rememberEntry>
                <span>{item.opponent.displayName}</span>
                <IdentityMetadata username={item.opponent.username} country={item.opponent.countryCode}/>
              </AppLink>
            </span> : <span className="profile-th09-opponent">
              <span className="account-avatar profile-th09-guest-avatar" aria-hidden="true"><UserRound /></span>
              <span className="profile-th09-opponent-name">{t('th09.guestOpponent')}</span>
            </span>}
            <div className="profile-th09-history-meta">
              {!!item.replayCount && <Button type="button" size="sm" variant="ghost" {...trigger}>
                <span>{t('profile.matchReplays',{count:item.replayCount})}</span><ChevronDown aria-hidden="true"/>
              </Button>}
              {item.durationFrames != null && <small>{formatPlaytime(item.durationFrames / 60, locale)}</small>}
            </div>
            </div>
            {!!item.replayCount && <DisclosurePanel {...panel} className="profile-th09-replays">
              <DuelReplayGroup className="profile-match-replays" username={username} duelId={item.duelId}
                count={item.replayCount} watch={watchTh09Replay} open={open}/>
            </DisclosurePanel>}
            </>}</Disclosure>
          </li>;
        })}
      </ul>
    </div>)}
  </div>;
}

function RatingHistory({ratingHistory, rankHistory, previewRating, characterRating, showRank, onCloseCharacter, onSeriesChange, backInHeader}: {
  ratingHistory?: RatingPoint[]; rankHistory?: z.infer<typeof rankPoint>[];
  previewRating?: number; characterRating?: CharacterRating;
  showRank: boolean; onCloseCharacter(): void; onSeriesChange(series: ChartSeries): void; backInHeader: boolean;
}) {
  const {t, locale} = useLocale();
  const reduced = useSiteReducedMotion();
  const transition = {duration:reduced ? 0 : 0.24, ease:ratingEase};
  const numbers = new Intl.NumberFormat(locale);
  const rank = (showRank || !ratingHistory?.length) && !!rankHistory?.length;
  const ranks = useMemo(() => rankHistory?.map(point => ({at: point.at, rating: -point.rank})), [rankHistory]);
  const points = rank ? ranks : characterRating?.history ?? ratingHistory;
  const comparison = characterRating ? (rank ? characterRating.history : ratingHistory) : undefined;
  const overallRating = ratingHistory?.at(-1)?.rating ?? previewRating;
  const currentRank = rankHistory?.at(-1)?.rank;
  const characterName = characterRating && th09CharacterName(characterRating.character, t);
  const baseTitle = t(rank ? 'th09.rankHistory' : 'th09.ratingHistory');
  const title = characterName && !rank ? `${characterName} | ${t('th09.characterRatingHistory')}` : baseTitle;
  const series = rank ? 'rank' : 'whr';
  const firstAt = points?.[0]?.at;
  const lastAt = points?.at(-1)?.at;
  const startAt = firstAt === undefined ? undefined : Math.min(firstAt, comparison?.[0]?.at ?? firstAt);
  const endAt = lastAt === undefined ? undefined : Math.max(lastAt, comparison?.at(-1)?.at ?? lastAt);
  const selectedHeader = characterRating && <motion.div key={characterRating.character} className="profile-th09-comparison"
    initial={reduced ? false : {opacity:0, x:-8}} animate={{opacity:1, x:0}}
    exit={{opacity:0, x:-8, pointerEvents:'none'}} transition={transition}>
    {!backInHeader && <Button className="profile-th09-comparison-back" variant="ghost" size="icon-sm" aria-label={t('action.back')}
      title={t('action.back')} onClick={onCloseCharacter}><ArrowLeft aria-hidden="true" /></Button>}
    <div className="profile-th09-comparison-character">
      <Th09CharacterPortrait character={characterRating.character} />
      <span className="profile-th09-comparison-name">{characterName}</span>
      <div className="profile-th09-comparison-values">
        <strong>{numbers.format(characterRating.rating)}</strong>
        {characterRating.rank !== undefined && <span className="profile-th09-comparison-rank" title={t('th09.rank')}>
          #{numbers.format(characterRating.rank)}
        </span>}
      </div>
    </div>
  </motion.div>;

  return <figure className="profile-th09-whr" id="profile-th09-rating-history">
    <figcaption>
      <div className="profile-th09-whr-heading">
        {reduced ? selectedHeader : <AnimatePresence initial={false} mode="popLayout">{selectedHeader}</AnimatePresence>}
        <motion.div className="profile-th09-current" layout={reduced ? false : 'position'} transition={transition}>
          {overallRating !== undefined && <div><span>WHR</span><strong>{numbers.format(overallRating)}</strong></div>}
          {currentRank !== undefined && <div><span>{t('th09.rank')}</span><strong>#{numbers.format(currentRank)}</strong></div>}
        </motion.div>
      </div>
      {currentRank !== undefined && !!ratingHistory?.length &&
        <ChoiceGroup className="profile-th09-series" type="single" value={series}
          aria-label={baseTitle} onValueChange={value => {
            if (value === 'whr' || value === 'rank') onSeriesChange(value);
          }}>
          <ChoiceItem value="whr">WHR</ChoiceItem>
          <ChoiceItem value="rank">{t('th09.rank')}</ChoiceItem>
        </ChoiceGroup>}
    </figcaption>
    <div className="profile-th09-chart-body">
      {points && startAt !== undefined && endAt !== undefined &&
        <RatingPlot points={points} comparison={comparison} character={characterRating?.character}
          startAt={startAt} endAt={endAt} rank={rank} title={title} reduced={reduced} />}
    </div>
  </figure>;
}

function ratingBounds(points: RatingPoint[]) {
  let low = points[0].rating;
  let high = low;
  let bestIndex = 0;
  for (const [index, point] of points.entries()) {
    low = Math.min(low, point.rating);
    if (point.rating >= high) {
      high = point.rating;
      bestIndex = index;
    }
  }
  return {low, high, bestIndex};
}

function ratingScale(min: number, max: number, tick: number) {
  const low = Math.floor(min / tick) * tick;
  const high = Math.max(low + tick, Math.ceil(max / tick) * tick);
  return (rating: number) => (high - rating) * 100 / (high - low);
}

function RatingPlot({points, comparison, character, startAt, endAt, rank, title, reduced}: {
  points: RatingPoint[]; comparison?: RatingPoint[]; character?: number; startAt: number; endAt: number;
  rank: boolean; title: string; reduced: boolean;
}) {
  const {locale} = useLocale();
  const [selected, setSelected] = useState<number | null>(null);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Switching series clears the selected point while keeping existing lines mounted.
  useEffect(() => setSelected(null), [rank, character]);
  const numbers = new Intl.NumberFormat(locale);
  const dates = new Intl.DateTimeFormat(locale, {dateStyle: 'medium', timeZone: 'UTC'});
  const formatValue = (value: number) => rank ? '#' + numbers.format(-value) : numbers.format(value);
  const lastIndex = points.length - 1;
  const axisDates = new Intl.DateTimeFormat(locale, {
    month:'short', day:'numeric', timeZone:'UTC',
    year:new Date(startAt).getUTCFullYear() === new Date(endAt).getUTCFullYear() ? undefined : '2-digit',
  });
  const span = endAt - startAt;
  const index = selected == null ? lastIndex : Math.min(selected, lastIndex);
  const current = points[index];
  const plot = useMemo(() => {
    const {low, high, bestIndex} = ratingBounds(points);
    const other = comparison?.length ? ratingBounds(comparison) : undefined;
    const primaryScale = ratingScale(
      !rank && other ? Math.min(low, other.low) : low,
      !rank && other ? Math.max(high, other.high) : high,
      rank ? 1 : 50,
    );
    const comparisonScale = rank && other ? ratingScale(other.low, other.high, 50) : primaryScale;
    const coordinate = (point: RatingPoint, scale: (rating: number) => number) => ({
      x: span > 0 ? (point.at - startAt) * 100 / span : 100,
      y: scale(point.rating),
    });
    const coordinates = points.map(point => coordinate(point, primaryScale));
    const line = coordinates.flatMap(({x,y}, i) => rank && i > 0
      ? [`${x * 10},${coordinates[i - 1].y}`, `${x * 10},${y}`]
      : [`${x * 10},${y}`]).join(' ');
    const reference = comparison?.map(point => coordinate(point, comparisonScale)) ?? [];
    const referenceLine = reference.map(({x,y}) => `${x * 10},${y}`).join(' ');
    return {coordinates, line, bestIndex, reference, referenceLine};
  }, [points, comparison, rank, startAt, span]);
  const marker = plot.coordinates[index];
  const peak = plot.coordinates[plot.bestIndex];
  const comparingRank = rank && plot.reference.length > 0;
  const characterLine = character === undefined ? undefined : `character-${character}`;
  const primaryLine = {id:rank ? 'rank' : characterLine ?? 'whr', path:plot.line,
    size:points.length, muted:comparingRank};
  const referenceLine = {id:comparingRank ? characterLine : 'whr', path:plot.referenceLine,
    size:plot.reference.length, muted:!comparingRank};
  const lines = comparingRank ? [primaryLine, referenceLine] : [referenceLine, primaryLine];
  const paths = lines.filter(line => line.size > 1).map(line => {
    const isCharacter = line.id === characterLine;
    return <motion.polyline key={line.id} points={line.path} vectorEffect="non-scaling-stroke"
      data-muted={line.muted || undefined} data-rank-reference={line.id === 'rank' && line.muted || undefined}
      initial={reduced ? false : {opacity:0, ...(isCharacter && {pathLength:0.35})}}
      animate={{opacity:line.muted ? 0.5 : 1, ...(isCharacter && {pathLength:1})}} exit={{opacity:0}}
      transition={{duration:reduced ? 0 : 0.24, ease:ratingEase,
        pathLength:{duration:reduced ? 0 : 0.34, ease:ratingEase}}} />;
  });

  function selectAt(event: PointerEvent<HTMLDivElement>) {
    const rect = event.currentTarget.getBoundingClientRect();
    if (!rect.width) return;
    const at = startAt + (event.clientX - rect.left) / rect.width * span;
    let left = 0;
    let right = lastIndex;
    while (left < right) {
      const middle = Math.floor((left + right) / 2);
      if (at <= (points[middle].at + points[middle + 1].at) / 2) right = middle;
      else left = middle + 1;
    }
    setSelected(left);
  }

  function stepThrough(event: KeyboardEvent<HTMLDivElement>) {
    const targets: Record<string, number | undefined> = {
      ArrowLeft: index - 1, ArrowDown: index - 1,
      ArrowRight: index + 1, ArrowUp: index + 1,
      Home: 0, End: lastIndex,
    };
    const target = targets[event.key];
    if (target == null) return;
    event.preventDefault();
    setSelected(Math.max(0, Math.min(lastIndex, target)));
  }

  return <>
      <div className="profile-th09-whr-plot" role="slider" tabIndex={0} data-rank-comparison={comparingRank || undefined}
        aria-label={title} aria-valuemin={1} aria-valuemax={points.length}
        aria-valuenow={index + 1} aria-valuetext={`${dates.format(current.at)} | ${formatValue(current.rating)}`}
        onPointerDown={selectAt} onPointerMove={selectAt}
        onPointerLeave={event => {if (event.pointerType === 'mouse') setSelected(null);}}
        onBlur={() => setSelected(null)}
        onKeyDown={event => {
          if (event.key === 'Escape') setSelected(null);
          else stepThrough(event);
        }}>
        <svg aria-hidden="true" viewBox="0 0 1000 100" preserveAspectRatio="none">
          {reduced ? paths : <AnimatePresence initial={false}>{paths}</AnimatePresence>}
        </svg>
        {plot.reference.length === 1 && <span className="profile-th09-whr-reference-point" data-muted={!comparingRank || undefined} aria-hidden="true"
          key={comparingRank ? characterLine : 'whr'} data-character-point={comparingRank || undefined}
          style={{left:`${plot.reference[0].x}%`, top:`${plot.reference[0].y}%`}} />}
        <span className="profile-th09-whr-guide" data-visible={selected !== null || undefined} style={{left: `${marker.x}%`}} />
        <span className="profile-th09-whr-tooltip" data-visible={selected !== null || undefined} aria-hidden="true"
          data-below={marker.y < 40 || undefined}
          style={{left:`${marker.x}%`, top:`${marker.y}%`, translate:`-${marker.x}% var(--profile-rating-tip-offset)`}}>
          <strong>{formatValue(current.rating)}</strong>
          <time dateTime={new Date(current.at).toISOString()}>{axisDates.format(current.at)}</time>
        </span>
        <span className="profile-th09-whr-peak-guide" aria-hidden="true" style={{top: `${peak.y}%`}} />
        <span className="profile-th09-whr-peak" aria-hidden="true" key={primaryLine.id}
          data-character-point={!!characterLine && !rank && points.length === 1 || undefined}
          style={{left: `${peak.x}%`, top: `${peak.y}%`}} />
        <span className="profile-th09-whr-point" data-at-peak={index === plot.bestIndex || undefined}
          style={{left: `${marker.x}%`, top: `${marker.y}%`}} />
      </div>
      <div className="profile-th09-whr-dates" aria-hidden="true">
        {span > 0 && <time dateTime={new Date(startAt).toISOString()}>{axisDates.format(startAt)}</time>}
        <time dateTime={new Date(endAt).toISOString()}>{axisDates.format(endAt)}</time>
      </div>
  </>;
}

function RecordHeading({title, controls, expanded, onToggle, action}: {
  title: string; controls: string; expanded?: boolean; onToggle(): void; action?: string;
}) {
  const separateAction = !!action && expanded !== undefined;
  return <h3 className="profile-th09-record-heading" data-has-action={separateAction || undefined}>
    {separateAction && <span>{title}</span>}
    {expanded === undefined ? title : <Button variant="ghost" size="sm" aria-controls={controls}
      aria-expanded={expanded} onClick={onToggle}>{action ?? title}<ChevronDown aria-hidden="true" /></Button>}
  </h3>;
}
