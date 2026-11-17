import {createContext, useContext, useEffect, useLayoutEffect, useRef} from 'react';
import type {BlockerFunction} from 'react-router';

export interface UnsavedChangesGuard {
  shouldBlock: BlockerFunction;
  discard(): void;
}
interface UnsavedChangesOwner {
  register(guard: UnsavedChangesGuard): () => void;
  confirmDiscard(): Promise<boolean>;
}
export const UnsavedChangesContext = createContext<UnsavedChangesOwner | null>(null);
const allowDiscard = () => Promise.resolve(true);

export function useConfirmDiscardChanges() {
  return useContext(UnsavedChangesContext)?.confirmDiscard ?? allowDiscard;
}

/** Forms register with the document's sole Router blocker. */
export function useUnsavedChanges(dirty: boolean, onDiscard: () => void) {
  const register = useContext(UnsavedChangesContext)?.register;
  const discard = useRef(onDiscard);
  useLayoutEffect(() => {discard.current = onDiscard;});
  useLayoutEffect(() => {
    if (!dirty || !register) return;
    return register({
      shouldBlock: ({currentLocation, nextLocation}) =>
        currentLocation.pathname !== nextLocation.pathname || currentLocation.hash !== nextLocation.hash,
      discard: () => discard.current(),
    });
  }, [dirty, register]);
  useEffect(() => {
    if (!dirty) return;
    const beforeUnload = (event: BeforeUnloadEvent) => {
      event.preventDefault();
      event.returnValue = '';
    };
    window.addEventListener('beforeunload', beforeUnload);
    return () => window.removeEventListener('beforeunload', beforeUnload);
  }, [dirty]);
}
