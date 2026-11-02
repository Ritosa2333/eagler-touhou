import {Input} from '../../ui/input';
import {Button as UiButton} from '../../ui/controls';
import {ArrowLeft, Upload, Folder, FileText, MoreHorizontal, Play, Pause, RotateCcw} from 'lucide-react';
import {useContext, useEffect, useRef, useState, useSyncExternalStore} from 'react';
import {createPortal} from 'react-dom';
import type {BrowserSession} from '../../session/browser-session';
import {useSurfaceNavigation} from '../../navigation/surface-navigation';
import {useSurfaceLayerState} from '../../navigation/surface-layer';
import {groupReplayFolders} from '../../models/replays';
import {isTh09ReplayDirectory} from '../../../src/launcher/th09-replay-match.mts';
import {isReplayImportFileName} from '../../../src/launcher/replay-files.mts';
import {FullscreenIcons} from './utility-icons';
import {MainSelect} from '../launcher/MainSelect';
import {PlayerStage} from './PlayerSurface';
import {ReplayFileMenu} from './ReplayFileMenu';
import {ReplayTransport, type ReplayTransportState} from './ReplayTransport';
import {ReplayFileRow} from './ReplayFileRow';
import {useLocale} from '../../i18n';
import {errorText} from '../../services/error-text';
import {gameIdForProduct, PRODUCT_GAMES} from '../../../src/contracts/product-catalog.mts';
import type {ReplayDescription} from '../../../src/launcher/replay-description.mts';
import type {UiMessageKey} from '../../../src/launcher/i18n.mts';
import './replay-workspace.css';

export interface ReplayPlaybackState extends ReplayTransportState {
  kind: 'replay'; difficulty: number; left: number; right: number; stage: number;
  description?: ReplayDescription;
}

