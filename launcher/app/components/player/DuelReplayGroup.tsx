import {useEffect, useState, type ReactNode} from 'react';
import {Download, Play} from 'lucide-react';
import {z} from 'zod';
import {useLocale} from '../../i18n';
import {th09CharacterIds, th09CharacterName} from '../../models/th09-character-ratings';
import {th09PublicUsername, type WatchTh09Replay} from '../../models/th09-replay-archive';
import {accountRequest} from '../../services/account-client';
import {useAccountSession} from '../../services/account-session';
import {Button} from '../../ui/button';
import {AccountAvatar} from '../launcher/AccountAvatar';
import {Th09CharacterPortrait} from '../launcher/Th09CharacterPortrait';
import {ParticipantAvatar} from '../room/ParticipantAvatar';
import {replayClock} from './ReplaySeekBar';
import './duel-replay-group.css';

const character = z.number().refine(id => th09CharacterIds.includes(id));
const seatUsername = th09PublicUsername.nullable().catch(null);
const seatProfile = z.object({
  username: th09PublicUsername,
  displayName: z.string().optional(),
  avatar: z.string().regex(/^[a-f0-9]{16}$/).optional().catch(undefined),
}).nullable().catch(null);
const duelReplayFiles = z.object({
  playerUsernames: z.tuple([seatUsername, seatUsername]).optional().catch(undefined),
  playerProfiles: z.tuple([seatProfile, seatProfile]).optional().catch(undefined),
  files: z.array(z.object({
    roundId: z.string().regex(/^\d{1,16}:\d{1,16}$/),
    name: z.string().min(1).max(160).regex(/^[^<>:"/\\|?*\p{Cc}\p{Cf}]+$/u).optional(),
    frames: z.number().int().positive().max(Number.MAX_SAFE_INTEGER).nullish(),
    characters: z.tuple([character, character]),
  })),
});
type ReplayList = z.infer<typeof duelReplayFiles>;
type ReplayFile = ReplayList['files'][number];

export function DuelReplayGroup({username, duelId, count, watch, summary, className, currentRoundId, disabled, open}: {
  username: string; duelId: string; count: number; watch?: WatchTh09Replay;
  summary?: ReactNode; className: string; currentRoundId?: string; disabled?: boolean; open?: boolean;
}) {
  const {t, locale} = useLocale();
  const {state: account} = useAccountSession();
  const accountLoading = account.status === 'loading';
  const viewer = account.status === 'member' ? account.user.username.toLowerCase() : undefined;
  const [selfOpen, setSelfOpen] = useState(false);
  const [attempt, setAttempt] = useState(0);
  const [loaded, setLoaded] = useState<{viewer?: string; value: ReplayList}>();
  const replays = !accountLoading && loaded?.viewer === viewer ? loaded?.value : undefined;
  const [failed, setFailed] = useState(false);
  const [busy, setBusy] = useState(false);
  const [actionError, setActionError] = useState(false);
  const endpoint = `v1/th09/duels/${duelId}/replays`;
  const expanded = open ?? selfOpen;
  const inline = open !== undefined;
  // biome-ignore lint/correctness/useExhaustiveDependencies: Retrying starts a fresh request for the same replay group.
  useEffect(() => {
    if (!expanded || accountLoading || replays) return;
    const controller = new AbortController();
    setFailed(false);
    void accountRequest(endpoint, undefined, {signal: controller.signal}).then(data => {
      if (!controller.signal.aborted) setLoaded({viewer, value: duelReplayFiles.parse(data)});
    }).catch(() => {if (!controller.signal.aborted) setFailed(true);});
    return () => controller.abort();
  }, [endpoint, expanded, attempt, replays, accountLoading, viewer]);
  function access(file: ReplayFile, action: 'play' | 'download') {
    if (!watch || busy || disabled) return;
    setBusy(true);
    setActionError(false);
    void watch({username, duelId, roundId: file.roundId, name: file.name}, action)
      .catch(() => setActionError(true)).finally(() => setBusy(false));
  }
  function renderSide(file: ReplayFile, side: 0 | 1) {
    const oldId = replays?.playerUsernames?.[side];
    const profile: z.infer<typeof seatProfile> = replays?.playerProfiles ? replays.playerProfiles[side] : oldId ? {username: oldId} : null;
    const self = !!profile && profile.username.toLowerCase() === viewer;
    return <>
      <span className="th09-replay-side" data-seat={side} data-self={self || undefined}>
        {th09CharacterName(file.characters[side], t)}
      </span>
      {profile && <ParticipantAvatar className="th09-replay-avatar" data-seat={side} data-self={self || undefined}
          profile={profile} name={profile.displayName || profile.username} navigateOnClick>
        <AccountAvatar/>
      </ParticipantAvatar>}
    </>;
  }
  function actions(file: ReplayFile, round?: string) {
    const icon = round !== undefined;
    const playLabel = t('replay.ui.play');
    const downloadLabel = t('action.download');
    const download = `/api/${endpoint}/${file.roundId.replace(':', '-')}`;
    return <div className={icon ? 'profile-round-actions' : 'th09-replay-actions'}>
      {watch && <Button type="button" variant="ghost" size={icon ? 'icon-sm' : undefined} disabled={busy || disabled}
        aria-current={currentRoundId === file.roundId || undefined} aria-label={icon ? playLabel + ' | ' + round : undefined}
        onClick={() => access(file, 'play')}>{icon ? <Play aria-hidden="true"/> : playLabel}</Button>}
      {watch ? <Button type="button" variant="ghost" size={icon ? 'icon-sm' : undefined} disabled={busy || disabled}
        aria-label={icon ? downloadLabel + ' | ' + round : undefined}
        onClick={() => access(file, 'download')}>{icon ? <Download aria-hidden="true"/> : downloadLabel}</Button>
        : icon ? <Button asChild variant="ghost" size="icon-sm"><a href={download} download={file.name}
          aria-label={downloadLabel + ' | ' + round}><Download aria-hidden="true"/></a></Button>
        : <a href={download} download={file.name}>{downloadLabel}</a>}
    </div>;
  }
  const status = <>
    {failed && <p role="alert">{t('account.requestFailed')} <Button type="button" variant="ghost" onClick={() => setAttempt(value => value + 1)}>{t('action.retry')}</Button></p>}
    {actionError && <p role="alert">{t('account.requestFailed')}</p>}
    {expanded && !failed && !replays && <p role="status">{t('replay.ui.loading')}</p>}
  </>;
  if (inline) return <div className={className} aria-busy={expanded && !failed && !replays}>
    {status}
    {replays && (replays.files.length ? <table>
      <thead><tr><th scope="col">{t('th09.roundNumber')}</th><th scope="col">{t('game.matrixLoadout')}</th>
        <th scope="col">{t('th09.roundDuration')}</th><th scope="col"><span className="sr-only">{t('replay.ui.play')} | {t('action.download')}</span></th></tr></thead>
      <tbody>{replays.files.map((file,index) => {
        const [left,right] = file.characters;
        const round = t('profile.matchRound',{round:index+1});
        return <tr key={file.roundId}>
          <th scope="row" aria-label={round}>{String(index+1).padStart(2,'0')}</th>
          <td><div className="profile-round-loadouts">
            <span><Th09CharacterPortrait character={left}/>{th09CharacterName(left,t)}</span>
            <small>vs</small>
            <span><Th09CharacterPortrait character={right}/>{th09CharacterName(right,t)}</span>
          </div></td>
          <td>{file.frames != null ? replayClock(file.frames) : '—'}</td>
          <td>{actions(file,round)}</td>
        </tr>;
      })}</tbody>
    </table> : <p>{t('profile.matchNoReplays')}</p>)}
  </div>;
  return <details className={className} onToggle={event => setSelfOpen(event.currentTarget.open)}>
    <summary>{summary ?? <>{t('replay.ui.heading')} | {t('replay.fileCount', {count})}</>}</summary>
    {status}
    {replays && <ol>
      {replays.files.map(file => <li key={file.roundId}>
        <div className="th09-replay-row">
          <div className="th09-replay-matchup">
            {renderSide(file, 0)}<span className="th09-replay-vs">vs</span>{renderSide(file, 1)}
          </div>
          {file.frames != null && <span className="th09-replay-duration">{formatDuelDuration(file.frames, locale)}</span>}
          {actions(file)}
        </div>
      </li>)}
    </ol>}
  </details>;
}

function formatDuelDuration(frames: number, locale: string) {
  const seconds = Math.floor(frames / 60);
  const minutes = Math.floor(seconds / 60);
  const values: [number, string][] = minutes
    ? [[Math.floor(minutes / 1440),'d'], [Math.floor(minutes / 60) % 24,'h'], [minutes % 60,'min']]
    : [[seconds || 1,'s']];
  const numbers = new Intl.NumberFormat(locale);
  const parts = values.filter(([value]) => value > 0);
  const prefix = seconds ? '' : '<';
  return prefix + parts.map(([value, unit]) => numbers.format(value) + unit).join(' ');
}
