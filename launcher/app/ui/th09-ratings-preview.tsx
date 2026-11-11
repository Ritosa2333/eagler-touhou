import { ArrowDown, ArrowDownUp, ArrowLeft, ArrowUp, ChevronDown, CircleHelp } from 'lucide-react';
import { Fragment, type ReactNode, useEffect, useRef, useState } from 'react';
import { z } from 'zod';
import { th09CharacterLoadout } from '../../src/launcher/th09-room-stats.mjs';
import { AccountAvatar } from '../components/launcher/AccountAvatar';
import { BookThemeToggle } from '../components/launcher/BookThemeToggle';
import { useLocale, type UiMessageKey } from '../i18n';
import { sitePageTitle } from '../navigation/document-locale';
import {accountsEnabled} from '../release-features';
import {accountRequest} from '../services/account-client';
import { ActionMenu, ActionMenuItem } from './action-menu';
import { Badge } from './badge';
import { Button } from './button';
import { Input } from './input';
import { Popover, PopoverContent, PopoverTrigger } from './popover';
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from './select';
import { Skeleton } from './skeleton';
import './th09-ratings-preview.css';

const count = z.number().int().nonnegative();
const timestamp = count.max(8.64e15);
const status = z.enum(['active', 'provisional', 'inactive']);
const boardSchema = z.object({
  as_of: timestamp,
  default_min_games: count,
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
    top_characters: z.array(z.object({ character: count.max(15), games: count.positive() })).max(3),
  })),
});
type Board = z.infer<typeof boardSchema>;
type Filter = z.infer<typeof status> | 'all';
type Row = Board['board'][number] & { position: number };
const statusLabels = { active: '正式', provisional: '暂定', inactive: '不活跃' };
const filters = ['active', 'provisional', 'inactive', 'all'] as const;
const numbers = new Intl.NumberFormat('zh-CN', { maximumFractionDigits: 0 });
const percentages = new Intl.NumberFormat('zh-CN', { style: 'percent', maximumFractionDigits: 1 });
const dates = new Intl.DateTimeFormat('zh-CN', { dateStyle: 'medium', timeStyle: 'short' });
const days = new Intl.DateTimeFormat('zh-CN', { dateStyle: 'medium' });
const winRate = (row: Row) => row.wins / row.games;
const playerName = (row: Row) => row.profile?.nickname ?? row.player;

type Column = {
  key: string;
  title: string;
  compact?: boolean;
  numeric?: boolean;
  value(row: Row): number | string;
  render?(row: Row): ReactNode;
};
const columns: Column[] = [
  { key: 'position', title: '#', compact: true, numeric: true, value: row => row.position },
  { key: 'player', title: '玩家', compact: true, value: playerName },
  { key: 'rating', title: '评分', compact: true, numeric: true, value: row => row.rating },
  { key: 'deviation', title: '偏差', numeric: true, value: row => row.interval_halfwidth,
    render: row => `± ${numbers.format(row.interval_halfwidth)}` },
  { key: 'games', title: '局数', numeric: true, value: row => row.games },
  { key: 'winRate', title: '胜率', numeric: true, value: winRate, render: row => percentages.format(winRate(row)) },
  { key: 'characters', title: '常用机体', numeric: true, value: row => row.top_characters[0]?.games ?? 0,
    render: row => <TopCharacters characters={row.top_characters} /> },
  { key: 'community', title: '交手组', value: row => row.connected_group, render: row => `组 ${row.connected_group}` },
  { key: 'last', title: '最近对局', numeric: true, value: row => row.last_at,
    render: row => <time dateTime={new Date(row.last_at).toISOString()}>{days.format(row.last_at)}</time> },
  { key: 'status', title: '状态', value: row => row.status,
    render: row => <Badge variant="outline">{statusLabels[row.status]}</Badge> },
];
const detailColumns = columns.filter(column => !column.compact);

function renderRatingCell(column: Column, row: Row) {
  return column.render ? column.render(row) : numbers.format(Number(column.value(row)));
}

