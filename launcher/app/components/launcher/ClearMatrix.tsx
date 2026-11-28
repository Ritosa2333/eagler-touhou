import {useState, type CSSProperties, type ReactNode} from 'react';
import {Check, Dot} from 'lucide-react';
import {PRODUCT_GAMES, multiplayerConfigForProduct, multiplayerProductIdForGame, type GameId} from '../../../src/contracts/product-catalog.mts';
import {type SaveProgress} from '../../services/cloud-saves';
import {Button} from '../../ui/button';
import {ChoiceGroup, ChoiceItem} from '../../ui/choice-group';
import {Popover, PopoverContent, PopoverTrigger} from '../../ui/popover';
import {useLocale, type Translate, type UiMessageKey} from '../../i18n';
import {bookGameArtwork} from './book-artwork';
import '../../ui/menu-motion.css';
import './clear-matrix.css';

const difficulties = ['easy','normal','hard','lunatic','extra','phantasm','overdrive'] as const;
const difficultyName = {easy:'Easy',normal:'Normal',hard:'Hard',lunatic:'Lunatic',extra:'Extra',phantasm:'Phantasm',overdrive:'Overdrive'};
const statusLabels = {
  'one-credit': 'profile.clear.oneCredit', cleared: 'profile.clear.cleared', uncleared: 'profile.clear.uncleared',
  'not-attempted': 'profile.clear.notAttempted',
  unavailable: 'profile.clear.unavailable', unread: 'profile.clear.unread',
} as const;

function clearStatus(items: readonly SaveProgress[]): keyof typeof statusLabels {
  if (items.some(item => item.oneCreditClear)) return 'one-credit';
  if (items.some(item => item.cleared)) return 'cleared';
  if (items.some(item => item.cleared === false && (item.playCount !== 0 || (item.bestScore ?? 0) > 0))) return 'uncleared';
  if (items.some(item => item.cleared === false && item.playCount === 0)) return 'not-attempted';
  return items.length ? 'unavailable' : 'unread';
}

export type MatrixWork = {game: GameId; progress: readonly SaveProgress[]};
type MatrixProps = {works: readonly MatrixWork[]; onSelectWork(game: GameId, trigger: HTMLButtonElement): void} |
  {work: MatrixWork; readOnly?: boolean; renderDetails?(selection: SaveProgress, label: string): ReactNode};

const withoutExtra: ReadonlySet<GameId> = new Set(['th01', 'th03', 'th075', 'th19']);
const letterShots: ReadonlySet<GameId> = new Set(['th04', 'th06', 'th07', 'th10', 'th11', 'th12', 'th14']);
const th08Teams = ['profile.team.barrier','profile.team.magic','profile.team.scarlet','profile.team.ghost'] as const;

function hasDifficulty(game: GameId, difficulty: SaveProgress['difficulty']) {
  if (difficulty === 'extra') return !withoutExtra.has(game);
  if (difficulty === 'phantasm') return game === 'th07';
  if (difficulty === 'overdrive') return game === 'th20';
  return true;
}

function gameLoadouts(game: GameId) {
  const product = multiplayerProductIdForGame(game);
  return product ? multiplayerConfigForProduct(product)?.loadouts ?? [] : [];
}

function groupProgress(game: GameId, progress: readonly SaveProgress[]) {
  const groups = new Map<number, Map<number | null, SaveProgress[]>>();
  const records = (character: number, shot: number | null) => {
    const shots = groups.get(character) ?? new Map<number | null, SaveProgress[]>();
    groups.set(character, shots);
    const items = shots.get(shot) ?? [];
    shots.set(shot, items);
    return items;
  };
  // Room configuration includes fighting-game decks and other options absent from score files.
  const roster = game === 'th03' ? [] : gameLoadouts(game);
  const countByCharacter = new Map<number, number>();
  for (const {character} of roster) countByCharacter.set(character, (countByCharacter.get(character) ?? 0) + 1);
  const characterOnly = new Set<number>();
  for (const {character, shot} of roster) {
    if (game === 'th075' || countByCharacter.get(character) === 1 && shot === 0) characterOnly.add(character);
    records(character, characterOnly.has(character) ? null : shot);
  }
  for (const item of progress) {
    const shot = item.shot === 0 && characterOnly.has(item.character) ? null : item.shot;
    records(item.character, shot).push(item);
  }
  return [...groups].sort(([left], [right]) => left - right);
}

