import {useEffect, useLayoutEffect, useRef, type ReactNode} from 'react';
import {useLocale} from '../../i18n';
import {mastheadIcons} from './masthead-icons';
import {AppLink} from './AppLink';
import {closeMainSelectMenus, MainSelectPrefix} from './MainSelect';
import {BookThemeToggle} from './BookThemeToggle';
import {useLocation} from 'react-router';
import {ProfilePreview} from './ProfilePreview';

export interface LauncherMastheadProps {
  /** Published pre-session markup has native links and no menu owner. */
  interactive?: boolean;
  variant: 'library' | 'lobby';
  assetUrl: (path: string) => string;
  launcherHref: string;
  faqHref: string;
  aboutHref: string;
  migrationHref?: string;
  migrationPolicyKnown?: boolean;
  updatedAge?: {text: string; title?: string; dateTime?: string};
  menuOpen: boolean;
  onMenuOpenChange: (open: boolean) => void;
  lessMotion: boolean;
  onToggleMotion: () => void;
  noticeEnabled: boolean;
  onToggleNotice: () => void;
  diagnosticsEnabled: boolean;
  onToggleDiagnostics: () => void;
  onDonation: () => void;
  donationAvailable?: boolean;
  onFirstUse: () => void;
  /** The shared MainSelect, with the original uiLanguageSelect native props. */
  languageControl: ReactNode;
  onReplay?: () => void;
  replayActive?: boolean;
}

/** Original index.html320–345 / lobby.html45–70 shared masthead.
 * Menu keyboard/outside-click behavior: main app.mts8749–8774.
 */
