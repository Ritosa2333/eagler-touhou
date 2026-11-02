import {useLayoutEffect, useRef, useState} from 'react';
import {closeMainSelectMenus} from './MainSelect';
import type {OptionsTargets} from './options-ownership';
import type {LibraryProduct} from './products';
import {useSurfaceMotion} from '../../ui/use-surface-motion';

export interface LibraryOptionsPresenceInput {
  /** The Router has already committed this state; this hook never navigates. */
  open: boolean;
  context: 'library' | 'lobby';
  product: LibraryProduct | undefined;
  /** The document-lived site preference, supplied by its existing owner. */
  lessMotion: boolean;
  /** Running game conceals its retained settings without a home-close motion. */
  concealed?: boolean;
}

export interface LibraryOptionsPresence {
  /** Keep the panel, selected card and inert library together until exit settles. */
  open: boolean;
  closing: boolean;
  product: LibraryProduct | undefined;
  targets: OptionsTargets;
  openedProduct: LibraryProduct['id'] | undefined;
}

type Presence = Omit<LibraryOptionsPresence, 'openedProduct' | 'targets'>;
/** Router state commits immediately; the visual surface retires on completion. */
export function useLibraryOptionsPresence({open, context, product, lessMotion, concealed = false}: LibraryOptionsPresenceInput): LibraryOptionsPresence {
  const [presence, setPresence] = useState<Presence>(() => ({open, closing: false, product}));
  const targets = useRef<OptionsTargets>({selectedCard: null, panel: null, backdrop: null});
  const panel = useRef<HTMLElement>(null), backdrop = useRef<HTMLElement>(null);
  const previous = useRef({open, context});
  const closingCard = useRef<HTMLElement | null>(null), pendingFocus = useRef<HTMLElement | null>(null);

  useLayoutEffect(() => () => {document.body.classList.remove('library-tools-open', 'library-tools-closing');}, []);
  useLayoutEffect(() => {
    const before = previous.current; previous.current = {open, context};
    if (open || before.context !== context) {
      if (before.open && (!open || before.context !== context)) closeMainSelectMenus();
      closingCard.current = null; pendingFocus.current = null;
      setPresence(current => {
        const nextProduct = product ?? current.product;
        return current.open === open && !current.closing && current.product === nextProduct ? current : {open, closing: false, product: nextProduct};
      });
    } else if (before.open) {
      closeMainSelectMenus();
      closingCard.current = targets.current.selectedCard;
      if (concealed) setPresence(current => ({...current, open: false, closing: false}));
      else setPresence(current => ({...current, closing: true}));
    } else if (concealed && presence.closing) {
      closingCard.current = null; pendingFocus.current = null;
      setPresence(current => ({...current, open: false, closing: false}));
    } else if (product && !presence.closing) {
      setPresence(current => current.product === product ? current : {...current, product});
    }
  }, [open, context, product, concealed, presence.closing]);

  useLayoutEffect(() => {
    document.body.classList.toggle('library-tools-open', presence.open && !concealed);
    document.body.classList.toggle('library-tools-closing', presence.closing && !concealed);
    panel.current = targets.current.panel; backdrop.current = targets.current.backdrop ?? null;
    if (!concealed && !open && !presence.open && pendingFocus.current) {
      const selected = pendingFocus.current; pendingFocus.current = null;
      if (selected.isConnected) selected.focus({preventScroll: true});
    }
  });
  const phase = presence.open && !concealed ? presence.closing ? 'exit' : 'enter' : null;
  useSurfaceMotion(panel, phase, {reduced: lessMotion, onComplete() {
    if (!presence.closing) return;
    pendingFocus.current = closingCard.current; closingCard.current = null;
    setPresence(current => ({...current, open: false, closing: false}));
  }});
  useSurfaceMotion(backdrop, phase, {kind: 'backdrop', reduced: lessMotion});
  return {...presence, targets: targets.current, open: presence.open && !concealed, closing: presence.closing && !concealed, openedProduct: presence.open && !concealed ? presence.product?.id : undefined};
}
