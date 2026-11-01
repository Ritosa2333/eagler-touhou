import { type ReactNode, useRef } from 'react';
import { useLocation } from 'react-router';
import { PRODUCT_IDS, productEnabledForBuild } from '../../../src/contracts/product-catalog.mts';
import { localeHome, translateUi, type UiLocale } from '../../../src/launcher/i18n.mts';
import { LocaleProvider, useLocale } from '../../i18n';
import { LobbyRoomTools } from '../directory/LobbyRoomTools';
import { LauncherMasthead } from '../launcher/LauncherMasthead';
import { LibrarySurface } from '../launcher/LibrarySurface';
import { LobbySurface } from '../launcher/LobbySurface';
import { createLibraryProducts } from '../launcher/products';
import { SiteFooter } from '../launcher/SiteFooter';

const assetUrl = (path: string) => path;
const inactive = () => {};
// Main's static compiler uses the default catalog gate, before Host/query policy.
const products = createLibraryProducts(PRODUCT_IDS, assetUrl, (id) => `?game=${id}`).map((product) => ({
  ...product,
  hidden: !productEnabledForBuild(product.id),
}));

/** Published main chrome/cards, using the same view components as the live
 * session. No storage, network, settings owner or Runtime exists in this phase. */
export function InitialLauncher() {
  const { pathname } = useLocation();
  const lobby = /\/(?:lobby|lobby\.html)\/?$/.test(pathname);
  const locale = /\/ja\.html\/?$/.test(pathname) ? 'ja' : /\/en\.html\/?$/.test(pathname) ? 'en' : 'zh-CN';
  return (
    <LocaleProvider locale={locale}>
      <InitialSurface lobby={lobby} />
    </LocaleProvider>
  );
}

function InitialSurface({ lobby }: { lobby: boolean }) {
  const { locale, t } = useLocale();
  const masthead = (
    <LauncherMasthead
      interactive={false}
      variant={lobby ? 'lobby' : 'library'}
      assetUrl={assetUrl}
      launcherHref={localeHome(locale)}
      faqHref="faq.html"
      aboutHref="about.html"
      menuOpen={false}
      onMenuOpenChange={inactive}
      lessMotion={false}
      onToggleMotion={inactive}
      noticeEnabled
      diagnosticsEnabled={false}
      onToggleNotice={inactive}
      onToggleDiagnostics={inactive}
      onDonation={inactive}
      onFirstUse={inactive}
      languageControl={
        <select
          className="option-select ui-language-select"
          id="uiLanguageSelect"
          aria-label={t('ui.language')}
          defaultValue={locale}
        >
          <option value="zh-CN">{t('ui.language.zhCN')}</option>
          <option value="en">{t('ui.language.en')}</option>
          <option value="ja">{t('ui.language.ja')}</option>
        </select>
      }
    />
  );
  return lobby ? (
    <LobbySurface
      masthead={masthead}
      products={[]}
      interactive={false}
      onSelect={inactive}
      onActivate={inactive}
      onGuide={inactive}
      onNetwork={inactive}
      roomTools={<LobbyRoomTools disabled />}
      roomList={null}
      hasRooms={false}
      loading
      emptyState={null}
      roomCount=""
      roomUsersArtwork="assets/room-users.svg"
    />
  ) : (
    <LibrarySurface
      masthead={masthead}
      footer={<SiteFooter onDonation={inactive} />}
      products={products}
      interactive={false}
      onSelect={inactive}
      onActivate={inactive}
      optionsOpen={false}
      onBack={inactive}
      lobbyHref="lobby.html"
      roomUsersArtwork="assets/room-users.svg"
    />
  );
}

/** Static fallbacks remain readable without loading any application JavaScript. */
export function OriginalNoscript({ lobby, locale = 'zh-CN' }: { lobby: boolean; locale?: UiLocale }) {
  const t = (key: Parameters<typeof translateUi>[1]) => translateUi(locale, key);
  return lobby ? (
    <noscript>
      <style>{'#lobbyPreload{display:none}'}</style>
      <p>
        {t('startup.noScriptLobby')}
        <a href={localeHome(locale)}>{t('startup.noScriptHome')}</a>
      </p>
    </noscript>
  ) : (
    <noscript>
      <p className="noscript-message">
        {t('startup.noScriptLauncher')}
        <a href="faq.html">{t('nav.faq')}</a>
        {t('startup.noScriptAnd')}
        <a href="about.html">{t('startup.noScriptAbout')}</a>
        {t('startup.noScriptEnd')}
      </p>
    </noscript>
  );
}

/** Static class seed only. LauncherApplication owns the live lobby-page token;
 * replacing body.className on navigation would erase other services' classes. */
export function InitialDocumentBody({ lobby, children }: { lobby: boolean; children: ReactNode }) {
  const initialClassName = useRef(lobby ? 'lobby-page' : undefined).current;
  return <body className={initialClassName}>{children}</body>;
}
