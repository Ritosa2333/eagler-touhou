import {useEffect, useLayoutEffect, useMemo, useRef, useState, useSyncExternalStore} from 'react';
import {useLocale, type UiMessageKey} from '../../i18n';
import {announcementKey, type PublishedAnnouncement} from '../../services/launcher-messages';
import {Button} from '../../ui/button';
import {PublicAccountAvatar} from './AccountAvatar';
import {BOOK_PREFERENCES_EVENT, readBookPreference, stringArray, writeBookPreference} from './book-preferences';
import {announcements, type Announcement} from './announcements';

const seenKey = 'announcements-read';
const noPublished: readonly PublishedAnnouncement[] = [];
const unread = (published: readonly PublishedAnnouncement[]) => {
  const seen = new Set(readBookPreference(seenKey, [], stringArray));
  return announcements.filter(item => !seen.has(item.id)).length
    + published.filter(item => !seen.has(announcementKey(item.id))).length;
};
function subscribeUnread(update: () => void) {
  window.addEventListener(BOOK_PREFERENCES_EVENT, update);
  window.addEventListener('storage', update);
  return () => {
    window.removeEventListener(BOOK_PREFERENCES_EVENT, update);
    window.removeEventListener('storage', update);
  };
}
export function useAnnouncementUnread(published = noPublished) {
  return useSyncExternalStore(subscribeUnread, () => unread(published), () => 0);
}

function RelativeTime({published}: {published: string}) {
  const {t, locale} = useLocale();
  const [now, setNow] = useState<number>();
  useEffect(() => {setNow(Date.now());}, []);
  const date = new Date(published);
  if (now === undefined) return <time dateTime={published}>{date.toISOString().slice(0, 10)}</time>;
  const age = Math.max(0, now - date.valueOf());
  const format = new Intl.RelativeTimeFormat(locale, {numeric: 'always'});
  return <time dateTime={published}>{age >= 86400000 ? format.format(-Math.floor(age / 86400000), 'day') :
    age >= 3600000 ? format.format(-Math.floor(age / 3600000), 'hour') :
    age >= 60000 ? format.format(-Math.floor(age / 60000), 'minute') : t('announcements.justNow')}</time>;
}

function AnnouncementCopy({entry, lines}: {entry: Announcement; lines: 6 | 10}) {
  const {t} = useLocale();
  const content = useRef<HTMLDivElement>(null);
  const [expanded, setExpanded] = useState(false);
  const [height, setHeight] = useState({full: 0, limit: 0});
  useLayoutEffect(() => {
    const node = content.current;
    if (!node) return;
    const measure = () => {
      const full = Math.ceil(node.getBoundingClientRect().height);
      const limit = parseFloat(getComputedStyle(node).lineHeight) * lines;
      setHeight(previous => previous.full === full && previous.limit === limit ? previous : {full, limit});
    };
    measure();
    const observer = new ResizeObserver(measure);
    observer.observe(node);
    return () => observer.disconnect();
  }, [lines]);
  const overflow = height.full > height.limit + 1;
  const collapsed = overflow && !expanded;
  const action = (key: UiMessageKey) => <Button variant="link" className="announcement-more"
    aria-expanded={!collapsed} onClick={() => setExpanded(collapsed)}>{t(key)}</Button>;
  return <div className="announcement-copy" data-collapsed={collapsed}>
    <div className="announcement-clip" style={{height: height.full ? (collapsed ? height.limit : height.full) : undefined}}>
      <div ref={content} className="announcement-copy-content">
        {'body' in entry ? <p className="announcement-body">{entry.body}</p> : entry.groups.map(group => <div key={group.points.join('|')}>
          {group.type && <strong lang="en">{group.type}</strong>}
          <ul>{group.points.map(point => <li key={point}>{t(point)}</li>)}</ul>
        </div>)}
      </div>
    </div>
    {overflow && action(collapsed ? 'announcements.more' : 'announcements.less')}
  </div>;
}

const timestamp = (value: string) => new Date(Date.parse(value) + 8 * 3600_000)
  .toISOString().slice(0, 19).replaceAll('-', '.').replace('T', ' ') + ' (UTC+8)';

export function AnnouncementTimeline({recent = false, published = noPublished, includeLegacy = true, assetUrl}: {
  recent?: boolean; published?: readonly PublishedAnnouncement[]; includeLegacy?: boolean; assetUrl: (path: string) => string;
}) {
  const {t} = useLocale();
  const entries = useMemo((): readonly Announcement[] => {
    const live = published.map(item => ({
      ...item, id: announcementKey(item.id), published: new Date(item.publishedAt).toISOString(),
    }));
    const oldestLive = live.at(-1)?.published;
    const bundled = includeLegacy || !oldestLive ? announcements :
      announcements.filter(item => item.published >= oldestLive);
    const sorted = [...live, ...bundled].sort((a, b) => b.published.localeCompare(a.published));
    return recent ? sorted.slice(0, 1) : sorted;
  }, [published, recent, includeLegacy]);
  useEffect(() => {
    const seen = new Set(readBookPreference(seenKey, [], stringArray));
    const readCount = seen.size;
    for (const item of entries) seen.add(item.id);
    if (seen.size !== readCount) writeBookPreference(seenKey, [...seen]);
  }, [entries]);
  return <ol className="announcement-timeline" data-density={recent ? 'recent' : 'full'}>
    {entries.map(entry => <li key={entry.id} className="announcement-entry">
      <PublicAccountAvatar username={entry.authorUsername} assetUrl={assetUrl} loading="lazy"/>
      <article>
        <RelativeTime published={entry.published}/>
        <AnnouncementCopy entry={entry} lines={recent ? 6 : 10}/>
        <footer>{t('announcements.byline', {author: entry.author, time: timestamp(entry.published)})}</footer>
      </article>
    </li>)}
  </ol>;
}
