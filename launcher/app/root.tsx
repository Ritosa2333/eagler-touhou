import {useDocumentUiLocale} from './navigation/document-locale';
import {Links, Meta, Outlet, Scripts, ScrollRestoration, useLocation} from 'react-router';
import {useRef, type ReactNode} from 'react';
import {BrowserLauncher} from './BrowserLauncher';
import {OriginalBootstrapHead, OriginalPreloads, OriginalModuleRequestMark, OriginalBootstrapTail} from './components/startup/OriginalBootstrap';
import {AppShellDeploymentMeta} from './components/startup/AppShellDeploymentMeta';
import {InitialLauncher, InitialDocumentBody, OriginalNoscript} from './components/startup/InitialLauncher';
import './ui/foundation.css';
import './components/launcher/lobby-options.css';
import './components/launcher/shared-options.css';
import './components/launcher/book-ui.css';
import './components/launcher/book-surfaces.css';
import './components/room/room-light.css';
import './ui/system.css';
import './ui/tailwind.css';
import './components/launcher/book-library.css';
import './components/launcher/options-sheet.css';
import './components/settings/settings-ui.css';
import {UiTheme} from './ui/theme';
import {isLauncherHomePath} from './navigation/addresses';
import UptimeRoute from './routes/uptime';

const isStandalonePage = (pathname: string) => /\/uptime(?:\/(?:index\.html)?)?$/.test(pathname);

// Overlay history shares the page's current scroll position.
const pageScrollKey = (location: {pathname: string}) => location.pathname;

export function Layout({children}: {children: ReactNode}) {
  const {pathname} = useLocation();
  const review = /\/ui-review\.html\/?$/.test(pathname) || isStandalonePage(pathname);
  const home = isLauncherHomePath(pathname);
  const locale = useDocumentUiLocale();
  const lobby = /\/(?:lobby|lobby\.html)\/?$/.test(pathname);
  // Original boot owns removal. Keep this initial prop constant so later SPA
  // route renders cannot reintroduce the document's retired loading gate.
  const initialLobby = useRef(lobby).current;
  return <html data-ui-app="" lang={locale} data-ui-locale={locale} data-ui-home={home ? '' : undefined} className="dark" data-book-theme="dark" data-original-entry={initialLobby ? 'lobby' : 'library'} data-lobby-boot={initialLobby ? 'loading' : undefined} suppressHydrationWarning><head><meta charSet="utf-8"/><script>{`try{var theme=JSON.parse(localStorage.getItem('eagler.book.theme'));if(theme==='light'||theme==='dark'){document.documentElement.setAttribute('data-book-theme',theme);document.documentElement.className=theme;}}catch(e){}`}</script>{!review && <OriginalBootstrapHead/>}<AppShellDeploymentMeta/><meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover"/><Meta/><Links/></head>
    <InitialDocumentBody lobby={lobby}><UiTheme/>{!review && <OriginalPreloads/>}{children}{!review && <OriginalNoscript lobby={lobby} locale={locale}/>}<ScrollRestoration getKey={pageScrollKey}/>{!review && <OriginalModuleRequestMark/>}<Scripts data-eagler-entry="true"/>{!review && <OriginalBootstrapTail/>}</InitialDocumentBody></html>;
}
/** Framework's index.html SPA fallback renders this rather than App. */
export function HydrateFallback() {return isStandalonePage(useLocation().pathname) ? <UptimeRoute/> : <InitialLauncher/>;}
/** The document root, not a route component, owns the unkeyed Player/iframe. */
export default function App() {
  const pathname = useLocation().pathname;
  const review = /\/ui-review\.html\/?$/.test(pathname) || isStandalonePage(pathname);
  return review ? <Outlet/> : <><BrowserLauncher/><Outlet/></>;
}
