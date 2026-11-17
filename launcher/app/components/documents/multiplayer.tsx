import { BookOpen, Check, ChevronDown } from 'lucide-react';
import type { ReactNode } from 'react';
import { Link } from 'react-router';
import type { Folder, Item } from 'fumadocs-core/page-tree';
import { Card } from '@fumadocs/base-ui/components/card';
import { Callout } from '@fumadocs/base-ui/components/callout';
import { COOP_GUIDE_GAMES, VERSUS_GUIDE_GAMES, multiplayerGuidePath, multiplayerGuideTopic } from '../../../lib/multiplayer-guide.mjs';
import { withUiLanguage } from '../../../src/launcher/i18n.mts';
import { useLocale, type UiMessageKey } from '../../i18n';
import { Button } from '../../ui/button';
import { ActionMenu, ActionMenuGroup, ActionMenuItem, ActionMenuLabel } from '../../ui/action-menu';

export type GuideTopic = NonNullable<ReturnType<typeof multiplayerGuideTopic>>;

export function guideTitle(topic: GuideTopic): UiMessageKey {
  if (topic === 'start') return 'document.multiplayer.title';
  if (topic === 'coop') return 'document.multiplayer.common';
  return `guide.${topic}`;
}

type Translate = ReturnType<typeof useLocale>['t'];

function guidePages(t: Translate) {
  const page = (topic: GuideTopic): Item => ({
    type: 'page', name: t(guideTitle(topic)), url: multiplayerGuidePath(topic),
  });
  return {
    start: page('start'),
    groups: [
      { label: t('document.multiplayer.coop'), pages: [page('coop'), ...COOP_GUIDE_GAMES.map(page)] },
      { label: t('directory.group.versus'), pages: VERSUS_GUIDE_GAMES.map(page) },
    ],
  };
}

export function multiplayerGuideTree(t: Translate): Folder {
  const {start, groups} = guidePages(t);
  return {type:'folder', name:start.name, index:start, children:groups.map(({label, pages}) => ({
    type:'folder', name:label, children:pages,
  }))};
}

export function MultiplayerGuidePicker({topic}: {topic: GuideTopic}) {
  const {locale, t} = useLocale(), {start, groups} = guidePages(t);
  const current = multiplayerGuidePath(topic);
  const link = (page: Item) => <ActionMenuItem key={page.url} asChild>
    <Link to={withUiLanguage(page.url, locale)} aria-current={current === page.url ? 'page' : undefined}>
      {page.name}{current === page.url && <Check className="ms-auto" aria-hidden="true"/>}
    </Link>
  </ActionMenuItem>;
  return <div className="documentation-guide-picker">
    <ActionMenu variant="selection" label={t('document.multiplayer.navigation')} trigger={<Button variant="outline" size="icon" aria-label={t('document.multiplayer.navigation')}>
      <ChevronDown aria-hidden="true"/>
    </Button>} align="end">
      {link(start)}
      {groups.map(group => <ActionMenuGroup key={group.label}>
        <ActionMenuLabel className="px-2 pt-2 text-xs font-medium text-muted-foreground">{group.label}</ActionMenuLabel>{group.pages.map(link)}
      </ActionMenuGroup>)}
    </ActionMenu>
  </div>;
}

export function MultiplayerStart() {
  const {t} = useLocale();
  return <>
    <ol className="documentation-guide-steps">
      <li>{t('document.multiplayer.create')}</li>
      <li>{t('document.multiplayer.join')}</li>
      <li>{t('document.multiplayer.ready')}</li>
    </ol>
    <p>{t('document.multiplayer.basicFlow')}</p>
    <section>
      <h2>{t('document.multiplayer.coop')}</h2>
      <p>{t('document.multiplayer.coopIntro')}</p>
      <p>{t('document.multiplayer.coopGuide')}</p>
      <Card title={t('document.multiplayer.common')} icon={<BookOpen/>} href={multiplayerGuidePath('coop')}/>
    </section>
    <section>
      <h2>{t('directory.group.versus')}</h2>
      <p>{t('document.multiplayer.versusIntro')}</p>
      <p className="text-fd-muted-foreground">{t('document.multiplayer.versusPending')}</p>
    </section>
  </>;
}

