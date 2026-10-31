import { useContext, useEffect, useRef, useState, useSyncExternalStore } from 'react';
import { createPortal } from 'react-dom';
import { gameIdForProduct, PRODUCT_GAMES } from '../../../src/contracts/product-catalog.mts';
import type { UiMessageKey } from '../../../src/launcher/i18n.mts';
import type { ReplayDescription } from '../../../src/launcher/replay-description.mts';
import { isReplayImportFileName } from '../../../src/launcher/replay-files.mts';
import { isTh09ReplayDirectory } from '../../../src/launcher/th09-replay-match.mts';
import { localizedGameTitle, useLocale } from '../../i18n';
import { formatReplayBytes, groupReplayFolders } from '../../models/replays';
import { useSurfaceNavigation } from '../../navigation/surface-navigation';
import type { BrowserSession } from '../../session/browser-session';
import { MainSelect } from '../launcher/MainSelect';
import { Button } from '../ui/button';
import { PlayerStage } from './PlayerSurface';
import { ReplayFileMenu } from './ReplayFileMenu';
import { replayClock as clock, ReplaySeekBar } from './ReplaySeekBar';
import { playerUtilityIcons } from './utility-icons';
import './replay-workspace.css';

export interface ReplayPlaybackState {
  kind: 'replay';
  path: string;
  frame: number;
  total: number;
  paused: boolean;
  seeking: boolean;
  target: number | null;
  rate: number;
  ended: boolean;
  difficulty: number;
  left: number;
  right: number;
  stage: number;
  roundStarts?: number[];
  description?: ReplayDescription;
  startFrame?: number;
}
const rates = [0.5, 1, 2, 4, 8, 0];

