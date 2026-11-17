import { useMemo, type ComponentProps } from 'react';
import { useHref, useLocation, Link } from 'react-router';
import { findNeighbour, type Root } from 'fumadocs-core/page-tree';
import { RootProvider } from '@fumadocs/base-ui/provider/react-router';
import { DocsLayout } from '@fumadocs/base-ui/layouts/spacious';
import { DocsPage, DocsTitle, DocsBody } from '@fumadocs/base-ui/layouts/spacious/page';
import { marked } from 'marked';
import progressMarkdown from '../../../docs/TOUHOU_PROGRESS.md?raw';
import { StandalonePageShell } from '../standalone-pages';
import { useLocale, translate } from '../i18n';
import { withUiLanguage } from '../../src/launcher/i18n.mts';
import { multiplayerGuideTopic } from '../../lib/multiplayer-guide.mjs';
import { sitePageTitle } from '../navigation/document-locale';
import documentHistory from 'virtual:document-history';
import { helpDocuments, helpDocumentAtPath, helpDocumentLocale, type HelpDocument } from '../components/documents/catalog';
import { prepareHelpDocument, prepareMultiplayerRules } from '../components/documents/content';
import { guideTitle, multiplayerGuideTree, MultiplayerExceptions, MultiplayerGuidePicker, MultiplayerOverviewTable, MultiplayerScope, MultiplayerStart, type GuideTopic } from '../components/documents/multiplayer';
import '../components/documents/documents.css';

const progressAnchors = new Map<string, string>();
let progressHeadingCount = 0;
for (const token of marked.lexer(progressMarkdown)) {
  if (token.type !== 'heading' || token.depth < 2) continue;
  progressHeadingCount++;
  const slug = token.text.toLowerCase().replace(/[^\p{L}\p{N}\s_-]/gu, '').replace(/\s/g, '-');
  if (!progressAnchors.has(slug)) progressAnchors.set(slug, `section-${progressHeadingCount}`);
}
const progressHtml = marked.parse(progressMarkdown
  .replaceAll('../launcher/docs/assets/', 'https://raw.githubusercontent.com/Ritosa2333/eagler-touhou/main/launcher/docs/assets/')
  .replaceAll('(adaptation/README.md)', '(https://github.com/Ritosa2333/eagler-touhou/blob/main/docs/adaptation/README.md)')
  .replace(/\]\(#([^)]*)\)/g, (original, slug: string) => progressAnchors.has(slug) ? `](#${progressAnchors.get(slug)})` : original), {async: false});

export const meta = ({ location }: { location: { pathname: string } }) => {
  const topic = multiplayerGuideTopic(location.pathname);
  const title = topic ? guideTitle(topic) : (helpDocumentAtPath(location.pathname) ?? helpDocuments[1]).title;
  return [{ title: sitePageTitle(translate('zh-CN', title)) }];
};
export default function HelpRoute() {
  const { pathname } = useLocation(),
    page = helpDocumentAtPath(pathname) ?? helpDocuments[1],
    topic = multiplayerGuideTopic(pathname), title = topic ? guideTitle(topic) : page.title;
  return (
    <StandalonePageShell titleKey={title} mainClassName="documentation-shell">
      <Documentation page={page} topic={topic} />
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
function Documentation({ page, topic }: { page: HelpDocument; topic?: GuideTopic }) {
  const { pathname, search } = useLocation(),
    { locale, t } = useLocale(),
    base = useHref('/'),
    progress = page.id === 'faq' && new URLSearchParams(search).get('view') === 'progress',
    contentLocale = helpDocumentLocale(page, locale);
  const { body, toc } = useMemo(() => {
    if (progress) return prepareHelpDocument(progressHtml, base, 'zh-CN');
    if (!topic) return prepareHelpDocument(translate(contentLocale, page.content), base, locale);
    if (topic === 'start' || topic === 'th09' || topic === 'th19') return {body:null, toc:[]};
    return prepareMultiplayerRules(t('content.multiplayer'), base, locale, topic, t('guide.specific'));
  }, [page.content, contentLocale, locale, t, base, topic, progress]);
  const helpTree: Root = {
    $id: `help-${locale}`,
    name: t('document.title'),
    children: [
      { type: 'separator' as const, name: t('nav.help') },
      ...helpDocuments.filter(document => document.id !== 'about' && document.id !== 'privacy').map((document) => document.id === 'multiplayer' ? multiplayerGuideTree(t) : ({
        type: 'page' as const,
        name: t(document.title),
        url: '/' + document.path,
      })),
    ],
  };
  const tree: Root = {
    ...helpTree,
    children: [
      ...helpTree.children,
      { type: 'separator', name: t('nav.aboutSite') },
      ...helpDocuments.filter(document => document.id === 'privacy').map(document => ({type: 'page' as const, name: t(document.title), url: '/' + document.path})),
    ],
  };
  const footer = findNeighbour(helpTree, pathname.replace(/\/$/, ''));
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
        nav={{ title: tree.name, url: '/faq.html' }}
        themeSwitch={{ enabled: false }}
        searchToggle={{ enabled: false }}
        tabs={false}
      >
        <DocsPage
          key={(progress ? 'progress' : topic ?? page.id) + locale}
          className="pt-4"
          toc={toc}
          breadcrumb={{ enabled: false }}
          footer={{ items: footer }}
          tableOfContent={{ container: { className: 'pt-6' } }}
          tableOfContentPopover={{ trigger: { 'aria-label': t('document.toc'), className: 'documentation-toc-trigger' } }}
        >
          {topic ? <div className="documentation-guide-heading">
            <DocsTitle>{t(guideTitle(topic))}</DocsTitle><MultiplayerGuidePicker topic={topic} />
          </div> : <DocsTitle>{progress ? '东方作品进度' : t(page.title)}</DocsTitle>}
          <DocsBody data-help-page={page.id} data-guide-topic={topic} lang={progress ? 'zh-CN' : contentLocale}>
            {topic === 'start' && <MultiplayerStart />}
            {topic === 'coop' && <>
              <MultiplayerOverviewTable />
              <p>{t('document.multiplayer.tableNote')}</p>
            </>}
            {topic && topic !== 'start' && topic !== 'coop' && (body?.length || topic === 'th09' || topic === 'th19'
              ? <MultiplayerScope topic={topic} /> : <p>{t('guide.none')}</p>)}
            {body}
            {topic === 'coop' && <MultiplayerExceptions />}
          </DocsBody>
          {!progress && <DocumentRevision page={page} />}
        </DocsPage>
      </DocsLayout>
    </RootProvider>
  );
}
function DocumentRevision({ page }: { page: HelpDocument }) {
  const { locale, t } = useLocale(),
    revision = documentHistory[helpDocumentLocale(page, locale)]?.[page.id];
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