export function MultiplayerOverviewTable() {
  const {locale, t} = useLocale();
  const life = t('document.multiplayer.lifeTerm');
  const lifeLabel = <>{locale === 'zh-CN' ? <ruby>{life}<rt>生命</rt></ruby> : life}{t('document.multiplayer.lifeTransferTail')}</>;
  const rule = (anchor: string, title: ReactNode, description: string) => (
    <li><a href={`#${anchor}`}>{title}</a>{' '}<span>{description}</span></li>
  );
  return <table className="documentation-mechanisms">
    <thead><tr><th scope="col">{t('document.multiplayer.category')}</th><th scope="col">{t('document.multiplayer.mechanism')}</th></tr></thead>
    <tbody>
      <tr><th scope="row">{t('document.multiplayer.mode')}</th><td><ul>
        {rule('challenge-mode', t('room.challengeMode'), t('document.multiplayer.challengeSummary'))}
      </ul></td></tr>
      <tr><th scope="row">{t('document.multiplayer.resources')}</th><td><ul>
        {rule('life-transfer', lifeLabel, t('document.multiplayer.lifeAction'))}
        {rule('item-drops', t('document.multiplayer.powerDropsLabel'), t('document.multiplayer.powerDrops'))}
        {rule('power-transfer', t('document.multiplayer.powerTransferLabel'), t('document.multiplayer.powerTransfer'))}
      </ul></td></tr>
      <tr><th scope="row">{t('document.multiplayer.survival')}</th><td><ul>
        {rule('death-and-revival', t('document.multiplayer.deathLabel'), t('document.multiplayer.deathSummary'))}
      </ul></td></tr>
      <tr><th scope="row">{t('document.multiplayer.combat')}</th><td><ul>
        {rule('deathbomb', t('document.multiplayer.deathbombLabel'), t('document.multiplayer.deathbombSummary'))}
        {rule('boss-damage', t('document.multiplayer.bossLabel'), t('document.multiplayer.bossSummary'))}
      </ul></td></tr>
      <tr><th scope="row">{t('document.multiplayer.play')}</th><td><ul>
        {rule('shared-controls', t('document.multiplayer.controlsLabel'), t('document.multiplayer.controlsSummary'))}
      </ul></td></tr>
      <tr><th scope="row">{t('document.multiplayer.progress')}</th><td><ul>
        {rule('unlocks', t('document.multiplayer.unlockLabel'), t('document.multiplayer.unlockSummary'))}
      </ul></td></tr>
    </tbody>
  </table>;
}

export function MultiplayerExceptions() {
  const {locale, t} = useLocale();
  return <Callout type="info" role="note" title={t('document.multiplayer.exceptions')}>
    <ul>
      {COOP_GUIDE_GAMES.filter(game => game !== 'th06').map(game => <li key={game}>
        <Link to={withUiLanguage(multiplayerGuidePath(game), locale)}>{t(guideTitle(game))}</Link>
        {t(`document.multiplayer.exception.${game}`)}
      </li>)}
    </ul>
  </Callout>;
}

export function MultiplayerScope({topic}: {topic: GuideTopic}) {
  const {locale, t} = useLocale();
  if (topic === 'th09' || topic === 'th19') return <>
    <p>{t('document.multiplayer.versus')}</p>
    <p>{t(`document.multiplayer.${topic}`)}</p>
  </>;
  return <Callout type="info">
    {t('document.multiplayer.differences')}{' '}
    <Link to={withUiLanguage(multiplayerGuidePath('coop'), locale)}>{t('document.multiplayer.common')}</Link>
  </Callout>;
}
