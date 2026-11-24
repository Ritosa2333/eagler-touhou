import {useId, useMemo, useState, type CSSProperties} from 'react';
import {Tooltip} from 'radix-ui';
import {useLocale} from '../../i18n';
import {useCompactNavigation} from '../../navigation/navigation-motion';
import {Popover, PopoverContent, PopoverTrigger} from '../../ui/popover';
import {ChoiceGroup, ChoiceItem} from '../../ui/choice-group';
import {useContentMotion} from '../../ui/use-surface-motion';
import type {SaveProgress, SpellArchive, SpellCardCapture} from '../../services/cloud-saves';
import {th06SpellStages} from './th06-spell-layout';
import {th07SpellStages} from './th07-spell-layout';
import '@fontsource-variable/noto-sans-jp';
import '../../ui/menu-motion.css';
import './spell-card-archive.css';

type Spell = {number: number; name: string; attempts: number; captures: number; midboss: boolean};

function spellPaint(spell: Spell) {
  const ratio = spell.attempts ? spell.captures / spell.attempts : 0;
  return {
    'data-capture': spell.captures ? 'captured' : spell.attempts ? 'attempted' : 'unknown',
    'data-capture-bright': ratio >= .5 || undefined,
    style: {'--capture-intensity': `${28 + 72 * ratio}%`} as CSSProperties,
  };
}
const spellNumber = (number: number) => 'No.' + String(number).padStart(2, '0');
const captureFraction = ({attempts, captures}: Spell) => captures + '/' + attempts;

function StageProgress({label, spells}: {label: string; spells: Spell[]}) {
  const captured = spells.filter(spell => spell.captures > 0).length;
  return <span className="spell-stage-progress">
    <meter aria-label={label} min={0} max={spells.length} value={captured}/><small>{captured}/{spells.length}</small>
  </span>;
}

function SpellTile({spell, compact}: {spell: Spell; compact: boolean}) {
  const description = spellNumber(spell.number) + ' ' + spell.name + ' ' + captureFraction(spell);
  const paint = spellPaint(spell);
  const trigger = <button type="button" className="spell-tile" aria-label={description} {...paint}>
    <span aria-hidden="true">{spell.attempts ? captureFraction(spell) : ''}</span>
  </button>;
  const details = <>
    <span className="spell-number">{spellNumber(spell.number)}</span>
    <span lang="ja">{spell.name}</span>
    <strong>{captureFraction(spell)}</strong>
  </>;
  if (compact) return <Popover>
    <PopoverTrigger asChild>{trigger}</PopoverTrigger>
    <PopoverContent className="spell-tooltip ui-hint-motion" aria-label={description}
      onOpenAutoFocus={event => event.preventDefault()} {...paint}>{details}</PopoverContent>
  </Popover>;
  return <Tooltip.Root>
    <Tooltip.Trigger asChild>{trigger}</Tooltip.Trigger>
    <Tooltip.Portal><Tooltip.Content className="spell-tooltip ui-hint-motion" sideOffset={6} collisionPadding={12} {...paint}>
      {details}
    </Tooltip.Content></Tooltip.Portal>
  </Tooltip.Root>;
}