export default function Th09RatingsPreview() {
  const [board, setBoard] = useState<Board | null>(null);
  const [failed, setFailed] = useState(false);
  const [live, setLive] = useState(false);

  useEffect(() => {
    const previousTitle = document.title;
    document.title = sitePageTitle('th09mp 评分原型');
    const controller = new AbortController();
    let pending = false;
    async function refresh() {
      if (!accountsEnabled || pending || document.hidden) return false;
      pending = true;
      try {
        const next = boardSchema.parse(await accountRequest('v1/th09/ratings/board', undefined, {signal: controller.signal}));
        if (!controller.signal.aborted) {setBoard(next);setLive(true);setFailed(false);}
        return true;
      } catch {return false;}
      finally {pending = false;}
    }
    void refresh().then(current => {
      if (current || controller.signal.aborted) return;
      return fetch('./.cache/th09-ratings.json', { signal: controller.signal, cache: 'no-store' });
    }).then(async response => {
      if (!response) return;
      if (!response.ok) throw new Error('Missing rating snapshot');
      const next = boardSchema.parse(await response.json());
      if (!controller.signal.aborted) setBoard(next);
    }).catch(() => {
      if (!controller.signal.aborted) setFailed(true);
    });
    const timer = accountsEnabled ? window.setInterval(refresh, 12_000) : undefined;
    return () => {
      if (timer !== undefined) window.clearInterval(timer);
      controller.abort();
      document.title = previousTitle;
    };
  }, []);

  return <div className="ui-review" lang="zh-CN">
    <a className="catalog-skip" href="#rating-main">跳到正文</a>
    <header className="ui-review-header">
      <a href="./ui-review.html"><ArrowLeft aria-hidden="true" />UI 手册</a>
      <BookThemeToggle />
    </header>
    <main className="rating-preview" id="rating-main">
      <div className="rating-heading">
        <h1>th09mp 评分榜</h1>
        <Badge variant="outline">{live ? '实时预览' : '本地原型'}</Badge>
        <Popover>
          <PopoverTrigger asChild>
            <Button variant="ghost" size="icon-sm" aria-label="评分与定级说明"><CircleHelp aria-hidden="true" /></Button>
          </PopoverTrigger>
          <PopoverContent className="rating-help" aria-label="评分与定级说明">
            <p>± 是约 95% 的评分区间。区间收窄到 ±220 以内即转正。</p>
            <p>交手组按实际对战连接划分，跨组名次参考性较弱。</p>
          </PopoverContent>
        </Popover>
      </div>
      {failed ? <p role="alert">榜单快照读取失败。生成本地榜单后刷新此页。</p>
        : board ? <RatingBoard snapshot={board} />
        : <Skeleton className="h-44 w-full" role="status" aria-label="正在读取榜单" />}
    </main>
  </div>;
}