/** Composes around the permanent game iframe. File selection never remounts it. */
export function ReplayWorkspace({session}: {session: BrowserSession}) {
  const {t} = useLocale();
  const characterName = (id: number, side: number) => {
    const loadout = PRODUCT_GAMES.th09.multiplayer.loadouts.find(item => item.character === id);
    return loadout ? t(loadout.labelKey as UiMessageKey) : `P${side + 1}`;
  };
  const model = session.getReplays()!, navigation = useSurfaceNavigation();
  const stage = useContext(PlayerStage);
  const viewport = stage?.querySelector<HTMLElement>('.game-viewport');
  const app = useSyncExternalStore(session.subscribe, session.getSnapshot, session.getSnapshot);
  const files = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const open = navigation.infoDialogOpen('replayDialog') && files.open;
  const game = files.productId ? gameIdForProduct(files.productId) : 'th09', versus = game === 'th09';
  const [expanded, setExpanded] = useSurfaceLayerState<string | null>('replay-details', null);
  const [folder, setFolder] = useSurfaceLayerState<string | null>('replay-folder', null);
  const [moving, setMoving] = useSurfaceLayerState<string | null>('replay-move', null);
  const [destination, setDestination] = useState('replay'), [newFolder, setNewFolder] = useState(''), [busy, setBusy] = useState(false);
  const [error, setError] = useState('');
  const closing = useRef(false), routeVisible = useRef(false), routeOpen = navigation.infoDialogOpen('replayDialog');
  const playback = app.runtime?.runtimeInfo.playback as ReplayPlaybackState | undefined;
  const folders = groupReplayFolders(files.rows), rows = files.rows.filter(row => row.path.slice(0, row.path.lastIndexOf('/')) === (folder ?? 'replay'));
  useEffect(() => {
    if (routeOpen) {routeVisible.current = true; if (!files.open && files.productId) void model.open(files.productId).catch(session.reportError);}
    else if (routeVisible.current) {routeVisible.current = false; if (files.open) model.close();}
  }, [routeOpen, model, files.open, files.productId, session.reportError]);
  // biome-ignore lint/correctness/useExhaustiveDependencies: File navigation state resets when switching the manager's product.
  useEffect(() => {setExpanded(null); if (folder) setFolder(null); else setMoving(null); setError('');}, [files.productId]);
  const folderExists = !folder || folders.some(item => item.key === folder);
  useEffect(() => {if (!folderExists) setFolder(null);}, [folderExists]);
  if (!open) return null;
  const report = (error: unknown) => {setError(error instanceof Error ? error.message : String(error));};
  async function control(action: 'pause'|'play'|'seek'|'rate', value?: number) {
    setError(''); try {await session.getRuntime()!.send('playback', {action, ...(value !== undefined ? {value} : {})});} catch (error) {report(error);}
  }
  const importFile = async (file: File) => {
    if (!isReplayImportFileName(file.name)) throw Error('请选择录像或录像压缩包');
    await session.importFile('replay', file, files.productId!);
  };
  async function close() {
    if(closing.current) return;
    closing.current=true; setBusy(true);
    try {
      // Runtime owns save confirmation and teardown. Retire the workspace only
      // after it commits the close, keeping the current view usable on cancel.
      if (!app.replayViewer || await session.requestRuntimeClose()) {
        // The route effect retires the model after the Router commits Back.
        // Closing here publishes open=false while the old route is still live;
        // a delayed BrowserHistory commit then makes that effect open it again.
        navigation.closeInfoDialog('replayDialog');
      }
    } catch (error) {report(error);}
    finally {closing.current=false; setBusy(false);}
  }
  const icons = {'replay-upload-simple': Upload, 'replay-folder': Folder, 'replay-file-text': FileText, 'replay-dots-three': MoreHorizontal, 'player-play': Play, 'player-pause': Pause, 'replay-arrow-counter-clockwise': RotateCcw, 'replay-arrow-left': ArrowLeft};
  const icon = (name: string) => {const Icon = icons[name as keyof typeof icons]; return Icon ? <Icon aria-hidden="true"/> : <img src={new URL(`assets/${name}.svg`, session.baseUrl).href} alt=""/>;};
  const timestamp = (value?: number) => value && value > 0 && value <= 8.64e15 ? <time dateTime={new Date(value).toISOString()}>{new Date(value).toLocaleString(app.locale, {year: 'numeric', month: '2-digit', day: '2-digit', hour: '2-digit', minute: '2-digit', hour12: false})}</time> : <span>{t('replay.ui.unknownTime')}</span>;
  return <>
    <header className="replay-workspace-header" data-replay-versus={versus}>
      <UiButton variant="icon" type="button" className="replay-back replay-button" aria-label={t('replay.ui.back')} disabled={busy} onClick={() => {void close();}}>{icon('replay-arrow-left')}</UiButton>
      <h1>{t('replay.manager')}</h1>
    </header>
    {stage && !playback && stage.querySelector('.game-viewport') && createPortal(<div className="replay-workspace-idle" data-ui-surface="game">{icon('player-play')}<p>{versus ? t('replay.ui.choosePlay') : t('replay.ui.chooseStage')}</p><small>{t('replay.ui.browseWhileLoading')}</small></div>, stage.querySelector('.game-viewport')!)}
    {viewport && createPortal(<UiButton variant="icon" type="button" className="replay-fullscreen replay-fullscreen-exit replay-button" aria-label={t('replay.ui.exitFullscreen')} onClick={() => {void session.togglePlayerFullscreen();}} data-fullscreen="true"><FullscreenIcons/></UiButton>, viewport)}
    {stage && createPortal(<>
    <ReplayTransport playback={playback} busy={busy} fullscreen={session.nativeTouch.isFullscreen()} speedId="replaySpeedSelect" onControl={(action, value) => {void control(action, value);}} onToggleFullscreen={() => {void session.togglePlayerFullscreen();}}/>
    <section className="replay-current" aria-label={t('replay.ui.info')}>
      <h2 title={playback?.path}>{playback?.path.split('/').pop() || (files.phase === 'loading' ? t('replay.ui.reading') : t('replay.ui.chooseOne'))}</h2>
      <p>{playback ? <><span>{PRODUCT_GAMES[game].title}</span><span>{['Easy','Normal','Hard','Lunatic','Extra'][playback.difficulty] ?? ''}</span>{versus ? <span>{characterName(playback.left, 0)} vs {characterName(playback.right, 1)}</span> : <span>{playback.description?.name} | {playback.description?.stages.find(item => item.id === playback.stage)?.label ?? t('replay.ui.stageNumber',{stage:playback.stage})}</span>}</> : t('replay.ui.chooseToPlay')}</p>
      {error && <p className="replay-workspace-error" role="alert">{errorText(error,t)}</p>}
    </section>
    </>, stage)}
    <aside className="replay-file-browser" aria-label={t('replay.ui.files')} onDragOver={event => {if (!busy && event.dataTransfer.types.includes('Files')) event.preventDefault();}} onDrop={event => {event.preventDefault(); if (busy || files.phase !== 'ready') return; const list = [...event.dataTransfer.files]; if (list.length !== 1) {report(Error('请每次导入一个文件')); return;} void importFile(list[0]).catch(report);}}>
      <header><h2>{t('replay.ui.heading')}</h2><UiButton type="button" className="replay-button" disabled={busy || files.phase !== 'ready'} onClick={() => {setBusy(true); void session.chooseReplayFile(files.productId!).catch(report).finally(() => setBusy(false));}}>{icon('replay-upload-simple')}<span>{t('replay.ui.import')}</span></UiButton></header>
      <nav className="replay-breadcrumb" aria-label={t('replay.ui.location')}>{icon('replay-folder')}<UiButton variant="quiet" size="sm" type="button" onClick={() => {if (folder) setFolder(null); else setMoving(null);}}>{t('replay.ui.all')}</UiButton>{folder && <span title={folder.slice(7)}>/ {folder.slice(7)}</span>}</nav>
      {moving && <form className="replay-workspace-move" onSubmit={event => {event.preventDefault();setBusy(true); void model.move(moving, destination === 'new' ? `replay/${newFolder.trim()}` : destination).then(ok => {if (ok) setMoving(null);}).finally(() => setBusy(false));}}>
        <strong title={moving}>{t('replay.ui.moving',{name:moving.split('/').pop()})}</strong><MainSelect id="replayMoveSelect" className="option-select" aria-label={t('replay.ui.moveTo')} value={destination} onChange={event => setDestination(event.target.value)}><option value="replay">{t('replay.ui.all')}</option>{folders.map(item => <option key={item.key} value={item.key}>{item.name}</option>)}<option value="new">{t('replay.ui.newFolder')}</option></MainSelect>
        {destination === 'new' && <Input aria-label={t('replay.ui.folderName')} maxLength={128} value={newFolder} onChange={event => setNewFolder(event.target.value)}/>}
        <div><UiButton type="button" className="replay-button" onClick={() => setMoving(null)}>{t('replay.ui.cancel')}</UiButton><UiButton variant="primary" type="submit" className="replay-button" disabled={busy || !isTh09ReplayDirectory(destination === 'new' ? `replay/${newFolder.trim()}` : destination)}>{t('replay.ui.move')}</UiButton></div>
      </form>}
      <div className="replay-workspace-files" key={folder ?? 'replay'} aria-busy={files.phase === 'loading' || busy}>
        {files.phase === 'loading' ? <p className="replay-workspace-empty">{t('replay.ui.loading')}</p> : files.phase === 'error' ? <div className="replay-workspace-empty"><p>{t('replay.ui.failed')}</p><UiButton type="button" onClick={() => {void model.refresh().catch(report);}}>{t('replay.ui.retry')}</UiButton></div> : <>
          {!folder && folders.map(item => <div className="replay-workspace-file replay-workspace-folder" key={item.key}><UiButton variant="unstyled" type="button" data-replay-folder={item.key} onClick={() => {setFolder(item.key);}}>{icon('replay-folder')}<strong title={item.name}>{item.name}</strong><small>{t('replay.ui.count',{count:item.rows.length})}</small></UiButton><ReplayFileMenu disabled={busy} label={t('replay.ui.manageFolder',{name:item.name})} actions={[{label: t('replay.ui.download'), run: () => {void model.downloadFolder(item.key);}}]}/></div>)}
          {rows.map(row => <ReplayFileRow key={row.path} path={row.path} name={row.name} size={row.size} versus={versus} selected={playback?.path === row.path} expanded={expanded === row.path} busy={busy}
            onOpen={() => {setError(''); if (versus) {setBusy(true); void model.watch(row.path).finally(() => setBusy(false));} else {setExpanded(current => current === row.path ? null : row.path); void model.inspect(row.path);}}}
            metadata={!versus && files.descriptions[row.path] ? <ReplaySummary description={files.descriptions[row.path]!} game={game}/> : <small title={row.replayMatch?.startedAt ? t('replay.ui.matchStarted') : t('replay.ui.modified')}>{timestamp(row.replayMatch?.startedAt || row.modifiedAt)}</small>}
            actions={[
              {label: t('replay.ui.download'), run: () => {void model.download(row.path);}},
              {label: t('replay.ui.move'), run: () => {setMoving(row.path); setDestination('replay'); setNewFolder('');}},
              {label: t('replay.ui.rename'), run: () => {void model.rename(row.path);}},
              {label: t('replay.ui.delete'), destructive: true, run: () => {void model.remove(row.path);}},
            ]}>
            {!versus && expanded === row.path && <section className="replay-stage-list" aria-label={t('replay.ui.details',{name:row.name})}>
              {files.descriptions[row.path] ? <>
                <ReplayMetadata description={files.descriptions[row.path]!} game={game}/>
                <div className="replay-stage-heading"><span>{t('replay.ui.stage')}</span><span>{t('replay.ui.lastScore')}</span></div>
                {files.descriptions[row.path]!.stages.map(item => <UiButton variant="unstyled" type="button" key={item.id} data-replay-stage={item.id} disabled={busy} onClick={() => {setBusy(true); void model.watch(row.path, item.id).finally(() => setBusy(false));}}><span>{item.label ?? t('replay.ui.stageNumber',{stage:item.id})}</span><span>{item.score === null ? '—' : Math.trunc(item.score).toLocaleString('en-US', {useGrouping: false})}</span></UiButton>)}
              </> : <p role="status">{files.inspecting === row.path ? t('replay.ui.parsing') : t('replay.ui.parseFailed')}{!files.inspecting && <UiButton type="button" onClick={() => {void model.inspect(row.path);}}>{t('replay.ui.retry')}</UiButton>}</p>}
            </section>}

          </ReplayFileRow>)}
          {!files.rows.length && <p className="replay-workspace-empty">{t('replay.ui.empty')}<br/><small>{t('replay.ui.drop')}</small></p>}
        </>}
      </div>
      <footer>{t('replay.ui.count',{count:files.rows.length})}</footer>
    </aside>
  </>;
}

