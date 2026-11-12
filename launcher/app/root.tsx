import { useDocumentUiLocale } from './navigation/document-locale';
import { Links, Meta, Outlet, Scripts, ScrollRestoration, UNSAFE_ViewTransitionContext, useLocation, useNavigate } from 'react-router';
import { useContext, useEffect, useRef, lazy, Suspense, type ReactNode } from 'react';
import { BrowserLauncher } from './BrowserLauncher';
import {
  LauncherNavigationTransitionOptIn,
  OriginalBootstrapHead,
  OriginalPreloads,
  OriginalModuleRequestMark,
  OriginalBootstrapTail,
} from './components/startup/OriginalBootstrap';
import { AppShellDeploymentMeta } from './components/startup/AppShellDeploymentMeta';
import { InitialLauncher, InitialDocumentBody, OriginalNoscript } from './components/startup/InitialLauncher';
import './ui/foundation.css';
import './components/launcher/lobby-options.css';
import './components/launcher/shared-options.css';
import './components/launcher/book-ui.css';
import './components/launcher/book-surfaces.css';
import './components/room/room-light.css';
import './ui/system.css';
import './ui/tailwind.css';
import './components/launcher/book-library.css';
import './components/launcher/masthead.css';
import './components/launcher/options-sheet.css';
import './components/settings/settings-ui.css';
import './ui/scrollbars.css';
import { UiTheme } from './ui/theme';
import { BackgroundController } from './components/launcher/CustomBackground';
import { UiScrollbars } from './ui/scrollbars';
import { isLauncherHomePath } from './navigation/addresses';
import UptimeRoute from './routes/uptime';
import { helpDocumentAtPath, isStandalonePage, isTh09RatingPage } from './components/documents/catalog';
import { isPreviewOnlyPage } from './release-features';
import { SITE_BRAND_PREPAINT_SCRIPT } from '../lib/site-brand.mjs';
const HelpRoute = lazy(() => import('./routes/help'));
const Th09RatingsRoute = lazy(() => import('./routes/th09-ratings'));

// Overlay history shares page scroll. Profile anchors have separate history entries.
const pageScrollKey = (location: { pathname: string; key: string }) =>
  /\/profile\.html\/?$/.test(location.pathname) ? location.key : location.pathname;

export function Layout({ children }: { children: ReactNode }) {
  const { pathname } = useLocation();
  const transition = useContext(UNSAFE_ViewTransitionContext);
  // The Router may replay a page transition on Back even when only the overlay URL changed.
  const samePageTransition = transition.isTransitioning && transition.currentLocation.pathname === transition.nextLocation.pathname;
  const previewOnly = isPreviewOnlyPage(pathname);
  const review = !previewOnly && (/\/ui-review\.html\/?$/.test(pathname) || isStandalonePage(pathname));
  const home = previewOnly || isLauncherHomePath(pathname);
  const locale = useDocumentUiLocale();
  const lobby = /\/(?:lobby|lobby\.html)\/?$/.test(pathname);
  // Original boot owns removal. Keep this initial prop constant so later SPA
  // route renders cannot reintroduce the document's retired loading gate.
  const initialLobby = useRef(lobby).current;
  return (
    <html
      data-ui-app=""
      lang={locale}
      data-ui-locale={locale}
      data-ui-home={home ? '' : undefined}
      data-ui-route-transition={transition.isTransitioning ? (samePageTransition ? 'inline' : 'page') : undefined}
      className="dark"
      data-book-theme="dark"
      data-original-entry={initialLobby ? 'lobby' : 'library'}
      data-lobby-boot={initialLobby ? 'loading' : undefined}
      suppressHydrationWarning
    >
      <head>
        <meta charSet="utf-8" />
        <style id="ui-style-layers">{'@layer properties,foundation,theme,base,components,utilities;'}</style>
        <LauncherNavigationTransitionOptIn />
        <meta name="darkreader-lock" />
        <script>{`try{var theme=JSON.parse(localStorage.getItem('eagler.book.theme'));if(theme==='light'||theme==='dark'){document.documentElement.setAttribute('data-book-theme',theme);document.documentElement.className=theme;}}catch(e){}`}</script>
        {!review && <OriginalBootstrapHead />}
        <AppShellDeploymentMeta />
        <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover" />
        <Meta />
        <Links />
      </head>
      <InitialDocumentBody lobby={lobby}>
        <UiTheme />
        <BackgroundController />
        <UiScrollbars />
        {!review && <OriginalPreloads />}
        {children}
        {/* The parser owns this bootstrap; server/client minifiers may rename its local functions. */}
        <script suppressHydrationWarning>{SITE_BRAND_PREPAINT_SCRIPT}</script>
        {!review && <OriginalNoscript lobby={lobby} locale={locale} />}
        <ScrollRestoration getKey={pageScrollKey} />
        {!review && <OriginalModuleRequestMark />}
        <Scripts data-eagler-entry="true" />
        {!review && <OriginalBootstrapTail />}
      </InitialDocumentBody>
    </html>
  );
}
/** Framework's index.html SPA fallback renders this rather than App. */
export function HydrateFallback() {
  const pathname = useLocation().pathname;
  if (isTh09RatingPage(pathname) && !isPreviewOnlyPage(pathname)) return <Suspense><Th09RatingsRoute /></Suspense>;
  return helpDocumentAtPath(pathname) ? (
    <Suspense>
      <HelpRoute />
    </Suspense>
  ) : isStandalonePage(pathname) && !isPreviewOnlyPage(pathname) ? (
    <UptimeRoute />
  ) : (
    <InitialLauncher />
  );
}
/** The document root, not a route component, owns the unkeyed Player/iframe. */
export default function App() {
  const location = useLocation(),
    navigate = useNavigate(),
    pathname = location.pathname,
    previewOnly = isPreviewOnlyPage(pathname);
  useEffect(() => {
    if (previewOnly) navigate({ pathname: '/', search: location.search }, { replace: true });
  }, [location.search, navigate, previewOnly]);
  if (previewOnly) return <InitialLauncher />;
  const review = /\/ui-review\.html\/?$/.test(pathname) || isStandalonePage(pathname);
  return review ? (
    <Outlet />
  ) : (
    <>
      <BrowserLauncher />
      <Outlet />
    </>
  );
}