function RatingBoard({snapshot}: {snapshot: Board}) {
  const [filter, setFilter] = useState<Filter>('active');
  const [group, setGroup] = useState('all');
  const [minimum, setMinimum] = useState(String(snapshot.default_min_games));
  const [query, setQuery] = useState('');
  const [showContext, setShowContext] = useState(false);
  const [sort, setSort] = useState({ key: 'rating', direction: -1 });
  const [expanded, setExpanded] = useState<string | null>(null);
  const focusRow = useRef<HTMLTableRowElement>(null);
  const groups = [...new Set(snapshot.board.map(row => row.connected_group))].sort((a, b) => a - b);
  const minGames = Math.max(1, Math.trunc(Number(minimum)) || 1);
  const search = query.trim().toLowerCase();
  const scoped = snapshot.board.filter(row => row.games >= minGames && (group === 'all' || row.connected_group === Number(group)));
  const counts = { active: 0, provisional: 0, inactive: 0 };
  for (const row of scoped) counts[row.status] += 1;
  const pool = scoped.filter(row => filter === 'all' || row.status === filter)
    .map((row, index) => ({ ...row, position: index + 1 }));
  const found = search ? pool.filter(row => playerName(row).toLowerCase().includes(search)
    || (row.profile && `@${row.profile.username.toLowerCase()}`.includes(search))) : pool;
  const located = showContext ? found[0]?.player : undefined;
  const column = columns.find(item => item.key === sort.key) ?? columns[2];
  const visible = [...(showContext ? pool : found)].sort((a, b) => {
    const left = column.value(a), right = column.value(b);
    const comparison = typeof left === 'number' && typeof right === 'number'
      ? left - right : String(left).localeCompare(String(right), 'zh-CN');
    return sort.direction * comparison || a.position - b.position;
  });

  useEffect(() => {
    if (!located) return;
    focusRow.current?.focus({ preventScroll: true });
    focusRow.current?.scrollIntoView({ block: 'nearest', inline: 'nearest' });
  }, [located]);

  function sortBy(next: Column) {
    setSort(current => ({ key: next.key, direction: current.key === next.key ? -current.direction
      : next.numeric && next.key !== 'position' ? -1 : 1 }));
  }

  function clearFilters() {
    setQuery('');
    setMinimum('1');
    setFilter('all');
    setGroup('all');
    setShowContext(false);
  }

  function sortDirection(item: Column) {
    if (sort.key !== item.key) return null;
    return sort.direction === 1 ? <ArrowUp aria-hidden="true" /> : <ArrowDown aria-hidden="true" />;
  }

  return <>
    <div className="rating-controls">
      <label className="rating-field"><span>最少局数</span>
        <Input type="number" min={1} step={1} value={minimum}
          onChange={event => { setMinimum(event.currentTarget.value); setShowContext(false); }} />
      </label>
      <div className="rating-field"><span id="rating-group-label">交手组</span>
        <Select value={group} onValueChange={value => { setGroup(value); setShowContext(false); }}>
          <SelectTrigger aria-labelledby="rating-group-label"><SelectValue /></SelectTrigger>
          <SelectContent><SelectItem value="all">全部</SelectItem>
            {groups.map(value => <SelectItem key={value} value={String(value)}>组 {value}</SelectItem>)}
          </SelectContent>
        </Select>
      </div>
      <div className="rating-field"><span id="rating-status-label">显示</span>
        <Select value={filter} onValueChange={value => { setFilter(value as Filter); setShowContext(false); }}>
          <SelectTrigger aria-labelledby="rating-status-label"><SelectValue /></SelectTrigger>
          <SelectContent>{filters.map(value => <SelectItem key={value} value={value}>
            {value === 'all' ? '全部' : statusLabels[value]}
          </SelectItem>)}</SelectContent>
        </Select>
      </div>
      <div className="rating-field rating-search">
        <label htmlFor="rating-search">查找玩家</label>
        <div className="rating-search-actions">
          <Input id="rating-search" type="search" value={query}
            onChange={event => { setQuery(event.currentTarget.value); setShowContext(false); }} />
          <Button variant="outline" size="sm" disabled={!search || !found.length || showContext}
            onClick={() => setShowContext(true)}>在全榜定位</Button>
        </div>
      </div>
    </div>
    <p className="rating-status-line" role="status">
      <time dateTime={new Date(snapshot.as_of).toISOString()}>{dates.format(snapshot.as_of)}</time>
      {' | '}正式 {numbers.format(counts.active)} | 暂定 {numbers.format(counts.provisional)} | 不活跃 {numbers.format(counts.inactive)}
    </p>
    {pool.length > 0 && <RatingDistribution rows={pool} matches={search ? found : []} />}
    {visible.length ? <section className="rating-table-wrap" aria-label="评分榜表格">
      <table className="rating-table" aria-label="th09mp 评分榜">
        <thead><tr>{columns.map(item => <th key={item.key} scope="col" data-column={item.key}
          data-compact={item.compact || undefined} data-numeric={item.numeric && item.key !== 'characters'}
          aria-sort={item.key === sort.key ? sort.direction === 1 ? 'ascending' : 'descending' : undefined}>
          <Button variant="ghost" size="sm" className="rating-sort" onClick={() => sortBy(item)}>
            {item.title}{sortDirection(item)}
          </Button>
        </th>)}<th className="rating-mobile-cell" scope="col">
          <ActionMenu label="排序" trigger={<Button variant="ghost" size="icon" aria-label="排序"><ArrowDownUp aria-hidden="true" /></Button>}>
            {columns.map(item => <ActionMenuItem key={item.key} onSelect={() => sortBy(item)}>
              {item.key === 'position' ? '名次' : item.title}{sortDirection(item)}
            </ActionMenuItem>)}
          </ActionMenu>
        </th></tr></thead>
        <tbody>{visible.map(row => {
          const isExpanded = expanded === row.player;
          const detailId = `rating-detail-${encodeURIComponent(row.player)}`;
          return <Fragment key={row.player}>
            <tr data-located={located === row.player || undefined} ref={located === row.player ? focusRow : null}
              tabIndex={located === row.player ? -1 : undefined}>
              {columns.map(item => item.key === 'player'
                ? <th key={item.key} scope="row" data-compact="true" title={`不同对手 ${numbers.format(row.opponents)}`}>
                    <PlayerIdentity row={row} /><span className="sr-only">，不同对手 {numbers.format(row.opponents)}</span>
                  </th>
                : <td key={item.key} data-column={item.key} data-compact={item.compact || undefined}
                    data-numeric={item.numeric && item.key !== 'characters'}>{renderRatingCell(item, row)}</td>)}
              <td className="rating-mobile-cell">
                <Button variant="ghost" size="icon" aria-expanded={isExpanded} aria-controls={isExpanded ? detailId : undefined}
                  aria-label={`${isExpanded ? '收起' : '展开'}${playerName(row)}的战绩`}
                  onClick={() => setExpanded(isExpanded ? null : row.player)}><ChevronDown aria-hidden="true" /></Button>
              </td>
            </tr>
            {isExpanded && <tr className="rating-mobile-detail" id={detailId}>
              <td colSpan={4}><dl className="rating-detail-grid">
                {detailColumns.map(item => <div key={item.key} data-column={item.key}>
                  <dt>{item.title}</dt><dd>{renderRatingCell(item, row)}</dd>
                </div>)}
                <div><dt>不同对手</dt><dd>{numbers.format(row.opponents)}</dd></div>
              </dl></td>
            </tr>}
          </Fragment>;
        })}</tbody>
      </table>
    </section> : <div className="rating-empty">
      <p>{snapshot.board.length ? '没有符合条件的玩家。' : '暂无战绩。'}</p>
      {snapshot.board.length > 0 && <Button variant="outline" size="sm" onClick={clearFilters}>清除筛选</Button>}
    </div>}
  </>;
}

