import {Button as UiButton} from '../../ui/button';
import {X, Users, ChevronRight, Info} from 'lucide-react';
import {useRef, type ReactNode} from 'react';
import {useOptionsViewport} from './use-options-viewport';
import {useOptionsInteractions} from './use-options-interactions';
import {useOptionsTarget} from './options-ownership';
import {useLocale} from '../../i18n';
import type {LibraryProduct} from './products';
import {AppLink} from './AppLink';
import {useRoomSettingsRelocation} from './use-room-settings-relocation';

export interface OptionsPanelProps {
  product: LibraryProduct;
  coverArtwork?: string | null;
  open: boolean;
  closing?: boolean;
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
export function OptionsPanel({product, coverArtwork, open, closing=false, onBack, children, actions, lobbyHref, embeddedInLobby = false, roomDrawer = false, multiplayerOnline, multiplayerDiagnostics, assetUrl}: OptionsPanelProps) {
  const {t} = useLocale();
  const panel = useRef<HTMLElement>(null); useOptionsViewport(panel); useOptionsInteractions(panel, open&&!closing, onBack);
  const panelTarget = useOptionsTarget('panel', panel);
  useRoomSettingsRelocation(panel, roomDrawer && product.multiplayer, product.id);
  const credit = product.credit ?? {name: t('gameNotice.credit'), url: 'https://b23.tv/WOQhahY'};
  // Main app.mts4626–4634 carries this product into the directory filter.
  const productLobbyHref = lobbyHref ? lobbyHrefForProduct(lobbyHref, product.id) : undefined;
  return <aside ref={panelTarget} data-options-panel data-state={open&&!closing?'open':'closed'} className={`tools${open ? ' is-open' : ''}${product.multiplayer ? ' mp-mode' : ''}`} role="dialog" aria-labelledby="gameTitle" aria-modal={open&&!closing || undefined} inert={!open||closing}>
    <div className="tools-head">
      <img className="options-cover" id="optionsCover" src={coverArtwork ?? product.artwork ?? undefined} hidden={!(coverArtwork ?? product.artwork)} alt="" decoding="async"/>
      <div className="tools-title"><div className="game-id" id="gameId" data-game={product.gameId}>{product.gameId}{product.multiplayer ? ' | MP' : ''}</div><h3 id="gameTitle" title={product.title}>{product.title}</h3><p className="options-subtitle" id="optionsSubtitle" title={product.subtitle}>{product.subtitle}</p></div>
      <UiButton variant="secondary" size="icon" className="library-back" id="libraryBack" type="button" aria-label={t(roomDrawer || embeddedInLobby ? 'settings.drawerClose' : 'library.back')} onClick={onBack}><X aria-hidden="true"/></UiButton>
    </div>
    <div className="options-scroll">
      {product.multiplayer && !embeddedInLobby && productLobbyHref && <AppLink className="options-lobby-link" id="optionsLobbyLink" href={productLobbyHref} aria-label={`${product.title} | ${t('lobby.title')}`}><Users aria-hidden="true"/><span>{t('lobby.title')}</span><ChevronRight aria-hidden="true"/></AppLink>}
      {!product.multiplayer && product.adaptationNotice && <details className="tools-callout" id="gameNoticeCallout" aria-labelledby="gameNoticeCalloutTitle">
        <summary className="tools-callout-title" id="gameNoticeCalloutTitle"><Info aria-hidden="true"/><span>{t('options.adaptation')}</span></summary>
        <p>{t('gameNotice.earlyTest')}</p>
        <p className="tools-callout-line"><span>{t('gameNotice.creditBefore')}</span><a className="site-notice-brand" id="gameNoticeCredit" href={credit.url} target="_blank" rel="noopener noreferrer"><span className="site-notice-brand-icon"><img src={assetUrl('assets/notice-bilibili.svg')} alt="" decoding="async"/></span><span id="gameNoticeCreditName">{credit.name}</span></a><span>{t('gameNotice.creditAfter')}</span></p>
        <p className="tools-callout-line"><a className="site-notice-brand" id="gameNoticeRepo" href={product.sourceRepository} target="_blank" rel="noopener noreferrer"><span className="site-notice-brand-icon"><img src={assetUrl('assets/notice-github.svg')} alt="" decoding="async"/></span><span>{t('gameNotice.repo')}</span></a></p>
      </details>}
      {/* Main's MP shell/diagnostics exist before selection, hidden by product
          mode. Keep that DOM contract and one probe presentation owner while
          the one active shared settings body changes its product context. */}
      <section className="mp-shell" id="mpShell" hidden={!product.multiplayer}>{multiplayerOnline}<section className="mp-settings" id="mpSettingsFold">{product.multiplayer ? children : null}</section>{multiplayerDiagnostics}</section>
      {!product.multiplayer && children}
    </div>
    {actions}
  </aside>;
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
