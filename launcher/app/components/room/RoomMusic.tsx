import {
  ArrowRight,
  Check,
  Disc3,
  ListMusic,
  LoaderCircle,
  LogOut,
  MoreHorizontal,
  Music2,
  Pause,
  Play,
  Plus,
  Repeat,
  Repeat1,
  RotateCw,
  Search,
  Shuffle,
  SkipBack,
  SkipForward,
  Volume2,
  X,
} from 'lucide-react';
import { type CSSProperties, useEffect, useRef, useState, useSyncExternalStore } from 'react';
import { useLocale } from '../../i18n';
import { type MusicPlatform, type MusicProvider, type MusicTrack, musicPlaylistId } from '../../services/music-provider';
import type { MusicError, MusicLookup, RoomMusic } from '../../services/room-music';
import { Button } from '../../ui/controls';
import { Dialog, DialogContent, DialogDescription, DialogTitle } from '../../ui/dialog';
import { InputGroup, InputGroupInput } from '../../ui/input-group';
import { Popover, PopoverClose, PopoverContent, PopoverTrigger } from '../../ui/popover';
import { Slider } from '../../ui/slider';
import { Tabs, TabsContent, TabsList, TabsTrigger } from '../../ui/tabs';
import { MainSelect } from '../launcher/MainSelect';
import './room-music.css';
import '../../ui/menu-motion.css';

const time = (seconds: number) =>
  `${Math.floor(Math.max(0, seconds) / 60)}:${String(Math.floor(Math.max(0, seconds) % 60)).padStart(2, '0')}`;
export type MusicPanelTab = 'search' | 'queue';
const tracksPerPage = 30;
const paletteCache = new Map<string, string[]>();
function MusicCover({ url, onColors }: { url?: string; onColors?(colors: string[]): void }) {
  const [cors, setCors] = useState(!!onColors),
    [failed, setFailed] = useState(false);
  return url && !failed ? (
    <img
      className="room-music-cover"
      src={url}
      alt=""
      loading={onColors ? 'eager' : 'lazy'}
      crossOrigin={cors ? 'anonymous' : undefined}
      referrerPolicy="no-referrer"
      onError={() => {
        if (cors) setCors(false);
        else setFailed(true);
      }}
      onLoad={async (event) => {
        if (!cors || !onColors) return;
        const img = event.currentTarget,
          cached = paletteCache.get(url);
        if (cached) {
          onColors(cached);
          return;
        }
        try {
          const { FastAverageColor } = await import('fast-average-color');
          const sampler = new FastAverageColor();
          try {
            const width = Math.floor(img.naturalWidth / 2);
            const colors = [0, width].map((left) => {
              const color = sampler.getColor(img, { algorithm: 'dominant', mode: 'speed', left, width, silent: true });
              if (color.error) throw color.error;
              // The theme blends this raw accent with its own surface color.
              return `rgb(${color.value.slice(0, 3).join(',')})`;
            });
            const oldest = paletteCache.keys().next().value;
            if (paletteCache.size >= 32 && oldest) paletteCache.delete(oldest);
            paletteCache.set(url, colors);
            if (img.isConnected) onColors(colors);
          } finally {
            sampler.destroy();
          }
        } catch {}
      }}
    />
  ) : (
    <span className="room-music-cover room-music-cover-empty">
      <Music2 aria-hidden="true" />
    </span>
  );
}

export function RoomMusicInvite({ model, panelOpen }: { model: RoomMusic; panelOpen: boolean }) {
  const { t } = useLocale();
  const invite = useSyncExternalStore(model.subscribe, model.getPendingInvitation, model.getPendingInvitation);
  const focusReturn = useRef<HTMLElement | null>(null);
  const cancel = useRef<HTMLButtonElement>(null);
  const session = invite?.session;
  const track = invite?.queue?.[invite.queueIndex ?? 0]?.name;
  const open = !!session;
  const anchor = panelOpen ? document.getElementById('mpRoomPanel') : null;

  return (
    <Dialog open={open} onOpenChange={next => { if (!next) model.declineInvitation(); }}>
      <DialogContent
        portalAnchor={anchor}
        showClose={false}
        onOpenAutoFocus={event => {
          event.preventDefault();
          focusReturn.current = document.activeElement instanceof HTMLElement ? document.activeElement : null;
          cancel.current?.focus({ preventScroll: true });
        }}
        onCloseAutoFocus={event => {
          event.preventDefault();
          focusReturn.current?.focus({ preventScroll: true });
        }}
      >
        <DialogTitle>{t('music.offer')}</DialogTitle>
        <DialogDescription className="break-words">{track || t('music.invitePrompt')}</DialogDescription>
        <div className="flex justify-end gap-2">
          <Button ref={cancel} type="button" onClick={model.declineInvitation}>{t('action.cancel')}</Button>
          <Button variant="primary" type="button" onClick={() => {
            if (model.getPendingInvitation()?.session === session) model.requestSync();
          }}>{t('music.join')}</Button>
        </div>
      </DialogContent>
    </Dialog>
  );
}

