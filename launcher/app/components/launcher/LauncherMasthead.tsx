import { useLayoutEffect, useRef, useState, type ReactNode } from 'react';
import { DropdownMenu } from 'radix-ui';
import { GitHubLogoIcon } from '@radix-ui/react-icons';
import {
  Languages,
  ChevronLeft,
  ChevronRight,
  Heart as HeartIcon,
  Info as InfoCircledIcon,
  Bell as BellIcon,
  Activity as ActivityLogIcon,
  BookOpen as ReaderIcon,
  Check as CheckIcon,
} from 'lucide-react';
import { Button } from '../../ui/button';
import { uiPortalHost } from '../../ui/portal';
import { usePopupPadding } from '../../ui/use-popup-padding';
import { useLocale } from '../../i18n';
import { AppLink } from './AppLink';
import { BookThemeToggle } from './BookThemeToggle';
import { useLocation } from 'react-router';
import { ProfilePreview } from './ProfilePreview';
import { NavigationIcon } from './NavigationIcon';
import { SiteBrand } from './SiteBrand';
import { UI_LOCALES, UI_LOCALE_LABELS, withUiLanguage } from '../../../src/launcher/i18n.mts';
import { previewFeaturesEnabled, siteNoticeEnabled } from '../../release-features';

export interface LauncherMastheadProps {
  /** Published pre-session markup has native links and no menu owner. */
  interactive?: boolean;
  compactDocumentation?: boolean;
  variant: 'library' | 'lobby' | 'help';
  nativeLinks?: boolean;
  assetUrl: (path: string) => string;
  launcherHref: string;
  faqHref: string;
  aboutHref: string;
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
  /** The shared MainSelect, with the original uiLanguageSelect native props. */
  languageControl: ReactNode;
  onReplay?: () => void;
  replayActive?: boolean;
}

