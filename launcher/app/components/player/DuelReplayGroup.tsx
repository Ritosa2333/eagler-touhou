import {useEffect, useState, type ReactNode} from 'react';
import {z} from 'zod';
import {useLocale} from '../../i18n';
import {th09CharacterIds, th09CharacterName} from '../../models/th09-character-ratings';
import {th09PublicUsername, type WatchTh09Replay} from '../../models/th09-replay-archive';
import {accountRequest} from '../../services/account-client';
import {useAccountSession} from '../../services/account-session';
import {Button} from '../../ui/button';
import './duel-replay-group.css';

const character = z.number().refine(id => th09CharacterIds.includes(id));
const seatUsername = th09PublicUsername.nullable().catch(null);
const duelReplayFiles = z.object({
  playerUsernames: z.tuple([seatUsername, seatUsername]).optional().catch(undefined),
  files: z.array(z.object({
    roundId: z.string().regex(/^\d{1,16}:\d{1,16}$/),
    name: z.string().min(1).max(160).regex(/^[^<>:"/\\|?*\p{Cc}\p{Cf}]+$/u).optional(),
    frames: z.number().int().positive().max(Number.MAX_SAFE_INTEGER).nullish(),
    characters: z.tuple([character, character]),
  })),
});
type ReplayList = z.infer<typeof duelReplayFiles>;
type ReplayFile = ReplayList['files'][number];

export function DuelReplayGroup({username, duelId, count, watch, summary, className, currentRoundId, disabled}: {
  username: string; duelId: string; count: number; watch?: WatchTh09Replay;
  summary?: ReactNode; className: string; currentRoundId?: string; disabled?: boolean;
}) {
  const {t, locale} = useLocale();
  const {state: account} = useAccountSession();
  const accountLoading = account.status === 'loading';
  const viewer = account.status === 'member' ? account.user.username.toLowerCase() : undefined;
  const [request, setRequest] = useState<{endpoint: string} | null>(null);
  const [loaded, setLoaded] = useState<{viewer?: string; value: ReplayList}>();
  const replays = !accountLoading && loaded?.viewer === viewer ? loaded?.value : undefined;
  const [failed, setFailed] = useState(false);
  const [busy, setBusy] = useState(false);
  const [actionError, setActionError] = useState(false);
  const endpoint = `v1/th09/duels/${duelId}/replays`;
  useEffect(() => {
    if (!request || accountLoading || replays) return;
    const controller = new AbortController();
    setFailed(false);
    void accountRequest(request.endpoint, undefined, {signal: controller.signal}).then(data => {
      if (!controller.signal.aborted) setLoaded({viewer, value: duelReplayFiles.parse(data)});
    }).catch(() => {if (!controller.signal.aborted) setFailed(true);});
    return () => controller.abort();
  }, [request, replays, accountLoading, viewer]);
  function access(file: ReplayFile, action: 'play' | 'download') {
    if (!watch || busy || disabled) return;
    setBusy(true);
    setActionError(false);
    void watch({username, duelId, roundId: file.roundId, name: file.name}, action)
      .catch(() => setActionError(true)).finally(() => setBusy(false));
  }
  function renderSide(file: ReplayFile, side: 0 | 1) {
    const id = replays?.playerUsernames?.[side];
    const self = !!id && id.toLowerCase() === viewer;
    return <span className="th09-replay-side" data-self={self || undefined}>
      {th09CharacterName(file.characters[side], t)}{id && <> <bdi className="th09-replay-id">@{id}</bdi></>}
    </span>;
  }
  return <details className={className} onToggle={event => setRequest(event.currentTarget.open ? {endpoint} : null)}>
    <summary>{summary ?? <>{t('replay.ui.heading')} | {t('replay.fileCount', {count})}</>}</summary>
    {failed && <p role="alert">{t('account.requestFailed')} <Button type="button" variant="ghost" onClick={() => setRequest({endpoint})}>{t('action.retry')}</Button></p>}
    {actionError && <p role="alert">{t('account.requestFailed')}</p>}
    {request && !failed && !replays && <span role="status">{t('replay.ui.loading')}</span>}
    {replays && <ol>
      {replays.files.map(file => <li key={file.roundId}>
        <div className="th09-replay-row">
          <span>{renderSide(file, 0)}{' vs '}{renderSide(file, 1)}
            {file.frames != null && <> <span className="th09-replay-duration">| {formatDuelDuration(file.frames, locale)}</span></>}</span>
          <div className="th09-replay-actions">
            {watch && <Button type="button" variant="ghost" disabled={busy || disabled} aria-current={currentRoundId === file.roundId || undefined} onClick={() => access(file, 'play')}>{t('replay.ui.play')}</Button>}
            {watch ? <Button type="button" variant="ghost" disabled={busy || disabled} onClick={() => access(file, 'download')}>{t('action.download')}</Button>
              : <a href={`/api/${endpoint}/${file.roundId.replace(':', '-')}`} download>{t('action.download')}</a>}
          </div>
        </div>
      </li>)}
    </ol>}
  </details>;
}

export function formatDuelDuration(frames: number, locale: string, emphasize = false) {
  const seconds = Math.floor(frames / 60);
  const minutes = Math.floor(seconds / 60);
  const values: [number, string][] = minutes
    ? [[Math.floor(minutes / 1440),'d'], [Math.floor(minutes / 60) % 24,'h'], [minutes % 60,'min']]
    : [[seconds || 1,'s']];
  const numbers = new Intl.NumberFormat(locale);
  const parts = values.filter(([value]) => value > 0);
  const prefix = seconds ? '' : '<';
  if (!emphasize) return prefix + parts.map(([value, unit]) => numbers.format(value) + unit).join(' ');
  return <span className="profile-th09-duration-summary">{prefix}
    {parts.map(([value, unit], index) => <span key={unit}>
      {index > 0 && ' '}<strong>{numbers.format(value)}</strong>{unit}
    </span>)}
  </span>;
}
