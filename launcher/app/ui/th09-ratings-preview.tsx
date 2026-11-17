import { ArrowDown, ArrowDownUp, ArrowLeft, ArrowUp, ChevronDown, ListOrdered, LocateFixed, SlidersHorizontal } from 'lucide-react';
import { Fragment, type ReactNode, useEffect, useRef, useState, useSyncExternalStore } from 'react';
import {Link, useSearchParams} from 'react-router';
import { z } from 'zod';
import { AccountAvatar } from '../components/launcher/AccountAvatar';
import { PlayerAvatarCard } from '../components/launcher/PlayerProfileCard';
import { BookThemeToggle } from '../components/launcher/BookThemeToggle';
import { MainSelect, MainSelectPrefix } from '../components/launcher/MainSelect';
import { Th09CharacterPortrait } from '../components/launcher/Th09CharacterPortrait';
import th09Artwork from '../components/launcher/book-covers/th09.webp?url';
import { useLocale, type UiMessageKey } from '../i18n';
import { sitePageTitle } from '../navigation/document-locale';
import {accountsEnabled} from '../release-features';
import {accountRequest} from '../services/account-client';
import {th09CharacterIds, th09CharacterName, th09CharacterRating} from '../models/th09-character-ratings';
import { ActionMenu, ActionMenuItem } from './action-menu';
import { Badge } from './badge';
import { Button } from './button';
import { Input } from './input';
import { InputGroup, InputGroupInput } from './input-group';
import { Field, FieldLabel } from './field';
import { FieldHelp } from './field-help';
import { Popover, PopoverContent, PopoverTrigger } from './popover';
import { Skeleton } from './skeleton';

const count = z.number().int().nonnegative();
const timestamp = count.max(8.64e15);
const status = z.enum(['active', 'provisional', 'inactive']);
const boardSchema = z.object({
  as_of: timestamp,
  board: z.array(z.object({
    player: z.string(),
    rating: z.number(),
    interval_halfwidth: z.number().nonnegative(),
    status,
    games: count.positive(),
    wins: count,
    opponents: count,
    connected_group: count,
    last_at: timestamp,
    profile: z.object({
      username: z.string().regex(/^[A-Za-z0-9._]{3,12}$/),
      nickname: z.string().min(1),
      avatar: z.string().max(11_000).regex(/^(?:data:image\/(?:webp|jpeg);base64,[A-Za-z0-9+/]+={0,2}|\/api\/v1\/profiles\/[A-Za-z0-9._]{3,12}\/avatar\/[a-f0-9]{16})$/).optional(),
    }).optional(),
    top_characters: z.array(z.object({character:count.max(15),games:count.positive()})).max(3).optional(),
    character_ratings: z.array(th09CharacterRating).max(16).optional(),
  })),
});
type Board = z.infer<typeof boardSchema>;
type Filter = z.infer<typeof status> | 'all';
type Entry = Board['board'][number] & { characterScore?: z.infer<typeof th09CharacterRating>; rank?: number };
type RankedEntry = Entry & { position: number };
const statusLabels = { active: 'th09.boardActive', provisional: 'th09.boardProvisional', inactive: 'th09.boardInactive' } as const;
const filters = ['active', 'provisional', 'inactive', 'all'] as const;
const defaultMinimum = 11;
const numbers = new Intl.NumberFormat('zh-CN', { maximumFractionDigits: 0 });
const percentages = new Intl.NumberFormat('zh-CN', { style: 'percent', maximumFractionDigits: 1 });
const winRate = (row: Entry) => row.wins / row.games;
const playerName = (row: Entry) => row.profile?.nickname ?? row.player;
const subscribeHydration = () => () => {};
const boardGames = (row: Entry) => row.characterScore?.games ?? row.games;
const boardRating = (row: Entry) => row.characterScore?.rating ?? row.rating;
const boardDeviation = (row: Entry) => row.characterScore?.interval ?? row.interval_halfwidth;

