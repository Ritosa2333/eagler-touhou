import {Download, X} from 'lucide-react';
import {useSyncExternalStore} from 'react';
import {PRODUCT_GAMES} from '../../../src/contracts/product-catalog.mts';
import {useLocale} from '../../i18n';
import {LoadingActivity} from '../../ui/loading-activity';
import type {GameDownloads} from '../../models/game-downloads';
import {Button} from '../../ui/button';
import {Dialog, DialogContent, DialogTitle, DialogTrigger} from '../../ui/dialog';
import {Progress} from '../../ui/progress';
import './game-download-progress.css';

const speedLabel = (bytes: number) => bytes >= 1048576 ? `${(bytes / 1048576).toFixed(1)} MiB/s` : `${Math.round(bytes / 1024)} KiB/s`;

export function GameDownloadProgress({downloads, assetUrl}: {downloads: GameDownloads; assetUrl(path: string): string}) {
  const {active, pending, failed, progress, network} = useSyncExternalStore(downloads.subscribe, downloads.getSnapshot, downloads.getSnapshot);
  const {t} = useLocale();
  if (!active) return null;
  const title = PRODUCT_GAMES[active.game].title;
  const task = network?.active.find(item => item.kind !== 'descriptor');
  const preparing = !task || task.phase === 'requesting';
  const bytesPercent = task?.total ? Math.min(100, Math.floor(100 * task.loaded / task.total)) : null;
  const elapsed = task ? (task.updatedAt - task.startedAt) / 1000 : 0;
  const speed = task && elapsed > 0 && task.loaded > 0 ? speedLabel(task.loaded / elapsed) : null;
  const remaining = progress?.total != null && progress.completed != null ? progress.total - progress.completed : null;
  return <Dialog>
    <DialogTrigger asChild>
      <Button className="game-download-trigger" variant="outline" aria-label={t('downloadTasks.title')} title={t('downloadTasks.title')}>
        <Download aria-hidden="true"/>
        <span aria-hidden="true">{bytesPercent === null ? t('downloadTasks.downloading') : `${bytesPercent}%`}</span>
        {pending.length > 0 && <span className="game-download-count" aria-hidden="true">+{pending.length}</span>}
      </Button>
    </DialogTrigger>
    <DialogContent className="game-download-dialog" aria-describedby={undefined}>
      <DialogTitle>{t('downloadTasks.title')}</DialogTitle>
      <section className="game-download-active" aria-label={title} data-download-active={active.game}>
        <div className="game-download-row">
          <strong>{title}</strong>
          <Button variant="ghost" size="icon-sm" aria-label={t('action.cancel')} title={t('action.cancel')} onClick={() => downloads.cancel(active.game)}><X aria-hidden="true"/></Button>
        </div>
        {preparing && <div className="game-download-wait" role="status">
          <LoadingActivity key={active.game} assetUrl={assetUrl} context="install" compact
            status={t(task ? 'transfer.waitingServer' : 'transfer.preparingShort')}/>
        </div>}
        <Progress value={bytesPercent} aria-label={title}/>
        <div className="game-download-meta">
          {!preparing && <span>{t('downloadTasks.downloading')}{bytesPercent !== null && ` · ${bytesPercent}%`}</span>}
          {speed && <span>{speed}</span>}
          {remaining !== null && <span>{t('downloadTasks.remaining', {count: remaining})}</span>}
        </div>
      </section>
      {pending.length > 0 && <ol className="game-download-list" aria-label={t('downloadTasks.queued')}>
        {pending.map(({game}) => <li className="game-download-row" key={game} data-download-queued={game}>
          <span>{PRODUCT_GAMES[game].title}</span>
          <small>{t('downloadTasks.queued')}</small>
          <Button variant="ghost" size="icon-sm" aria-label={t('action.cancel')} title={t('action.cancel')} onClick={() => downloads.cancel(game)}><X aria-hidden="true"/></Button>
        </li>)}
      </ol>}
      {failed.length > 0 && <ul className="game-download-list">
        {failed.map(({game}) => <li className="game-download-row" key={game} data-download-failed={game}>
          <div><strong>{PRODUCT_GAMES[game].title}</strong><p className="game-download-error">{t('downloadTasks.failed')}</p></div>
          <Button variant="outline" size="sm" onClick={() => downloads.retry(game)}>{t('action.retry')}</Button>
        </li>)}
      </ul>}
    </DialogContent>
  </Dialog>;
}
