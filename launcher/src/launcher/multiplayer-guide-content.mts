import { translateUi, type UiLocale } from './i18n.mts';

/** Shared original authored-guide transformation and local tab interactions.
 * Requires already-sanitized content; owns no fetch, dialog, history or React state. */
interface AuthoredGuideGroup {
  key: string;
  nodes: HTMLElement[];
}

const GUIDE_GAMES = Object.freeze([
  Object.freeze({ id: 'th06', short: 'TH06', titleKey: 'guide.th06' }),
  Object.freeze({ id: 'th07', short: 'TH07', titleKey: 'guide.th07' }),
  Object.freeze({ id: 'th08', short: 'TH08', titleKey: 'guide.th08' }),
  Object.freeze({ id: 'th09', short: 'TH09', titleKey: 'guide.th09' }),
  Object.freeze({ id: 'th10', short: 'TH10', titleKey: 'guide.th10' }),
  Object.freeze({ id: 'th11', short: 'TH11', titleKey: 'guide.th11' }),
  Object.freeze({ id: 'th15', short: 'TH15', titleKey: 'guide.th15' }),
] as const);

export const DEFAULT_GUIDE_GAME = 'th07';

function normalizedGroupHeading(value: string): string {
  return value.replace(/\s+/g, ' ').trim();
}

function collectAuthoredGroups(target: HTMLElement): Map<string, AuthoredGuideGroup> {
  const groups = new Map<string, AuthoredGuideGroup>();
  let current: AuthoredGuideGroup | null = null;
  for (const child of Array.from(target.children) as HTMLElement[]) {
    if (child.tagName === 'H2') {
      const key = normalizedGroupHeading(child.textContent || '');
      current = { key, nodes: [] };
      groups.set(key, current);
    } else if (current) current.nodes.push(child);
  }
  return groups;
}

function appendAuthoredGroup(
  panel: HTMLElement,
  documentObj: Document,
  group: AuthoredGuideGroup | undefined,
  skipHeading = '',
): void {
  if (!group) return;
  let section: HTMLElement | null = null;
  let flattenSubheadings = false;
  for (const source of group.nodes) {
    const headingText = normalizedGroupHeading(source.textContent || '');
    if (source.tagName === 'H3') {
      if (headingText === skipHeading) {
        section = null;
        flattenSubheadings = true;
        continue;
      }
      section = documentObj.createElement('section');
      section.className = 'multiplayer-rule-section';
      const heading = documentObj.createElement('h2');
      heading.textContent = source.textContent || '';
      section.append(heading);
      panel.append(section);
      flattenSubheadings = false;
      continue;
    }
    if (source.tagName === 'H4') {
      if (section && !flattenSubheadings) {
        const heading = documentObj.createElement('h3');
        heading.className = 'multiplayer-rule-subheading';
        heading.textContent = source.textContent || '';
        section.append(heading);
      } else {
        section = documentObj.createElement('section');
        section.className = 'multiplayer-rule-section';
        const heading = documentObj.createElement('h2');
        heading.textContent = source.textContent || '';
        section.append(heading);
        panel.append(section);
      }
      continue;
    }
    (section || panel).append(source.cloneNode(true));
  }
}

function makeDisclosure(
  documentObj: Document,
  scope: 'common' | 'specific',
  label: string,
  open: boolean,
): { details: HTMLDetailsElement; body: HTMLElement } {
  const details = documentObj.createElement('details');
  details.className = `multiplayer-rule-disclosure multiplayer-rule-disclosure-${scope}`;
  details.dataset.scope = scope;
  details.open = open;
  const summary = documentObj.createElement('summary');
  summary.className = 'multiplayer-rule-disclosure-summary';
  const title = documentObj.createElement('span');
  title.className = 'multiplayer-rule-disclosure-title';
  title.textContent = label;
  summary.append(title);
  const body = documentObj.createElement('div');
  body.className = 'multiplayer-rule-disclosure-body';
  details.append(summary, body);
  return { details, body };
}