type Column = {
  key: string;
  title: UiMessageKey | '#';
  compact?: boolean;
  numeric?: boolean;
  value(row: RankedEntry): number | string;
  render?(row: RankedEntry): ReactNode;
};
const ratingColumn: Column = { key: 'rating', title: 'th09.boardRating', compact: true, numeric: true, value: boardRating };
const deviationColumn: Column = { key: 'deviation', title: 'th09.boardDeviation', numeric: true, value: boardDeviation,
  render: row => `± ${numbers.format(boardDeviation(row))}` };

function useRatingColumns(characterBoard: boolean): Column[] {
  const {locale, t} = useLocale();
  const days = new Intl.DateTimeFormat(locale, { year: 'numeric', month: '2-digit', day: '2-digit' });
  const scoreColumns: Column[] = characterBoard ? [
    { ...ratingColumn, title: 'th09.characterRating', render: row => <span className="rating-character-score">
      {numbers.format(boardRating(row))}<small className="rating-compact-deviation"><span className="sr-only">{t('th09.boardCharacterDeviation')} </span>±{numbers.format(boardDeviation(row))}</small>
    </span> },
    { ...deviationColumn, title: 'th09.boardCharacterDeviation' },
    { ...ratingColumn, key: 'overall', value: row => row.rating },
  ] : [ratingColumn, deviationColumn];
  return [
    { key: 'position', title: '#', compact: true, numeric: true, value: row => row.position },
    { key: 'player', title: 'th09.boardPlayer', compact: true, value: playerName },
    ...scoreColumns,
    { key: 'games', title: 'th09.boardGames', numeric: true, value: boardGames },
    { key: 'winRate', title: 'th09.winRate', numeric: true, value: winRate, render: row => percentages.format(winRate(row)) },
    { key: 'community', title: 'th09.boardGroup', value: row => row.connected_group,
      render: row => t('th09.boardGroupNumber', {number: row.connected_group}) },
    { key: 'last', title: 'th09.boardLastGame', numeric: true, value: row => row.last_at,
      render: row => <time dateTime={new Date(row.last_at).toISOString()}>{days.format(row.last_at)}</time> },
    { key: 'status', title: 'th09.boardStatus', value: row => row.status,
      render: row => <Badge variant="outline">{t(statusLabels[row.status])}</Badge> },
  ];
}

function renderRatingCell(column: Column, row: RankedEntry) {
  return column.render ? column.render(row) : numbers.format(Number(column.value(row)));
}

export default function Th09RatingsPreview() {
  const {t, locale} = useLocale();
  return <div className="ui-review" lang={locale}>
    <a className="catalog-skip" href="#rating-main">{t('th09.boardSkip')}</a>
    <header className="ui-review-header">
      <a href="./lobby.html"><ArrowLeft aria-hidden="true" />{t('lobby.title')}</a>
      <BookThemeToggle />
    </header>
    <Th09RatingBoard />
  </div>;
}