export function SpellCardArchive({game, cards, archive, selection, label}: {
  game: 'th06' | 'th07'; cards?: SpellCardCapture[]; archive?: SpellArchive; selection: SaveProgress; label: string;
}) {
  const {locale, t} = useLocale();
  const compact = useCompactNavigation();
  const id = useId();
  const [view, setView] = useState<'map' | 'list'>('map');
  const records = useMemo(() => new Map(cards?.map(card => [card.number,card])), [cards]);
  const archiveRecords = useMemo(() => new Map(archive?.cards.map(card => [card.number,card])), [archive]);
  const loaded = !!(cards || archive);
  const totals = archive?.scope === 'total';
  const shot = selection.character * 2 + (selection.shot ?? 0);
  const archiveMotion = useContentMotion<HTMLDivElement>(loaded ? view + ':' + shot + ':' + selection.difficulty : null);
  const formatRate = new Intl.NumberFormat(locale, {style: 'percent', maximumFractionDigits: 1});
  const layout = game === 'th06' ? th06SpellStages : th07SpellStages;
  const stages = layout(selection.difficulty).map(({stage, cards}) => ({
    stage, label: stage >= 7 ? (stage === 7 ? 'Extra' : 'Phantasm') : t('game.spellStage', {stage}),
    spells: cards.map(({number, nameLength, midboss}): Spell => {
      const legacy = records.get(number), card = archiveRecords.get(number);
      const tally = totals ? card?.total : card?.loadouts?.find(row =>
        row.character === selection.character && row.shot === selection.shot && row.difficulty === selection.difficulty);
      const attempts = tally?.attempts ?? legacy?.attempts[shot] ?? 0;
      const captures = Math.min(tally?.captures ?? legacy?.captures[shot] ?? 0, attempts);
      const name = card?.name ?? legacy?.name;
      return {number, midboss, name: attempts && name ? name : '？'.repeat(nameLength), attempts, captures};
    }),
  }));
  return <section className="spell-archive" aria-labelledby={id}>
    <header><h2 id={id}>{t('game.spellCards')}</h2><small>{totals ? t('game.spellAllLoadouts') : label}</small>
      {loaded && <ChoiceGroup className="spell-view-switch" type="single" value={view} aria-label={t('game.spellCards')} onValueChange={next => {
        if (next === 'map' || next === 'list') setView(next);
      }}><ChoiceItem value="map">{t('game.spellMap')}</ChoiceItem><ChoiceItem value="list">{t('game.spellList')}</ChoiceItem></ChoiceGroup>}
    </header>
    {!loaded ? <p>{t('game.noSpellStats')}</p> : view === 'map' ?
      <Tooltip.Provider delayDuration={170}>
        <div ref={archiveMotion} className="spell-map-scroll">
          <table className="spell-map">
            <thead><tr><th>{t('game.spellStageColumn')}</th><th lang="en">Midboss</th><th lang="en">Boss</th><th>{t('game.spellCaptured')}</th></tr></thead>
            <tbody>{stages.map(({stage, label, spells}) => <tr key={stage} data-stage={stage}>
                <th scope="row">{label}</th>
                {[true,false].map(midboss => {
                  const group = spells.filter(spell => spell.midboss === midboss);
                  return <td key={String(midboss)}><div className="spell-tiles">{group.length ?
                    group.map(spell => <SpellTile key={spell.number} spell={spell} compact={compact}/>) : <span className="spell-empty-group">—</span>}</div></td>;
                })}
                <td><StageProgress label={label + ' | ' + t('game.spellCaptured')} spells={spells}/></td>
              </tr>)}</tbody>
          </table>
          <div className="spell-map-legend">
            <span><span className="spell-legend-scale" aria-hidden="true">{[24, 62, 100].map(level =>
              <i key={level} className="spell-tile" data-capture="captured" style={{'--capture-intensity': level + '%'} as CSSProperties}/>)}</span>{t('game.spellCaptured')}</span>
            <span><i className="spell-tile" data-capture="attempted" aria-hidden="true"/>{t('game.spellAttempted')}</span>
            <span><i className="spell-tile" data-capture="unknown" aria-hidden="true"/>{t('game.spellUnknown')}</span>
          </div>
        </div>
      </Tooltip.Provider> :
      <div ref={archiveMotion} className="spell-list">
        <div className="spell-list-column-head" aria-hidden="true">
          <span>{t('game.spellName')}</span><span>{t('game.spellRate')}</span><span>{t('game.spellFraction')}</span>
        </div>
        {stages.map(({stage, label, spells}) => <section key={stage}>
        <header><h3>{label}</h3><StageProgress label={label + ' | ' + t('game.spellCaptured')} spells={spells}/></header>
        <div className="spell-list-groups">{[true,false].map(midboss => {
          const group = spells.filter(spell => spell.midboss === midboss);
          return group.length > 0 && <div className="spell-list-group" key={String(midboss)}>
            <h4 lang="en">{midboss ? 'Midboss' : 'Boss'}</h4>
            <ol>{group.map(spell => <li key={spell.number} {...spellPaint(spell)}>
              <span className="spell-number">{spellNumber(spell.number)}</span><span lang="ja">{spell.name}</span>
              <span className="spell-list-rate">{spell.attempts ? formatRate.format(spell.captures / spell.attempts) : '—'}</span>
              <span>{spell.attempts ? captureFraction(spell) : '—'}</span>
            </li>)}</ol>
          </div>;
        })}</div>
      </section>)}</div>}
  </section>;
}
