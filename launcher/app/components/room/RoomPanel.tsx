import {Button as UiButton} from '../../ui/controls';
import type {ReactNode, RefObject} from 'react';
import {useMainDialog} from '../notices/use-main-dialog';
import {useLocale} from '../../i18n';
import type {RoomPanelKind} from './types';
/** Router owns the entry. The native owner animates visibility changes and
 * restores the exact opening control after committed closure. */
export function RoomPanel({open, kind, peerId, onCloseRequest, onClosed, trigger, children}: {
  open: boolean; kind: RoomPanelKind; peerId?: string; onCloseRequest(): void; onClosed(): void;
  trigger: RefObject<HTMLElement | null>; children: ReactNode;
}) {
  const {t} = useLocale();
  const dialog = useMainDialog({open, onCloseRequest, onClosed() {
    onClosed();
    if (trigger.current?.isConnected) trigger.current.focus({preventScroll:true});
  }});
  // biome-ignore lint/a11y/useKeyWithClickEvents: Native cancel and the Done button provide keyboard dismissal; this click checks outside the panel rectangle.
  return <dialog ref={dialog.ref} id="mpRoomPanel" className="mp-room-panel" aria-labelledby="mpRoomPanelTitle" data-panel={kind} data-network-peer={peerId}
    onCancel={event => {event.preventDefault(); onCloseRequest();}} onClick={event => {if (event.target !== event.currentTarget) return; const rect = event.currentTarget.getBoundingClientRect(); if (event.clientX < rect.left || event.clientX > rect.right || event.clientY < rect.top || event.clientY > rect.bottom) onCloseRequest();}}>
    <header className="mp-panel-head"><h3 id="mpRoomPanelTitle">{t(kind === 'personal' ? 'room.playerOptions' : kind === 'network' ? 'room.network' : kind === 'game' ? 'multiplayer.gameSettings' : 'room.spectatorLounge')}</h3><UiButton variant="icon" type="button" id="mpRoomPanelClose" onClick={onCloseRequest}>{t('room.done')}</UiButton></header>{children}
  </dialog>;
}
