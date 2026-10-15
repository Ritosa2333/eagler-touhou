import {useContext, useEffect, useRef, useState, useSyncExternalStore, type CSSProperties} from 'react';
import {createPortal} from 'react-dom';
import type {BrowserSession} from '../../session/browser-session';
import {useSurfaceNavigation} from '../../navigation/surface-navigation';
import {formatReplayBytes, groupReplayFolders} from '../../models/replays';
import {isTh09ReplayDirectory} from '../../../src/launcher/th09-replay-match.mts';
import {isReplayImportFileName} from '../../../src/launcher/replay-files.mts';
import {playerUtilityIcons} from './utility-icons';
import {MainSelect} from '../launcher/MainSelect';
import {PlayerStage} from './PlayerSurface';
import {ReplayFileMenu} from './ReplayFileMenu';
import './replay-workspace.css';

export interface ReplayPlaybackState {
  kind: 'replay'; path: string; frame: number; total: number; paused: boolean;
  seeking: boolean; target: number | null; rate: number; ended: boolean;
  difficulty: number; left: number; right: number; stage: number;
}
const clock = (frame: number) => {const seconds = Math.floor(Math.max(0, frame) / 60); return `${Math.floor(seconds / 60)}:${String(seconds % 60).padStart(2, '0')}`;};
const characters = ['灵梦','魔理沙','咲夜','妖梦','铃仙','琪露诺','莉莉卡','梅露兰','露娜萨','米斯蒂娅','帝','文','梅蒂欣','幽香','小町','映姬'];
const rates = [.5, 1, 2, 4, 8, 0];