function PlayerIdentity({row}: {row: Row}) {
  const profile = row.profile;
  if (!profile) return <span className="rating-player"><AccountAvatar />{row.player}</span>;
  return <a className="rating-player" href={'https://touhouonline.com/u/' + encodeURIComponent(profile.username)}
    target="_blank" rel="noreferrer">
    <AccountAvatar src={profile.avatar} /><span className="rating-player-name">
      <span className="rating-nickname">{profile.nickname}</span>{' '}<span className="rating-handle">@{profile.username}</span>
    </span>
  </a>;
}

function TopCharacters({characters}: {characters: Row['top_characters']}) {
  const {t} = useLocale();
  if (!characters.length) return <span className="rating-no-characters"><span className="sr-only">暂无机体记录</span><span aria-hidden="true">—</span></span>;
  return <span className="rating-characters">{characters.map(({character, games}, index) => {
    const loadout = th09CharacterLoadout(character);
    const name = loadout ? t(loadout.labelKey as UiMessageKey) : t('profile.loadoutNumber', {number: character + 1});
    const hasNext = index < characters.length - 1;
    return <Fragment key={character}>
      <span>{name} <span>({numbers.format(games)})</span>{hasNext && ','}</span>{hasNext && ' '}
    </Fragment>;
  })}</span>;
}

const binSize = 50;

function RatingDistribution({rows, matches}: {rows: Row[]; matches: Row[]}) {
  const binFor = (row: Row) => Math.floor(row.rating / binSize);
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

  return <figure className="rating-distribution" aria-label="评分分布">
    <div className="rating-chart" aria-hidden="true">
      <div className="rating-chart-y">{levels.map(value => <span key={value}
        style={{ bottom: `${100 * value / ceiling}%` }}>{value}</span>)}</div>
      <svg aria-hidden="true" viewBox={`0 0 ${bins.length} ${ceiling}`} preserveAspectRatio="none">
        {levels.map(value => <line key={value} x1={0} x2={bins.length} y1={ceiling - value} y2={ceiling - value}
          vectorEffect="non-scaling-stroke" />)}
        {bins.map((bin, index) => <g key={bin.rating}>
          <title>{numbers.format((start + index) * binSize)}–{numbers.format((start + index + 1) * binSize - 1)} 分 | {bin.count} 人</title>
          <rect x={index + 0.04} y={ceiling - bin.count} width={0.92} height={bin.count} />
          {bin.matches > 0 && <rect className="rating-chart-match" x={index + 0.04} y={ceiling - bin.matches}
            width={0.92} height={bin.matches} />}
        </g>)}
      </svg>
      <div className="rating-chart-x">{ticks.map(value => <span key={value}
        style={{ left: `${100 * (value - start + 0.5) / bins.length}%` }}>{numbers.format(value * binSize)}</span>)}</div>
    </div>
    <figcaption>{numbers.format(rows.length)} 人 | 每 {binSize} 分一档
      {matches.length > 0 && ` | 高亮 ${numbers.format(matches.length)} 个搜索结果`}
    </figcaption>
  </figure>;
}
