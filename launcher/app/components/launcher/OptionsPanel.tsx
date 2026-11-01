import { ArrowLeft, ChevronRight, Info, Users } from 'lucide-react';
import { type ReactNode, useRef } from 'react';
import { localizedGameTitle, useLocale } from '../../i18n';
import { Button } from '../ui/button';
import { AppLink } from './AppLink';
import { useOptionsTarget } from './options-ownership';
import type { LibraryProduct } from './products';
import { useOptionsInteractions } from './use-options-interactions';
import { useOptionsViewport } from './use-options-viewport';
import { useRoomSettingsRelocation } from './use-room-settings-relocation';

export interface OptionsPanelProps {
  product: LibraryProduct;
  open: boolean;
  onBack: () => void;
  children: ReactNode;
  /** Original .launch-wrap.launch-actions is a direct aside child, not scroll content. */
  actions?: ReactNode;
  /** Preserve the original MP link only when that product is actually published. */
  lobbyHref?: string;
  embeddedInLobby?: boolean;
  /** Existing room settings carrier receives the same header and MP fold. */
  roomDrawer?: boolean;
  multiplayerOnline?: ReactNode;
  multiplayerDiagnostics?: ReactNode;
  assetUrl: (path: string) => string;
}
/** Main index.html354–365 shell; body is the shared SettingsBody, never a copy. */
export function OptionsPanel({
  product,
  open,
  onBack,
  children,
  actions,
  lobbyHref,
  embeddedInLobby = false,
  roomDrawer = false,
  multiplayerOnline,
  multiplayerDiagnostics,
  assetUrl,
}: OptionsPanelProps) {
  const { t, locale } = useLocale();
  const panel = useRef<HTMLElement>(null);
  useOptionsViewport(panel);
  useOptionsInteractions(panel, open, onBack);
  const panelTarget = useOptionsTarget('panel', panel);
  useRoomSettingsRelocation(panel, roomDrawer && product.multiplayer, product.id);
  const credit = product.credit ?? { name: t('gameNotice.credit'), url: 'https://b23.tv/WOQhahY' };
  // Main app.mts4626–4634 carries this product into the directory filter.
  const productLobbyHref = lobbyHref ? lobbyHrefForProduct(lobbyHref, product.id) : undefined;
  return (
    <aside
      ref={panelTarget}
      className={`tools${open ? ' is-open' : ''}${product.multiplayer ? ' mp-mode' : ''}`}
      role={open ? 'dialog' : 'complementary'}
      aria-labelledby="gameTitle"
      aria-modal={open}
      aria-hidden={!open}
    >
      <div className="tools-head">
        <img
          className="options-cover"
          id="optionsCover"
          src={product.artwork ?? undefined}
          hidden={!product.artwork}
          alt=""
          decoding="async"
        />
        <Button
          variant="legacy"
          size="legacy"
          className="library-back"
          id="libraryBack"
          type="button"
          aria-label={t(roomDrawer || embeddedInLobby ? 'settings.drawerClose' : 'library.back')}
          onClick={onBack}
        >
          <ArrowLeft size={24} aria-hidden="true" focusable="false" />
        </Button>
        <div className="tools-title">
          <div className="game-id visually-hidden" id="gameId" data-game={product.gameId}>
            {product.gameId.toUpperCase()}
            {product.multiplayer ? ' MP' : ''}
          </div>
          <h3 className="visually-hidden" id="gameTitle">
            {localizedGameTitle(locale, product.gameId, product.title)}
          </h3>
          <span className="options-number" id="optionsNumber" aria-hidden="true">
            {product.number}
          </span>
          <div className="options-subtitle-line">
            <span className="options-subtitle" id="optionsSubtitle">
              {product.subtitle}
            </span>
            <span className="mp-title-badge" id="mpTitleBadge" hidden={!product.multiplayer}>
              {t('library.multiplayerCaption')}
            </span>
          </div>
        </div>
      </div>
      <div className="options-scroll">
        {product.multiplayer && !embeddedInLobby && productLobbyHref && (
          <AppLink
            className="options-lobby-link"
            id="optionsLobbyLink"
            href={productLobbyHref}
            aria-label={`${localizedGameTitle(locale, product.gameId, product.title)} · ${t('lobby.title')}`}
          >
            <Users size={20} aria-hidden="true" focusable="false" />
            <span>{t('lobby.title')}</span>
            <ChevronRight size={24} aria-hidden="true" focusable="false" />
          </AppLink>
        )}
        {!product.multiplayer && product.adaptationNotice && (
          <details className="tools-callout" id="gameNoticeCallout" aria-labelledby="gameNoticeCalloutTitle">
            <summary className="tools-callout-title" id="gameNoticeCalloutTitle">
              <Info size={24} aria-hidden="true" focusable="false" />
              <span>{t('options.adaptation')}</span>
            </summary>
            <p>{t('gameNotice.earlyTest')}</p>
            <p className="tools-callout-line">
              <span>{t('gameNotice.creditBefore')}</span>
              <a
                className="site-notice-brand"
                id="gameNoticeCredit"
                href={credit.url}
                target="_blank"
                rel="noopener noreferrer"
              >
                <span className="site-notice-brand-icon">
                  <img src={assetUrl('assets/notice-bilibili.svg')} alt="" decoding="async" />
                </span>
                <span id="gameNoticeCreditName">{credit.name}</span>
              </a>
              <span>{t('gameNotice.creditAfter')}</span>
            </p>
            <p className="tools-callout-line">
              <a
                className="site-notice-brand"
                id="gameNoticeRepo"
                href={product.sourceRepository}
                target="_blank"
                rel="noopener noreferrer"
              >
                <span className="site-notice-brand-icon">
                  <img src={assetUrl('assets/notice-github.svg')} alt="" decoding="async" />
                </span>
                <span>{t('gameNotice.repo')}</span>
              </a>
            </p>
          </details>
        )}
        {/* Main's MP shell/diagnostics exist before selection, hidden by product
          mode. Keep that DOM contract and one probe presentation owner while
          the one active shared settings body changes its product context. */}
        <section className="mp-shell" id="mpShell" hidden={!product.multiplayer}>
          {multiplayerOnline}
          <section className="mp-settings" id="mpSettingsFold">
            {product.multiplayer ? children : null}
          </section>
          {multiplayerDiagnostics}
        </section>
        {!product.multiplayer && children}
      </div>
      {actions}
    </aside>
  );
}

function lobbyHrefForProduct(href: string, productId: string): string {
  const hashIndex = href.indexOf('#');
  const address = hashIndex < 0 ? href : href.slice(0, hashIndex);
  const hash = hashIndex < 0 ? '' : href.slice(hashIndex);
  const queryIndex = address.indexOf('?');
  const path = queryIndex < 0 ? address : address.slice(0, queryIndex);
  const params = new URLSearchParams(queryIndex < 0 ? '' : address.slice(queryIndex + 1));
  params.set('game', productId);
  return `${path}?${params}${hash}`;
}