/** Composes around the permanent game iframe. File selection never remounts it. */
export function ReplayWorkspace({session}: {session: BrowserSession}) {
  const model = session.getReplays()!, navigation = useSurfaceNavigation();
  const stage = useContext(PlayerStage);
  const app = useSyncExternalStore(session.subscribe, session.getSnapshot, session.getSnapshot);
  const files = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const open = navigation.infoDialogOpen('replayDialog') && files.open && (files.productId === 'th09' || files.productId === 'th09mp');
  const [folder, setFolder] = useState<string | null>(null), [moving, setMoving] = useState<string | null>(null);
  const [destination, setDestination] = useState('replay'), [newFolder, setNewFolder] = useState(''), [busy, setBusy] = useState(false);
  const [previewFrame, setPreviewFrame] = useState<number | null>(null), [error, setError] = useState('');
  const [fileMenu, setFileMenu] = useState<{anchor: HTMLElement; path: string; folder: boolean} | null>(null);
  const routeVisible = useRef(false), routeOpen = navigation.infoDialogOpen('replayDialog');
  const playback = app.runtime?.runtimeInfo.playback as ReplayPlaybackState | undefined;
  const folders = groupReplayFolders(files.rows), rows = files.rows.filter(row => row.path.slice(0, row.path.lastIndexOf('/')) === (folder ?? 'replay'));
  useEffect(() => {
    if (routeOpen) {routeVisible.current = true; if (!files.open && (files.productId === 'th09' || files.productId === 'th09mp')) void model.open(files.productId).catch(session.reportError);}
    else if (routeVisible.current) {routeVisible.current = false; if (files.open && (files.productId === 'th09' || files.productId === 'th09mp')) model.close();}
  }, [routeOpen, model]);
  useEffect(() => {setFolder(null); setMoving(null); setFileMenu(null); setError('');}, [files.productId]);
  useEffect(() => {setFileMenu(null);}, [folder, files.rows, open]);
  useEffect(() => {if (folder && !folders.some(item => item.key === folder)) setFolder(null);}, [files.rows, folder]);
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
    setBusy(true);
    setFileMenu(null);
    const animations: Animation[] = [];
    try {
      if (stage && !window.matchMedia('(prefers-reduced-motion: reduce)').matches && !stage.closest('.less-motion')) {
        for (const element of stage.parentElement?.querySelectorAll<HTMLElement>('.replay-workspace-header,.player-stage,.replay-file-browser') ?? []) {
          if (typeof element.animate === 'function') animations.push(element.animate([{opacity: 1, transform: 'none'}, {opacity: 0, transform: 'translateY(12px) scale(.98)'}], {duration: 180, easing: 'cubic-bezier(.2,0,0,1)', fill: 'both'}));
        }
        await Promise.allSettled(animations.map(animation => animation.finished));
      }
      // Retire the child document before consuming the parent history entry.
      // This also lets the existing save-failure decision keep the viewer open.
      if (!app.replayViewer || await session.requestRuntimeClose()) {model.close(); navigation.closeInfoDialog('replayDialog');}
    } catch (error) {report(error);}
    finally {for (const animation of animations) animation.cancel(); setBusy(false);}
  }
  const icon = (name: string) => <img src={new URL(`assets/${name}.svg`, session.baseUrl).href} alt=""/>;
  const speed = (id: string) => <MainSelect id={id} className="option-select" aria-label="播放速度" value={playback?.rate ?? 1} disabled={!playback || busy}
    onChange={event => {void control('rate', Number(event.target.value));}}>{rates.map(rate => <option key={rate} value={rate}>{rate ? `${rate}×` : '满速'}</option>)}</MainSelect>;
  const frame = previewFrame ?? playback?.frame ?? 0;
  const timestamp = (value?: number) => value && value > 0 && value <= 8.64e15 ? <time dateTime={new Date(value).toISOString()}>{new Date(value).toLocaleString(app.locale, {year: 'numeric', month: '2-digit', day: '2-digit', hour: '2-digit', minute: '2-digit', hour12: false})}</time> : <span>时间未知</span>;
  const toggleFileMenu = (anchor: HTMLElement, path: string, folder: boolean) => setFileMenu(current => current?.anchor === anchor ? null : {anchor, path, folder});
  return <>
    <header className="replay-workspace-header">
      <button type="button" className="replay-back replay-button" aria-label="返回" disabled={busy} onClick={() => {void close();}}>{icon('replay-arrow-left')}</button>
      <h1>录像管理</h1>
    </header>
    {stage && !app.replayViewer && stage.querySelector('.game-viewport') && createPortal(<div className="replay-workspace-idle">{icon('player-play')}<p>选择录像开始播放</p><small>游戏加载期间也可浏览和下载</small></div>, stage.querySelector('.game-viewport')!)}
    {stage && createPortal(<>
    <button type="button" className="replay-fullscreen replay-fullscreen-exit replay-button" aria-label="退出全屏" onClick={() => {void session.togglePlayerFullscreen();}} data-fullscreen="true" dangerouslySetInnerHTML={{__html: playerUtilityIcons.fullscreenToggle}}/>
    <section className="replay-transport" aria-label="录像播放控制">
      <div className="replay-seek-row">
        <input type="range" min="0" max={Math.max(1, playback?.total ?? 0)} step="1" value={frame} disabled={!playback?.total || busy}
          style={{'--replay-progress': `${Math.min(100, frame / Math.max(1, playback?.total ?? 0) * 100)}%`} as CSSProperties}
          aria-label="录像进度" aria-valuetext={clock(frame)}
          onChange={event => setPreviewFrame(Number(event.target.value))}
          onPointerUp={event => {const target = Number(event.currentTarget.value); setPreviewFrame(null); void control('seek', target);}}
          onKeyUp={event => {if (['ArrowLeft','ArrowRight','Home','End','PageUp','PageDown'].includes(event.key)) {const target = Number(event.currentTarget.value); setPreviewFrame(null); void control('seek', target);}}}
          onPointerCancel={() => setPreviewFrame(null)}/>
        <span>{clock(frame)}<small> / {clock(playback?.total ?? 0)}</small></span>
      </div>
      <div className="replay-transport-buttons">
        <button type="button" className="replay-toggle replay-button" aria-label={playback?.paused || playback?.ended ? '播放' : '暂停'} disabled={!playback || busy} onClick={() => {void control(playback?.paused || playback?.ended ? 'play' : 'pause');}}>{icon(`player-${playback?.paused || playback?.ended ? 'play' : 'pause'}`)}</button>
        <button type="button" className="replay-restart replay-button" aria-label="重播" disabled={!playback || busy} onClick={() => {void control('seek', 0);}}>{icon('replay-arrow-counter-clockwise')}<span>重播</span></button>
        <div className="replay-speed">{speed('replaySpeedSelect')}</div>
        <span className="replay-seek-status" role="status">{playback?.seeking ? `定位至 ${clock(playback.target ?? 0)}` : playback?.ended ? '录像已结束' : ''}</span>
        <button type="button" className="replay-fullscreen replay-button" aria-label="切换全屏" onClick={() => {void session.togglePlayerFullscreen();}} data-fullscreen={session.nativeTouch.isFullscreen()} dangerouslySetInnerHTML={{__html: playerUtilityIcons.fullscreenToggle}}/>
      </div>
    </section>
    <section className="replay-current" aria-label="录像信息">
      <h2 title={playback?.path}>{playback?.path.split('/').pop() || (files.phase === 'loading' ? '正在读取录像' : '选择一份录像')}</h2>
      <p>{playback ? <><span>东方花映塚</span><span>{['Easy','Normal','Hard','Lunatic','Extra'][playback.difficulty] ?? ''}</span><span>{characters[playback.left] ?? 'P1'} vs {characters[playback.right] ?? 'P2'}</span></> : '选择录像即可播放'}</p>
      {error && <p className="replay-workspace-error" role="alert">{error}</p>}
    </section>
    </>, stage)}
    <aside className="replay-file-browser" aria-label="录像文件" onDragOver={event => {if (!busy && event.dataTransfer.types.includes('Files')) event.preventDefault();}} onDrop={event => {event.preventDefault(); if (busy || files.phase !== 'ready') return; const list = [...event.dataTransfer.files]; if (list.length !== 1) {report(Error('请每次导入一个文件')); return;} void importFile(list[0]).catch(report);}}>
      <header><h2>录像</h2><button type="button" className="replay-button" disabled={busy || files.phase !== 'ready'} onClick={() => {setBusy(true); void session.chooseReplayFile(files.productId!).catch(report).finally(() => setBusy(false));}}>{icon('replay-upload-simple')}<span>导入</span></button></header>
      <nav className="replay-breadcrumb" aria-label="录像位置">{icon('replay-folder')}<button type="button" onClick={() => {setFolder(null); setMoving(null);}}>全部录像</button>{folder && <span title={folder.slice(7)}>/ {folder.slice(7)}</span>}</nav>
      {moving && <form className="replay-workspace-move" onSubmit={event => {event.preventDefault();setBusy(true); void model.move(moving, destination === 'new' ? `replay/${newFolder.trim()}` : destination).then(ok => {if (ok) setMoving(null);}).finally(() => setBusy(false));}}>
        <strong title={moving}>移动 {moving.split('/').pop()}</strong><MainSelect id="replayMoveSelect" className="option-select" aria-label="移动到" value={destination} onChange={event => setDestination(event.target.value)}><option value="replay">全部录像</option>{folders.map(item => <option key={item.key} value={item.key}>{item.name}</option>)}<option value="new">新建文件夹</option></MainSelect>
        {destination === 'new' && <input aria-label="文件夹名称" maxLength={128} value={newFolder} onChange={event => setNewFolder(event.target.value)}/>}
        <div><button type="button" className="replay-button" onClick={() => setMoving(null)}>取消</button><button type="submit" className="replay-button" disabled={busy || !isTh09ReplayDirectory(destination === 'new' ? `replay/${newFolder.trim()}` : destination)}>移动</button></div>
      </form>}
      <div className="replay-workspace-files" key={folder ?? 'replay'} aria-busy={files.phase === 'loading' || busy}>
        {files.phase === 'loading' ? <p className="replay-workspace-empty">正在读取录像…</p> : files.phase === 'error' ? <div className="replay-workspace-empty"><p>录像读取失败</p><button type="button" onClick={() => {void model.refresh().catch(report);}}>重试</button></div> : <>
          {!folder && folders.map(item => <div className="replay-workspace-file replay-workspace-folder" key={item.key}><button type="button" data-replay-folder={item.key} onClick={() => {setFolder(item.key); setMoving(null);}}>{icon('replay-folder')}<strong title={item.name}>{item.name}</strong><small>{item.rows.length} 份录像</small></button><button type="button" className="replay-file-menu-trigger" disabled={busy} aria-label={`管理文件夹 ${item.name}`} aria-haspopup="menu" aria-expanded={fileMenu?.path === item.key} onClick={event => toggleFileMenu(event.currentTarget, item.key, true)}>{icon('replay-dots-three')}</button></div>)}
          {rows.map(row => <div className={`replay-workspace-file${playback?.path === row.path ? ' selected' : ''}`} key={row.path} data-replay-file={row.path}>
            <button type="button" data-replay-watch={row.path} disabled={busy} onClick={() => {setError(''); setBusy(true); void model.watch(row.path).finally(() => setBusy(false));}}>{playback?.path === row.path && <span className="replay-file-selected">{icon('player-play')}</span>}{icon('replay-file-text')}<span className="replay-file-description"><strong title={row.name}>{row.name}</strong><small title={row.replayMatch?.startedAt ? '对战开始时间' : '文件修改时间'}>{timestamp(row.replayMatch?.startedAt || row.modifiedAt)}</small></span><small>{formatReplayBytes(row.size)}</small></button>
            <button type="button" className="replay-file-menu-trigger" disabled={busy} aria-label={`管理 ${row.name}`} aria-haspopup="menu" aria-expanded={fileMenu?.path === row.path} onClick={event => toggleFileMenu(event.currentTarget, row.path, false)}>{icon('replay-dots-three')}</button>
          </div>)}
          {!files.rows.length && <p className="replay-workspace-empty">暂无录像<br/><small>导入录像，或将文件拖到这里</small></p>}
        </>}
      </div>
      <footer>{files.rows.length} 份录像</footer>
    </aside>
    {fileMenu && <ReplayFileMenu key={fileMenu.path} anchor={fileMenu.anchor} label={`管理 ${fileMenu.path.split('/').pop()}`} onClose={() => setFileMenu(null)} actions={fileMenu.folder ? [{label: '下载', run: () => {void model.downloadFolder(fileMenu.path);}}] : [
      {label: '下载', run: () => {void model.download(fileMenu.path);}},
      {label: '移动', run: () => {setMoving(fileMenu.path); setDestination('replay'); setNewFolder('');}},
      {label: '重命名', run: () => {void model.rename(fileMenu.path);}},
      {label: '删除', destructive: true, run: () => {void model.remove(fileMenu.path);}},
    ]}/>}
  </>;
}
