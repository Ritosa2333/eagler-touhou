import {createContext, useContext, useLayoutEffect, useRef, type MutableRefObject} from 'react';

/** The active editor owns Escape before the enclosing dialog can close. */
export const InputCapture = createContext<MutableRefObject<(() => void) | null> | null>(null);
export function useInputCapture(active: boolean, cancel: () => void) {
  const owner = useContext(InputCapture), latest = useRef(cancel); latest.current = cancel;
  useLayoutEffect(() => {
    if (!owner || !active) return;
    const clear = () => latest.current(); owner.current = clear;
    return () => {if (owner.current === clear) owner.current = null;};
  }, [active, owner]);
}
