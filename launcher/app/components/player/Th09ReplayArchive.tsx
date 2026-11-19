import {ChevronRight, Folder} from 'lucide-react';
import {useEffect, useState} from 'react';
import {z} from 'zod';
import {useLocale} from '../../i18n';
import {th09Duel, type Th09ReplayEntry, type WatchTh09Replay} from '../../models/th09-replay-archive';
import {accountRequest} from '../../services/account-client';
import {Button} from '../../ui/controls';
import {DuelReplayGroup} from './DuelReplayGroup';

const archivePage = z.object({
  duels: z.array(th09Duel).max(30),
  nextCursor: z.string().max(256).nullable(),
});
type ReplayDuel = z.infer<typeof th09Duel>;

export function Th09ReplayArchive({current, watch}: {current: Th09ReplayEntry; watch: WatchTh09Replay}) {
  const {t, locale} = useLocale();
  const {username} = current;
  const [groups, setGroups] = useState<ReplayDuel[]>([]);
  const [nextCursor, setNextCursor] = useState<string | null>(null);
  const [request, setRequest] = useState<{cursor: string | null}>({cursor: null});
  const [pending, setPending] = useState(true);
  const [failed, setFailed] = useState(false);
  const [busy, setBusy] = useState(false);
  useEffect(() => {
    const controller = new AbortController();
    setPending(true);
    setFailed(false);
    const query = request.cursor ? new URLSearchParams({cursor: request.cursor}) : undefined;
    void accountRequest('v1/profiles/' + encodeURIComponent(username) + '/th09-replays', undefined, {signal: controller.signal, query}).then(data => {
      if (controller.signal.aborted) return;
      const page = archivePage.parse(data);
      setGroups(previous => [...new Map([...previous, ...page.duels].map(duel => [duel.duelId, duel])).values()]);
      setNextCursor(page.nextCursor);
    }).catch(() => {if (!controller.signal.aborted) setFailed(true);})
      .finally(() => {if (!controller.signal.aborted) setPending(false);});
    return () => controller.abort();
  }, [username, request]);
  const access: WatchTh09Replay = async (entry, action) => {
    setBusy(true);
    try {await watch(entry, action);}
    finally {setBusy(false);}
  };
  const dates = new Intl.DateTimeFormat(locale, {dateStyle: 'medium'});
  return <aside className="replay-file-browser" data-profile-replays aria-label={t('replay.ui.files')}>
    <header><h2>{t('replay.ui.heading')} | @{username}</h2></header>
    <div className="replay-workspace-files" aria-busy={pending || busy}>
      {groups.map(group => !!group.replayCount && <DuelReplayGroup key={group.duelId} className="replay-archive-set"
        username={username} duelId={group.duelId} count={group.replayCount} watch={access} disabled={busy}
        currentRoundId={current.duelId === group.duelId ? current.roundId : undefined}
        summary={<>
          <Folder aria-hidden="true"/>
          <span className="replay-file-description">
            <span className="replay-file-heading"><strong>{group.opponent?.displayName ?? t('th09.guestOpponent')}</strong><span>{group.wins}:{group.losses}</span></span>
            <small><time dateTime={new Date(group.playedAt).toISOString()}>{dates.format(group.playedAt)}</time> | {t('replay.fileCount', {count: group.replayCount})}</small>
          </span>
          <ChevronRight className="replay-archive-chevron" aria-hidden="true"/>
        </>}/>)}
      {pending && <p className="replay-workspace-empty" role="status">{t('replay.ui.loading')}</p>}
      {failed && <div className="replay-workspace-empty" role="alert">{t('account.requestFailed')}{' '}
        <Button type="button" onClick={() => setRequest({...request})}>{t('action.retry')}</Button>
      </div>}
      {!pending && !failed && (!groups.length && !nextCursor
        ? <p className="replay-workspace-empty">{t('replay.ui.empty')}</p>
        : nextCursor && <Button type="button" onClick={() => setRequest({cursor: nextCursor})}>{t('th09.moreDuels')}</Button>)}
    </div>
  </aside>;
}