/** Composes around the permanent game iframe. File selection never remounts it. */
export function ReplayWorkspace({ session }: { session: BrowserSession }) {
  const { t, locale } = useLocale();
  const characterName = (id: number, side: number) => {
    const loadout = PRODUCT_GAMES.th09.multiplayer.loadouts.find((item) => item.character === id);
    return loadout ? t(loadout.labelKey as UiMessageKey) : `P${side + 1}`;
  };
  const model = session.getReplays()!,
    navigation = useSurfaceNavigation();
  const stage = useContext(PlayerStage);
  const viewport = stage?.querySelector<HTMLElement>('.game-viewport');
  const app = useSyncExternalStore(session.subscribe, session.getSnapshot, session.getSnapshot);
  const files = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const open = navigation.infoDialogOpen('replayDialog') && files.open;
  const game = files.productId ? gameIdForProduct(files.productId) : 'th09',
    versus = game === 'th09';
  const [expanded, setExpanded] = useState<string | null>(null);
  const [folder, setFolder] = useState<string | null>(null),
    [moving, setMoving] = useState<string | null>(null);
  const [destination, setDestination] = useState('replay'),
    [newFolder, setNewFolder] = useState(''),
    [busy, setBusy] = useState(false);
  const [error, setError] = useState('');
  const [fileMenu, setFileMenu] = useState<{ anchor: HTMLElement; path: string; folder: boolean } | null>(null);
  const routeVisible = useRef(false),
    routeOpen = navigation.infoDialogOpen('replayDialog');
  const playback = app.runtime?.runtimeInfo.playback as ReplayPlaybackState | undefined;
  const folders = groupReplayFolders(files.rows),
    rows = files.rows.filter((row) => row.path.slice(0, row.path.lastIndexOf('/')) === (folder ?? 'replay'));
  useEffect(() => {
    if (routeOpen) {
      routeVisible.current = true;
      if (!files.open && files.productId) void model.open(files.productId).catch(session.reportError);
    } else if (routeVisible.current) {
      routeVisible.current = false;
      if (files.open) model.close();
    }
  }, [routeOpen, model]);
  useEffect(() => {
    setExpanded(null);
    setFolder(null);
    setMoving(null);
    setFileMenu(null);
    setError('');
  }, [files.productId]);
  useEffect(() => {
    setFileMenu(null);
  }, [folder, files.rows, open]);
  useEffect(() => {
    if (folder && !folders.some((item) => item.key === folder)) setFolder(null);
  }, [files.rows, folder]);
  if (!open) return null;
  const report = (error: unknown) => {
    setError(error instanceof Error ? error.message : String(error));
  };
  async function control(action: 'pause' | 'play' | 'seek' | 'rate', value?: number) {
    setError('');
    try {
      await session.getRuntime()!.send('playback', { action, ...(value !== undefined ? { value } : {}) });
    } catch (error) {
      report(error);
    }
  }
  const importFile = async (file: File) => {
    if (!isReplayImportFileName(file.name)) throw Error(t('replay.ui.choose'));
    await session.importFile('replay', file, files.productId!);
  };
  async function close() {
    setBusy(true);
    setFileMenu(null);
    const animations: Animation[] = [];
    try {
      if (stage && !window.matchMedia('(prefers-reduced-motion: reduce)').matches && !stage.closest('.less-motion')) {
        for (const element of stage.parentElement?.querySelectorAll<HTMLElement>(
          '.replay-workspace-header,.player-stage,.replay-file-browser',
        ) ?? []) {
          if (typeof element.animate === 'function')
            animations.push(
              element.animate(
                [
                  { opacity: 1, transform: 'none' },
                  { opacity: 0, transform: 'translateY(12px) scale(.98)' },
                ],
                { duration: 180, easing: 'cubic-bezier(.2,0,0,1)', fill: 'both' },
              ),
            );
        }
        await Promise.allSettled(animations.map((animation) => animation.finished));
      }
      // Retire the child document before consuming the parent history entry.
      // This also lets the existing save-failure decision keep the viewer open.
      if (!app.replayViewer || (await session.requestRuntimeClose())) {
        model.close();
        navigation.closeInfoDialog('replayDialog');
      }
    } catch (error) {
      report(error);
    } finally {
      for (const animation of animations) animation.cancel();
      setBusy(false);
    }
  }
  const icon = (name: string) => <img src={new URL(`assets/${name}.svg`, session.baseUrl).href} alt="" />;
  const speed = (id: string) => (
    <MainSelect
      id={id}
      className="option-select"
      aria-label={t('replay.ui.speed')}
      value={playback?.rate ?? 1}
      disabled={!playback || busy}
      onChange={(event) => {
        void control('rate', Number(event.target.value));
      }}
    >
      {rates.map((rate) => (
        <option key={rate} value={rate}>
          {rate ? `${rate}×` : t('replay.ui.fullSpeed')}
        </option>
      ))}
    </MainSelect>
  );
  const timestamp = (value?: number) =>
    value && value > 0 && value <= 8.64e15 ? (
      <time dateTime={new Date(value).toISOString()}>
        {new Date(value).toLocaleString(app.locale, {
          year: 'numeric',
          month: '2-digit',
          day: '2-digit',
          hour: '2-digit',
          minute: '2-digit',
          hour12: false,
        })}
      </time>
    ) : (
      <span>{t('replay.ui.unknownTime')}</span>
    );
  const toggleFileMenu = (anchor: HTMLElement, path: string, folder: boolean) =>
    setFileMenu((current) => (current?.anchor === anchor ? null : { anchor, path, folder }));
  return (
    <>
      <header className="replay-workspace-header" data-replay-versus={versus}>
        <Button
          variant="legacy"
          size="legacy"
          type="button"
          className="replay-back replay-button"
          aria-label={t('replay.ui.back')}
          disabled={busy}
          onClick={() => {
            void close();
          }}
        >
          {icon('replay-arrow-left')}
        </Button>
      </header>
      {stage &&
        !playback &&
        stage.querySelector('.game-viewport') &&
        createPortal(
          <div className="replay-workspace-idle">
            {icon('player-play')}
            <p>{versus ? t('replay.ui.choosePlay') : t('replay.ui.chooseStage')}</p>
            <small>{t('replay.ui.browseWhileLoading')}</small>
          </div>,
          stage.querySelector('.game-viewport')!,
        )}
      {viewport &&
        createPortal(
          <Button
            variant="legacy"
            size="legacy"
            type="button"
            className="replay-fullscreen replay-fullscreen-exit replay-button"
            aria-label={t('replay.ui.exitFullscreen')}
            onClick={() => {
              void session.togglePlayerFullscreen();
            }}
            data-fullscreen="true"
            dangerouslySetInnerHTML={{ __html: playerUtilityIcons.fullscreenToggle }}
          />,
          viewport,
        )}
      {stage &&
        createPortal(
          <>
            <section className="replay-transport" aria-label={t('replay.ui.transport')}>
              <div className="replay-transport-buttons">
                <Button
                  variant="legacy"
                  size="legacy"
                  type="button"
                  className="replay-toggle replay-button"
                  aria-label={playback?.paused || playback?.ended ? t('replay.ui.play') : t('replay.ui.pause')}
                  disabled={!playback || busy}
                  onClick={() => {
                    void control(playback?.paused || playback?.ended ? 'play' : 'pause');
                  }}
                >
                  {icon(`player-${playback?.paused || playback?.ended ? 'play' : 'pause'}`)}
                </Button>
                <Button
                  variant="legacy"
                  size="legacy"
                  type="button"
                  className="replay-restart replay-button"
                  aria-label={t('replay.ui.restart')}
                  disabled={!playback || busy}
                  onClick={() => {
                    void control('seek', playback?.startFrame ?? 0);
                  }}
                >
                  {icon('replay-arrow-counter-clockwise')}
                  <span>{t('replay.ui.restart')}</span>
                </Button>
                <div className="replay-speed">{speed('replaySpeedSelect')}</div>
              </div>
              <ReplaySeekBar
                key={`${playback?.path ?? ''}:${playback?.startFrame ?? 0}`}
                frame={Math.max(0, (playback?.frame ?? 0) - (playback?.startFrame ?? 0))}
                total={Math.max(0, (playback?.total ?? 0) - (playback?.startFrame ?? 0))}
                roundStarts={playback?.roundStarts}
                disabled={busy || !playback}
                onSeek={(target) => {
                  void control('seek', target + (playback?.startFrame ?? 0));
                }}
              />
              <Button
                variant="legacy"
                size="legacy"
                type="button"
                className="replay-fullscreen replay-button"
                aria-label={t('replay.ui.fullscreen')}
                onClick={() => {
                  void session.togglePlayerFullscreen();
                }}
                data-fullscreen={session.nativeTouch.isFullscreen()}
                dangerouslySetInnerHTML={{ __html: playerUtilityIcons.fullscreenToggle }}
              />
              <span className="replay-seek-status" role="status">
                {playback?.seeking
                  ? t('replay.ui.seek', {
                      time: clock(Math.max(0, (playback.target ?? 0) - (playback.startFrame ?? 0))),
                    })
                  : playback?.ended
                    ? t('replay.ui.ended')
                    : ''}
              </span>
            </section>
            <section className="replay-current" aria-label={t('replay.ui.info')}>
              <h2 title={playback?.path}>
                {playback?.path.split('/').pop() ||
                  (files.phase === 'loading' ? t('replay.ui.reading') : t('replay.ui.chooseOne'))}
              </h2>
              <p>
                {playback ? (
                  <>
                    <span>{localizedGameTitle(locale, game, PRODUCT_GAMES[game].title)}</span>
                    <span>{['Easy', 'Normal', 'Hard', 'Lunatic', 'Extra'][playback.difficulty] ?? ''}</span>
                    {versus ? (
                      <span>
                        {characterName(playback.left, 0)} vs {characterName(playback.right, 1)}
                      </span>
                    ) : (
                      <span>
                        {playback.description?.name} ·{' '}
                        {playback.description?.stages.find((item) => item.id === playback.stage)?.label ??
                          t('replay.ui.stageNumber', { stage: playback.stage })}
                      </span>
                    )}
                  </>
                ) : (
                  t('replay.ui.chooseToPlay')
                )}
              </p>
              {error && (
                <p className="replay-workspace-error" role="alert">
                  {error}
                </p>
              )}
            </section>
          </>,
          stage,
        )}
      <aside
        className="replay-file-browser"
        aria-label={t('replay.ui.files')}
        onDragOver={(event) => {
          if (!busy && event.dataTransfer.types.includes('Files')) event.preventDefault();
        }}
        onDrop={(event) => {
          event.preventDefault();
          if (busy || files.phase !== 'ready') return;
          const list = [...event.dataTransfer.files];
          if (list.length !== 1) {
            report(Error(t('replay.ui.oneAtATime')));
            return;
          }
          void importFile(list[0]).catch(report);
        }}
      >
        <header>
          <h2>{t('replay.ui.heading')}</h2>
          <Button
            variant="legacy"
            size="legacy"
            type="button"
            className="replay-button"
            disabled={busy || files.phase !== 'ready'}
            onClick={() => {
              setBusy(true);
              void session
                .chooseReplayFile(files.productId!)
                .catch(report)
                .finally(() => setBusy(false));
            }}
          >
            {icon('replay-upload-simple')}
            <span>{t('replay.ui.import')}</span>
          </Button>
        </header>
        <nav className="replay-breadcrumb" aria-label={t('replay.ui.location')}>
          {icon('replay-folder')}
          <Button
            variant="legacy"
            size="legacy"
            type="button"
            onClick={() => {
              setFolder(null);
              setMoving(null);
            }}
          >
            {t('replay.ui.all')}
          </Button>
          {folder && <span title={folder.slice(7)}>/ {folder.slice(7)}</span>}
        </nav>
        {moving && (
          <form
            className="replay-workspace-move"
            onSubmit={(event) => {
              event.preventDefault();
              setBusy(true);
              void model
                .move(moving, destination === 'new' ? `replay/${newFolder.trim()}` : destination)
                .then((ok) => {
                  if (ok) setMoving(null);
                })
                .finally(() => setBusy(false));
            }}
          >
            <strong title={moving}>{t('replay.ui.moving', { name: moving.split('/').pop() })}</strong>
            <MainSelect
              id="replayMoveSelect"
              className="option-select"
              aria-label={t('replay.ui.moveTo')}
              value={destination}
              onChange={(event) => setDestination(event.target.value)}
            >
              <option value="replay">{t('replay.ui.all')}</option>
              {folders.map((item) => (
                <option key={item.key} value={item.key}>
                  {item.name}
                </option>
              ))}
              <option value="new">{t('replay.ui.newFolder')}</option>
            </MainSelect>
            {destination === 'new' && (
              <input
                aria-label={t('replay.ui.folderName')}
                maxLength={128}
                value={newFolder}
                onChange={(event) => setNewFolder(event.target.value)}
              />
            )}
            <div>
              <Button
                variant="legacy"
                size="legacy"
                type="button"
                className="replay-button"
                onClick={() => setMoving(null)}
              >
                {t('replay.ui.cancel')}
              </Button>
              <Button
                variant="legacy"
                size="legacy"
                type="submit"
                className="replay-button"
                disabled={
                  busy || !isTh09ReplayDirectory(destination === 'new' ? `replay/${newFolder.trim()}` : destination)
                }
              >
                {t('replay.ui.move')}
              </Button>
            </div>
          </form>
        )}
        <div className="replay-workspace-files" key={folder ?? 'replay'} aria-busy={files.phase === 'loading' || busy}>
          {files.phase === 'loading' ? (
            <p className="replay-workspace-empty">{t('replay.ui.loading')}</p>
          ) : files.phase === 'error' ? (
            <div className="replay-workspace-empty">
              <p>{t('replay.ui.failed')}</p>
              <Button
                variant="legacy"
                size="legacy"
                type="button"
                onClick={() => {
                  void model.refresh().catch(report);
                }}
              >
                {t('replay.ui.retry')}
              </Button>
            </div>
          ) : (
            <>
              {!folder &&
                folders.map((item) => (
                  <div className="replay-workspace-file replay-workspace-folder" key={item.key}>
                    <Button
                      variant="legacy"
                      size="legacy"
                      type="button"
                      data-replay-folder={item.key}
                      onClick={() => {
                        setFolder(item.key);
                        setMoving(null);
                      }}
                    >
                      {icon('replay-folder')}
                      <strong title={item.name}>{item.name}</strong>
                      <small>{t('replay.ui.count', { count: item.rows.length })}</small>
                    </Button>
                    <Button
                      variant="legacy"
                      size="legacy"
                      type="button"
                      className="replay-file-menu-trigger"
                      disabled={busy}
                      aria-label={t('replay.ui.manageFolder', { name: item.name })}
                      aria-haspopup="menu"
                      aria-expanded={fileMenu?.path === item.key}
                      onClick={(event) => toggleFileMenu(event.currentTarget, item.key, true)}
                    >
                      {icon('replay-dots-three')}
                    </Button>
                  </div>
                ))}
              {rows.map((row) => (
                <div
                  className={`replay-workspace-file${!versus ? ' replay-workspace-recording' : ''}${playback?.path === row.path ? ' selected' : ''}`}
                  key={row.path}
                  data-replay-file={row.path}
                >
                  <Button
                    variant="legacy"
                    size="legacy"
                    type="button"
                    data-replay-watch={row.path}
                    aria-expanded={versus ? undefined : expanded === row.path}
                    disabled={busy}
                    onClick={() => {
                      setError('');
                      if (versus) {
                        setBusy(true);
                        void model.watch(row.path).finally(() => setBusy(false));
                      } else {
                        setExpanded((current) => (current === row.path ? null : row.path));
                        void model.inspect(row.path);
                      }
                    }}
                  >
                    {versus && (
                      <>
                        {playback?.path === row.path && (
                          <span className="replay-file-selected">{icon('player-play')}</span>
                        )}
                        {icon('replay-file-text')}
                      </>
                    )}
                    <span className="replay-file-description">
                      <span className="replay-file-heading">
                        <strong title={row.name}>{row.name}</strong>
                        {!versus && <small>{formatReplayBytes(row.size)}</small>}
                      </span>
                      {!versus && files.descriptions[row.path] ? (
                        <ReplaySummary description={files.descriptions[row.path]!} game={game} />
                      ) : (
                        <small
                          title={row.replayMatch?.startedAt ? t('replay.ui.matchStarted') : t('replay.ui.modified')}
                        >
                          {timestamp(row.replayMatch?.startedAt || row.modifiedAt)}
                        </small>
                      )}
                    </span>
                    {versus && <small>{formatReplayBytes(row.size)}</small>}
                  </Button>
                  <Button
                    variant="legacy"
                    size="legacy"
                    type="button"
                    className="replay-file-menu-trigger"
                    disabled={busy}
                    aria-label={t('replay.ui.manage', { name: row.name })}
                    aria-haspopup="menu"
                    aria-expanded={fileMenu?.path === row.path}
                    onClick={(event) => toggleFileMenu(event.currentTarget, row.path, false)}
                  >
                    {icon('replay-dots-three')}
                  </Button>
                  {!versus && expanded === row.path && (
                    <div className="replay-stage-list" aria-label={t('replay.ui.details', { name: row.name })}>
                      {files.descriptions[row.path] ? (
                        <>
                          <ReplayMetadata description={files.descriptions[row.path]!} game={game} />
                          <div className="replay-stage-heading">
                            <span>{t('replay.ui.stage')}</span>
                            <span>{t('replay.ui.lastScore')}</span>
                          </div>
                          {files.descriptions[row.path]!.stages.map((item) => (
                            <Button
                              variant="legacy"
                              size="legacy"
                              type="button"
                              key={item.id}
                              data-replay-stage={item.id}
                              disabled={busy}
                              onClick={() => {
                                setBusy(true);
                                void model.watch(row.path, item.id).finally(() => setBusy(false));
                              }}
                            >
                              <span>{item.label ?? t('replay.ui.stageNumber', { stage: item.id })}</span>
                              <span>
                                {item.score === null
                                  ? '—'
                                  : Math.trunc(item.score).toLocaleString('en-US', { useGrouping: false })}
                              </span>
                            </Button>
                          ))}
                        </>
                      ) : (
                        <p role="status">
                          {files.inspecting === row.path ? t('replay.ui.parsing') : t('replay.ui.parseFailed')}
                          {!files.inspecting && (
                            <Button
                              variant="legacy"
                              size="legacy"
                              type="button"
                              onClick={() => {
                                void model.inspect(row.path);
                              }}
                            >
                              {t('replay.ui.retry')}
                            </Button>
                          )}
                        </p>
                      )}
                    </div>
                  )}
                </div>
              ))}
              {!files.rows.length && (
                <p className="replay-workspace-empty">
                  {t('replay.ui.empty')}
                  <br />
                  <small>{t('replay.ui.drop')}</small>
                </p>
              )}
            </>
          )}
        </div>
        <footer>{t('replay.ui.count', { count: files.rows.length })}</footer>
      </aside>
      {fileMenu && (
        <ReplayFileMenu
          key={fileMenu.path}
          anchor={fileMenu.anchor}
          label={t('replay.ui.manage', { name: fileMenu.path.split('/').pop() })}
          onClose={() => setFileMenu(null)}
          actions={
            fileMenu.folder
              ? [
                  {
                    label: t('replay.ui.download'),
                    run: () => {
                      void model.downloadFolder(fileMenu.path);
                    },
                  },
                ]
              : [
                  {
                    label: t('replay.ui.download'),
                    run: () => {
                      void model.download(fileMenu.path);
                    },
                  },
                  {
                    label: t('replay.ui.move'),
                    run: () => {
                      setMoving(fileMenu.path);
                      setDestination('replay');
                      setNewFolder('');
                    },
                  },
                  {
                    label: t('replay.ui.rename'),
                    run: () => {
                      void model.rename(fileMenu.path);
                    },
                  },
                  {
                    label: t('replay.ui.delete'),
                    destructive: true,
                    run: () => {
                      void model.remove(fileMenu.path);
                    },
                  },
                ]
          }
        />
      )}
    </>
  );
}