export function LauncherMasthead(props: LauncherMastheadProps) {
  const {t, locale} = useLocale();
  const profileOpen = /\/profile\.html\/?$/.test(useLocation().pathname);
  const {variant, menuOpen, onMenuOpenChange, interactive = true} = props;
  const Link = interactive ? AppLink : 'a';
  const menu = useRef<HTMLDivElement>(null), toggle = useRef<HTMLButtonElement>(null), panel = useRef<HTMLDivElement>(null);
  const focusFirst = useRef(false), callbacks = useRef(onMenuOpenChange); callbacks.current = onMenuOpenChange;
  const focusMenu = () => {
    const items = panel.current?.querySelectorAll<HTMLElement>('.mizuki-select-trigger, .masthead-menu-item');
    const first = Array.from(items ?? []).find(item => {
      const mobileNav = item.closest('.book-mobile-nav');
      return !mobileNav || getComputedStyle(mobileNav).display !== 'none';
    });
    first?.focus();
  };
  useEffect(() => {
    if (!interactive) return;
    const outside = (event: MouseEvent) => {
      if (variant === 'lobby' && event.target instanceof Element && event.target.closest('.mizuki-select-menu')) return;
      if (event.target instanceof Node && !menu.current?.contains(event.target)) callbacks.current(false);
    };
    document.addEventListener('click', outside);
    return () => document.removeEventListener('click', outside);
  }, [variant, interactive]);
  useLayoutEffect(() => {
    if (!interactive) return;
    if (!menuOpen && variant === 'lobby') closeMainSelectMenus();
    if (menuOpen && focusFirst.current) {
      focusFirst.current = false;
      focusMenu();
    }
  }, [menuOpen, variant, interactive]);
  return <header className={`masthead book-masthead${variant === 'lobby' ? ' lobby-masthead' : ''}`}>
    <Link className="brand book-brand" id="launcherLink" href={props.launcherHref} aria-label="EAGLER TOUHOU"><span className="book-brand-symbol" aria-hidden="true">☯</span><span>EAGLER <b>TOUHOU</b></span><time className="brand-version" id="brandUpdateAge" title={props.updatedAge?.title} dateTime={props.updatedAge?.dateTime}>{props.updatedAge?.text ?? t('brand.neverUpdated')}</time></Link>
    <nav className="book-main-nav" aria-label={locale === 'en' ? 'Main navigation' : '主导航'}>
      <Link href={props.launcherHref} aria-current={variant === 'library' && !props.replayActive && !profileOpen ? 'page' : undefined}>{locale === 'en' ? '▷ Solo' : '▷ 单机'}</Link>
      <Link className="book-lobby-link" href={props.assetUrl('lobby.html')} aria-current={variant === 'lobby' && !props.replayActive && !profileOpen ? 'page' : undefined}>{locale === 'en' ? '♧ Multiplayer' : '♧ 联机'}</Link>
    </nav>
    <nav className="masthead-links" aria-label={t('nav.siteInfo')}>
      <BookThemeToggle interactive={interactive}/>
      <ProfilePreview interactive={interactive} active={profileOpen} href={props.assetUrl('profile.html')}/>
      <a className="masthead-link" id="originMigrationOpen" href={props.migrationHref ?? 'migrate.html'} hidden={!props.migrationHref} data-policy={props.migrationPolicyKnown ? 'host-manifest-origin-migration-policy/1' : undefined}><span className="masthead-label"><span>{t('nav.oldSitePart1')}</span><wbr/><span>{t('nav.migrationPart2')}</span></span></a>
      <a className="masthead-link book-top-faq" href={props.faqHref}><span className="masthead-label"><span>{t('nav.faqFirst')}</span><wbr/><span>{t('nav.faqSecond')}</span></span></a>
      <div ref={menu} className="masthead-menu" id="mastheadMenu" onKeyDown={event => {
        if (event.key !== 'Escape') return;
        if (variant === 'lobby') event.stopPropagation();
        event.preventDefault(); onMenuOpenChange(false); toggle.current?.focus();
      }}>
        <button ref={toggle} className="masthead-link masthead-menu-toggle" id="mastheadMenuToggle" type="button" aria-label={t('nav.more')} aria-expanded={menuOpen} aria-controls="mastheadMenuPanel" onClick={() => onMenuOpenChange(!menuOpen)} onKeyDown={event => {
          if (event.key !== 'ArrowDown') return;
          event.preventDefault();
          if (menuOpen) focusMenu();
          else {focusFirst.current = true; onMenuOpenChange(true);}
        }}><svg viewBox="0 0 24 24" aria-hidden="true"><path d="M4 6h16M4 12h16M4 18h16"/></svg></button>
        <div ref={panel} className="masthead-menu-panel" id="mastheadMenuPanel" aria-hidden={!menuOpen} inert={!menuOpen}>
          <nav className="book-mobile-nav" aria-label={locale === 'en' ? 'Main navigation' : '主导航'}>
            <Link className="masthead-menu-item" href={props.launcherHref} aria-current={variant === 'library' && !props.replayActive && !profileOpen ? 'page' : undefined} onClick={() => onMenuOpenChange(false)}>{locale === 'en' ? '▷ Solo' : '▷ 单机'}</Link>
            <Link className="masthead-menu-item" href={props.assetUrl('lobby.html')} aria-current={variant === 'lobby' && !props.replayActive && !profileOpen ? 'page' : undefined} onClick={() => onMenuOpenChange(false)}>{locale === 'en' ? '♧ Multiplayer' : '♧ 联机'}</Link>
          </nav>
          <button className="masthead-menu-item motion-menu-toggle" id={variant === 'lobby' ? 'lobbyLessMotion' : 'lessMotionToggle'} type="button" aria-pressed={props.lessMotion} title={t(props.lessMotion ? 'nav.motionFullTitle' : 'nav.motionLessTitle')} onClick={props.onToggleMotion}><MenuIcon name="motion"/><span>{t('nav.lessMotion')}</span><i className="site-notice-toggle-state" aria-hidden="true"/></button>
          <button className="masthead-menu-item" id="siteNoticeToggle" type="button" role="switch" aria-checked={props.noticeEnabled} onClick={props.onToggleNotice}><MenuIcon name="notice"/><span>{t('notice.label')}</span><i className="site-notice-toggle-state" aria-hidden="true"/></button>
          <button className="masthead-menu-item" id="runtimeDiagnosticsToggle" type="button" role="switch" aria-checked={props.diagnosticsEnabled} onClick={props.onToggleDiagnostics}><MenuIcon name="diagnostics"/><span>{t('diagnostics.toggle')}</span><i className="site-notice-toggle-state" aria-hidden="true"/></button>
          <div className="ui-language-control masthead-menu-language"><MainSelectPrefix prefix={<MenuIcon name="language"/>}>{props.languageControl}</MainSelectPrefix><template data-select-trigger-prefix="" dangerouslySetInnerHTML={{__html: mastheadIcons.language}}/></div>
          <button className="masthead-menu-item" id="firstUseNoticeOpen" type="button" onClick={() => {onMenuOpenChange(false); props.onFirstUse();}}><MenuIcon name="firstUse"/><span>{t('nav.firstUseNotice')}</span></button>
          <a className="masthead-menu-item" href={props.aboutHref}><MenuIcon name="about"/><span>{t('nav.about')}</span></a>
          <a className="masthead-menu-item" href="https://github.com/YomotsuHisami/eagler-touhou" target="_blank" rel="noopener noreferrer"><MenuIcon name="github"/><span>GitHub</span></a>
          <button className="masthead-menu-item book-menu-donation" type="button" hidden={props.donationAvailable === false} onClick={() => {onMenuOpenChange(false); props.onDonation();}}><MenuIcon name="donation"/><span>{t('nav.donate')}</span></button>
        </div>
      </div>
    </nav>
  </header>;
}

function MenuIcon({name}: {name: keyof typeof mastheadIcons}) {
  // Preserve the original SVG as the direct child, with no layout wrapper.
  const svg = mastheadIcons[name];
  const path = svg.match(/<path d="([^"]*)"/)![1];
  return <svg className="masthead-menu-icon" viewBox="0 0 24 24" aria-hidden="true"><path d={path}/></svg>;
}
