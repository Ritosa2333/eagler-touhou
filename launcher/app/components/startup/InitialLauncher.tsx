import { MainSelect } from '../launcher/MainSelect';
import { useLocation } from 'react-router';
import { lazy, Suspense, useRef, type ReactNode } from 'react';
import { PRODUCT_IDS, productEnabledForBuild } from '../../../src/contracts/product-catalog.mts';
import { LocaleProvider, useLocale, translate } from '../../i18n';
import { LibrarySurface } from '../launcher/LibrarySurface';
import { LobbySurface } from '../launcher/LobbySurface';
import { LauncherMasthead } from '../launcher/LauncherMasthead';
import { createLibraryProducts } from '../launcher/products';
import { LobbyRoomTools } from '../directory/LobbyRoomTools';
import {
  UI_LOCALES,
  UI_LOCALE_LABELS,
  localeFromPath,
  localeHome,
  withUiLanguage,
  type UiLocale,
} from '../../../src/launcher/i18n.mts';
import { accountsEnabled } from '../../release-features';

const AccountPage = lazy(() => import('../launcher/AccountPage').then(module => ({default:module.AccountPage})));
const ProfilePage = lazy(() => import('../launcher/ProfilePreview').then(module => ({default:module.ProfilePage})));

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
  const locale = localeFromPath(pathname);
  return (
    <LocaleProvider locale={locale}>
      <InitialSurface
        account={accountsEnabled && /\/login\.html\/?$/.test(pathname)}
        lobby={lobby}
        profile={accountsEnabled && /^(?:\/profile\.html|\/u\/[^/]+)\/?$/.test(pathname)}
      />
    </LocaleProvider>
  );
}

function InitialSurface({ lobby, profile, account }: { lobby: boolean; profile: boolean; account: boolean }) {
  const { locale, t } = useLocale();
  const masthead = (
    <LauncherMasthead
      interactive={false}
      variant={lobby ? 'lobby' : 'library'}
      assetUrl={assetUrl}
      launcherHref={localeHome(locale)}
      faqHref={withUiLanguage('faq.html', locale)}
      aboutHref={withUiLanguage('about.html', locale)}
      menuOpen={false}
      onMenuOpenChange={inactive}
      lessMotion={false}
      onToggleMotion={inactive}
      diagnosticsEnabled={false}
      onToggleDiagnostics={inactive}
      onDonation={inactive}
      onFirstUse={inactive}
      languageControl={
        <MainSelect
          className="option-select ui-language-select"
          id="uiLanguageSelect"
          aria-label={t('ui.language')}
          data-trigger-i18n="ui.language.menu"
          defaultValue={locale}
          disabled
        >
          {UI_LOCALES.map((value) => (
            <option key={value} value={value}>
              {t(UI_LOCALE_LABELS[value])}
            </option>
          ))}
        </MainSelect>
      }
    />
  );
  return account ? (
    <>
      {masthead}
      <Suspense fallback={<main className="account-page" aria-busy="true"/>}><AccountPage /></Suspense>
    </>
  ) : profile ? (
    <>
      {masthead}
      <Suspense fallback={<main className="player-profile" aria-busy="true"/>}><ProfilePage /></Suspense>
    </>
  ) : lobby ? (
    <LobbySurface
      masthead={masthead}
      products={[]}
      interactive={false}
      onSelect={inactive}
      onActivate={inactive}
      guideHref={null}
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
      products={products}
      interactive={false}
      onSelect={inactive}
      onActivate={inactive}
      optionsOpen={false}
    />
  );
}

/** Static help remains readable when scripting is unavailable. */
export function OriginalNoscript({ lobby, locale = 'zh-CN' }: { lobby: boolean; locale?: UiLocale }) {
  const t = (key: Parameters<typeof translate>[1]) => translate(locale, key);
  const helpHref = (path: string) => (locale === 'zh-CN' ? path : withUiLanguage(path, locale));
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
        <a href={helpHref('faq.html')}>{t('nav.faq')}</a>
        {t('startup.noScriptAnd')}
        <a href={helpHref('about.html')}>{t('startup.noScriptAbout')}</a>
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