function TrackArtwork({ provider, track, url }: { provider: MusicProvider; track: MusicTrack; url?: string }) {
  const { platform, id } = track;
  const element = useRef<HTMLSpanElement>(null);
  const [thumbnail, setThumbnail] = useState<string>();
  useEffect(() => {
    if (url || thumbnail || !provider.cover || !element.current || typeof IntersectionObserver === 'undefined') return;
    const controller = new AbortController();
    const observer = new IntersectionObserver(([entry]) => {
      if (!entry.isIntersecting) return;
      observer.disconnect();
      void provider.cover!({ platform, id }, controller.signal).then((cover) => {
        if (!controller.signal.aborted) setThumbnail(cover);
      }).catch(() => {});
    });
    observer.observe(element.current);
    return () => { observer.disconnect(); controller.abort(); };
  }, [provider, platform, id, url, thumbnail]);
  const artwork = url ?? thumbnail;
  return <span className="room-music-artwork" ref={element}>
    <MusicCover key={artwork ?? 'empty'} url={artwork} />
    <span className="room-music-artwork-action"><Play size={18} fill="currentColor" aria-hidden="true" /></span>
  </span>;
}

export function RoomMusicToggle({ model, open, onToggle }: {
  model: RoomMusic;
  open: boolean;
  onToggle(): void;
}) {
  const { t } = useLocale();
  const offered = useSyncExternalStore(model.subscribe, () => !!model.getPendingInvitation());
  const playing = useSyncExternalStore(model.subscribe, () => model.getSnapshot().status === 'playing', () => false);
  return (
    <Button variant="quiet" size="sm" className="room-workspace-music-toggle" aria-label={t('settings.music')} title={t('settings.music')} data-playing={playing} aria-expanded={open}
      aria-controls={open ? 'roomMusicPlayer' : undefined} onClick={onToggle}>
      <svg className="room-music-record" viewBox="0 0 32 32" fill="none" aria-hidden="true">
        <circle cx="14" cy="17" r="11"/><circle cx="14" cy="17" r="7"/><circle cx="14" cy="17" r="2"/>
        <g className="room-music-tonearm"><circle cx="26" cy="5" r="2"/><path d="M26 7v6l-5 5"/><path d="m19.5 17 2.5 2.5-2 2-2.5-2.5z"/></g>
      </svg>
      {offered && <span className="room-music-invite-dot" role="status" aria-label={t('music.offer')} />}
    </Button>
  );
}

