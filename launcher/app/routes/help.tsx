import { useMemo, type ComponentProps } from 'react';
import { useHref, useLocation, Link } from 'react-router';
import { RootProvider } from 'fumadocs-ui/provider/react-router';
import { DocsLayout } from 'fumadocs-ui/layouts/docs';
import { DocsPage, DocsTitle, DocsBody } from 'fumadocs-ui/layouts/docs/page';
import { StandalonePageShell } from '../standalone-pages';
import { useLocale, translate } from '../i18n';
import { withUiLanguage, localeHome } from '../../src/launcher/i18n.mts';
import { NavigationIcon } from '../components/launcher/NavigationIcon';
import documentHistory from 'virtual:document-history';
import { helpDocuments, helpDocumentAtPath, type HelpDocument } from '../components/documents/catalog';
import { prepareHelpDocument } from '../components/documents/content';
import '../components/documents/documents.css';

export const meta = ({ location }: { location: { pathname: string } }) => [
  { title: `${translate('zh-CN', (helpDocumentAtPath(location.pathname) ?? helpDocuments[1]).title)} | EAGLER TOUHOU` },
];
export default function HelpRoute() {
  const { pathname } = useLocation(),
    page = helpDocumentAtPath(pathname) ?? helpDocuments[1];
  return (
    <StandalonePageShell titleKey={page.title} mainClassName="documentation-shell">
      <Documentation page={page} />
    </StandalonePageShell>
  );
}
function DocumentLink({ href = '', prefetch, ...props }: ComponentProps<'a'> & { prefetch?: boolean }) {
  const { locale } = useLocale();
  const local =
    href.startsWith('#') ||
    (href.startsWith('/') && !!helpDocumentAtPath(new URL(href, 'https://launcher.invalid').pathname));
  return local ? (
    <Link
      {...props}
      to={href.startsWith('#') ? href : withUiLanguage(href, locale)}
      prefetch={prefetch ? 'intent' : 'none'}
    />
  ) : (
    <a {...props} href={href.startsWith('/') ? withUiLanguage(href, locale) : href} />
  );
}
function Documentation({ page }: { page: HelpDocument }) {
  const { locale, t } = useLocale(),
    base = useHref('/');
  const { body, toc } = useMemo(
    () => prepareHelpDocument(page.id === 'privacy' ? translate('zh-CN', page.content) : t(page.content), base, locale),
    [page.content, locale, t, base],
  );
  const tree = {
    $id: `help-${locale}`,
    name: t('nav.help'),
    children: helpDocuments.map((document) => ({
      type: 'page' as const,
      name: t(document.title),
      url: '/' + document.path,
    })),
  };
  const labels = {
    'On this page(table of contents)': t('document.toc'),
    'Next Page(pagination)': t('document.nextPage'),
    'Previous Page(pagination)': t('document.previousPage'),
    'Collapse Sidebar(sidebar)(aria-label)': t('document.collapseNavigation'),
    'Open Sidebar(sidebar)(aria-label)': t('document.openNavigation'),
    'Close Sidebar(sidebar)(aria-label)': t('document.closeNavigation'),
  };
  return (
    <RootProvider
      components={{ Link: DocumentLink }}
      theme={{ enabled: false }}
      search={{ enabled: false }}
      i18n={{ locale, translations: labels }}
    >
      <DocsLayout
        tree={tree}
        nav={{ title: t('nav.help'), url: '/faq.html', enabled: false }}
        themeSwitch={{ enabled: false }}
        searchToggle={{ enabled: false }}
        sidebar={{ collapsible: false }}
        tabs={false}
      >
        <DocsPage
          key={page.id + locale}
          toc={toc}
          breadcrumb={{ enabled: false }}
          tableOfContent={{ style: 'normal' }}
          tableOfContentPopover={{
            header: <MobileDocumentNavigation />,
            trigger: { 'aria-label': `${t('nav.help')} | ${t(page.title)}` },
          }}
        >
          <DocsTitle>{t(page.title)}</DocsTitle>
          <DocsBody data-help-page={page.id} lang={page.id === 'privacy' ? 'zh-CN' : undefined}>{body}</DocsBody>
          <DocumentRevision page={page} />
        </DocsPage>
      </DocsLayout>
    </RootProvider>
  );
}
function DocumentRevision({ page }: { page: HelpDocument }) {
  const { locale, t } = useLocale(),
    revision = documentHistory[locale]?.[page.id];
  if (!revision) return null;
  if (revision.pending) return <p className="documentation-revision">{t('document.uncommitted')}</p>;
  if (!revision.commit || !revision.date || !revision.author) return null;
  return (
    <p className="documentation-revision">
      {t('document.lastModified')}{' '}
      <a
        href={`https://github.com/YomotsuHisami/eagler-touhou/commit/${revision.commit}`}
        target="_blank"
        rel="noopener noreferrer"
      >
        {revision.author}
      </a>{' '}
      |{' '}
      <time dateTime={revision.date}>
        {new Intl.DateTimeFormat(locale, {
          year: 'numeric',
          month: 'short',
          day: 'numeric',
          timeZone: 'Asia/Singapore',
        }).format(new Date(revision.date))}
      </time>
    </p>
  );
}
function MobileDocumentNavigation() {
  const { locale, t } = useLocale(),
    base = useHref('/');
  return (
    <div className="documentation-mobile-links">
      <nav className="documentation-site-links" aria-label={t('nav.main')}>
        <DocumentLink href={base + localeHome(locale)}>
          <NavigationIcon name="single" />
          {t('nav.single')}
        </DocumentLink>
        <DocumentLink href={base + 'lobby.html'}>
          <NavigationIcon name="multiplayer" />
          {t('nav.lobby')}
        </DocumentLink>
      </nav>
      <nav className="documentation-page-links" aria-label={t('nav.help')}>
        {helpDocuments.map((page) => (
          <DocumentLink key={page.id} href={base + page.path}>
            {t(page.title)}
          </DocumentLink>
        ))}
      </nav>
    </div>
  );
}