function loadoutLabel(game: GameId, character: number, shot: number | null, t: Translate) {
  const option = gameLoadouts(game).find(item => item.character === character && item.shot === (shot ?? 0));
  if (option) {
    if (shot === null && (game === 'th075' || game === 'th17') && option.selection?.[0]) {
      return t(option.selection[0].labelKey as UiMessageKey);
    }
    const team = game === 'th08' ? th08Teams[character] : undefined;
    return t(team ?? option.labelKey as UiMessageKey);
  }
  const name = t('profile.loadoutNumber', {number: character + 1});
  return shot === null ? name : name + ' ' + String.fromCharCode(65 + shot);
}

function MatrixCell({difficulty, records, active, onSelect, showUnread}: {
  difficulty: SaveProgress['difficulty'];
  records: readonly SaveProgress[]; active?: boolean; onSelect?: () => void; showUnread?: boolean;
}) {
  const {t, locale} = useLocale();
  const status = clearStatus(records), label = t(statusLabels[status]);
  const icon = status === 'one-credit' ? <Check aria-hidden="true"/> :
    status === 'cleared' || status === 'uncleared' ? <Dot aria-hidden="true"/> : null;
  if (!onSelect || !records.length) return <td data-difficulty={difficulty}>{status === 'unread' && !showUnread ? null :
    <span className="clear-mark" data-status={status} role="img" aria-label={label} title={label}>{icon}</span>}</td>;
  const score = records[0]?.bestScore;
  const bestScore = score == null ? undefined : score.toLocaleString(locale);
  const detailLabel = bestScore === undefined ? label : `${label} | ${t('game.bestScore')} ${bestScore}`;
  const cell = <Button variant="ghost" className="clear-mark clear-mark-button"
    data-status={status} aria-label={detailLabel} aria-pressed={active} title={detailLabel} onClick={onSelect}>{icon}</Button>;
  return <td data-difficulty={difficulty}>{bestScore === undefined ? cell : <Popover>
    <PopoverTrigger asChild>{cell}</PopoverTrigger>
    <PopoverContent className="clear-score-popup ui-hint-motion" aria-label={t('game.bestScore')}
      onOpenAutoFocus={event => event.preventDefault()}>{t('game.bestScore')}<strong>{bestScore}</strong></PopoverContent>
  </Popover>}</td>;
}