function ReplaySummary({description: d, game}: {description: ReplayDescription; game: keyof typeof PRODUCT_GAMES}) {
  const {locale,t} = useLocale();
  return <span className="replay-file-summary"><span title={t('replay.ui.playerName')}>{d.name || '—'}</span><span title={t('replay.ui.loadout')}>{replayLoadout(d, game, t)}</span><span title={t('replay.ui.date')}>{replayDate(d,locale)}</span><span title={d.spell ? t('replay.ui.spell') : t('replay.ui.difficulty')}>{d.spell || replayRank(d.difficulty)}</span></span>;
}
const replayRank = (difficulty: number) => ['Easy','Normal','Hard','Lunatic','Extra','Phantasm'][difficulty] ?? difficulty;
const replayDate = (d: ReplayDescription, locale: string) => d.date || (d.timestamp && d.timestamp <= 8.64e12 ? new Date(d.timestamp * 1000).toLocaleDateString(locale) : '—');
function replayLoadout(d: ReplayDescription, game: keyof typeof PRODUCT_GAMES, t: (key: UiMessageKey) => string) {
  const product = PRODUCT_GAMES[game], loadouts = 'multiplayer' in product ? product.multiplayer.loadouts : [];
  const loadout = loadouts.find(item => item.character === d.character && item.shot === (d.shot ?? 0));
  return loadout ? t(loadout.labelKey as UiMessageKey) : String(d.character);
}
function ReplayMetadata({description: d, game}: {description: ReplayDescription; game: keyof typeof PRODUCT_GAMES}) {
  const {t}=useLocale();
  return <div className="replay-metadata">
    {d.score !== undefined && <span title={t('replay.ui.score')}>{Math.trunc(d.score)}</span>}
    {d.slowdown !== undefined && <span title={t('replay.ui.slowdown')}>{d.slowdown.toFixed(['th06','th07','th08'].includes(game) ? 3 : 2)}%</span>}
    {d.mode && <span title={t('replay.ui.mode')}>{d.mode}</span>}
    {d.spellName && <span className="replay-spell-name" title={t('replay.ui.spellName')}>{d.spellName}</span>}
  </div>;
}