export function Th09RatingBoard() {
  const {t} = useLocale();
  const [search, setSearch] = useSearchParams();
  // A prerendered document always starts at the overall board.
  const hydrated = useSyncExternalStore(subscribeHydration, () => true, () => false);
  const characterParam = hydrated ? search.get('character') : null;
  const selected = characterParam && /^\d+$/.test(characterParam) ? Number(characterParam) : -1;
  const character = th09CharacterIds.includes(selected) ? selected : hydrated && search.get('view') === 'characters' ? 0 : undefined;
  const [board, setBoard] = useState<Board | null>(null);
  const [failed, setFailed] = useState(false);
  const [attempt, setAttempt] = useState(0);

  useEffect(() => {
    const previousTitle = document.title;
    document.title = sitePageTitle(t('th09.ratingBoard'));
    return ()=>{document.title = previousTitle;};
  }, [t]);

  // biome-ignore lint/correctness/useExhaustiveDependencies: Manual retry starts a fresh request cycle.
  useEffect(() => {
    const controller = new AbortController();
    let pending = false;
    async function refresh() {
      if (!accountsEnabled || pending || document.hidden) return false;
      pending = true;
      try {
        const next = boardSchema.parse(await accountRequest('v1/th09/ratings/board', undefined, {signal: controller.signal, anonymous:true}));
        if (!controller.signal.aborted) {setBoard(next);setFailed(false);}
        return true;
      } catch {
        if (!controller.signal.aborted) setFailed(true);
        return false;
      }
      finally {pending = false;}
    }
    if (accountsEnabled) void refresh();
    else setFailed(true);
    const refreshVisible = () => {if (!document.hidden) void refresh();};
    document.addEventListener('visibilitychange', refreshVisible);
    const timer = accountsEnabled ? window.setInterval(refresh, 12_000) : undefined;
    return () => {
      if (timer !== undefined) window.clearInterval(timer);
      document.removeEventListener('visibilitychange', refreshVisible);
      controller.abort();
    };
  }, [attempt]);

  function changeCharacter(value: number | undefined) {
    const next = new URLSearchParams(search);
    next.delete('view');
    if (value === undefined) next.delete('character');
    else next.set('character', String(value));
    setSearch(next);
  }

  const rows = character === undefined ? board?.board : board?.board.flatMap(row => {
    const characterScore = row.character_ratings?.find(score => score.character === character);
    return characterScore ? [{...row, characterScore}] : [];
  }).sort((a, b) => b.characterScore.rating - a.characterScore.rating || b.characterScore.games - a.characterScore.games
    || a.player.localeCompare(b.player)).map((row, index) => ({...row, rank: index + 1}));
  function retry() {
    setFailed(false);
    setAttempt(value => value + 1);
  }
  return <div className="rating-preview" id="rating-main">
    <RatingBoard rows={rows} updatedAt={board?.as_of} character={character}
      onCharacterChange={changeCharacter} failed={failed && !board} retry={retry} />
  </div>;
}

const defaultSort = {key:'rating', direction:-1};