export function RoomMusicStrip({
  model,
  open,
  onOpen,
  embedded = false,
}: {
  model: RoomMusic;
  open: boolean;
  onOpen(element: HTMLElement, tab: MusicPanelTab, keyboard: boolean): void;
  embedded?: boolean;
}) {
  const { t } = useLocale(),
    state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const [preview, setPreview] = useState<{ key: string; seconds: number } | null>(null),
    [tint, setTint] = useState<{ url?: string; colors: string[] } | null>(null);
  const current = state.current || state.selected,
    session = state.session;
  const active = session?.startedAt != null;
  const failed = !!state.error;
  const loading = state.status === 'loading';
  const playing = active && session.pausedAt == null && state.status !== 'ended' && !failed;
  const enabled = state.inRoom;
  const readOnly = state.joined && !state.seated;
  const {syncState} = state;
  const syncLabel = t(syncState === 'offered' ? 'music.join' :
    syncState === 'shared' ? 'music.leave' : 'music.title');
  const canSync = state.enabled && state.connected &&
    (syncState !== 'solo' || (state.seated && active && !!state.duration && !loading && !failed));
  const repeat = session?.repeat ?? 'off';
  const repeatLabel = t(`music.repeat.${repeat}`);
  const label = t(failed ? 'music.retry' : playing ? 'music.pause' : 'music.play');
  const progressKey = `${session?.session}:${session?.playback}:${session?.revision}`;
  const interactionKey = useRef(progressKey);
  const shown = preview?.key === progressKey && enabled && !failed ? preview.seconds : state.seekTarget ?? state.position;
  const style =
    tint && tint.url === current?.cover
      ? ({ '--music-tint-a': tint.colors[0], '--music-tint-b': tint.colors[1] } as CSSProperties)
      : undefined;
  const heading = (
    <span className="room-music-identity" key={`${current?.platform}:${current?.id}`}>
      <MusicCover
        key={current?.cover ?? 'empty'}
        url={current?.cover}
        onColors={(colors) => setTint({ url: current?.cover, colors })}
      />
      <span className="room-music-track">
        <strong>{current?.name || t('music.title')}</strong>
        <small>{current?.artist || t('music.choose')}</small>
      </span>
    </span>
  );
  return (
    <section id={embedded ? undefined : 'roomMusicPlayer'} className="room-music-strip" aria-label={t('music.title')} style={style}>
      <div className="room-music-heading">
        {embedded || readOnly ? (
          heading
        ) : (
          <Button
            variant="unstyled"
            className="room-music-trigger"
            aria-haspopup="dialog"
            aria-controls="mpRoomPanel"
            aria-expanded={open}
            onClick={(event) => onOpen(event.currentTarget, 'search', event.detail === 0)}
          >
            {heading}
          </Button>
        )}
        <Button
          variant={syncState === 'solo' ? 'quiet' : 'action'}
          size="sm"
          aria-label={syncState === 'solo' ? undefined : `${t('music.title')} | ${syncLabel}`}
          title={syncState === 'solo' && !canSync
            ? t(!state.enabled ? 'music.disabled' : !state.seated ? 'music.seatedOnly' : 'music.choose')
            : syncLabel}
          aria-pressed={syncState === 'shared'}
          disabled={!canSync}
          onClick={syncState === 'shared' ? model.leave : model.requestSync}
        >
          {t('music.title')}
        </Button>
        {!embedded && !readOnly && (
          <Button
            variant="quiet"
            size="icon-sm"
            aria-label={t('music.queue')}
            title={t('music.queue')}
            onClick={(event) => onOpen(event.currentTarget, 'queue', event.detail === 0)}
          >
            <ListMusic aria-hidden="true" />
          </Button>
        )}
        <Popover>
          <PopoverTrigger asChild>
            <Button variant="quiet" size="icon-sm" aria-label={t('music.options')} title={t('music.options')}>
              <MoreHorizontal aria-hidden="true" />
            </Button>
          </PopoverTrigger>
          <PopoverContent className="room-music-volume-popup ui-dropdown-motion" align="end">
            <div className="room-music-volume-control">
              <div className="room-music-volume-label">
                <Volume2 aria-hidden="true" size={16} />
                <span>{t('music.volume')}</span>
                <output>{Math.round(state.volume * 100)}%</output>
              </div>
              <Slider
                aria-label={t('music.volume')}
                value={state.volume}
                min={0}
                max={1}
                step={0.05}
                onValueChange={model.setVolume}
              />
            </div>
            {syncState === 'shared' && (
              <div className="room-music-leave-row">
                <PopoverClose asChild>
                  <Button variant="quiet" size="sm" onClick={model.leave}>
                    <LogOut size={16} aria-hidden="true" />
                    {t('music.leave')}
                  </Button>
                </PopoverClose>
              </div>
            )}
          </PopoverContent>
        </Popover>
      </div>
      <div className="room-music-progress">
        <Slider
          aria-label={t('music.progress')}
          aria-valuetext={`${time(shown)} / ${time(state.duration)}`}
          min={0}
          max={Math.max(1, state.duration)}
          step={0.1}
          value={Math.min(shown, state.duration)}
          disabled={readOnly || !enabled || !active || loading || !state.duration}
          onPointerDownCapture={() => {
            interactionKey.current = progressKey;
          }}
          onKeyDownCapture={() => {
            interactionKey.current = progressKey;
          }}
          onValueChange={(seconds) => setPreview({ key: progressKey, seconds })}
          onValueCommit={(seconds) => {
            if (interactionKey.current === progressKey) model.seek(seconds);
            setPreview(null);
          }}
          onValueCancel={() => setPreview(null)}
        />
        <div className="room-music-times">
          <span>{time(shown)}</span>
          <span>-{time(state.duration - shown)}</span>
        </div>
      </div>
      {!readOnly && <div className="room-music-transport">
        <Button
          variant="quiet"
          size="icon-sm"
          aria-label={t('music.shuffle')}
          title={t('music.shuffle')}
          aria-pressed={session?.shuffle ?? false}
          disabled={!enabled || !active}
          onClick={() => model.setShuffle(!session?.shuffle)}
        >
          <Shuffle aria-hidden="true" />
        </Button>
        <Button
          variant="quiet"
          size="icon-sm"
          aria-label={t('music.previous')}
          title={t('music.previous')}
          disabled={!enabled || !active}
          onClick={model.previous}
        >
          <SkipBack aria-hidden="true" fill="currentColor" />
        </Button>
        <Button
          variant="quiet"
          size="icon"
          className="room-music-play"
          aria-label={label}
          title={label}
          disabled={!enabled || loading || (!session && !state.selected)}
          onClick={() => {
            if (failed) model.retry();
            else if (active) model.togglePlayback();
            else if (state.selected) model.choose(state.selected);
          }}
        >
          {loading ? (
            <LoaderCircle className="room-music-loading" aria-hidden="true" />
          ) : failed ? (
            <RotateCw aria-hidden="true" />
          ) : (
            <span className="room-music-play-state" data-playing={!!playing} aria-hidden="true">
              <Play fill="currentColor" />
              <Pause fill="currentColor" />
            </span>
          )}
        </Button>
        <Button
          variant="quiet"
          size="icon-sm"
          aria-label={t('music.next')}
          title={t('music.next')}
          disabled={!enabled || !active || !session?.hasNext}
          onClick={model.next}
        >
          <SkipForward aria-hidden="true" fill="currentColor" />
        </Button>
        <Button
          variant="quiet"
          size="icon-sm"
          aria-label={repeatLabel}
          title={repeatLabel}
          aria-pressed={repeat !== 'off'}
          disabled={!enabled || !active}
          onClick={() => model.setRepeat(repeat === 'off' ? 'all' : repeat === 'all' ? 'one' : 'off')}
        >
          {repeat === 'one' ? <Repeat1 aria-hidden="true" /> : <Repeat aria-hidden="true" />}
        </Button>
      </div>}
      {state.error && (
        <p className="room-music-feedback" role="alert">
          {t(`music.${state.error}`)}
          {readOnly && <Button size="sm" variant="quiet" onClick={model.retry}>{t('music.retry')}</Button>}
        </p>
      )}
    </section>
  );
}

