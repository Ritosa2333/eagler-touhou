import {FileText, Play} from 'lucide-react';
import type {ReactNode} from 'react';
import {useLocale} from '../../i18n';
import {formatReplayBytes} from '../../models/replays';
import {Button as UiButton} from '../../ui/controls';
import {ReplayFileMenu, type ReplayFileAction} from './ReplayFileMenu';

/** File actions and expanded stage content stay with the calling workspace. */
export function ReplayFileRow({path, name, size, selected, versus = false, expanded, busy, metadata, actions, onOpen, children}: {
  path: string; name: string; size: number; selected?: boolean; versus?: boolean;
  expanded?: boolean; busy?: boolean; metadata: ReactNode; actions: ReplayFileAction[];
  onOpen(): void; children?: ReactNode;
}) {
  const {t} = useLocale();
  return <div className={`replay-workspace-file${!versus ? ' replay-workspace-recording' : ''}${selected ? ' selected' : ''}`} data-replay-file={path}>
    <UiButton variant="unstyled" type="button" data-replay-watch={path} aria-expanded={versus ? undefined : expanded} disabled={busy} onClick={onOpen}>
      {versus && <>{selected && <span className="replay-file-selected"><Play aria-hidden="true"/></span>}<FileText aria-hidden="true"/></>}
      <span className="replay-file-description"><span className="replay-file-heading"><strong title={name}>{name}</strong>{!versus && <small>{formatReplayBytes(size)}</small>}</span>{metadata}</span>
      {versus && <small>{formatReplayBytes(size)}</small>}
    </UiButton>
    <ReplayFileMenu disabled={busy} label={t('replay.ui.manage', {name})} actions={actions}/>
    {children}
  </div>;
}