function RatingBoard({rows, updatedAt, character, onCharacterChange, failed, retry}: {
  rows?: Entry[]; updatedAt?: number; character?: number; onCharacterChange(value: number | undefined): void;
  failed: boolean; retry(): void;
}) {
  const {t, locale} = useLocale();
  const characterBoard = character !== undefined;
  const columns = useRatingColumns(characterBoard);
  const [filter, setFilter] = useState<Filter>('active');
  const [group, setGroup] = useState('all');
  const [minimum, setMinimum] = useState<string>();
  const [query, setQuery] = useState('');
  const [contextScope, setContextScope] = useState<string | null>(null);
  const [sortState, setSort] = useState<(typeof defaultSort & {scope:string})>();
  const [expanded, setExpanded] = useState<{scope:string; player:string} | null>(null);
  const [distributionOpen, setDistributionOpen] = useState(false);
  const focusRow = useRef<HTMLTableRowElement>(null);
  const scope = characterBoard ? String(character) : 'overall';
  const showContext = contextScope === scope;
  const sort = sortState?.scope === scope ? sortState : defaultSort;
  const compactColumnCount = columns.filter(column => column.compact).length;
  const detailColumns = columns.filter(column => !column.compact && (!characterBoard || column.key !== 'deviation'));
  const groups = [...new Set(rows?.map(row => row.connected_group))].sort((a, b) => a - b);
  const minGames = Math.max(1, Math.trunc(Number(minimum ?? defaultMinimum)) || 1);
  const search = query.trim().toLowerCase();
  const pool = (rows ?? []).map((row, index) => ({ ...row, position: row.rank ?? index + 1 }))
    .filter(row => boardGames(row) >= minGames && (filter === 'all' || row.status === filter)
      && (characterBoard || group === 'all' || row.connected_group === Number(group)));
  const nameQuery = search.replace(/^@/, '');
  const found = search ? pool.filter(row => playerName(row).toLowerCase().includes(nameQuery)
    || (row.profile && `@${row.profile.username.toLowerCase()}`.includes(search))) : pool;
  const located = showContext ? found[0]?.player : undefined;
  const column = columns.find(item => item.key === sort.key) ?? ratingColumn;
  const visible = [...(showContext ? pool : found)].sort((a, b) => {
    const left = column.value(a), right = column.value(b);
    const comparison = typeof left === 'number' && typeof right === 'number'
      ? left - right : String(left).localeCompare(String(right), locale);
    return sort.direction * comparison || a.position - b.position;
  });

  useEffect(() => {
    if (!located) return;
    focusRow.current?.focus({ preventScroll: true });
    focusRow.current?.scrollIntoView({ block: 'nearest', inline: 'nearest' });
  }, [located]);

  function sortBy(next: Column) {
    setSort({ scope, key: next.key, direction: sort.key === next.key ? -sort.direction
      : next.numeric && next.key !== 'position' ? -1 : 1 });
  }

  function clearFilters() {
    setQuery('');
    setMinimum('1');
    setFilter('all');
    setGroup('all');
    setContextScope(null);
  }

  function sortDirection(item: Column) {
    if (sort.key !== item.key) return null;
    return sort.direction === 1 ? <ArrowUp aria-hidden="true" /> : <ArrowDown aria-hidden="true" />;
  }

  const filtered = minGames > 1 || filter !== 'all' || (!characterBoard && group !== 'all');
  const loading = !rows && !failed;
  const boardTitle = t('th09.ratingBoard');
  const columnTitle = (item: Column) => item.title === '#' ? item.title : t(item.title);

  return <div className="rating-layout" data-character-board={characterBoard || undefined}>
    <div className="rating-heading">
      <div className="rating-heading-title">
        <h1><img className="rating-heading-icon" src={th09Artwork} width={36} height={36} alt={boardTitle} /></h1>
        {characterBoard ? <FieldHelp id="character-rating-help" label={t('th09.boardCharacterHelp')}>
          {t('th09.boardCharacterHelpText')}
        </FieldHelp> : <FieldHelp id="rating-explanation" label={t('th09.boardHelp')}>
          {t('th09.boardHelpText')}
        </FieldHelp>}
      </div>
      <Button className="rating-distribution-trigger" variant="ghost" size="sm"
        aria-expanded={distributionOpen} aria-controls="rating-overview" disabled={!rows}
        onClick={() => setDistributionOpen(open => !open)}>
        {t('th09.boardDistribution')}<ChevronDown aria-hidden="true" />
      </Button>
    </div>
    <aside className="rating-overview" id="rating-overview" aria-label={t('th09.boardDistribution')} hidden={!distributionOpen}>
      {distributionOpen && updatedAt !== undefined && <RatingOverview rows={pool} matches={search ? found : []} updatedAt={updatedAt} characterBoard={characterBoard} />}
    </aside>
    <fieldset className="rating-tools" aria-label={t('th09.boardTools')}>
      <div className="rating-character-select">
        <MainSelectPrefix prefix={<RatingCharacterThumbnail character={character} />}>
          <MainSelect id="rating-board-scope" aria-label={t('th09.boardSelect')} value={scope} onChange={event => {
            const value = event.currentTarget.value;
            setContextScope(null);
            setSort(undefined);
            setExpanded(null);
            onCharacterChange(value === 'overall' ? undefined : Number(value));
          }} renderOption={({value, label}) => <span className="rating-character-option">
            <RatingCharacterThumbnail character={value === 'overall' ? undefined : Number(value)} />
            <span>{label}</span>
          </span>}>
            <option value="overall">{t('th09.boardOverall')}</option>
            {th09CharacterIds.map(id => <option key={id} value={String(id)}>{th09CharacterName(id, t)}</option>)}
          </MainSelect>
        </MainSelectPrefix>
      </div>
      <InputGroup className="rating-search">
        <InputGroupInput type="search" aria-label={t('th09.boardSearch')} placeholder={t('th09.boardSearch')} value={query}
          onChange={event => { setQuery(event.currentTarget.value); setContextScope(null); }} />
        <Button variant="ghost" size="icon-sm" aria-label={t('th09.boardLocate')} title={t('th09.boardLocate')}
          disabled={!search || !found.length || showContext} onClick={() => setContextScope(scope)}>
          <LocateFixed aria-hidden="true" />
        </Button>
      </InputGroup>
      <Popover>
        <PopoverTrigger asChild>
          <Button className="rating-filter-trigger" variant="outline" size="sm" data-filtered={filtered || undefined}
            aria-label={filtered ? t('th09.boardFilterOn') : t('th09.boardFilter')}>
            <SlidersHorizontal aria-hidden="true" />{t('th09.boardFilter')}
          </Button>
        </PopoverTrigger>
        <PopoverContent className="rating-controls" align="end" aria-label={t('th09.boardFilter')}>
          <Field>
            <FieldLabel htmlFor="rating-minimum">{t('th09.boardMinimum')}</FieldLabel>
            <Input id="rating-minimum" type="number" min={1} step={1} value={minimum ?? defaultMinimum}
              onChange={event => { setMinimum(event.currentTarget.value); setContextScope(null); }} />
          </Field>
          {!characterBoard && <Field>
            <FieldLabel htmlFor="rating-group-trigger">{t('th09.boardGroup')}</FieldLabel>
            <MainSelect id="rating-group" aria-label={t('th09.boardGroup')} value={group} onChange={event => { setGroup(event.currentTarget.value); setContextScope(null); }}>
              <option value="all">{t('lobby.all')}</option>
              {groups.map(value => <option key={value} value={String(value)}>{t('th09.boardGroupNumber', {number: value})}</option>)}
            </MainSelect>
          </Field>}
          <Field>
            <FieldLabel htmlFor="rating-status-trigger">{t('th09.boardStatus')}</FieldLabel>
            <MainSelect id="rating-status" aria-label={t('th09.boardStatus')} value={filter}
              onChange={event => { setFilter(event.currentTarget.value as Filter); setContextScope(null); }}>
              {filters.map(value => <option key={value} value={value}>
                {value === 'all' ? t('lobby.all') : t(statusLabels[value])}
              </option>)}
            </MainSelect>
          </Field>
        </PopoverContent>
      </Popover>
    </fieldset>
    {loading && <span className="sr-only" role="status">{t('th09.boardLoading')}</span>}
    {(!rows || visible.length > 0) ? <section className="rating-table-wrap" aria-label={boardTitle} aria-busy={loading}>
      <table className="rating-table" aria-label={character === undefined ? boardTitle : t('th09.boardCharacter', {name: th09CharacterName(character, t)})}>
        <thead><tr>{columns.map(item => <th key={item.key} scope="col" data-column={item.key}
          data-compact={item.compact || undefined} data-numeric={item.numeric || undefined}
          aria-sort={item.key === sort.key ? sort.direction === 1 ? 'ascending' : 'descending' : undefined}>
          <Button variant="ghost" size="sm" className="rating-sort" onClick={() => sortBy(item)}>
            {columnTitle(item)}{sortDirection(item)}
          </Button>
        </th>)}<th className="rating-mobile-cell" scope="col">
          <ActionMenu label={t('th09.boardSort')} trigger={<Button variant="ghost" size="icon" aria-label={t('th09.boardSort')}><ArrowDownUp aria-hidden="true" /></Button>}>
            {columns.map(item => <ActionMenuItem key={item.key} onSelect={() => sortBy(item)}>
              {item.key === 'position' ? t('th09.rank') : columnTitle(item)}{sortDirection(item)}
            </ActionMenuItem>)}
          </ActionMenu>
        </th></tr></thead>
        <tbody>{loading && ['first','second','third','fourth'].map(key => <tr key={key} className="rating-placeholder-row" inert>
          {columns.map(item => <td key={item.key} data-column={item.key} data-compact={item.compact || undefined}
            data-numeric={item.numeric || undefined}><Skeleton /></td>)}
          <td className="rating-mobile-cell" />
        </tr>)}{visible.map(row => {
          const isExpanded = expanded?.scope === scope && expanded.player === row.player;
          const detailId = `rating-detail-${encodeURIComponent(row.player)}`;
          return <Fragment key={row.player}>
            <tr data-located={located === row.player || undefined} ref={located === row.player ? focusRow : null}
              tabIndex={located === row.player ? -1 : undefined}>
              {columns.map(item => item.key === 'player'
                ? <th key={item.key} scope="row" data-compact="true" title={t('th09.boardOpponentsCount', {count: numbers.format(row.opponents)})}>
                    <PlayerIdentity row={row} /><span className="sr-only"> {t('th09.boardOpponentsCount', {count: numbers.format(row.opponents)})}</span>
                  </th>
                : <td key={item.key} data-column={item.key} data-compact={item.compact || undefined}
                    data-numeric={item.numeric || undefined}>{renderRatingCell(item, row)}</td>)}
              <td className="rating-mobile-cell">
                <Button variant="ghost" size="icon" aria-expanded={isExpanded} aria-controls={isExpanded ? detailId : undefined}
                  aria-label={t(isExpanded ? 'th09.boardCollapse' : 'th09.boardExpand', {name: playerName(row)})}
                  onClick={() => setExpanded(isExpanded ? null : {scope, player:row.player})}><ChevronDown aria-hidden="true" /></Button>
              </td>
            </tr>
            {isExpanded && <tr className="rating-mobile-detail" id={detailId}>
              <td colSpan={compactColumnCount + 1}><dl className="rating-detail-grid">
                {detailColumns.map(item => <div key={item.key} data-column={item.key}>
                  <dt>{columnTitle(item)}</dt><dd>{renderRatingCell(item, row)}</dd>
                </div>)}
                <div><dt>{t('th09.boardOpponents')}</dt><dd>{numbers.format(row.opponents)}</dd></div>
              </dl></td>
            </tr>}
          </Fragment>;
        })}</tbody>
      </table>
      {!rows && failed && <div className="rating-table-state">
        <div className="rating-empty" role="alert"><p>{t('th09.boardFailed')}</p><Button variant="outline" size="sm" onClick={retry}>{t('lobby.retry')}</Button></div>
      </div>}
    </section> : <div className="rating-empty">
      <p>{t(rows?.length ? 'th09.boardNoMatches' : 'th09.boardEmpty')}</p>
      {!!rows?.length && <Button variant="outline" size="sm" onClick={clearFilters}>{t('th09.boardClear')}</Button>}
    </div>}
  </div>;
}