export function RoomMusicPanel({
  model,
  hidden,
  tab,
  onTabChange,
}: {
  model: RoomMusic;
  hidden: boolean;
  tab: MusicPanelTab;
  onTabChange(tab: MusicPanelTab): void;
}) {
  const { t } = useLocale(),
    state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const [platform, setPlatform] = useState<MusicPlatform>(state.selected?.platform || 'netease');
  const [mode, setMode] = useState<'search' | 'playlist'>('search');
  const [query, setQuery] = useState(''),
    [results, setResults] = useState<MusicTrack[]>([]);
  const [busy, setBusy] = useState(false),
    [error, setError] = useState<MusicError | null>(null),
    [visibleCount, setVisibleCount] = useState(tracksPerPage);
  const pending = useRef<AbortController | null>(null),
    searchKey = useRef(''),
    queryInput = useRef<HTMLInputElement>(null);
  const readOnly = state.joined && !state.seated;
  const enabled = state.inRoom && !readOnly;
  useEffect(() => {
    if (hidden || !enabled) {
      pending.current?.abort();
      pending.current = null;
      setBusy(false);
    }
    return () => {
      pending.current?.abort();
      pending.current = null;
    };
  }, [hidden, enabled]);
  function clearSearch() {
    pending.current?.abort();
    pending.current = null;
    searchKey.current = '';
    setBusy(false);
    setError(null);
    setResults([]);
    setVisibleCount(tracksPerPage);
  }
  async function search(entry: MusicLookup = { platform, mode, query }) {
    const { platform, mode, query } = entry;
    if (!query.trim() || !enabled) return;
    pending.current?.abort();
    const controller = new AbortController();
    pending.current = controller;
    const timeout = setTimeout(() => controller.abort(), 25000);
    controller.signal.addEventListener('abort', () => clearTimeout(timeout), { once: true });
    searchKey.current = query.trim();
    setBusy(true);
    setError(null);
    try {
      const value = mode === 'playlist' ? musicPlaylistId(platform, query) : query.trim();
      model.rememberLookup({ platform, mode, query: value });
      const found = mode === 'playlist'
        ? await model.provider.playlist(platform, value, controller.signal)
        : await model.provider.search(platform, value, controller.signal);
      if (controller.signal.aborted || pending.current !== controller) return;
      setResults(found);
      setVisibleCount(tracksPerPage);
    } catch (failure) {
      if (pending.current === controller)
        setError(failure instanceof Error && failure.message === 'invalidPlaylist' ? 'invalidPlaylist' : 'unavailable');
    } finally {
      clearTimeout(timeout);
      if (pending.current === controller) setBusy(false);
    }
  }
  const showHistory = !searchKey.current && state.lookups.length > 0;
  const queue = state.session?.queue ?? [];
  const sameTrack = (a: MusicTrack, b?: MusicTrack | null) => a.platform === b?.platform && a.id === b.id;
  function trackRow(track: MusicTrack, index: number, queued: boolean) {
    const current = sameTrack(track, state.current);
    const added = queue.some((item) => sameTrack(item, track));
    const artwork =
      track.cover ||
      (current ? state.current?.cover : undefined) ||
      (sameTrack(track, state.selected) ? state.selected?.cover : undefined) ||
      results.find((item) => sameTrack(item, track))?.cover;
    const action = t('music.select');
    return (
      <li key={`${track.platform}:${track.id}`} aria-current={current ? 'true' : undefined}>
        <Button
          variant="unstyled"
          className="room-music-row"
          disabled={!enabled}
          aria-label={`${action} | ${track.name || track.id} | ${track.artist}`}
          onClick={() => model.choose(track)}
        >
          <TrackArtwork provider={model.provider} track={track} url={artwork} />
          <span className="room-music-track">
            <strong>{track.name || track.id}</strong>
            <small>
              {[track.artist, track.album !== track.name ? track.album : undefined].filter(Boolean).join(' | ')}
            </small>
          </span>
        </Button>
        {queued && <small className="room-music-platform-name">{t(`music.${track.platform}`)}</small>}
        {queued ? (
          <Button
            variant="quiet"
            size="icon"
            aria-label={`${t(current ? 'music.playing' : 'music.remove')} | ${track.name}`}
            disabled={!enabled || index === state.session?.queueIndex}
            onClick={() => model.remove(index)}
          >
            {current ? <Disc3 aria-hidden="true" /> : <X aria-hidden="true" />}
          </Button>
        ) : (
          <Button
            variant="quiet"
            size="icon"
            disabled={!enabled || added}
            aria-label={`${t(added ? 'music.queued' : 'music.addQueue')} | ${track.name}`}
            title={t(added ? 'music.queued' : 'music.addQueue')}
            onClick={() => model.enqueue(track)}
          >
            {added ? <Check aria-hidden="true" /> : <Plus aria-hidden="true" />}
          </Button>
        )}
      </li>
    );
  }
  return (
    <section className="room-music-panel" hidden={hidden}>
      {!readOnly && <Tabs className="room-music-library" value={tab} onValueChange={(value) => onTabChange(value as MusicPanelTab)}>
        <TabsList aria-label={t('music.source')}>
          <TabsTrigger value="search">
            <Search size={16} aria-hidden="true" />
            {t('music.browse')}
          </TabsTrigger>
          <TabsTrigger value="queue">
            <ListMusic size={16} aria-hidden="true" />
            {t('music.queue')}
            <span className="room-music-count">{queue.length}</span>
          </TabsTrigger>
        </TabsList>
        <TabsContent className="room-music-library-content" value="queue">
          {queue.length ? (
            <ul className="room-music-results room-music-queue">
              {queue.map((track, index) => trackRow(track, index, true))}
            </ul>
          ) : (
            <div className="room-music-empty">
              <ListMusic size={32} strokeWidth={1.3} aria-hidden="true" />
              <p>{t('music.queueEmpty')}</p>
              <Button variant="quiet" size="sm" onClick={() => onTabChange('search')}>
                {t('music.choose')}
              </Button>
            </div>
          )}
        </TabsContent>
        <TabsContent className="room-music-library-content" value="search">
          <form
            className="room-music-search"
            onSubmit={(event) => {
              event.preventDefault();
              void search();
            }}
          >
            <InputGroup>
              {mode === 'playlist' ? <ListMusic aria-hidden="true" /> : <Search aria-hidden="true" />}
              <InputGroupInput
                id="roomMusicQuery"
                ref={queryInput}
                aria-label={t(mode === 'search' ? 'music.keyword' : 'music.playlistInput')}
                placeholder={t(mode === 'search' ? 'music.keyword' : 'music.playlistInput')}
                value={query}
                maxLength={500}
                onChange={(event) => {
                  clearSearch();
                  setQuery(event.target.value);
                }}
              />
              {query && <Button variant="quiet" size="icon-sm" type="button" aria-label={t('library.clearSearch')}
                onClick={() => { clearSearch(); setQuery(''); queryInput.current?.focus(); }}>
                <X aria-hidden="true" />
              </Button>}
              <Button
                variant="quiet"
                size="icon"
                type="submit"
                aria-label={t(busy ? 'music.loading' : 'music.search')}
                disabled={!enabled || busy || !query.trim()}
              >
                {busy ? (
                  <LoaderCircle className="room-music-loading" aria-hidden="true" />
                ) : (
                  <ArrowRight aria-hidden="true" />
                )}
              </Button>
            </InputGroup>
            <div className="room-music-filters">
              <MainSelect
                id="roomMusicPlatform"
                aria-label={t('music.platform')}
                value={platform}
                onChange={(event) => {
                  clearSearch();
                  setPlatform(event.target.value as MusicPlatform);
                }}
              >
                <option value="netease">{t('music.netease')}</option>
                <option value="qq">{t('music.qq')}</option>
              </MainSelect>
              <Button
                variant="quiet"
                size="sm"
                aria-pressed={mode === 'playlist'}
                onClick={() => {
                  clearSearch();
                  setQuery('');
                  setMode(mode === 'search' ? 'playlist' : 'search');
                }}
              >
                <ListMusic size={16} aria-hidden="true" />
                {t('music.playlist')}
              </Button>
            </div>
          </form>
          {error && (
            <p className="room-music-library-feedback" role="alert">
              {t(`music.${error}`)}
            </p>
          )}
          <div className="room-music-results-scroll" aria-busy={busy}>
            {!!results.length && (
              <>
                <div className="room-music-list-heading">
                  <h4>{t(mode === 'playlist' ? 'music.playlist' : 'music.songs')}</h4>
                  {mode === 'playlist' && (
                    <Button
                      variant="quiet"
                      size="sm"
                      disabled={!enabled}
                      onClick={() => model.choose(results[0], results)}
                    >
                      <Play size={14} fill="currentColor" />
                      {t('music.playPlaylist')}
                    </Button>
                  )}
                </div>
                <ul className="room-music-results">
                  {results.slice(0, visibleCount).map((track, index) => trackRow(track, index, false))}
                </ul>
              </>
            )}
            {showHistory && (
              <>
                <div className="room-music-list-heading">
                  <h4>{t('music.history')}</h4>
                </div>
                <ul className="room-music-results">
                  {state.lookups.map(entry => <li key={`${entry.platform}:${entry.mode}:${entry.query}`}>
                    <Button variant="unstyled" className="room-music-row" disabled={!enabled}
                      onClick={() => {
                        clearSearch();
                        setPlatform(entry.platform);
                        setMode(entry.mode);
                        setQuery(entry.query);
                        void search(entry);
                      }}>
                      {entry.mode === 'playlist' ? <ListMusic aria-hidden="true" /> : <Search aria-hidden="true" />}
                      <span className="room-music-track">
                        <strong>{entry.query}</strong>
                        <small>{t(`music.${entry.platform}`)}{entry.mode === 'playlist' ? ` | ${t('music.playlist')}` : ''}</small>
                      </span>
                    </Button>
                  </li>)}
                </ul>
              </>
            )}
            {!results.length && !showHistory && !error && (
              <div className="room-music-empty" role="status">
                {busy ? (
                  <LoaderCircle className="room-music-loading" size={30} />
                ) : (
                  <Music2 size={32} strokeWidth={1.3} aria-hidden="true" />
                )}
                <p>
                  {t(
                    busy
                      ? 'music.loading'
                      : searchKey.current
                        ? 'music.empty'
                        : mode === 'playlist'
                          ? 'music.playlistInput'
                          : 'music.keyword',
                  )}
                </p>
              </div>
            )}
            {visibleCount < results.length && (
              <Button
                className="room-music-load-more"
                variant="quiet"
                disabled={busy}
                onClick={() => setVisibleCount((count) => count + tracksPerPage)}
              >
                {t('music.more')}
              </Button>
            )}
          </div>
        </TabsContent>
      </Tabs>}
      {state.session && <RoomMusicStrip model={model} open embedded onOpen={(_element, next) => onTabChange(next)} />}
    </section>
  );
}