export function buildRuleGuide(
  target: HTMLElement,
  documentObj: Document,
  initialGameId: string,
  locale: UiLocale = 'zh-CN',
): void {
  if (!target.children?.length) return;
  const groups = collectAuthoredGroups(target);
  const t = (key: Parameters<typeof translateUi>[1]) => translateUi(locale, key);
  const commonGroup = groups.get(t('guide.common'));
  if (!commonGroup) return;

  const guide = documentObj.createElement('div');
  guide.className = 'multiplayer-rule-guide';
  guide.dataset.mpRuleGuide = '';

  const nav = documentObj.createElement('div');
  nav.className = 'multiplayer-rule-tabs-shell';
  const gameTabs = documentObj.createElement('div');
  gameTabs.className = 'multiplayer-rule-game-tabs';
  gameTabs.setAttribute('role', 'tablist');
  gameTabs.setAttribute('aria-label', t('guide.games'));
  for (const game of GUIDE_GAMES) {
    const button = documentObj.createElement('button');
    button.type = 'button';
    button.className = 'multiplayer-rule-game-tab';
    button.dataset.game = game.id;
    button.setAttribute('role', 'tab');
    button.textContent = t(game.titleKey);
    gameTabs.append(button);
  }
  nav.append(gameTabs);
  guide.append(nav);

  const panels = documentObj.createElement('div');
  panels.className = 'multiplayer-rule-panels';
  for (const game of GUIDE_GAMES) {
    const panel = documentObj.createElement('div');
    panel.className = 'multiplayer-rule-panel';
    panel.dataset.game = game.id;
    const common = makeDisclosure(documentObj, 'common', t('guide.common'), false);
    appendAuthoredGroup(common.body, documentObj, commonGroup);
    const specific = makeDisclosure(documentObj, 'specific', t('guide.specific'), false);
    appendAuthoredGroup(
      specific.body,
      documentObj,
      [...groups.values()].find((group) => group.key.startsWith(game.short + ' ')),
      t('guide.specific'),
    );
    if (!specific.body.children.length) {
      const empty = documentObj.createElement('p');
      empty.textContent = t('guide.none');
      specific.body.append(empty);
    }
    panel.append(common.details, specific.details);
    panels.append(panel);
  }
  guide.append(panels);
  target.replaceChildren(guide);

  const availableGames = new Set<string>(GUIDE_GAMES.map((game) => game.id));
  let activeGame = availableGames.has(initialGameId) ? initialGameId : DEFAULT_GUIDE_GAME;
  const gameButtons = Array.from(gameTabs.querySelectorAll<HTMLButtonElement>('button[data-game]'));
  const panelElements = Array.from(panels.querySelectorAll<HTMLElement>('.multiplayer-rule-panel'));

  const render = () => {
    for (const button of gameButtons) {
      const selected = button.dataset.game === activeGame;
      button.classList.toggle('selected', selected);
      button.setAttribute('aria-selected', String(selected));
      button.tabIndex = selected ? 0 : -1;
    }
    for (const panel of panelElements) {
      panel.hidden = panel.dataset.game !== activeGame;
    }
  };

  const revealActiveGame = () => {
    const selected = gameButtons.find((button) => button.dataset.game === activeGame);
    selected?.scrollIntoView({ block: 'nearest', inline: 'center' });
  };

  const resetRuleScroll = () => {
    target.scrollTop = 0;
  };

  const moveFocus = (buttons: readonly HTMLButtonElement[], current: HTMLButtonElement, delta: number) => {
    const index = buttons.indexOf(current);
    if (index < 0) return;
    buttons[(index + delta + buttons.length) % buttons.length]?.focus();
  };
  gameTabs.addEventListener('click', (event) => {
    const button = (event.target as Element | null)?.closest<HTMLButtonElement>('button[data-game]');
    if (!button?.dataset.game) return;
    activeGame = button.dataset.game;
    render();
    revealActiveGame();
    resetRuleScroll();
  });
  gameTabs.addEventListener('keydown', (event) => {
    const button = (event.target as Element | null)?.closest<HTMLButtonElement>('button[data-game]');
    if (!button) return;
    if (event.key === 'ArrowLeft') {
      event.preventDefault();
      moveFocus(gameButtons, button, -1);
    } else if (event.key === 'ArrowRight') {
      event.preventDefault();
      moveFocus(gameButtons, button, 1);
    }
  });
  guide.addEventListener('mp-guide-select-game', (event) => {
    const gameId = (event as CustomEvent<string>).detail;
    if (!availableGames.has(gameId)) return;
    activeGame = gameId;
    render();
    revealActiveGame();
    resetRuleScroll();
  });
  render();
  revealActiveGame();
}
