import { useEffect, useState } from 'react';
import { useLocale } from '../../i18n';
import { Button } from '../../ui/button';
import { Dialog, DialogContent, DialogTitle, DialogTrigger } from '../../ui/dialog';
import { BOOK_PREFERENCES_EVENT, readBookPreference, stringArray, writeBookPreference } from './book-preferences';
import { MastheadTooltip } from './MastheadTooltip';
import './library-announcements.css';

interface AnnouncementCopy {
  title: string;
  summary?: string;
}

export interface LibraryAnnouncement {
  id: string;
  date: string;
  'zh-CN': AnnouncementCopy;
  en?: AnnouncementCopy;
  ja?: AnnouncementCopy;
}

const readPreference = 'announcementReadIds';

export function LibraryAnnouncements({
  entries,
  disabled = false,
  persistRead = true,
}: {
  entries: readonly LibraryAnnouncement[];
  disabled?: boolean;
  persistRead?: boolean;
}) {
  const { locale, t } = useLocale();
  const [readIds, setReadIds] = useState<string[] | null>(persistRead ? null : []);
  useEffect(() => {
    if (!persistRead || disabled) return;
    const refresh = () => setReadIds(readBookPreference(readPreference, [], stringArray));
    refresh();
    window.addEventListener(BOOK_PREFERENCES_EVENT, refresh);
    window.addEventListener('storage', refresh);
    return () => {
      window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh);
      window.removeEventListener('storage', refresh);
    };
  }, [disabled, persistRead]);
  const label = t('notice.label');
  const latest = [...entries].sort((a, b) => b.date.localeCompare(a.date)).slice(0, 3);
  const unread = readIds !== null && latest.some((entry) => !readIds.includes(entry.id));
  const triggerLabel = unread ? t('notice.unread') : label;
  const formatDate = new Intl.DateTimeFormat(locale, { dateStyle: 'medium', timeZone: 'UTC' });

  function onOpenChange(open: boolean) {
    if (!open || !latest.length) return;
    const previous = persistRead ? readBookPreference(readPreference, [], stringArray) : readIds ?? [];
    const next = [...new Set([...previous, ...latest.map((entry) => entry.id)])];
    setReadIds(next);
    if (persistRead && next.length !== previous.length) writeBookPreference(readPreference, next);
  }

  return (
    <Dialog onOpenChange={onOpenChange}>
      <MastheadTooltip label={triggerLabel}>
        <DialogTrigger asChild>
          <Button
            variant="ghost"
            size="icon"
            className="masthead-action library-announcements-trigger"
            aria-label={triggerLabel}
            disabled={disabled}
          >
            <svg aria-hidden="true" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.8" strokeLinecap="round" strokeLinejoin="round">
              <path d="M3 9h4l9-4v14l-9-4H3a1 1 0 0 1-1-1v-4a1 1 0 0 1 1-1Z" />
              <path d="M5 15l1.5 5h3L8 15.5" />
              <path className="library-announcements-wave" d="M19 9.5a4 4 0 0 1 0 5" />
              <path className="library-announcements-wave library-announcements-wave-outer" d="M21 6.7a8 8 0 0 1 0 10.6" />
            </svg>
            {unread && <span className="library-announcements-unread" aria-hidden="true" />}
          </Button>
        </DialogTrigger>
      </MastheadTooltip>
      <DialogContent className="library-announcements-dialog" closeLabel={t('notice.close')} aria-describedby={undefined}>
        <DialogTitle>{label}</DialogTitle>
        {latest.length > 0 && <ul className="library-announcements-list" role="list">
          {latest.map((entry) => {
            const translation = entry[locale];
            const { title, summary } = translation ?? entry['zh-CN'];
            const language = translation ? locale : 'zh-CN';
            return (
              <li className="library-announcement" key={entry.id}>
                <header className="library-announcement-header">
                  <h3 lang={language}>{title}</h3>
                  <time dateTime={entry.date}>{formatDate.format(new Date(entry.date))}</time>
                </header>
                {summary && <p lang={language}>{summary}</p>}
              </li>
            );
          })}
        </ul>}
      </DialogContent>
    </Dialog>
  );
}