export function ClearMatrix(props: MatrixProps) {
  const {t} = useLocale();
  const work = 'work' in props ? props.work : null;
  const interactive = 'work' in props && !props.readOnly;
  const renderDetails = 'renderDetails' in props ? props.renderDetails : undefined;
  const [mode, setMode] = useState<'legacy' | 'pointdevice'>('legacy');
  const [selection, setSelected] = useState<SaveProgress>();
  const works = 'works' in props ? props.works : [];
  const chooseWork = 'onSelectWork' in props ? props.onSelectWork : undefined;
  const progress = work?.progress.filter(item => !item.mode || item.mode === mode) ?? [];
  const selected = !interactive ? undefined : selection && progress.includes(selection) ? selection :
    progress.reduce<SaveProgress | undefined>((best, item) =>
      !best || (item.playCount ?? 0) > (best.playCount ?? 0) ? item : best, undefined);
  const options = difficulties.filter(difficulty => work
    ? hasDifficulty(work.game, difficulty) || progress.some(item => item.difficulty === difficulty)
    : works.some(entry => entry.progress.some(item => item.difficulty === difficulty)));
  const workRows = works.map(entry => ({
    ...entry,
    cells: options.map(difficulty => entry.progress.filter(item => item.difficulty === difficulty)),
  }));
  const showUnattempted = work || workRows.some(row => row.cells.some(cell => clearStatus(cell) === 'not-attempted'));
  const modes = work?.progress.some(item => item.mode === 'pointdevice');
  const groups = work ? groupProgress(work.game, progress) : [];
  return <>
    {modes && <ChoiceGroup type="single" value={mode} onValueChange={value => {
      if (value === 'legacy' || value === 'pointdevice') {setMode(value); setSelected(undefined);}
    }} aria-label={t('game.matrixLoadout')} className="clear-matrix-modes">
      <ChoiceItem value="legacy">{t('profile.mode.legacy')}</ChoiceItem>
      <ChoiceItem value="pointdevice">{t('profile.mode.pointdevice')}</ChoiceItem>
    </ChoiceGroup>}
    <div className="clear-matrix" data-loadouts={!!work} data-clear-game={work?.game}>
      {/* biome-ignore lint/a11y/noNoninteractiveTabindex: Keyboard users can scroll the matrix on narrow screens. */}
      <section className="clear-matrix-scroll" aria-label={t('game.clearProgress')} tabIndex={0}>
        <table>
          <thead><tr><th scope="col">{t(work ? 'game.matrixLoadout' : 'game.matrixWork')}</th>
            {options.map(difficulty => <th key={difficulty} scope="col" data-difficulty={difficulty}>{difficultyName[difficulty]}</th>)}
          </tr></thead>
          <tbody>
            {workRows.map(entry => {
              const title = PRODUCT_GAMES[entry.game].title;
              const cover = bookGameArtwork(entry.game);
              const label = <>{cover && <img src={cover} alt="" loading="lazy"/>}<span title={title}>{title}</span></>;
              return <tr key={entry.game} data-unread={!entry.progress.length}>
                <th scope="row">{entry.progress.length ? <Button variant="ghost" className="clear-work-label" onClick={event => chooseWork?.(entry.game, event.currentTarget)}
                  aria-label={t('profile.clear.openGame', {game: title})}>{label}</Button> : <span className="clear-work-label">{label}</span>}</th>
                {entry.progress.length
                  ? entry.cells.map((records, index) => <MatrixCell key={options[index]} difficulty={options[index]} records={records}/>)
                  : <td className="clear-unread-state" colSpan={options.length}>{t('profile.clear.unread')}</td>}
              </tr>;
            })}
            {work && groups.length === 0 && <tr>
              <th scope="row">{t('profile.clear.unread')}</th>
              {options.map(difficulty => <MatrixCell key={difficulty} difficulty={difficulty} records={[]} showUnread/>)}
            </tr>}
            {work && groups.map(([character, shots]) => {
              const grouped = shots.size > 1 && letterShots.has(work.game);
              const entries = [...shots].sort(([left], [right]) => (left ?? -1) - (right ?? -1));
              const name = loadoutLabel(work.game, character, entries[0]?.[0] ?? null, t).replace(grouped ? /\s*[ABC]$/u : /$^/u, '');
              return entries.map(([shot, records], index) => <tr key={character + ':' + shot}
                data-character={character} data-shot={shot ?? 'none'} data-group-start={index === 0}>
                <th scope="row"><div className="clear-loadout-label" data-grouped={grouped}
                  title={grouped ? name : loadoutLabel(work.game, character, shot, t)}>
                  <span className={grouped && index === 0 ? 'clear-loadout-name' : undefined}
                    style={grouped && index === 0 ? {'--clear-branch-offset': (entries.length - 1) / 2} as CSSProperties : undefined}>
                    {grouped && index !== 0 ? <span className="sr-only">{name}</span> :
                      <span className="clear-loadout-text">{grouped ? name : loadoutLabel(work.game, character, shot, t)}</span>}
                  </span>
                  {grouped && <><i data-branch={index === 0 ? 'first' : index === entries.length - 1 ? 'last' : 'middle'}/>
                    <small>{shot === null ? '—' : String.fromCharCode(65 + shot)}</small></>}
                </div></th>
                {options.map(difficulty => {
                  const item = records.find(record => record.difficulty === difficulty);
                  return <MatrixCell key={difficulty} difficulty={difficulty} records={item ? [item] : []} active={!!item && selected === item} showUnread
                    onSelect={item && interactive && (renderDetails || item.bestScore !== null) ? () => setSelected(item) : undefined}/>;
                })}
              </tr>);
            })}
          </tbody>
        </table>
      </section>
      <div className="clear-matrix-legend">{Object.entries(statusLabels)
        .filter(([status]) => (work || status !== 'unread') && (showUnattempted || status !== 'not-attempted'))
        .map(([status, label]) =>
        <span key={status}><i className="clear-mark" data-status={status} aria-hidden="true"/>{t(label)}</span>)}</div>
    </div>
    {work && selected && renderDetails?.(selected,
      loadoutLabel(work.game, selected.character, selected.shot, t) + ' | ' + difficultyName[selected.difficulty])}
  </>;
}