export function LauncherMasthead(props: LauncherMastheadProps) {
  const { t, locale } = useLocale();
  const profileOpen = /\/(?:profile|login)\.html\/?$/.test(useLocation().pathname);
  const { variant, menuOpen, onMenuOpenChange, interactive = true } = props;
  const padding = usePopupPadding(interactive && menuOpen);
  const Link = interactive && !props.nativeLinks ? AppLink : 'a';
  const trigger = useRef<HTMLButtonElement>(null),
    language = useRef<HTMLDivElement>(null),
    [host, setHost] = useState<HTMLElement>();
  const content = useRef<HTMLDivElement>(null),
    returnToLanguage = useRef(false),
    [languagesOpen, setLanguagesOpen] = useState(false);
  const masthead = useRef<HTMLElement>(null);
  useLayoutEffect(() => {
    if (!interactive) return;
    const header = masthead.current,
      container = header?.parentElement;
    if (!header || !container) return;
    const property = '--site-header-height',
      previous = container.style.getPropertyValue(property);
    let applied = '';
    const sync = () => {
      const height = header.getBoundingClientRect().height;
      if (height > 0) {
        applied = `${height}px`;
        if (container.style.getPropertyValue(property) !== applied) container.style.setProperty(property, applied);
      }
    };
    sync();
    const observer = typeof ResizeObserver === 'undefined' ? null : new ResizeObserver(sync);
    observer?.observe(header);
    return () => {
      observer?.disconnect();
      if (applied && container.style.getPropertyValue(property) === applied) {
        if (previous) container.style.setProperty(property, previous);
        else container.style.removeProperty(property);
      }
    };
  }, [interactive]);
  const backToMenu = () => {
    returnToLanguage.current = true;
    setLanguagesOpen(false);
  };
  useLayoutEffect(() => {
    if (!menuOpen) return;
    if (languagesOpen)
      content.current?.querySelector<HTMLElement>('[role="menuitemradio"][data-state="checked"]')?.focus();
    else if (returnToLanguage.current) {
      returnToLanguage.current = false;
      content.current?.querySelector<HTMLElement>('.masthead-language-trigger')?.focus();
    }
  }, [languagesOpen, menuOpen]);
  return (
    <header
      ref={masthead}
      className={`masthead book-masthead${variant === 'lobby' ? ' lobby-masthead' : ''}${props.compactDocumentation ? ' documentation-masthead' : ''}`}
    >
      <Link className="brand book-brand" id="launcherLink" href={props.launcherHref} aria-label="EAGLER TOUHOU">
        <SiteBrand assetUrl={props.assetUrl} />
        <time
          className="brand-version"
          id="brandUpdateAge"
          title={props.updatedAge?.title}
          dateTime={props.updatedAge?.dateTime}
        >
          {props.updatedAge?.text ?? t('brand.neverUpdated')}
        </time>
      </Link>
      <nav className="book-main-nav" aria-label={t('nav.main')}>
        <Link
          href={props.launcherHref}
          aria-current={variant === 'library' && !props.replayActive && !profileOpen ? 'page' : undefined}
        >
          <NavigationIcon name="single" />
          {t('nav.single')}
        </Link>
        <Link
          className="book-lobby-link"
          href={withUiLanguage(props.assetUrl('lobby.html'), locale)}
          aria-current={variant === 'lobby' && !props.replayActive && !profileOpen ? 'page' : undefined}
        >
          <NavigationIcon name="multiplayer" />
          {t('nav.lobby')}
        </Link>
      </nav>
      <nav className="masthead-links" aria-label={t('nav.siteInfo')}>
        <BookThemeToggle interactive={interactive} />
        {previewFeaturesEnabled && (
          <ProfilePreview
            interactive={interactive}
            active={profileOpen}
            href={withUiLanguage(props.assetUrl('profile.html'), locale)}
          />
        )}
        <a
          className="masthead-link"
          id="originMigrationOpen"
          href={props.migrationHref ?? 'migrate.html'}
          hidden={!props.migrationHref}
          data-policy={props.migrationPolicyKnown ? 'host-manifest-origin-migration-policy/1' : undefined}
        >
          {t('nav.oldSiteMigration')}
        </a>
        <a className="masthead-link book-top-faq" href={props.faqHref}>
          {t('nav.faq')}
        </a>
        <div hidden ref={language}>
          {props.languageControl}
        </div>
        <DropdownMenu.Root
          modal={false}
          open={interactive && menuOpen}
          onOpenChange={(open) => {
            if (open) {
              setHost(uiPortalHost(trigger.current));
              setLanguagesOpen(false);
            }
            onMenuOpenChange(open);
          }}
        >
          <DropdownMenu.Trigger asChild>
            <Button
              ref={trigger}
              variant="ghost"
              size="icon"
              className="masthead-action masthead-menu-toggle [&_svg]:size-5"
              id="mastheadMenuToggle"
              aria-label={t('nav.more')}
              disabled={!interactive}
            >
              <NavigationIcon name="menu" />
            </Button>
          </DropdownMenu.Trigger>
          <DropdownMenu.Portal container={host}>
            <DropdownMenu.Content
              ref={content}
              className="ui-menu masthead-menu-panel"
              id="mastheadMenuPanel"
              align="end"
              sideOffset={8}
              collisionPadding={padding}
              onEscapeKeyDown={(event) => {
                if (languagesOpen) {
                  event.preventDefault();
                  backToMenu();
                }
              }}
              onKeyDown={(event) => {
                if (languagesOpen && event.key === 'ArrowLeft') {
                  event.preventDefault();
                  backToMenu();
                }
              }}
            >
              {languagesOpen ? (
                <>
                  <DropdownMenu.Item
                    className="ui-menu-item"
                    aria-label={`${t('action.back')} | ${t('ui.language')}`}
                    onSelect={(event) => {
                      event.preventDefault();
                      backToMenu();
                    }}
                  >
                    <ChevronLeft aria-hidden="true" />
                    {t('ui.language')}
                  </DropdownMenu.Item>
                  <DropdownMenu.Separator className="ui-menu-separator" />
                  <DropdownMenu.RadioGroup
                    className="masthead-menu-language"
                    aria-label={t('ui.language')}
                    value={locale}
                    onValueChange={(value) => {
                      const select = language.current?.querySelector('select');
                      if (select) {
                        select.value = value;
                        select.dispatchEvent(new Event('change', { bubbles: true }));
                      }
                    }}
                  >
                    {UI_LOCALES.map((value) => (
                      <DropdownMenu.RadioItem key={value} className="ui-menu-item" value={value}>
                        {t(UI_LOCALE_LABELS[value])}
                        <DropdownMenu.ItemIndicator>
                          <CheckIcon />
                        </DropdownMenu.ItemIndicator>
                      </DropdownMenu.RadioItem>
                    ))}
                  </DropdownMenu.RadioGroup>
                </>
              ) : (
                <>
                  {previewFeaturesEnabled && (
                    <>
                      <DropdownMenu.Group aria-label={t('nav.account')}>
                        <DropdownMenu.Item asChild>
                          <Link className="ui-menu-item" href={withUiLanguage(props.assetUrl('profile.html'), locale)}>
                            <NavigationIcon name="profile" />
                            {t('nav.profile')}
                          </Link>
                        </DropdownMenu.Item>
                        <DropdownMenu.Item asChild>
                          <Link className="ui-menu-item" href={withUiLanguage(props.assetUrl('login.html'), locale)}>
                            {t('nav.account')}
                          </Link>
                        </DropdownMenu.Item>
                      </DropdownMenu.Group>
                      <DropdownMenu.Separator className="ui-menu-separator" />
                    </>
                  )}
                  <DropdownMenu.Group aria-label={t('nav.settings')}>
                    <DropdownMenu.Label className="masthead-menu-heading">{t('nav.settings')}</DropdownMenu.Label>
                    <DropdownMenu.CheckboxItem
                      className="ui-menu-item"
                      id={variant === 'lobby' ? 'lobbyLessMotion' : 'lessMotionToggle'}
                      checked={props.lessMotion}
                      onCheckedChange={props.onToggleMotion}
                    >
                      <ActivityLogIcon />
                      {t('nav.lessMotion')}
                      <DropdownMenu.ItemIndicator>
                        <CheckIcon />
                      </DropdownMenu.ItemIndicator>
                    </DropdownMenu.CheckboxItem>
                    {siteNoticeEnabled && (
                      <DropdownMenu.CheckboxItem
                        className="ui-menu-item"
                        id="siteNoticeToggle"
                        checked={props.noticeEnabled}
                        onCheckedChange={props.onToggleNotice}
                      >
                        <BellIcon />
                        {t('notice.label')}
                        <DropdownMenu.ItemIndicator>
                          <CheckIcon />
                        </DropdownMenu.ItemIndicator>
                      </DropdownMenu.CheckboxItem>
                    )}
                    <DropdownMenu.CheckboxItem
                      className="ui-menu-item"
                      id="runtimeDiagnosticsToggle"
                      checked={props.diagnosticsEnabled}
                      onCheckedChange={props.onToggleDiagnostics}
                    >
                      <ActivityLogIcon />
                      {t('diagnostics.toggle')}
                      <DropdownMenu.ItemIndicator>
                        <CheckIcon />
                      </DropdownMenu.ItemIndicator>
                    </DropdownMenu.CheckboxItem>
                    <DropdownMenu.Item
                      className="ui-menu-item masthead-language-trigger"
                      onSelect={(event) => {
                        event.preventDefault();
                        setLanguagesOpen(true);
                      }}
                      onKeyDown={(event) => {
                        if (event.key === 'ArrowRight') {
                          event.preventDefault();
                          setLanguagesOpen(true);
                        }
                      }}
                    >
                      <Languages aria-hidden="true" />
                      {t('ui.language')}
                      <span className="masthead-language-name">{t(UI_LOCALE_LABELS[locale])}</span>
                      <ChevronRight aria-hidden="true" />
                    </DropdownMenu.Item>
                  </DropdownMenu.Group>
                  <DropdownMenu.Separator className="ui-menu-separator" />
                  <DropdownMenu.Group aria-label={t('nav.help')}>
                    <DropdownMenu.Label className="masthead-menu-heading">{t('nav.siteInfo')}</DropdownMenu.Label>
                    {props.migrationHref && (
                      <DropdownMenu.Item asChild>
                        <a
                          className="ui-menu-item"
                          href={props.migrationHref}
                          data-policy={
                            props.migrationPolicyKnown ? 'host-manifest-origin-migration-policy/1' : undefined
                          }
                        >
                          <ReaderIcon />
                          {t('nav.oldSiteMigration')}
                        </a>
                      </DropdownMenu.Item>
                    )}
                    {previewFeaturesEnabled && (
                      <DropdownMenu.Item asChild>
                        <a
                          className="ui-menu-item"
                          href={withUiLanguage(props.assetUrl('ui-review.html'), locale)}
                          target="_blank"
                          rel="noopener noreferrer"
                        >
                          {t('nav.uiHandbook')}
                        </a>
                      </DropdownMenu.Item>
                    )}
                    <DropdownMenu.Item className="ui-menu-item" id="firstUseNoticeOpen" onSelect={props.onFirstUse}>
                      <ReaderIcon />
                      {t('nav.firstUseNotice')}
                    </DropdownMenu.Item>
                    <DropdownMenu.Item asChild>
                      <a className="ui-menu-item" href={props.faqHref}>
                        <InfoCircledIcon />
                        {t('nav.faq')}
                      </a>
                    </DropdownMenu.Item>
                    <DropdownMenu.Item asChild>
                      <a className="ui-menu-item" href={props.aboutHref}>
                        <InfoCircledIcon />
                        {t('nav.about')}
                      </a>
                    </DropdownMenu.Item>
                    <DropdownMenu.Item asChild>
                      <a className="ui-menu-item" href={withUiLanguage(props.assetUrl('uptime/'), locale)}>
                        <ActivityLogIcon />
                        {t('nav.serviceStatus')}
                      </a>
                    </DropdownMenu.Item>
                    <DropdownMenu.Item asChild>
                      <a
                        className="ui-menu-item"
                        href="https://github.com/YomotsuHisami/eagler-touhou"
                        target="_blank"
                        rel="noopener noreferrer"
                      >
                        <GitHubLogoIcon />
                        GitHub
                      </a>
                    </DropdownMenu.Item>
                    {props.donationAvailable !== false && (
                      <DropdownMenu.Item className="ui-menu-item book-menu-donation" onSelect={props.onDonation}>
                        <HeartIcon />
                        {t('nav.donate')}
                      </DropdownMenu.Item>
                    )}
                  </DropdownMenu.Group>
                </>
              )}
            </DropdownMenu.Content>
          </DropdownMenu.Portal>
        </DropdownMenu.Root>
      </nav>
    </header>
  );
}