function RatingCharacterThumbnail({character}: {character?: number}) {
  return <span className="rating-character-thumbnail" aria-hidden="true">
    {character === undefined ? <ListOrdered /> : <Th09CharacterPortrait character={character} />}
  </span>;
}

function PlayerIdentity({row}: {row: Entry}) {
  const profile = row.profile;
  const {locale} = useLocale();
  if (!profile) return <span className="rating-player"><AccountAvatar />{row.player}</span>;
  const query = new URLSearchParams({lang: locale});
  if (row.characterScore) query.set('character', String(row.characterScore.character));
  return <span className="rating-player">
    <PlayerAvatarCard username={profile.username} name={profile.nickname} avatar={profile.avatar} boardRating={row}>
      <button type="button" className="player-card-avatar-button" aria-label={profile.nickname}><AccountAvatar src={profile.avatar} loading="lazy"/></button>
    </PlayerAvatarCard>
    <Link className="rating-player-name" to={`/u/${encodeURIComponent(profile.username)}?${query}`}
      target="_blank" rel="noreferrer">
      <span className="rating-nickname">{profile.nickname}</span>{' '}<span className="rating-handle">@{profile.username}</span>
    </Link>
  </span>;
}

function RatingOverview({rows, matches, updatedAt, characterBoard}: {rows: Entry[]; matches: Entry[]; updatedAt: number; characterBoard: boolean}) {
  const {t, locale} = useLocale();
  const dates = new Intl.DateTimeFormat(locale, {dateStyle: 'medium', timeStyle: 'short'});
  const counts = {active: 0, provisional: 0, inactive: 0};
  for (const row of rows) counts[row.status] += 1;

  const updated = <time className="rating-overview-date" dateTime={new Date(updatedAt).toISOString()}>
    {t('th09.boardUpdated', {time: dates.format(updatedAt)})}
  </time>;

  return <>
    <div className="rating-overview-heading">
      <p className="rating-overview-total">{t('th09.boardPlayerCount', {count: numbers.format(rows.length)})}</p>
      {!characterBoard && rows.length > 0 && <dl className="rating-overview-counts">
        <div><dt>{t(statusLabels.active)}</dt><dd>{numbers.format(counts.active)}</dd></div>
        <div><dt>{t(statusLabels.provisional)}</dt><dd>{numbers.format(counts.provisional)}</dd></div>
        {counts.inactive > 0 && <div><dt>{t(statusLabels.inactive)}</dt><dd>{numbers.format(counts.inactive)}</dd></div>}
      </dl>}
    </div>
    {rows.length > 0 ? <RatingDistribution rows={rows} matches={matches} updated={updated} /> : updated}
  </>;
}

