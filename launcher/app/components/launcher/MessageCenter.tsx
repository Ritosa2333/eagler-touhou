import {useState} from 'react';
import {Mail, Megaphone} from 'lucide-react';
import {useLocation} from 'react-router';
import {useLocale} from '../../i18n';
import {accountsEnabled} from '../../release-features';
import {
  fetchAnnouncements, publishAnnouncement, useInboxSummary, useMessageIdentity,
  usePublishedAnnouncements, validMessageUsername, type PublishedAnnouncement,
} from '../../services/launcher-messages';
import {Button} from '../../ui/button';
import {useAccountLinks} from './AccountIdentity';
import {AppLink} from './AppLink';
import {LauncherBack} from './LauncherBack';
import {AnnouncementTimeline, useAnnouncementUnread} from './AnnouncementTimeline';
import {MessageComposer} from './MessageComposer';
import {PrivateMessages} from './PrivateMessages';
import {launcherSectionHref, type MessageSection} from './launcher-sections';
import './message-center.css';

export function MessageCenter({section, homeHref, assetUrl}: {
  section: MessageSection; homeHref: string; assetUrl: (path: string) => string;
}) {
  const {t} = useLocale();
  const {state: navigation, search} = useLocation();
  const {state, retry} = useMessageIdentity();
  const {loginHref} = useAccountLinks();
  const member = state.status === 'member' ? state.user.username : undefined;
  const feed = usePublishedAnnouncements(member);
  const announcementUnread = useAnnouncementUnread(feed?.items);
  const messageUnread = useInboxSummary(section === 'messages' ? undefined : member);
  const tabs = [
    {id: 'announcements', title: t('announcements.title'), Icon: Megaphone, unread: announcementUnread},
    {id: 'messages', title: t('messages.private'), Icon: Mail, unread: messageUnread ?? 0},
  ] as const;
  const target = new URLSearchParams(search).get('to')?.toLowerCase() ?? '';
  const initialPeer = target !== member && validMessageUsername(target) ? target : undefined;
  return <main id="main" className="message-center" aria-labelledby="message-center-heading">
    <aside className="message-center-navigation">
      <header><LauncherBack href={homeHref} replace returnToEntry/><h1 id="message-center-heading">{t('messages.center')}</h1></header>
      <nav aria-label={t('messages.center')}>
        {tabs.map(({id, title, Icon, unread}) => <AppLink key={id} href={launcherSectionHref(homeHref, id)} state={navigation} replace
          aria-current={section === id ? 'page' : undefined}>
          <Icon aria-hidden="true"/><span>{title}</span>{unread > 0 && <small>{unread}</small>}
        </AppLink>)}
      </nav>
    </aside>
    <section className="message-center-content" key={section} aria-label={section === 'messages' ? t('messages.private') : t('announcements.title')}>
      {section === 'announcements' ? <Announcements key={member ?? 'guest'} feed={feed} assetUrl={assetUrl}/> :
        member ? <PrivateMessages key={member + ':' + (initialPeer ?? '')} initialPeer={initialPeer}/> :
          <div className="message-empty">
            <Mail aria-hidden="true"/>
            <p role="status">{t(!accountsEnabled ? 'messages.unavailable' : state.status === 'error'
              ? 'messages.loadFailed' : state.status === 'loading' ? 'messages.loading' : 'messages.signIn')}</p>
            {accountsEnabled && state.status === 'guest' && <Button asChild><AppLink href={loginHref} rememberEntry>{t('account.login')}</AppLink></Button>}
            {accountsEnabled && state.status === 'error' && <Button variant="outline" onClick={retry}>{t('messages.refresh')}</Button>}
          </div>}
    </section>
  </main>;
}

function Announcements({feed, assetUrl}: {
  feed: ReturnType<typeof usePublishedAnnouncements>; assetUrl: (path: string) => string;
}) {
  const {t} = useLocale();
  const [older, setOlder] = useState<{items: PublishedAnnouncement[]; nextBefore: number | null}>();
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(false);
  const [published, setPublished] = useState(false);
  const next = older ? older.nextBefore : feed?.nextBefore;
  const entries = older ? [...new Map([...feed?.items ?? [], ...older.items].map(item => [item.id, item])).values()] : feed?.items;
  return <div className="message-announcements">
    {feed?.canPublish && <>
      <MessageComposer label="announcements.write" action="announcements.publish" limit={4000}
        send={async (body, requestId) => {setPublished(false); await publishAnnouncement(body, requestId); setPublished(true);}}/>
      {published && <p role="status">{t('announcements.published')}</p>}
    </>}
    <AnnouncementTimeline assetUrl={assetUrl} published={entries} includeLegacy={!next}/>
    {error && <p role="status" className="message-request-error">{t('messages.loadFailed')}</p>}
    {next && <Button variant="outline" disabled={loading} onClick={async () => {
      setLoading(true);
      try {
        const page = await fetchAnnouncements(next);
        setOlder(previous => ({items: [...previous?.items ?? [], ...page.items], nextBefore: page.nextBefore}));
        setError(false);
      } catch {setError(true);}
      finally {setLoading(false);}
    }}>{t('messages.older')}</Button>}
  </div>;
}
