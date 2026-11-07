import {Button as UiButton} from '../../ui/controls';
import {useEffect, useLayoutEffect, useRef, useState} from 'react';
import {createEdgeDrawerGesture} from '../../../src/launcher/edge-drawer-gesture.mts';
import {cycleFocus} from '../cycle-focus';
import {useLocale} from '../../i18n';
import {useOptionsViewport} from '../launcher/use-options-viewport';
import {useSurfaceMotion} from '../../ui/use-surface-motion';
export interface RoomSettingsDrawerProps {
  roomOpen: boolean;
  open: boolean;
  onCloseRequest(): void;
  /** Retain settings while the fixed Player owns the editor foreground. */
  foregroundActive?: boolean;
  onClosingChange?(closing: boolean): void;
}
/** The shared OptionsPanel renders its header and fold into these outlets. */
export function RoomSettingsDrawer({roomOpen, open, foregroundActive = false, onCloseRequest, onClosingChange}: RoomSettingsDrawerProps) {
  const {t} = useLocale(), drawer = useRef<HTMLElement>(null), backdrop = useRef<HTMLButtonElement>(null), generation = useRef(0);
  useOptionsViewport(drawer);
  const [presence, setPresence] = useState({hidden: true, closing: false});
  const suspendedFocus = useRef<HTMLElement | null>(null);
  const pendingReturnFocus = useRef(false);
  const closingFocus = useRef(false);
  const requested = useRef({open, roomOpen, foregroundActive: false});
  const latest = useRef({open, roomOpen, foregroundActive, onCloseRequest, onClosingChange}); latest.current = {open, roomOpen, foregroundActive, onCloseRequest, onClosingChange};
  useLayoutEffect(() => {
    const node = drawer.current, token = ++generation.current;
    if (!node) return;
    const before = requested.current; requested.current = {open, roomOpen, foregroundActive};
    const resuming = before.foregroundActive && before.open && before.roomOpen;
    if (roomOpen && open) {
      pendingReturnFocus.current = false;
      closingFocus.current = false;
      setPresence({hidden: false, closing: false});
      if (!foregroundActive && !resuming) queueMicrotask(() => {if (generation.current === token && !latest.current.foregroundActive) node.querySelector<HTMLElement>('#libraryBack')?.focus({preventScroll: true});});
    } else if (node.hidden || !roomOpen || foregroundActive) {
      pendingReturnFocus.current = node.contains(document.activeElement) && roomOpen && !foregroundActive;
      setPresence({hidden: true, closing: false});
    } else {closingFocus.current = node.contains(document.activeElement); setPresence({hidden: false, closing: true});}
    return () => {generation.current++;};
  }, [roomOpen, open, foregroundActive]);
  const phase = presence.hidden || foregroundActive ? null : presence.closing ? 'exit' : 'enter';
  useSurfaceMotion(drawer, phase, {onComplete() {
    if (!presence.closing) return;
    pendingReturnFocus.current = closingFocus.current && latest.current.roomOpen && !latest.current.foregroundActive;
    closingFocus.current = false;
    setPresence({hidden: true, closing: false});
  }});
  useSurfaceMotion(backdrop, phase, {kind: 'backdrop'});
  useLayoutEffect(() => {
    onClosingChange?.(presence.closing);
    if (presence.hidden && pendingReturnFocus.current) {
      pendingReturnFocus.current = false;
      // Main unhides the cue before restoring focus. The parent owns its
      // hidden prop, so wait for that closing=false commit to land first.
      const token = generation.current;
      queueMicrotask(() => {if (generation.current === token && latest.current.roomOpen && !latest.current.open && !latest.current.foregroundActive) document.getElementById('mpSettingsRoomDrawerToggle')?.focus({preventScroll: true});});
    }
  }, [presence, onClosingChange]);
  useLayoutEffect(() => {
    const node = drawer.current;
    if (!node) return;
    if (foregroundActive) {if (document.activeElement instanceof HTMLElement && node.contains(document.activeElement)) suspendedFocus.current = document.activeElement;}
    else if (open && suspendedFocus.current?.isConnected && node.contains(suspendedFocus.current)) {suspendedFocus.current.focus({preventScroll: true}); suspendedFocus.current = null;}
  }, [foregroundActive, open]);
  useEffect(() => {
    const node = drawer.current;
    if (!node) return;
    // Native bubbling keeps Escape and Tab scoped to the visible drawer.
    const keydown = (event: KeyboardEvent) => {
      if (!latest.current.open || latest.current.foregroundActive || event.defaultPrevented) return;
      if (event.key === 'Escape') {event.preventDefault(); latest.current.onCloseRequest();}
      cycleFocus(node, event);
    };
    node.addEventListener('keydown', keydown);
    const gesture = createEdgeDrawerGesture({side: 'right', drawer: node,
      enabled: () => latest.current.open && !latest.current.foregroundActive,
      isOpen: () => latest.current.open, open: () => {}, close: () => latest.current.onCloseRequest()});
    return () => {node.removeEventListener('keydown', keydown); gesture.destroy();};
  }, []);
  return <><UiButton variant="unstyled" ref={backdrop} className={`settings-room-backdrop${presence.closing ? ' closing' : ''}`} id="mpSettingsRoomBackdrop" type="button" aria-label={t('settings.drawerClose')} hidden={!roomOpen || presence.hidden || foregroundActive} onClick={onCloseRequest}/>
    <aside ref={drawer} className={`mp-settings-room-drawer settings-ui${presence.closing ? ' closing' : ''}`} id="mpSettingsRoomDrawer" data-ui-surface="room-settings" role="dialog" aria-modal="true" aria-label={t('settings.title')} hidden={presence.hidden || foregroundActive} inert={foregroundActive}><div id="mpSettingsRoomDrawerHeader" className="mp-settings-room-header"/><div className="mp-settings-room-content" id="mpSettingsRoomDrawerContent"/></aside>
  </>;
}