const binSize = 50;

function RatingDistribution({rows, matches, updated}: {rows: Entry[]; matches: Entry[]; updated: ReactNode}) {
  const {t} = useLocale();
  const binFor = (row: Entry) => Math.floor(boardRating(row) / binSize);
  const start = binFor(rows[rows.length - 1]);
  const end = binFor(rows[0]);
  const bins = Array.from({ length: end - start + 1 }, (_, index) => ({rating: (start + index) * binSize, count: 0, matches: 0}));
  for (const row of rows) bins[binFor(row) - start].count += 1;
  for (const row of matches) bins[binFor(row) - start].matches += 1;
  const peak = Math.max(...bins.map(bin => bin.count));
  const interval = Math.ceil(peak / 4);
  const steps = Math.ceil(peak / interval);
  const ceiling = steps * interval;
  const levels = Array.from({ length: steps + 1 }, (_, index) => index * interval);
  const ticks = [...new Set([start, Math.round((start + end) / 2), end])];

  return <figure className="rating-distribution" aria-label={t('th09.boardDistribution')}>
    <div className="rating-chart" aria-hidden="true">
      <div className="rating-chart-y">{levels.map(value => <span key={value}
        style={{ bottom: `${100 * value / ceiling}%` }}>{value}</span>)}</div>
      <svg aria-hidden="true" viewBox={`0 0 ${bins.length} ${ceiling}`} preserveAspectRatio="none">
        {levels.map(value => <line key={value} x1={0} x2={bins.length} y1={ceiling - value} y2={ceiling - value}
          vectorEffect="non-scaling-stroke" />)}
        {bins.map((bin, index) => <g key={bin.rating}>
          <title>{numbers.format((start + index) * binSize)}–{numbers.format((start + index + 1) * binSize - 1)} | {t('th09.boardPlayerCount', {count: bin.count})}</title>
          <rect x={index + 0.04} y={ceiling - bin.count} width={0.92} height={bin.count} />
          {bin.matches > 0 && <rect className="rating-chart-match" x={index + 0.04} y={ceiling - bin.matches}
            width={0.92} height={bin.matches} />}
        </g>)}
      </svg>
      <div className="rating-chart-x">{ticks.map(value => <span key={value}
        style={{ left: `${100 * (value - start + 0.5) / bins.length}%` }}>{numbers.format(value * binSize)}</span>)}</div>
    </div>
    <figcaption>
      <span>{t('th09.boardBinSize', {points: binSize})}{matches.length > 0 && ` | ${t('th09.boardHighlights', {count: numbers.format(matches.length)})}`}</span>
      {updated}
    </figcaption>
  </figure>;
}
