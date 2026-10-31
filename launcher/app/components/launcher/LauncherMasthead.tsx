import { CircleHelp, Gamepad2, Heart, PanelLeftClose, PanelLeftOpen, Settings2, Users, X } from 'lucide-react';
import { type ReactNode, useEffect, useLayoutEffect, useRef, useState } from 'react';
import { localizedContentHref } from '../../../src/launcher/localized-content.mts';
import { useLocale } from '../../i18n';
import { useSurfaceMotion } from '../../lib/surface-motion';
import { Button } from '../ui/button';
import { AppLink } from './AppLink';
import { closeMainSelectMenus } from './MainSelect';

type Group = 'help' | 'settings';
export interface LauncherMastheadProps {
  interactive?: boolean;
  nativeLinks?: boolean;
  variant: 'library' | 'lobby' | 'help';
  assetUrl: (path: string) => string;
  launcherHref: string;
  faqHref: string;
  aboutHref: string;
  uptimeHref?: string;
  migrationHref?: string;
  migrationPolicyKnown?: boolean;
  updatedAge?: { text: string; title?: string; dateTime?: string };
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
  languageControl: ReactNode;
}

/** One navigation owner for the launcher, directory and standalone site pages. */
export function LauncherMasthead(props: LauncherMastheadProps) {
  const { locale, t } = useLocale();
  const { interactive = true, menuOpen } = props;
  const Link = interactive && !props.nativeLinks ? AppLink : 'a';
  const [group, setGroup] = useState<Group>('settings');
  const [compact, setCompact] = useState(false);
  const [collapsed, setCollapsed] = useState(false);
  useEffect(() => {
    try {
      setCollapsed(localStorage.getItem('eagler-touhou-sidebar-collapsed-v1') === '1');
    } catch {}
  }, []);
  const toggleSidebar = () => {
    const next = !collapsed;
    setCollapsed(next);
    try {
      localStorage.setItem('eagler-touhou-sidebar-collapsed-v1', next ? '1' : '0');
    } catch {}
  };
  const root = useRef<HTMLElement>(null),
    inner = useRef<HTMLDivElement>(null);
  useSurfaceMotion(inner, menuOpen, 'menu');
  const toggle = useRef<HTMLButtonElement>(null),
    opener = useRef<HTMLElement | null>(null);
  const focusPending = useRef(false);
  const closeTimer = useRef<ReturnType<typeof setTimeout> | null>(null);
  const openTimer = useRef<ReturnType<typeof setTimeout> | null>(null);
  const callback = useRef(props.onMenuOpenChange);
  callback.current = props.onMenuOpenChange;
  const base = new URL(props.launcherHref, 'https://launcher.invalid/');
  const href = (path: string) => {
    const result = new URL(path, base);
    if (path === 'lobby.html' && locale !== 'zh-CN') result.searchParams.set('lang', locale);
    return result.origin === 'https://launcher.invalid' && !props.launcherHref.startsWith('/')
      ? result.pathname.slice(1) + result.search + result.hash
      : result.origin === 'https://launcher.invalid'
        ? result.pathname + result.search + result.hash
        : result.href;
  };
  const labels = { single: t('nav.single'), lobby: t('nav.lobby'), help: t('nav.help'), settings: t('nav.settings') };
  const cancelClose = () => {
    if (closeTimer.current !== null) clearTimeout(closeTimer.current);
    closeTimer.current = null;
  };
  const cancelOpen = () => {
    if (openTimer.current !== null) clearTimeout(openTimer.current);
    openTimer.current = null;
  };
  const close = () => {
    focusPending.current = false;
    cancelOpen();
    cancelClose();
    callback.current(false);
    closeMainSelectMenus();
  };
  function open(next: Group, source?: HTMLElement, focus = false) {
    cancelOpen();
    cancelClose();
    focusPending.current = focus;
    setGroup(next);
    opener.current = source ?? toggle.current;
    callback.current(true);
  }
  // Focus only after React commits the selected panel and removes inert.
  useLayoutEffect(() => {
    if (!menuOpen || !focusPending.current) return;
    focusPending.current = false;
    const frame = requestAnimationFrame(() =>
      root.current
        ?.querySelector<HTMLElement>(
          `[data-site-panel="${group}"] .site-panel-content :is(a,button,select,[tabindex="0"])`,
        )
        ?.focus({ preventScroll: true }),
    );
    return () => cancelAnimationFrame(frame);
  }, [menuOpen, group]);
  function hover(next: Group, source: HTMLElement) {
    cancelOpen();
    cancelClose();
    openTimer.current = setTimeout(() => open(next, source), 500);
  }
  useEffect(() => {
    if (!interactive) return;
    const media = window.matchMedia?.('(max-width: 1023px)');
    const changed = () => {
      setCompact(media?.matches ?? window.innerWidth < 1024);
      close();
    };
    changed();
    media?.addEventListener('change', changed);
    const outside = (event: MouseEvent) => {
      if (event.target instanceof Element && event.target.closest('.mizuki-select-menu')) return;
      if (event.target instanceof Node && !root.current?.contains(event.target)) close();
    };
    document.addEventListener('click', outside);
    return () => {
      cancelOpen();
      cancelClose();
      media?.removeEventListener('change', changed);
      document.removeEventListener('click', outside);
    };
  }, [interactive]);
  useLayoutEffect(() => {
    if (!interactive) return;
    if (!menuOpen) closeMainSelectMenus();
    const measure = () => {
      root.current?.style.setProperty('--site-menu-height', `${inner.current?.getBoundingClientRect().height ?? 0}px`);
      if (!compact && root.current && inner.current) {
        const trigger = root.current.querySelector<HTMLElement>(`[data-site-trigger="${group}"]`);
        const height = inner.current.getBoundingClientRect().height;
        const top = trigger?.getBoundingClientRect().top ?? 96;
        root.current.style.setProperty(
          '--site-panel-top',
          `${Math.max(12, Math.min(top, window.innerHeight - height - 12))}px`,
        );
      }
    };
    measure();
    const observer = typeof ResizeObserver === 'undefined' ? null : new ResizeObserver(measure);
    if (inner.current) observer?.observe(inner.current);
    return () => observer?.disconnect();
  }, [interactive, menuOpen, group, compact, locale]);
  function section(next: Group, children: ReactNode) {
    return (
      <section className="site-navigation-panel" data-site-panel={next} aria-label={labels[next]}>
        <div className="site-panel-header">
          <h2 className="site-panel-heading">{labels[next]}</h2>
          <Button
            variant="legacy"
            size="legacy"
            type="button"
            className="masthead-link site-panel-close"
            aria-label={t('nav.closePanel')}
            onClick={() => {
              close();
              opener.current?.focus();
            }}
          >
            <X size={18} aria-hidden="true" focusable="false" />
          </Button>
        </div>
        <div className="site-panel-content">{children}</div>
      </section>
    );
  }
  return (
    <header
      ref={root}
      id="mastheadMenu"
      className={`masthead site-header${props.variant === 'lobby' ? ' lobby-masthead' : ''}`}
      data-group={group}
      data-open={menuOpen}
      data-compact={compact}
      data-collapsed={collapsed}
      onPointerEnter={cancelClose}
      onPointerLeave={(event) => {
        cancelOpen();
        if (!compact && event.pointerType === 'mouse') closeTimer.current = setTimeout(close, 160);
      }}
      onClick={(event) => {
        if ((event.target as Element).closest('a')) close();
      }}
      onKeyDown={(event) => {
        if (event.key === 'Escape') {
          cancelOpen();
          if (menuOpen) {
            event.preventDefault();
            event.stopPropagation();
            close();
            (opener.current ?? toggle.current)?.focus();
          }
        }
      }}
    >
      <div className="site-topbar">
        <Link className="brand" id="launcherLink" aria-label="EAGLER TOUHOU" href={props.launcherHref}>
          <span className="brand-eagler">
            <span className="brand-title">EAGLER</span>
            {props.updatedAge && (
              <time
                className="brand-version"
                id="brandUpdateAge"
                title={props.updatedAge.title}
                dateTime={props.updatedAge.dateTime}
              >
                {props.updatedAge.text}
              </time>
            )}
          </span>
          <span className="brand-separator" aria-hidden="true">
            <span className="brand-yinyang">☯</span>
          </span>
          <span className="brand-title">TOUHOU</span>
        </Link>
        <div className="site-topbar-language">
          <span className="site-language-label">{t('ui.language.menu')}</span>
          <div className="ui-language-control">{props.languageControl}</div>
        </div>
      </div>
      <div className="site-header-row">
        <Button
          variant="legacy"
          size="legacy"
          type="button"
          className="masthead-link site-sidebar-toggle"
          id="siteSidebarToggle"
          aria-expanded={!collapsed}
          aria-label={t(collapsed ? 'nav.expandSidebar' : 'nav.collapseSidebar')}
          title={t(collapsed ? 'nav.expandSidebar' : 'nav.collapseSidebar')}
          onClick={toggleSidebar}
        >
          {collapsed ? (
            <PanelLeftOpen size={20} aria-hidden="true" focusable="false" />
          ) : (
            <PanelLeftClose size={20} aria-hidden="true" focusable="false" />
          )}
          <span className="site-nav-label">{t('nav.collapseSidebarLabel')}</span>
        </Button>
        <nav className="masthead-links site-primary-links" aria-label={t('nav.siteInfo')}>
          <Link
            className="masthead-link site-nav-page"
            data-site-link="single"
            aria-label={labels.single}
            title={collapsed ? labels.single : undefined}
            aria-current={props.variant === 'library' ? 'page' : undefined}
            href={props.launcherHref}
            onPointerEnter={() => {
              if (!compact) close();
            }}
          >
            <Gamepad2 size={20} aria-hidden="true" focusable="false" />
            <span className="site-nav-label">{labels.single}</span>
          </Link>
          <Link
            className="masthead-link site-nav-page"
            data-site-link="lobby"
            aria-label={labels.lobby}
            title={collapsed ? labels.lobby : undefined}
            aria-current={props.variant === 'lobby' ? 'page' : undefined}
            href={href('lobby.html')}
            onPointerEnter={() => {
              if (!compact) close();
            }}
          >
            <Users size={20} aria-hidden="true" focusable="false" />
            <span className="site-nav-label">{labels.lobby}</span>
          </Link>
          {(['help', 'settings'] as const).map((next) => (
            <Button
              variant="legacy"
              size="legacy"
              key={next}
              type="button"
              className="masthead-link site-nav-group"
              data-site-trigger={next}
              aria-label={labels[next]}
              title={collapsed ? labels[next] : undefined}
              aria-current={props.variant === 'help' && next === 'help' ? 'page' : undefined}
              id={next === 'settings' ? 'mastheadMenuToggle' : undefined}
              ref={next === 'settings' ? toggle : undefined}
              aria-expanded={menuOpen && group === next}
              aria-controls="mastheadMenuPanel"
              onPointerEnter={(event) => {
                if (!compact && event.pointerType === 'mouse') hover(next, event.currentTarget);
              }}
              onPointerLeave={cancelOpen}
              onFocus={() => {
                cancelOpen();
                cancelClose();
              }}
              onClick={(event) => {
                if (menuOpen && group === next) close();
                else open(next, event.currentTarget);
              }}
              onKeyDown={(event) => {
                if (event.key === 'ArrowDown' || event.key === 'ArrowRight' || (compact && event.key === 'ArrowUp')) {
                  event.preventDefault();
                  open(next, event.currentTarget, true);
                }
              }}
            >
              {next === 'help' ? (
                <CircleHelp size={20} aria-hidden="true" focusable="false" />
              ) : menuOpen && group === 'settings' ? (
                <X size={20} aria-hidden="true" focusable="false" />
              ) : (
                <Settings2 size={20} aria-hidden="true" focusable="false" />
              )}
              <span className="site-nav-label">{labels[next]}</span>
            </Button>
          ))}
          <Button
            variant="legacy"
            size="legacy"
            className="masthead-link site-donation"
            id="donationOpenTop"
            aria-label={t('nav.donate')}
            title={collapsed ? t('nav.donate') : undefined}
            type="button"
            hidden={props.donationAvailable === false}
            onClick={() => {
              close();
              props.onDonation();
            }}
          >
            <Heart size={20} aria-hidden="true" focusable="false" />
            <span className="site-nav-label">{t('nav.donate')}</span>
          </Button>
        </nav>
      </div>
      <div className="site-navigation-tray" id="mastheadMenuPanel" inert={!menuOpen} aria-hidden={!menuOpen}>
        <div ref={inner} className="site-navigation-inner">
          {section(
            'help',
            <>
              <a className="masthead-menu-item" href={localizedContentHref(props.aboutHref, locale)}>
                {t('nav.about')}
              </a>
              <a className="masthead-menu-item" href={localizedContentHref(props.faqHref, locale)}>
                {t('nav.faq')}
              </a>
              <a
                className="masthead-menu-item"
                href={localizedContentHref(props.uptimeHref ?? href('uptime/'), locale)}
              >
                {t('nav.serviceStatus')}
              </a>
              <Button
                variant="legacy"
                size="legacy"
                className="masthead-menu-item"
                id="firstUseNoticeOpen"
                type="button"
                onClick={() => {
                  close();
                  props.onFirstUse();
                }}
              >
                {t('nav.firstUseNotice')}
              </Button>
              <a
                className="masthead-menu-item"
                id="originMigrationOpen"
                href={props.migrationHref ?? 'migrate.html'}
                hidden={!props.migrationHref}
                data-policy={props.migrationPolicyKnown ? 'host-manifest-origin-migration-policy/1' : undefined}
              >
                {t('nav.oldSiteMigration')}
              </a>
              <Button
                variant="legacy"
                size="legacy"
                className="masthead-menu-item site-mobile-donation"
                id="donationOpenMobile"
                type="button"
                hidden={props.donationAvailable === false}
                onClick={() => {
                  close();
                  props.onDonation();
                }}
              >
                <Heart size={18} aria-hidden="true" focusable="false" />
                {t('nav.donate')}
              </Button>
              <a
                className="masthead-menu-item"
                href="https://github.com/Ritosa2333/eagler-touhou"
                target="_blank"
                rel="noopener noreferrer"
              >
                GitHub
              </a>
            </>,
          )}
          {section(
            'settings',
            <>
              <Button
                variant="legacy"
                size="legacy"
                className="masthead-menu-item site-setting"
                id="siteNoticeToggle"
                type="button"
                role="switch"
                aria-checked={props.noticeEnabled}
                onClick={props.onToggleNotice}
              >
                <span>{t('notice.label')}</span>
                <i className="site-setting-switch" aria-hidden="true" />
              </Button>
              <Button
                variant="legacy"
                size="legacy"
                className="masthead-menu-item site-setting motion-menu-toggle"
                id={props.variant === 'lobby' ? 'lobbyLessMotion' : 'lessMotionToggle'}
                type="button"
                role="switch"
                aria-checked={props.lessMotion}
                onClick={props.onToggleMotion}
              >
                <span>{t('nav.lessMotion')}</span>
                <i className="site-setting-switch" aria-hidden="true" />
              </Button>
              <Button
                variant="legacy"
                size="legacy"
                className="masthead-menu-item site-setting"
                id="runtimeDiagnosticsToggle"
                type="button"
                role="switch"
                aria-checked={props.diagnosticsEnabled}
                onClick={props.onToggleDiagnostics}
              >
                <span>{t('diagnostics.toggle')}</span>
                <i className="site-setting-switch" aria-hidden="true" />
              </Button>
            </>,
          )}
        </div>
      </div>
    </header>
  );
}
