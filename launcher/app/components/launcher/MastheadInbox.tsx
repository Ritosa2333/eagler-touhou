import {Popover} from 'radix-ui';
import {Mail, Megaphone} from 'lucide-react';
import {useLocale} from '../../i18n';
import {useInboxSummary, useMessageIdentity, usePublishedAnnouncements, type PublishedAnnouncement} from '../../services/launcher-messages';
import {Button} from '../../ui/button';
import {uiPortalHost} from '../../ui/portal';
import {AppLink} from './AppLink';
import {AnnouncementTimeline, useAnnouncementUnread} from './AnnouncementTimeline';
import {launcherSectionHref, type MessageSection} from './launcher-sections';
import {MastheadTooltip} from './MastheadTooltip';
import './message-center.css';

export function MastheadInbox({compact, interactive, homeHref, assetUrl, nativeLinks}: {
  compact: boolean; interactive: boolean; homeHref: string; assetUrl: (path: string) => string; nativeLinks?: boolean;
}) {
  const {t} = useLocale();
  const {state} = useMessageIdentity(interactive);
  const username = state.status === 'member' ? state.user.username : undefined;
  const feed = usePublishedAnnouncements(username, interactive);
  const messages = useInboxSummary(username);
  const unread = useAnnouncementUnread(feed?.items);
  const useAppLink = !nativeLinks && interactive;
  const Link = useAppLink ? AppLink : 'a';
  const entryLink = useAppLink ? {rememberEntry:true} : {};
  const items = [
    {id: 'announcements' as const, Icon: Megaphone, label: t('announcements.title'), count: unread, heading: t('announcements.latest')},
    {id: 'messages' as const, Icon: Mail, label: t('messages.private'), count: messages ?? 0, heading: t('messages.private')},
  ];
  return <nav className="masthead-inbox" aria-label={t('messages.center')}>
    {items.map(({id, Icon, label, count, heading}) => {
      const href = launcherSectionHref(homeHref, id);
      const contents = <><Icon aria-hidden="true"/>{count > 0 && <small aria-hidden="true">{count}</small>}</>;
      const accessible = count ? `${label} (${count})` : label;
      return compact || !interactive
        ? <Button key={id} asChild className="masthead-action masthead-inbox-action" variant="ghost" size="icon">
          <Link href={href} {...entryLink} aria-label={accessible} data-unread={count > 0}>{contents}</Link>
        </Button>
        : <Popover.Root key={id}>
          <MastheadTooltip label={label}><Popover.Trigger asChild>
            <Button variant="ghost" size="icon" className="masthead-action masthead-inbox-action"
              aria-label={accessible} data-unread={count > 0}>{contents}</Button>
          </Popover.Trigger></MastheadTooltip>
          <Popover.Portal container={uiPortalHost()}>
            <Popover.Content className="masthead-inbox-panel ui-dropdown-motion" aria-label={heading} align="end" sideOffset={7} collisionPadding={12}
              onOpenAutoFocus={event => event.preventDefault()}>
              <header><strong>{heading}</strong><Popover.Close asChild>
                <Link href={href} {...entryLink} className="masthead-inbox-all">{t('messages.viewAll')}</Link>
              </Popover.Close></header>
              <InboxPreview section={id} assetUrl={assetUrl} published={feed?.items} unread={messages}/>
            </Popover.Content>
          </Popover.Portal>
        </Popover.Root>;
    })}
  </nav>;
}

function InboxPreview({section, assetUrl, published, unread}: {
  section: MessageSection; assetUrl: (path: string) => string; published?: PublishedAnnouncement[]; unread?: number;
}) {
  const {t} = useLocale();
  return section === 'announcements' ? <AnnouncementTimeline recent assetUrl={assetUrl} published={published}/> :
    <p className="masthead-inbox-empty">{unread === undefined ? t('messages.viewInbox') : t('messages.unread', {count: unread})}</p>;
}