function ReplaySummary({ description: d, game }: { description: ReplayDescription; game: keyof typeof PRODUCT_GAMES }) {
  const { t } = useLocale();
  return (
    <span className="replay-file-summary">
      <span aria-label={t('replay.ui.playerName')}>{d.name || '—'}</span>
      <span aria-label={t('replay.ui.loadout')}>{replayLoadout(d, game, t)}</span>
      <span aria-label={t('replay.ui.date')}>{replayDate(d)}</span>
      <span aria-label={d.spell ? t('replay.ui.spell') : t('replay.ui.difficulty')}>
        {d.spell || replayRank(d.difficulty)}
      </span>
    </span>
  );
}
const replayRank = (difficulty: number) =>
  ['Easy', 'Normal', 'Hard', 'Lunatic', 'Extra', 'Phantasm'][difficulty] ?? difficulty;
const replayDate = (d: ReplayDescription) =>
  d.date || (d.timestamp && d.timestamp <= 8.64e12 ? new Date(d.timestamp * 1000).toLocaleDateString() : '—');
function replayLoadout(d: ReplayDescription, game: keyof typeof PRODUCT_GAMES, t: (key: UiMessageKey) => string) {
  const product = PRODUCT_GAMES[game],
    loadouts = 'multiplayer' in product ? product.multiplayer.loadouts : [];
  const loadout = loadouts.find((item) => item.character === d.character && item.shot === (d.shot ?? 0));
  return loadout ? t(loadout.labelKey as UiMessageKey) : String(d.character);
}
function ReplayMetadata({
  description: d,
  game,
}: {
  description: ReplayDescription;
  game: keyof typeof PRODUCT_GAMES;
}) {
  const { t } = useLocale();
  return (
    <div className="replay-metadata">
      {d.score !== undefined && <span aria-label={t('replay.ui.score')}>{Math.trunc(d.score)}</span>}
      {d.slowdown !== undefined && (
        <span aria-label={t('replay.ui.slowdown')}>
          {d.slowdown.toFixed(['th06', 'th07', 'th08'].includes(game) ? 3 : 2)}%
        </span>
      )}
      {d.mode && <span aria-label={t('replay.ui.mode')}>{d.mode}</span>}
      {d.spellName && (
        <span className="replay-spell-name" aria-label={t('replay.ui.spellName')}>
          {d.spellName}
        </span>
      )}
    </div>
  );
}
