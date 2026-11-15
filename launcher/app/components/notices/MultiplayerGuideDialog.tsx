import {useCallback, useEffect, useLayoutEffect, useRef} from 'react';
import {renderLocalizedContent} from '../../../src/launcher/localized-content.mts';
import '../../ui/document-body.css';
import {useLocale} from '../../i18n';
import {buildRuleGuide, DEFAULT_GUIDE_GAME} from '../../../src/launcher/multiplayer-guide-content.mts';
import type {MainDialogProps} from './use-main-dialog';
import {InformationalDialog, informationalDialogPresentation} from './InformationalDialog';
export interface MultiplayerGuideDialogProps extends MainDialogProps {
  gameId: string;
  contentUrl?: string;
  fetchImpl?: typeof fetch;
}
/** Main opens immediately, loads/sanitizes once, retries errors, selects current game. */
export function MultiplayerGuideDialog({gameId, contentUrl = 'content/MULTIPLAYER.html', fetchImpl = fetch, ...props}: MultiplayerGuideDialogProps) {
  const {t, locale} = useLocale();
  const content = useRef<HTMLDivElement>(null), loaded = useRef(false), pending = useRef<Promise<void> | null>(null);
  const sourceHtml = useRef('');
  const active = useRef(false), generation = useRef(0);
  const latest = useRef({t, locale, gameId, contentUrl, fetchImpl}); latest.current = {t, locale, gameId, contentUrl, fetchImpl};
  const renderContent = useCallback((selected = latest.current.gameId || DEFAULT_GUIDE_GAME) => {
    const target = content.current; if (!target) return;
    const scroll = target.scrollTop;
    const disclosures = new Map([...target.querySelectorAll<HTMLDetailsElement>('details[data-scope]')]
      .map(node => [`${node.parentElement?.dataset.game}:${node.dataset.scope}`, node.open]));
    const contentLocale = renderLocalizedContent(target, 'multiplayer', latest.current.locale, sourceHtml.current);
    buildRuleGuide(target, target.ownerDocument, selected, contentLocale);
    for (const node of target.querySelectorAll<HTMLDetailsElement>('details[data-scope]')) {
      const game = node.parentElement?.dataset.game;
      node.open = disclosures.get(`${game}:${node.dataset.scope}`) ?? node.dataset.scope === 'common';
    }
    target.scrollTop = scroll;
  }, []);
  const status = useCallback((text: string, className: string) => {
    const target = content.current; if (!target || !active.current) return;
    const paragraph = target.ownerDocument.createElement('p'); paragraph.className = className; paragraph.textContent = text; target.replaceChildren(paragraph);
  }, []);
  useLayoutEffect(() => {
    active.current = true;
    if (!content.current?.childNodes.length) status(latest.current.t('multiplayerGuide.loading'), 'multiplayer-guide-loading');
    return () => {active.current = false; generation.current++; pending.current = null;};
  }, [status]);
  useLayoutEffect(() => {
    if (content.current?.firstElementChild?.className === 'multiplayer-guide-loading') status(t('multiplayerGuide.loading'), 'multiplayer-guide-loading');
    else if (loaded.current) renderContent(content.current?.querySelector<HTMLElement>('[role="tab"][aria-selected="true"]')?.dataset.game);
  }, [t, status, renderContent]);
  useEffect(() => {
    if (!props.open) return;
    const token = generation.current;
    const select = () => {
      if (active.current && token === generation.current) content.current?.querySelector('[data-mp-rule-guide]')?.dispatchEvent(new CustomEvent('mp-guide-select-game', {detail: gameId}));
    };
    if (loaded.current) {select(); return;}
    if (!pending.current) {
      const request = (async () => {
        try {
          const {fetchImpl, contentUrl} = latest.current;
          const response = await fetchImpl(contentUrl);
          if (!response.ok) throw new Error(`HTTP ${response.status}`);
          const html = await response.text();
          if (!active.current || token !== generation.current || !content.current) return;
          sourceHtml.current = html; renderContent();
          loaded.current = true;
        } catch (error) {
          if (active.current && token === generation.current) status(latest.current.t('multiplayerGuide.readFailed', {reason: error instanceof Error ? error.message : String(error)}), 'multiplayer-guide-error');
        }
      })();
      pending.current = request;
      void request.finally(() => {if (pending.current === request) pending.current = null;});
    }
    void pending.current.then(select);
  }, [props.open, gameId, status, renderContent]);
  return <InformationalDialog {...props} id="mpGuideDialog" titleId="mpGuideTitle" closeId="mpGuideClose"
    title={t('multiplayerGuide.title')} closeLabel={t('multiplayerGuide.close')} presentation={informationalDialogPresentation.scrollable}>
    <div ref={content} className="multiplayer-guide-content ui-document prose" id="mpGuideContent"/>
  </InformationalDialog>;
}
