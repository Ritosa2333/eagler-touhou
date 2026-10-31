import { type CSSProperties, useLayoutEffect, useRef, useState, useSyncExternalStore } from 'react';
import { isReplayImportFileName } from '../../../src/launcher/replay-files.mts';
import { isTh09ReplayDirectory } from '../../../src/launcher/th09-replay-match.mts';
import { useLocale } from '../../i18n';
import { type createReplayModel, formatReplayBytes, groupReplayFolders } from '../../models/replays';
import { Button } from '../ui/button';
import { type MainDialogProps, useMainDialog } from './use-main-dialog';

export interface ReplayDialogProps extends MainDialogProps {
  model: ReturnType<typeof createReplayModel>;
  onImportRequest(): void;
  /** The actual replay importer must use model.mutations, as main importFile does. */
  onDropFile(file: File): Promise<void>;
  /** Route to original setStatus(errorReason) and toast(replay.importFailed). */
  onImportError(error: unknown): void;
}
/** Main index.html817–824 and app.mts6429–6575, without a new route or rename UI. */
export function ReplayDialog({ model, onImportRequest, onDropFile, onImportError, ...props }: ReplayDialogProps) {
  const { t } = useLocale(),
    state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const dialog = useMainDialog({ ...props, onClosed: model.close }, 220, 'replay-window-out', 'close');
  // Router Forward restores main's retained native dialog, not a new manager
  // command. Reactivate its cached actions/read without another file session.
  useLayoutEffect(() => {
    if (props.open) model.reopenRetained();
  }, [props.open, model]);
  const dragDepth = useRef(0),
    [dragging, setDragging] = useState(false);
  const [folderKey, setFolderKey] = useState<string | null>(null);
  const [moving, setMoving] = useState<string | null>(null),
    [destination, setDestination] = useState('replay');
  const [newFolder, setNewFolder] = useState(''),
    [moveBusy, setMoveBusy] = useState(false);
  const folders = groupReplayFolders(state.rows),
    folder = folders.find((item) => item.key === folderKey);
  const rows = folder ? folder.rows : state.rows.filter((row) => row.path.split('/').length === 2);
  const supportsFolders = state.productId === 'th09' || state.productId === 'th09mp';
  const moveTarget = destination === 'new' ? `replay/${newFolder.trim()}` : destination;
  useLayoutEffect(() => {
    setFolderKey(null);
    setMoving(null);
  }, [state.productId]);
  useLayoutEffect(() => {
    if (!props.open) setMoving(null);
  }, [props.open]);
  useLayoutEffect(() => {
    if (state.phase === 'ready' && folderKey && !folders.some((item) => item.key === folderKey)) setFolderKey(null);
  }, [state.phase, state.rows, folderKey]);
  const summary =
    state.phase === 'loading'
      ? t('status.readingReplay')
      : state.phase === 'error'
        ? t('status.replayReadFailed')
        : t('replay.fileCount', { count: folder ? folder.rows.length : state.rows.length });
  const resetDrag = () => {
    dragDepth.current = 0;
    setDragging(false);
  };
  return (
    <dialog
      ref={dialog.ref}
      data-launcher-document=""
      className="replay-dialog"
      id="replayDialog"
      onCancel={dialog.onCancel}
      onAnimationEnd={dialog.onAnimationEnd}
    >
      <form
        method="dialog"
        className={`replay-window${dragging ? ' dragging' : ''}`}
        onSubmit={(event) => {
          event.preventDefault();
          dialog.requestClose();
        }}
        onDragEnter={(event) => {
          if (!event.dataTransfer?.types.includes('Files')) return;
          event.preventDefault();
          dragDepth.current++;
          setDragging(true);
        }}
        onDragOver={(event) => {
          if (!event.dataTransfer?.types.includes('Files')) return;
          event.preventDefault();
          event.dataTransfer.dropEffect = 'copy';
        }}
        onDragLeave={() => {
          dragDepth.current = Math.max(0, dragDepth.current - 1);
          if (!dragDepth.current) setDragging(false);
        }}
        onDrop={(event) => {
          event.preventDefault();
          resetDrag();
          const files = [...(event.dataTransfer?.files || [])];
          void (async () => {
            if (files.length !== 1) throw new Error(t('replay.dropSingle'));
            if (!isReplayImportFileName(files[0].name)) throw new Error(t('replay.dropType'));
            await onDropFile(files[0]);
          })().catch(onImportError);
        }}
      >
        <header>
          <strong>{t('replay.manager')}</strong>
          <Button
            variant="legacy"
            size="legacy"
            type="button"
            data-replay-close=""
            aria-label={t('action.close')}
            onClick={dialog.requestClose}
          >
            ×
          </Button>
        </header>
        <div className="replay-drop-hint">{t('replay.dropHint')}</div>
        {folder && (
          <nav className="replay-path" aria-label={t('th09.replayLocation')}>
            <Button
              variant="legacy"
              size="legacy"
              type="button"
              data-replay-parent=""
              aria-label={t('replay.parentFolder')}
              onClick={() => {
                setFolderKey(null);
                setMoving(null);
              }}
            >
              ..
            </Button>
            <strong title={folder.name}>{folder.name}</strong>
          </nav>
        )}
        {moving && (
          <section className="replay-move" aria-label={t('replay.move')} aria-busy={moveBusy}>
            <strong title={moving.split('/').pop()}>{moving.split('/').pop()}</strong>
            <label>
              {t('replay.moveTo')}
              <select
                data-replay-destination=""
                value={destination}
                disabled={moveBusy}
                onChange={(event) => setDestination(event.target.value)}
              >
                <option value="replay">{t('replay.rootFolder')}</option>
                {folders.map((item) => (
                  <option key={item.key} value={item.key}>
                    {item.name}
                  </option>
                ))}
                <option value="new">{t('replay.newFolder')}…</option>
              </select>
            </label>
            {destination === 'new' && (
              <label>
                {t('replay.folderName')}
                <input
                  data-replay-new-folder=""
                  value={newFolder}
                  maxLength={128}
                  disabled={moveBusy}
                  onChange={(event) => setNewFolder(event.target.value)}
                  autoFocus
                />
              </label>
            )}
            <span className="replay-row-actions">
              <Button variant="legacy" size="legacy" type="button" disabled={moveBusy} onClick={() => setMoving(null)}>
                {t('action.cancel')}
              </Button>
              <Button
                variant="legacy"
                size="legacy"
                type="button"
                data-replay-move-confirm=""
                disabled={
                  moveBusy ||
                  !isTh09ReplayDirectory(moveTarget) ||
                  moving.slice(0, moving.lastIndexOf('/')).toLowerCase() === moveTarget.toLowerCase()
                }
                onClick={() => {
                  setMoveBusy(true);
                  void model
                    .move(moving, moveTarget)
                    .then((ok) => {
                      if (ok) setMoving(null);
                    })
                    .finally(() => setMoveBusy(false));
                }}
              >
                {t('replay.move')}
              </Button>
            </span>
          </section>
        )}
        <div className="replay-list" id="replayList">
          {state.phase === 'loading' ? (
            <div className="replay-loading">
              <i aria-hidden="true" />
              <span>{t('replay.loading')}</span>
            </div>
          ) : state.phase === 'error' ? (
            <div className="replay-empty replay-load-error">{t('status.replayReadFailed')}</div>
          ) : !state.rows.length ? (
            <div className="replay-empty">{t('status.noReplay')}</div>
          ) : (
            <>
              {!folder &&
                folders.map((item) => (
                  <div className="replay-row replay-folder" key={item.key}>
                    <Button
                      variant="legacy"
                      size="legacy"
                      type="button"
                      className="replay-folder-open"
                      data-replay-folder={item.key}
                      onClick={() => {
                        setFolderKey(item.key);
                        setMoving(null);
                      }}
                      aria-label={t('th09.replayOpenFolder', { name: item.name })}
                    >
                      <svg viewBox="0 0 24 24" aria-hidden="true">
                        <path d="M3 7V5a1 1 0 0 1 1-1h5l2 3h9a1 1 0 0 1 1 1v11a1 1 0 0 1-1 1H4a1 1 0 0 1-1-1Z" />
                      </svg>
                      <span className="replay-name" title={item.name}>
                        {item.name}
                      </span>
                      <span className="replay-size">{t('replay.fileCount', { count: item.rows.length })}</span>
                      <span aria-hidden="true">›</span>
                    </Button>
                    <Button
                      variant="legacy"
                      size="legacy"
                      type="button"
                      data-replay-download-folder={item.key}
                      aria-label={`${t('replay.downloadFolder')} ${item.name}`}
                      onClick={() => {
                        void model.downloadFolder(item.key);
                      }}
                    >
                      {t('action.download')}
                    </Button>
                  </div>
                ))}
              {rows.map((row, index) => (
                <div
                  data-replay-file={row.path}
                  className={`replay-row${state.animateRows ? ' replay-row-enter' : ''}`}
                  key={row.path}
                  style={
                    state.animateRows
                      ? ({ '--replay-row-delay': `${Math.min(index, 8) * 16}ms` } as CSSProperties)
                      : undefined
                  }
                >
                  <span className="replay-name" title={row.name}>
                    {row.name}
                    {row.replayMatch && <small>{new Date(row.replayMatch.startedAt).toLocaleString()}</small>}
                  </span>
                  <span className="replay-size">{formatReplayBytes(row.size)}</span>
                  <span className="replay-row-actions">
                    {supportsFolders && (
                      <Button
                        variant="legacy"
                        size="legacy"
                        type="button"
                        data-replay-watch={row.path}
                        onClick={() => {
                          void model.watch(row.path);
                        }}
                      >
                        {t('replay.watch')}
                      </Button>
                    )}
                    <Button
                      variant="legacy"
                      size="legacy"
                      type="button"
                      onClick={() => {
                        void model.download(row.path);
                      }}
                    >
                      {t('action.download')}
                    </Button>
                    {supportsFolders && (
                      <Button
                        variant="legacy"
                        size="legacy"
                        type="button"
                        data-replay-move={row.path}
                        onClick={() => {
                          setMoving(row.path);
                          setDestination(folder ? 'replay' : (folders[0]?.key ?? 'new'));
                          setNewFolder('');
                        }}
                      >
                        {t('replay.move')}
                      </Button>
                    )}
                    <Button
                      variant="legacy"
                      size="legacy"
                      type="button"
                      onClick={() => {
                        void model.rename(row.path);
                      }}
                    >
                      {t('action.rename')}
                    </Button>
                    <Button
                      variant="legacy"
                      size="legacy"
                      type="button"
                      className="replay-delete"
                      onClick={() => {
                        void model.remove(row.path);
                      }}
                    >
                      {t('action.delete')}
                    </Button>
                  </span>
                </div>
              ))}
            </>
          )}
        </div>
        <footer>
          <span id="replaySummary">{summary}</span>
          <span className="replay-actions">
            <Button variant="legacy" size="legacy" type="button" data-action="import-replay" onClick={onImportRequest}>
              {t('replay.import')}
            </Button>
            <Button variant="legacy" size="legacy" type="button" data-replay-close="" onClick={dialog.requestClose}>
              {t('action.close')}
            </Button>
          </span>
        </footer>
      </form>
    </dialog>
  );
}
